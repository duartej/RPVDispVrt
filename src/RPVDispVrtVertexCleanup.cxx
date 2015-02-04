#include "RPVDispVrt/RPVDispVrtVertexCleanup.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "RPVDispVrt/RPVDispVrtVertexer.h"
#include "VxVertex/VxTrackAtVertex.h"

#include <iostream>

///extern "C" {
/// void dsinv(long int & , double *, long int &, long int &);             
///}


namespace Trk {
  extern void dsinv(long int * , double *, long int , long int *);             
}

//constructor
RPVDispVrtVertexCleanup::RPVDispVrtVertexCleanup
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_vtxTool("RPVDispVrtVertexer",this)
{
  declareProperty("VertexerTool",          m_vtxTool);
  declareProperty("DoVertexDisassembly",   m_disassembleVertices=true);
  declareProperty("VertexDisassembleChi2Cut",m_disassembleChi2Cut=4.0);
  declareProperty("TrackDetachCut",        m_TrackDetachCut=6 ); //chi2
  declareProperty("VertexMergeSignifCut",  m_VertexMergeCut=3. ); //3D distance signif
  declareProperty("VertexMergeDistanceCut",m_VertexMergeDistCut=10.); //3D absolute distance
  declareProperty("VertexMergeFinalDistanceCut",m_VertexMergeFinalDistCut=1.); //3D absolute distance
  declareProperty("VertexMergeFinalSignificanceCut",m_VertexMergeFinalSigCut=3.); //3D distance signif
  declareProperty("DoFinalVertexMergeDistance",m_mergeFinalVerticesDistance=true); 
  declareProperty("DoFinalVertexMergeSignif",m_mergeFinalVerticesSignif=false); 
  declareInterface< RPVDispVrtVertexCleanup  >( this );
}

//destructor  
RPVDispVrtVertexCleanup::~RPVDispVrtVertexCleanup() {
}

StatusCode 
RPVDispVrtVertexCleanup::initialize() {
  StatusCode sc = m_vtxTool.retrieve();
  if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to retrieve vertexer"<<endreq;

  return sc;
}

StatusCode 
RPVDispVrtVertexCleanup::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode
RPVDispVrtVertexCleanup::cleanupVertices(xAOD::VertexContainer* vertices, 
					 const xAOD::TrackParticleContainer* trkColl) 
{  
  StatusCode sc = StatusCode::SUCCESS;
  
  

  if (m_disassembleVertices) {
/// "disassemble" vertices with bad chisq:
    sc = disassembleVertices(vertices,trkColl);
    if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to disassemble vertices"<<endreq;  
  }
  /// figure out which tracks are in which vertices
  std::vector<std::vector<long int> > trkInVrt(trkColl->size());
  sc = classifyTracks(vertices, trkInVrt);
  if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to classify tracks"<<endreq;

  msg(MSG::DEBUG)<<"Number of vertices before cleanup/merge is "<<vertices->size()<<endreq;

  double FoundMax=999999999.; 
  long int SelectedTrack=-1;
  long int SelectedVertex=-1;
  double foundMinVrtDst=1000000.; 
  int foundV1=-1;
  int foundV2=-1;  
  /// iteratively find the shared track with the highest chisq to a particular vertex:

  while((FoundMax=maxOfShared( vertices, trkInVrt, SelectedTrack, SelectedVertex))>0) {

    foundMinVrtDst = 1000000.;
    if(FoundMax<m_TrackDetachCut) {
      foundMinVrtDst = minVrtVrtSig( vertices, foundV1, foundV2);
    }
    if ((FoundMax<m_TrackDetachCut)&&(foundMinVrtDst < m_VertexMergeCut)) {
      sc = mergeAndRefitVertices(vertices, foundV1, foundV2, trkColl);
      if (sc.isFailure()) {
	msg(MSG::DEBUG)<<"refit failed"<<endreq;
	continue;
      }
      while ( minVrtVrtSigNext( vertices, foundV1, foundV2) < m_VertexMergeCut) {   //check other vertices
	sc = mergeAndRefitVertices(vertices, foundV1, foundV2, trkColl);
	if (sc.isFailure()) {
	  msg(MSG::DEBUG)<<"Vertex merge+refit failed for vertices "<<foundV1<<" "<<foundV2<<endreq;
	  break;
	}
      } //end of while nextvrt

      StatusCode sc = classifyTracks(vertices, trkInVrt);
      
      if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to classify tracks second time round"<<endreq;

    } else {
      removeTrackFromVertex(vertices, SelectedTrack, SelectedVertex);
      //      vertices->at(SelectedVertex) = m_vtxTool->refitVertex(vertices->at(SelectedVertex),trkColl);
      m_vtxTool->refitVertex(vertices->at(SelectedVertex),trkColl);

      sc = classifyTracks(vertices, trkInVrt);
      if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to classify tracks after track removal"<<endreq;
    }
  } //end of while FoundMax
  msg(MSG::DEBUG)<<"Number of vertices after cleanup/merge is "<<vertices->size()<<endreq;

  /// remove vertices that have had all their tracks assigned to other vertices.
  removeEmptyVertices(vertices);
  /// if required, do final merge of close-together vertex pairs.
  if (m_mergeFinalVerticesDistance || m_mergeFinalVerticesSignif) {
    mergeFinalVertices(vertices,trkColl);
    removeEmptyVertices(vertices);
  }
  
  /// for each vertex, calculate and store the distance to closest other vertex..
  setMinVrtDists(vertices);
  
  return StatusCode::SUCCESS;

}

StatusCode 
RPVDispVrtVertexCleanup::disassembleVertices(xAOD::VertexContainer* vertices, 
					     const xAOD::TrackParticleContainer* trkColl) {

  
  std::vector<const xAOD::TrackParticle*>  ListBaseTracks;
  std::vector<long int> SelTrk;

  xAOD::VertexContainer::iterator vIt = vertices->begin();
  xAOD::VertexContainer::iterator vEnd = vertices->end();

  for (unsigned int iv=0; iv<vertices->size(); ++iv) {
    if ((vertices->at(iv))->chiSquared() < m_disassembleChi2Cut*(vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").size())
      continue;
    //// OK now remove the worst track from the bad vertex
    int NTrk=(vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").size();
    if (NTrk < 3) continue;
    long int SelT=-1;
    double Chi2Max=0.;
    for(int i=0; i<NTrk; i++){
      if( (vertices->at(iv))->auxdata<std::vector<double> >("chi2ForTrack").at(i) > Chi2Max) { 
      Chi2Max=(vertices->at(iv))->auxdata<std::vector<double> >("chi2ForTrack").at(i);
	SelT=i;
      }
    }	    
    if(SelT==-1) continue;    
    for(int i=0; i<NTrk; i++) {
      if(i==SelT)continue;
      ListBaseTracks.clear();
      ListBaseTracks.push_back( trkColl->at((vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(i)));
      ListBaseTracks.push_back( trkColl->at((vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(SelT)));
      SelTrk.clear();
      SelTrk.push_back((vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(i)); 
      SelTrk.push_back((vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(SelT));
      
      xAOD::Vertex* newVertex = m_vtxTool->makeVertex(ListBaseTracks,SelTrk,trkColl);
      if (newVertex != 0)
	vertices->push_back(newVertex);

    } ///end of loop over tracks
    long int selv = iv;
    removeTrackFromVertex(vertices,(vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(SelT),selv);
    m_vtxTool->refitVertex(vertices->at(iv),trkColl);

  } ///end of loop over vertices
  
  return StatusCode::SUCCESS;
}



void 
RPVDispVrtVertexCleanup::removeTrackFromVertex(xAOD::VertexContainer* vertices, 
					       long int & SelectedTrack,
					       long int & SelectedVertex) {
  
  for (unsigned int i=0; i< (vertices->at(SelectedVertex))->auxdata<std::vector<long int> >("trackIndices").size(); ++i) {
    if ((vertices->at(SelectedVertex))->auxdata<std::vector<long int> >("trackIndices").at(i)==SelectedTrack) {

      std::vector<long int>::iterator trkIndexIt = (vertices->at(SelectedVertex))->auxdata<std::vector<long int> >("trackIndices").begin() + i;
      (vertices->at(SelectedVertex))->auxdata<std::vector<long int> >("trackIndices").erase(trkIndexIt);
      std::vector<Trk::VxTrackAtVertex>::iterator trkIt = (vertices->at(SelectedVertex))->vxTrackAtVertex().begin() + i;
      (vertices->at(SelectedVertex))->vxTrackAtVertex().erase(trkIt);
    }
  }
  
  return;
}

StatusCode 
RPVDispVrtVertexCleanup::classifyTracks(xAOD::VertexContainer* inputVertices, 
					std::vector<std::vector<long int> >& trkInVrt) {
  
  for (unsigned int i=0; i<trkInVrt.size(); ++i) {
    trkInVrt.at(i).clear();
  }

  int NSet = inputVertices->size();
  for (int iv=0; iv<NSet; iv++) {
    int NTrkAtVrt= (inputVertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").size();
    if (NTrkAtVrt<2) continue;
    for (int jt=0; jt<NTrkAtVrt; jt++) {
      int tracknum=(inputVertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").at(jt);
      trkInVrt.at(tracknum).push_back(iv);
    }
  }
  
  return StatusCode::SUCCESS;
}

double 
RPVDispVrtVertexCleanup::maxOfShared(xAOD::VertexContainer* vertices, 
				     std::vector<std::vector<long int > >& trkInVrt,
				     long int & SelectedTrack,
				     long int & SelectedVertex) {

  
  long int NTrack=trkInVrt.size(); long int it, jv, itmp, NVrt, VertexNumber;
 
  double MaxOf=-999999999;
  
  long int NShMax=0;
  for( it=0; it<NTrack; it++) {
    NVrt=(trkInVrt)[it].size();         ///Number of vertices for this track
    if(  NVrt > NShMax ) NShMax=NVrt;
  }
if(NShMax<=1) return MaxOf;              /// No shared tracks
    
  for( it=0; it<NTrack; it++) {    
    NVrt=trkInVrt[it].size();         //// Number of vertices for this track
    if(  NVrt < NShMax )    {
      continue;     ///Not a shared track with maximal sharing
    }
    for( jv=0; jv<NVrt; jv++ ){
      VertexNumber=trkInVrt[it][jv];
      if( vertices->at(VertexNumber)->auxdata<std::vector<long int> >("trackIndices").size() <= 1) {
	continue; // one track vertex - nothing to do
      }
      for( itmp=0; itmp<(int)(vertices->at(VertexNumber)->auxdata<std::vector<long int> >("trackIndices").size()); itmp++) {
	if( vertices->at(VertexNumber)->auxdata<std::vector<long int> >("trackIndices").at(itmp) == it ) {         ///Track found
	  
	  if( MaxOf < vertices->at(VertexNumber)->auxdata<std::vector<double> >("chi2ForTrack").at(itmp) ) { 
	    MaxOf = vertices->at(VertexNumber)->auxdata<std::vector<double> >("chi2ForTrack").at(itmp);
	    SelectedTrack=it;
	    SelectedVertex=VertexNumber;
	  }
	}
      }
    }
  }   
  
  return MaxOf;
}

double 
RPVDispVrtVertexCleanup::minVrtVrtSig( xAOD::VertexContainer* vertices, int& V1, int& V2) {  

  double foundMinVrtDst=1000000.;
  
  for(int iv=0; iv<(int)(vertices->size()); iv++) { 
    vertices->at(iv)->auxdata<int>("numCloseVtx") = 0; 
    vertices->at(iv)->auxdata<double>("distCloseVtx") = 1000000.; 
  }

  for(int iv=0; iv<(int)(vertices->size())-1; iv++) {
    
    if( vertices->at(iv)->auxdata<std::vector<long int> >("trackIndices").size()< 2) continue;   /// Bad vertices 
    AmgVector(3) IV_vertex_location( vertices->at(iv)->position().x(),
				     vertices->at(iv)->position().y(),
				     vertices->at(iv)->position().z());

    for(int jv=iv+1; jv<(int)(vertices->size()); jv++) {
      
      if( vertices->at(jv)->auxdata<std::vector<long int> >("trackIndices").size()< 2) continue;   /// Bad vertices 
      
      AmgVector(3) JV_vertex_location( vertices->at(jv)->position().x(),
				       vertices->at(jv)->position().y(),
				       vertices->at(jv)->position().z());
      

      //>?>?>?set minimal spatial distance of vertices>?>?>//
      //>?>?>?a weak constraint>?>?>?//
      double tmp = (IV_vertex_location - JV_vertex_location).mag();
      if( tmp > m_VertexMergeDistCut )continue;

      
      //>?>?>if use distances and covariance matrices >?>?>?//
      
      double tmpDst = VrtVrtDist(vertices->at(iv), vertices->at(jv));
      if( tmpDst < foundMinVrtDst) {
	foundMinVrtDst = tmpDst; 
	V1=iv; 
	V2=jv;
      } 
      if( tmpDst < vertices->at(iv)->auxdata<double>("distCloseVtx") ) {
	vertices->at(iv)->auxdata<double>("distCloseVtx") = tmpDst; 
	vertices->at(iv)->auxdata<int>("numCloseVtx") = jv;
      }
      if( tmpDst < vertices->at(jv)->auxdata<double>("distCloseVtx") ) {
	vertices->at(jv)->auxdata<double>("distCloseVtx") = tmpDst; 
	vertices->at(jv)->auxdata<int>("numCloseVtx") = iv;
      }
    }  // end of inner loop over vertices
  }   //end of outer loop over vertices
  
  return foundMinVrtDst;
}


double
RPVDispVrtVertexCleanup::minVrtVrtDist( xAOD::VertexContainer* vertices, int& V1, int& V2) {  
  double foundMinVrtDst=1000000.;
  
  for(int iv=0; iv<(int)(vertices->size()); iv++) { 
    vertices->at(iv)->auxdata<int>("numCloseVtx") = 0; 
    vertices->at(iv)->auxdata<double>("distCloseVtx") = 1000000.; 
  }
 
  for(int iv=0; iv<(int)(vertices->size())-1; iv++) {
    
    if( vertices->at(iv)->auxdata<std::vector<long int> >("trackIndices").size()< 2) continue;   ///Bad vertices 
    AmgVector(3) IV_vertex_location( vertices->at(iv)->position().x(),
				   vertices->at(iv)->position().y(),
				   vertices->at(iv)->position().z());

    for(int jv=iv+1; jv<(int)(vertices->size()); jv++) {
      
      if( vertices->at(jv)->auxdata<std::vector<long int> >("trackIndices").size()< 2) continue;   /// Bad vertices 
      
      AmgVector(3) JV_vertex_location( vertices->at(jv)->position().x(),
				       vertices->at(jv)->position().y(),
				       vertices->at(jv)->position().z());
      
      //>?>?>?set minimal spatial distance of vertices>?>?>//
      //>?>?>?a weak constraint>?>?>?//
      double tmp = (IV_vertex_location - JV_vertex_location).mag();
      if( tmp < foundMinVrtDst) {
	foundMinVrtDst = tmp;
	V1=iv; 
	V2=jv;
      }
    }  // end of inner loop over vertices
  }   //end of outer loop over vertices
  
  return foundMinVrtDst;
}


double 
RPVDispVrtVertexCleanup::minVrtVrtSigNext( xAOD::VertexContainer* vertices, int & V1, int & V2)
{  
  V1=0;
  V2 = 0;
  double foundMinVrtDst=1000000.;
  
  for(int iv=0; iv<(int)(vertices->size())-1; iv++) {
    if( (vertices->at(iv))->auxdata<std::vector<long int> >("trackIndices").size()< 2) {
      continue;   //bad vertex
    }
    if( (vertices->at(iv))->auxdata<int>("numCloseVtx") ==0  )  {
      continue;   // Used vertex 
    }
    if( (vertices->at(iv))->auxdata<double>("distCloseVtx") <foundMinVrtDst ) {
      int vtmp=vertices->at(iv)->auxdata<int>("numCloseVtx");
      if((vertices->at(vtmp))->auxdata<int>("numCloseVtx") == 0) {
	continue;
      }  // Close vertex to given [iv] one is modified already 
      foundMinVrtDst=vertices->at(iv)->auxdata<double>("distCloseVtx");
      V1=iv;
      V2=vtmp;
    }
  }
  
  return foundMinVrtDst;
}



double 
RPVDispVrtVertexCleanup::VrtVrtDist(const xAOD::Vertex* vrt1, const xAOD::Vertex* vrt2) {
  double Signif=0.;
  
  double distx =  vrt1->position().x()- vrt2->position().x();
  double disty =  vrt1->position().y()- vrt2->position().y();
  double distz =  vrt1->position().z()- vrt2->position().z();
  

  const AmgSymMatrix(3) posErr1 = vrt1->covariancePosition();
  const AmgSymMatrix(3) posErr2 = vrt2->covariancePosition();

  double mmm[3][3];

  mmm[0][0] =   posErr1(0,0) + posErr2(0,0);
  mmm[0][1] = mmm[1][0] =  posErr1(0,1) + posErr2(0,1);
  mmm[1][1] =   posErr1(1,1) + posErr2(1,1);
  mmm[0][2] = mmm[2][0] =  posErr1(0,2) + posErr2(0,2);
  mmm[1][2] = mmm[2][1] =  posErr1(1,2) + posErr2(1,2);
  mmm[2][2] =   posErr1(2,2) + posErr2(2,2);
  //  mmm[0][0] =   posErr1.covValue(Trk::x,Trk::x) + posErr2.covValue(Trk::x,Trk::x);
  // mmm[0][1] = mmm[1][0] =  posErr1.covValue(Trk::x,Trk::y) + posErr2.covValue(Trk::x,Trk::y);
  // mmm[1][1] =   posErr1.covValue(Trk::y,Trk::y) + posErr2.covValue(Trk::y,Trk::y);
  // mmm[0][2] = mmm[2][0] =  posErr1.covValue(Trk::x,Trk::z) + posErr2.covValue(Trk::x,Trk::z);
  //mmm[1][2] = mmm[2][1] =  posErr1.covValue(Trk::y,Trk::z) + posErr2.covValue(Trk::y,Trk::z);
  //mmm[2][2] =   posErr1.covValue(Trk::z,Trk::z) + posErr2.covValue(Trk::z,Trk::z);
  long int jfail,NN=3;
  //  dsinv( NN, &mmm[0][0], NN, jfail);
  Trk::dsinv( &NN, &mmm[0][0], NN, &jfail);

  if(jfail==0){
    Signif = distx*mmm[0][0]*distx
      +disty*mmm[1][1]*disty
      +distz*mmm[2][2]*distz
      +2.*distx*mmm[0][1]*disty
      +2.*distx*mmm[0][2]*distz
      +2.*disty*mmm[1][2]*distz;
    Signif=sqrt(Signif);
  }
  
  return Signif;
}

void
RPVDispVrtVertexCleanup::setMinVrtDists(xAOD::VertexContainer* vertices) {
  
  for (unsigned int i=0; i< vertices->size(); ++i) {
    double minSig = 100000.;
    double minDist = 100000.;
    int minSigIndex=0;
    for (unsigned int j=0; j< vertices->size(); ++j) {
      if (i==j) continue;
      double sig = VrtVrtDist(vertices->at(i), vertices->at(j));
      if (sig < minSig) {
	minSig= sig;
	minSigIndex=j;
	//// calculate distance in mm
	double distx =  (vertices->at(i))->position().x() - 
	  (vertices->at(j))->position().x();
	double disty =  (vertices->at(i))->position().y() - 
	  (vertices->at(j))->position().y();
	double distz =  (vertices->at(i))->position().z() - 
	  (vertices->at(j))->position().z();
	minDist = pow(distx*distx + disty*disty + distz*distz, 0.5);
      }
    }
    (vertices->at(i))->auxdata<double>("distCloseVtx") = minDist;
    (vertices->at(i))->auxdata<int>("numCloseVtx") = minSigIndex;
  }
  
  return;
}


StatusCode
RPVDispVrtVertexCleanup::mergeAndRefitVertices(xAOD::VertexContainer* vertices, 
					       int & V1, int & V2,
					       const xAOD::TrackParticleContainer* trkColl) {

  
  int nth=(vertices->at(V2))->vxTrackAtVertex().size();   //number of tracks in second vertex
    
  std::vector<long int> v1Indices = vertices->at(V1)->auxdata<std::vector<long int> >("trackIndices");
  std::vector<long int> v2Indices = vertices->at(V2)->auxdata<std::vector<long int> >("trackIndices");
  
  for(long int i=0;i<nth;i++) {
    if (std::find(v1Indices.begin(),v1Indices.end(),v2Indices.at(i))==v1Indices.end()) {
      (vertices->at(V1))->vxTrackAtVertex().push_back(*(vertices->at(V2)->vxTrackAtVertex().at(i).clone()));
      (vertices->at(V1))->auxdata<std::vector<long int> >("trackIndices").push_back(v2Indices.at(i));
    }
  }
  std::sort( (vertices->at(V1))->auxdata<std::vector<long int> >("trackIndices").begin(),(vertices->at(V1))->auxdata<std::vector<long int> >("trackIndices").end() );

  vertices->at(V2)->auxdata<std::vector<long int> >("trackIndices").clear();
  vertices->at(V2)->auxdata<int>("numCloseVtx") = 0;
  vertices->at(V2)->auxdata<double>("distCloseVtx") = 1000000.;
  
  ///refit V1
  m_vtxTool->refitVertex(vertices->at(V1),trkColl);
  vertices->at(V1)->auxdata<int>("numCloseVtx") = 0;
  vertices->at(V1)->auxdata<double>("distCloseVtx") = 1000000.;
  
 return StatusCode::SUCCESS;
}

void
RPVDispVrtVertexCleanup::removeEmptyVertices(xAOD::VertexContainer* vertices) {
  
  xAOD::VertexContainer::iterator vrtIt = vertices->begin();
  int count = 0;
  while (vrtIt != vertices->end()) {
    if ((*vrtIt)->auxdata<std::vector<long int> >("trackIndices").size() < 2) {
      vrtIt = vertices->erase(vrtIt);
    } else {
      ++vrtIt;
    }
    count++;
  }
  
  return;
}

void
RPVDispVrtVertexCleanup::mergeFinalVertices(xAOD::VertexContainer* vertices,
					    const xAOD::TrackParticleContainer* trkColl)
{
 
  /* 
  if (m_mergeFinalVerticesDistance) {
    while(true){
      int foundV1(0), foundV2(0);
      float dist(9999999.);
      float foundMinVrtDst = minVrtVrtDist( vertices, foundV1, foundV2);
      if( foundMinVrtDst >  m_VertexMergeFinalDistCut ) break;
      Hep3Vector Location_found1 = vertices->at(foundV1)->recVertex().position();
      Hep3Vector Location_found2 = vertices->at(foundV2)->recVertex().position();        
      dist = (Location_found1 - Location_found2).mag();
      if( dist <  m_VertexMergeFinalDistCut) { 
	msg(MSG::DEBUG)<<"Merging FINAL vertices "<<foundV1<<" and "<<foundV2
		       <<" which are separated by distance"<<dist<<endreq;
	StatusCode sc = mergeAndRefitVertices(vertices, foundV1, foundV2, trkColl);
	if (sc.isFailure()) msg(MSG::ERROR)<<"Final merging vertex refit failed"<<endreq;
      }    
    } 
  }
  if (m_mergeFinalVerticesSignif) {
    while(true){
      int foundV1(0), foundV2(0);
      float sig(9999999.);
      float foundMinVrtSig = minVrtVrtSig( vertices, foundV1, foundV2);
      if( foundMinVrtSig >  m_VertexMergeFinalSigCut ) break;
      sig = VrtVrtDist( vertices->at(foundV1), vertices->at(foundV2) );
      if( sig <  m_VertexMergeFinalSigCut) { 
	msg(MSG::DEBUG)<<"Merging FINAL vertices "<<foundV1<<" and "<<foundV2
		       <<" which are separated by significance"<<sig<<endreq;
	StatusCode sc = mergeAndRefitVertices(vertices, foundV1, foundV2, trkColl);
	if (sc.isFailure()) msg(MSG::ERROR)<<"Final merging vertex refit failed"<<endreq;
      }    
    } 
  }
  */
  return;
}
