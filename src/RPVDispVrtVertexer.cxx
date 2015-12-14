//Header include
#include "RPVDispVrt/RPVDispVrtVertexer.h" 
//----
#include  "TrkTrack/Track.h"
#include  "TrkTrackLink/ITrackLink.h"
#include  "TrkTrack/LinkToTrack.h"
#include  "TrkTrack/TrackCollection.h"
///#include  "TrkParameters/MeasuredPerigee.h"
//#include  "TrkEventPrimitives/ErrorMatrix.h"
#include  "xAODTracking/TrackParticleContainer.h"
#include  "xAODTracking/NeutralParticle.h"
#include  "VxVertex/VxTrackAtVertex.h"
//#include  "VxVertex/VxCandidate.h"
#include "xAODTracking/Vertex.h"
#include  "TTree.h"

namespace Trk {
  extern 
  int  pgraphm_(
                long int *weit, long int *edges, long int *nodes,
                long int *set, long int *nptr,  long int *nth);
}

extern "C" {
  float prob_(const float & Chi2, const long int& ndf); 
  void dsinv_(long int & , double *, long int &, long int &);                         
}


//constructor
RPVDispVrtVertexer::RPVDispVrtVertexer
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_vtxSel("RPVDispVrtVertexSelector"),
     m_fitSvc("Trk::TrkVKalVrtFitter",this)
  {
    declareInterface< RPVDispVrtVertexer  >( this );
    declareProperty("VertexSelectorTool",m_vtxSel,"RPVDispVrtVertexSelector");
    declareProperty("VertexFitterTool", m_fitSvc,"Trk::TrkVKalVrtFitter");
  }

RPVDispVrtVertexer::~RPVDispVrtVertexer()
{}




//__________________________________________________________________________
StatusCode RPVDispVrtVertexer::initialize() {
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  ///retrieve services and tools
   
  if (m_fitSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Can't find Trk::TrkVKalVrtFitter" << endreq;
    return StatusCode::SUCCESS; 
  } else {
    msg(MSG::DEBUG) << "Trk::TrkVKalVrtFitter found" << endreq;
  }

  if (m_vtxSel.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Can't find RPVDispVrtVertexSelector" << endreq;
    return StatusCode::SUCCESS; 
  } else {
    msg(MSG::DEBUG) << "RPVDispVrtVertexSelector found" << endreq;
  }
   return sc;
}



  //__________________________________________________________________________


StatusCode 
RPVDispVrtVertexer::finalize()
{
  msg(MSG::DEBUG) <<"RPVDispVrt finalize()" << endreq;

  return StatusCode::SUCCESS; 
}


//__________________________________________________________________________


StatusCode
RPVDispVrtVertexer::findVertices(xAOD::VertexContainer* vrts,
				 const xAOD::TrackParticleContainer* trackColl) {
  std::vector<int> usedTracks;
  usedTracks.clear();
  usedTracks = makeTwoTrackVertices(trackColl);
  msg(MSG::INFO)<<usedTracks.size()/2 <<" combinations among "<< trackColl->size() <<" selected tracks are incompatible"<<endreq;
  
  StatusCode sc = makeNTrackVertices(vrts,trackColl, usedTracks);
  if (sc.isFailure())  msg(MSG::DEBUG)<<"pgraphm made zero vertex" <<endreq;
  //  else
  //    msg(MSG::DEBUG)<<"pgraphm made" << vrts->size() << "vertex" <<endreq;
  return StatusCode::SUCCESS;
}


std::vector<int>
RPVDispVrtVertexer::makeTwoTrackVertices(const xAOD::TrackParticleContainer* trackColl) {
  std::vector<int> usedTrackIndices;
  usedTrackIndices.clear();
  std::vector<const xAOD::TrackParticle*> twoTrackCombo;
  std::vector<const xAOD::NeutralParticle*> dummyNeutralList;
  AmgVector(3) FitVertex;
  TLorentzVector Momentum;
  vector< vector<double> > TrkAtVrt; 
  vector<double> Chi2PerTrk,ErrorMatrix;
  long int           Charge;
  double             Chi2 = 0.;

  //  m_fitSvc->setDefault();
  //  Hep3Vector           FitVertex;
  /// HepLorentzVector     Momentum;

  int NTracks = trackColl->size();
  for (int i=0; i < NTracks-1; i++) {
    for (int j=i+1; j < NTracks; j++) {
      twoTrackCombo.clear();
      usedTrackIndices.push_back(i); usedTrackIndices.push_back(j);
      twoTrackCombo.push_back( (*trackColl)[i] ); twoTrackCombo.push_back( (*trackColl)[j] );
      AmgVector(3) IniVertex;

      StatusCode sc= m_fitSvc->VKalVrtFitFast(twoTrackCombo,IniVertex); /// Fast crude estimation 
      if( sc.isFailure() )continue;
      m_fitSvc->setApproximateVertex(IniVertex.x(), IniVertex.y(),IniVertex.z());
      ///______________end of Fast vertex fitting _______________////
      sc = m_fitSvc->VKalVrtFit(twoTrackCombo,dummyNeutralList,FitVertex, Momentum,Charge,
				ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
      if( sc.isFailure() ) continue; /// no fit

      if (!(m_vtxSel->goodTwoTrackVertex(twoTrackCombo, FitVertex, Momentum, Chi2))){
	msg(MSG::INFO) <<"    --> Incompatible two tracks" <<usedTrackIndices.size()/2-1<<" [ "<<i<<", "<<j<<" ]"<<endreq;
	continue;
      }
      usedTrackIndices.pop_back();
      usedTrackIndices.pop_back();
    }
  }  
  return usedTrackIndices;
}


StatusCode 
RPVDispVrtVertexer::makeNTrackVertices(xAOD::VertexContainer* vrts,
				       const xAOD::TrackParticleContainer* trkColl,
				       std::vector<int>& usedTrackIndices) {
  std::vector<const xAOD::NeutralParticle*> dummyNeutralList;
  long int NTracks = trkColl->size();
  long int* weit     = new long int[usedTrackIndices.size()];
  long int* Solution = new long int[NTracks];

  for(int i=0; i<(int)usedTrackIndices.size(); i++) *(weit+i)=(long int) (usedTrackIndices[i]+1); // +1 is needed for PGRAPH

  long int edges = usedTrackIndices.size()/2; 
  long int NPTR=0, nth=2; 
  vector<const xAOD::TrackParticle*>  GraphBaseTracks; 
  m_fitSvc->setDefault();
  vector<const xAOD::TrackParticle*>  ListBaseTracks;

  //  msg(MSG::INFO)<<"NICK before pgraphm loop "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;

  while(true) {
    // msg(MSG::INFO)<<"NICK about to call pgraphm "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;
    Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);
    // msg(MSG::INFO)<<"NICK just called pgraphm "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;
      if(NPTR <= 0)  break;      // No more solutions   
      if(NPTR == 1)  continue;   // Not a good solution 
      ListBaseTracks.clear();
      vector<long int> SelTrk;
      SelTrk.clear();
      for(int i=0;i<NPTR;i++) {
	SelTrk.push_back(Solution[i]-1);
	ListBaseTracks.push_back( trkColl->at(Solution[i]-1) );
      }
      msg(MSG::INFO)<<"pgraphm makes tracks : "<<SelTrk<<endreq;
      AmgVector(3)     vertex;
      TLorentzVector  vertexMom;
      long int Charge;
      std::vector<double> vertexCov;
      std::vector<double> Chi2PerTrk;
      std::vector< vector<double> > TrkAtVrt; 
      double Chi2;
      vertexCov.clear();
      TrkAtVrt.clear();
      ///// do the fit!!!////	
      StatusCode sc = m_fitSvc->VKalVrtFit(ListBaseTracks,
					   dummyNeutralList,
					   vertex,
					   vertexMom,
					   Charge,
					   vertexCov,
					   Chi2PerTrk, 
					   TrkAtVrt,
					   Chi2);
      msg(MSG::INFO)<<"After Ntrack Fit"<<endreq;
      msg(MSG::INFO)<<TrkAtVrt<<endreq;
      if (sc.isFailure()) { msg(MSG::INFO)<<"Vertex fit failed"<<endreq;	continue; }
      long int Ndf = m_fitSvc->VKalGetNDOF();
      if (Ndf == 0){msg(MSG::INFO)<<"Ndf == 0"<<endreq; continue;}
      /// make a vertex - this is quite messy so put into separate function!
      xAOD::Vertex* newVertex = new xAOD::Vertex();
      vrts->push_back(newVertex);
      addVertexInfo(newVertex,
		    trkColl,
		    ListBaseTracks,
		    vertex,
		    vertexMom,
		    Charge,
		    vertexCov,
		    Chi2PerTrk, 
		    TrkAtVrt,
		    SelTrk);   

      //      for(unsigned int ii=0; ii<SelTrk.size() ; ii++) {
      //	msg(MSG::INFO)<<"track "<<SelTrk.at(ii)<<" : TrkAtVrt (Phi, Theta, 1/p) = ("
      //		      <<TrkAtVrt[ii][0]<<", "<<TrkAtVrt[ii][1]<<", "<<TrkAtVrt[ii][2]<<")"<<endreq;
      //      }
      //      msg(MSG::INFO)<<" vertexMassPionHypo = "<<newVertex->auxdata<double>("massPionHypo")/1000<<" GeV, (x, y, z) = ( "
      //		    <<vertex(0)<<", "<<vertex(1)<<", "<<vertex(2)<<" )"<<endreq;      
      //      msg(MSG::INFO)<<"    --->  put vertex into container"<<endreq;
  }
  //  msg(MSG::INFO)<<"number of vertices "<<vrts->size()<<endreq;
  return StatusCode::SUCCESS;
}



xAOD::Vertex*
RPVDispVrtVertexer::makeVertex(std::vector<const xAOD::TrackParticle*>  ListBaseTracks,
			       std::vector<long int> SelTrk,
			       const xAOD::TrackParticleContainer* trkColl ) {

  std::vector<const xAOD::NeutralParticle*> dummyNeutralList;
  AmgVector(3) IniVertex;
  StatusCode sc= m_fitSvc->VKalVrtFitFast(ListBaseTracks,IniVertex);// Fast crude estimation 
  if( sc.isFailure() ) return 0;
  m_fitSvc->setApproximateVertex(IniVertex.x(), IniVertex.y(),IniVertex.z());
  AmgVector(3) vertex;
  TLorentzVector  vertexMom;
  long int Charge;
  std::vector<double> vertexCov;
  std::vector<double> Chi2PerTrk;
  std::vector< vector<double> > TrkAtVrt; 
  double Chi2;
  vertexCov.clear();
  TrkAtVrt.clear();
  ///// do the fit!!!////	
  sc = m_fitSvc->VKalVrtFit(ListBaseTracks,
			    dummyNeutralList,
			    vertex,
			    vertexMom,
			    Charge,
			    vertexCov,
			    Chi2PerTrk, 
			    TrkAtVrt,
			    Chi2);
  if (sc.isFailure()) {
    msg(MSG::DEBUG)<<"Vertex fit failed"<<endreq;
    return 0;
  }   

  long int Ndf = m_fitSvc->VKalGetNDOF();
  if (Ndf ==0) return 0;
  xAOD::Vertex* newVertex = new xAOD::Vertex();
  addVertexInfo(newVertex,
		trkColl,
		ListBaseTracks,
		vertex,
		vertexMom,
		Charge,
		vertexCov,
		Chi2PerTrk, 
		TrkAtVrt,
		SelTrk);
  return newVertex;
}

//xAOD::Vertex*
void
RPVDispVrtVertexer::addVertexInfo(xAOD::Vertex* newVertex,
				  const xAOD::TrackParticleContainer* trkColl,
				  vector<const xAOD::TrackParticle*>  &ListBaseTracks,
				  AmgVector(3) vertexPos,
				  TLorentzVector  vertexMom,
				  long int Charge,
				  std::vector<double> vertexCov,
				  std::vector<double> Chi2PerTrk,
				  std::vector< vector<double> > TrkAtVrt,
				  vector<long int> SelTrk) {  
  int NTrk = ListBaseTracks.size();  
  newVertex->setPosition( vertexPos );
  const Amg::Vector3D& gp(vertexPos);
  AmgSymMatrix(3) CovMtxV; 
  AmgSymMatrix(5) CovMtxP;
  CovMtxV = FillMatrix3x3(vertexCov);
  // msg(MSG::INFO)<<" CovMtxV has (0,0) "<<CovMtxV(0,0)<<" vertexCov(0) is "<< vertexCov[0]<<endreq;
  ////const Trk::RecVertex* tmpRecV = new Trk::RecVertex( vertex, CovMtxV, Ndf, Chi2 );
  
  std::vector<Trk::VxTrackAtVertex>* tmpVTAV = new std::vector<Trk::VxTrackAtVertex>();
  std::vector<double> CovFull;
  ///TEMPORARILY COMMENT THIS OUT - WE NEVER SEEM TO GET FULL COV MATRIX ANYWAY,
  ///AND THIS FUNCTION CRASHES WHEN RE-TRACKING IN THE SAME JOB
  StatusCode sc = m_fitSvc->VKalGetFullCov( (long int) NTrk, CovFull); 
  if (sc.isFailure()) msg(MSG::DEBUG)<<"Failed to get full covariance matrix"<<endreq;
  int covarExist=0; 
  if( sc.isSuccess() ) covarExist=1;
  for( int ii=0; ii<NTrk ; ii++) {
    if(covarExist){ CovMtxP = FillMatrix5x5( CovFull , ii);}
    else          { FillUnitMatrix5x5(CovMtxP);}
    Trk::PerigeeSurface surface(gp);
    Trk::Perigee* tmpMeasPer  =  new Trk::Perigee( 0.,0.,
						   TrkAtVrt[ii][0], /// Phi
						   TrkAtVrt[ii][1], /// Theta
						   TrkAtVrt[ii][2], /// 1/p ///
						   surface, /// Trk::PerigeeSurface(gp),
						   &CovMtxP);
    Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( Chi2PerTrk[ii], tmpMeasPer ) ;
    tmpVTAV->push_back(*tmpPointer);
    
    ElementLink<xAOD::TrackParticleContainer>  TEL(*trkColl, ListBaseTracks[ii]->index());   
    newVertex->addTrackAtVertex(TEL,1.);
  }

  newVertex->setCovariancePosition(CovMtxV);  
  //  newVertex->auxdata< std::vector< Trk::VxTrackAtVertex > >("vxTrackAtVertex") = *tmpVTAV; // need to add later??

  double vertexMassZeroHypo = calcMass(tmpVTAV,0.);
  double vertexMassPionHypo = calcMass(tmpVTAV,139.4);

  newVertex->auxdata<float>("mass") = vertexMassZeroHypo;
  newVertex->auxdata<double>("massPionHypo") = vertexMassPionHypo;
  newVertex->auxdata<int>("vtxCharge") = (int)Charge;
  newVertex->auxdata<std::vector<long int> >("trackIndices") = SelTrk;
  //  newVertex->auxdata<TLorentzVector>("vertexP4") = vertexMom;          //  need to add later !!
  newVertex->auxdata<float>("vertex_x") = vertexMom.X();        
  newVertex->auxdata<float>("vertex_y") = vertexMom.Y();        
  newVertex->auxdata<float>("vertex_z") = vertexMom.Z();        
  newVertex->auxdata<float>("vertex_m") = vertexMom.M();        
  newVertex->auxdata<std::vector<double> >("chi2ForTrack") = Chi2PerTrk;
  
  //  std::cout << "hiotono DVmass "<<newVertex->auxdata<double>("massPionHypo")/1000 << std::endl;
  //  std::cout << "hiotono nTrack "<<newVertex->nTrackParticles() << std::endl;

  //  for(int i=0;i<newVertex->nTrackParticles();i++){
  //    std::cout << "hiotono Track  "<<newVertex->auxdata< std::vector< Trk::VxTrackAtVertex > >("vxTrackAtVertex").at(i) << std::endl;
  //  }

  return;
}

double 
//RPVDispVrtVertexer::calcMass(std::vector<Trk::VxTrackAtVertex* >* vtxTracks, double trkMass) 
RPVDispVrtVertexer::calcMass(std::vector<Trk::VxTrackAtVertex >* vtxTracks, double trkMass) 
{
  TLorentzVector vtxP4(0,0,0,0);
  for (unsigned int i=0; i< vtxTracks->size(); ++i) 
    {
      //      const Trk::TrackParameters* perigeeParms = vtxTracks->at(i)->perigeeAtVertex();
      const Trk::TrackParameters* perigeeParms = vtxTracks->at(i).perigeeAtVertex();
      // double phi = perigeeParms->parameters()[Trk::phi0];
      // double theta = perigeeParms->parameters()[Trk::theta];
      // double qOverP = perigeeParms->parameters()[Trk::qOverP];
      // double magP= fabs(1.0/qOverP);
      
      AmgVector(3) trkP = perigeeParms->momentum();
      TLorentzVector trkP4(trkP(0), trkP(1), trkP(2), sqrt(trkMass*trkMass+trkP(0)*trkP(0)+trkP(1)*trkP(1)+trkP(2)*trkP(2) ) );
      vtxP4+=trkP4;
    }
  return vtxP4.M();
}



void
RPVDispVrtVertexer::refitVertex(xAOD::Vertex* vertex,const xAOD::TrackParticleContainer* trkColl) {
  
  std::vector<const xAOD::NeutralParticle*> dummyNeutralList;
  m_fitSvc->setDefault();
  AmgVector(3) vtx(vertex->position().x(),
		   vertex->position().y(),
		   vertex->position().z());
  //  TLorentzVector  vtxMom = vertex->auxdata<TLorentzVector>("vertexP4"); // need to add later !!
  TLorentzVector  vtxMom; 
  vtxMom.SetXYZM( vertex->auxdata<float>("vertex_x") ,
		  vertex->auxdata<float>("vertex_y") ,
		  vertex->auxdata<float>("vertex_z") ,
		  vertex->auxdata<float>("vertex_m") ); 

  //!  long int Charge = vertex->auxdata<int>("charge");
  long int Charge;
  std::vector<double> vertexCov;
  std::vector<double> Chi2PerTrk;
  std::vector< vector<double> > TrkAtVrt; 
  double Chi2(vertex->chiSquared());
  Chi2PerTrk.clear();
  vertexCov.clear();
  TrkAtVrt.clear();
  m_fitSvc->setApproximateVertex(vertex->position().x(),
				 vertex->position().y(),
				 vertex->position().z());

  std::vector<const xAOD::TrackParticle*>  ListTracks;
  ListTracks.clear();
  std::vector<long int> trkIndices(vertex->auxdata<std::vector<long int> >("trackIndices"));
  for (unsigned int i=0; i<trkIndices.size(); ++i) {
    ListTracks.push_back(trkColl->at(trkIndices.at(i)));
  }
  //delete vertex;




  StatusCode sc = m_fitSvc->VKalVrtFit(ListTracks,
				       dummyNeutralList,
				       vtx,
				       vtxMom,
				       Charge,
				       vertexCov,
				       Chi2PerTrk, 
				       TrkAtVrt,
				       Chi2);

  msg(MSG::INFO)<<"After Refit"<<endreq;
  msg(MSG::INFO)<<TrkAtVrt<<endreq;


  if (sc.isFailure()) 
    msg(MSG::DEBUG)<<"Vertex refit failed"<<endreq;

  long int Ndf = m_fitSvc->VKalGetNDOF();
  if (Ndf ==0) return;
  /// make a new RPVVrt - this is quite messy so put into separate function!
  /*
  xAOD::Vertex* newvertex = makeVertex(ListTracks,
				       vtx,
				       vtxMom,
				       Charge,
				       vertexCov,
				       Chi2PerTrk, 
				       TrkAtVrt,
				       Chi2,
				       Ndf,
				       *trkIndices);
				       */
  addVertexInfo(vertex,
		trkColl,
		ListTracks,
		vtx,
		vtxMom,
		Charge,
		vertexCov,
		Chi2PerTrk, 
		TrkAtVrt,
		trkIndices);
  return;
}




AmgSymMatrix(3)
RPVDispVrtVertexer::FillMatrix3x3(std::vector<double>& inputVec, int iTrk)
{

  AmgSymMatrix(3) CovMtx;
  /// Fill 3x3 matrix with vertex error
  if (iTrk == -1) {
    
    if (inputVec.size() < 6) {
      msg(MSG::ERROR)<<"Tried to make a covariance matrix from a vector with not enough elements"<<endreq;
      FillUnitMatrix3x3(CovMtx);
      return CovMtx;
    }   
    CovMtx(0,0)= inputVec[0];
    CovMtx(1,0)= inputVec[1]; CovMtx(0,1)= inputVec[1]; CovMtx(1,1)= inputVec[2];
    CovMtx(2,0)= inputVec[3]; CovMtx(0,2)= inputVec[3]; 
    CovMtx(2,1)= inputVec[4]; CovMtx(1,2)= inputVec[4]; CovMtx(2,2)= inputVec[5];
  }
  return CovMtx;
}


AmgSymMatrix(5)
RPVDispVrtVertexer::FillMatrix5x5(std::vector<double>& inputVec, int iTrk) {
  
  AmgSymMatrix(5) CovMtx;
  if (iTrk == -1) {
      
    /// fill 5x5 matrix with track covariance
    if (inputVec.size() < 21) {
      msg(MSG::ERROR)<<"Tried to make a 5x5 covariance matrix from a vector with not enough elements"<<endreq;
      FillUnitMatrix5x5(CovMtx);
      return CovMtx;
    }
    CovMtx(0,0) =  0;
    CovMtx(1,1) =  0;
    CovMtx(2,2)= inputVec[ 9];
    CovMtx(2,3)= inputVec[13]; 
    CovMtx(3,3)= inputVec[14];
    CovMtx(2,4)= inputVec[18]; 
    CovMtx(3,4)= inputVec[19]; 
    CovMtx(4,4)= inputVec[20];
  } else {
    int iTmp=(iTrk+1)*3;
    int NContent = inputVec.size();
    FillUnitMatrix5x5(CovMtx);        //Clean matrix for the beginning, then fill needed elements
    CovMtx(0,0) =  0;
    CovMtx(1,1) =  0;
    int pnt = (iTmp+1)*iTmp/2 + iTmp;   if( pnt   > NContent ) return CovMtx;
    CovMtx(2,2) =  inputVec[pnt];
    pnt = (iTmp+1+1)*(iTmp+1)/2 + iTmp; if( pnt+1 > NContent ){ FillUnitMatrix5x5(CovMtx);  return CovMtx; }
    CovMtx(2,3) =  inputVec[pnt];
    CovMtx(3,3) =  inputVec[pnt+1];
    pnt = (iTmp+2+1)*(iTmp+2)/2 + iTmp; if( pnt+2 > NContent ){ FillUnitMatrix5x5(CovMtx);  return CovMtx; }
    CovMtx(2,4) = inputVec[pnt];
    CovMtx(3,4) = inputVec[pnt+1];
    CovMtx(4,4) = inputVec[pnt+2];
  }
  return CovMtx; 
}

void
RPVDispVrtVertexer::FillUnitMatrix3x3(AmgSymMatrix(3)& CovMtx) {
  for (int i=1; i<= CovMtx.rows(); i++) {
    for (int j=1; j<= CovMtx.rows(); j++) {
      if (i==j) CovMtx(i,j)=1.;
      else CovMtx(i,j) = 0.;
    }
  }
  return;
}

void
RPVDispVrtVertexer::FillUnitMatrix5x5(AmgSymMatrix(5)& CovMtx) {
  for (int i=1; i<= CovMtx.rows(); i++) {
    for (int j=1; j<= CovMtx.rows(); j++) {
      if (i==j) CovMtx(i,j)=1.;
      else CovMtx(i,j) = 0.;
    }
  }
  return;
}
