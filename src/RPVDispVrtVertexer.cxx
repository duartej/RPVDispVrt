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
  msg(MSG::INFO)<<"Number of two-track vertices is "<<usedTracks.size()<<endreq;
  
 
  
  StatusCode sc = makeNTrackVertices(vrts,trackColl, usedTracks);
  if (sc.isFailure()) 
    msg(MSG::ERROR)<<"Making N track vertices failed"<<endreq;
  else
    msg(MSG::DEBUG)<<"Number of N track vertices found is "
		  <<vrts->size()<<endreq;
  

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


      if (!(m_vtxSel->goodTwoTrackVertex(twoTrackCombo, FitVertex, Momentum, Chi2))) continue;

      usedTrackIndices.pop_back();
      usedTrackIndices.pop_back();
      
    }
  }
  
  msg(MSG::DEBUG)<<"Number of two-track vertices is "<<usedTrackIndices.size()<<endreq;

  return usedTrackIndices;
}




StatusCode 
RPVDispVrtVertexer::makeNTrackVertices(xAOD::VertexContainer* vrts,
				       const xAOD::TrackParticleContainer* trkColl,
				       std::vector<int>& usedTrackIndices) {

  msg(MSG::INFO)<<"NICK here in makeNTrackVertices"<<endreq;
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

  msg(MSG::INFO)<<"NICK before pgraphm loop "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;

  while(true) {
    msg(MSG::INFO)<<"NICK about to call pgraphm "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;
    Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);
    msg(MSG::INFO)<<"NICK just called pgraphm "<<(*weit)<<" "<<edges<<" "<<NTracks<<" "<<(*Solution)<<" "<<NPTR<<" "<<nth<<endreq;
      if(NPTR <= 0)  break;      // No more solutions   
      if(NPTR == 1)  continue;   // Not a good solution 
      ListBaseTracks.clear();
      vector<long int> SelTrk;
      SelTrk.clear();
      for(int i=0;i<NPTR;i++) {
	msg(MSG::INFO)<<"nick pushing back "<<Solution[i]-1<<endreq;
	SelTrk.push_back(Solution[i]-1);
	ListBaseTracks.push_back( trkColl->at(Solution[i]-1) );
      }
      bool Good(true); 

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
      msg(MSG::INFO)<<" nick about to call fitsvc"<<endreq;
      StatusCode sc = m_fitSvc->VKalVrtFit(ListBaseTracks,
					   dummyNeutralList,
					   vertex,
					   vertexMom,
					   Charge,
					   vertexCov,
					   Chi2PerTrk, 
					   TrkAtVrt,
					   Chi2);
      if (sc.isFailure()) {
	msg(MSG::INFO)<<"Vertex fit failed"<<endreq;
	continue;
      }
      msg(MSG::INFO)<<" nick about to get nDOF"<<endreq;
      long int Ndf = m_fitSvc->VKalGetNDOF();
      if (Ndf ==0) continue;
      /// make a vertex - this is quite messy so put into separate function!


      xAOD::Vertex* newVertex = new xAOD::Vertex();
      msg(MSG::INFO)<<" nick made a vertex"<<endreq;
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
		    Chi2,
		    Ndf,
		    SelTrk);
      

      msg(MSG::INFO)<<" nick put vertex into container"<<endreq;

  }



  msg(MSG::INFO)<<"number of vertices "<<vrts->size()<<endreq;
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
		Chi2,
		Ndf,
		SelTrk);

  
  
  return newVertex;
}









//xAOD::Vertex*
void
RPVDispVrtVertexer::addVertexInfo(xAOD::Vertex* newVertex,
				  const xAOD::TrackParticleContainer* trkColl,
				  vector<const xAOD::TrackParticle*>  ListBaseTracks,
				  AmgVector(3) vertexPos,
				  TLorentzVector  vertexMom,
				  long int Charge,
				  std::vector<double> vertexCov,
				  std::vector<double> Chi2PerTrk,
				  std::vector< vector<double> > TrkAtVrt,
				  double Chi2,
				  double Ndf,
				  vector<long int> SelTrk) {
  
  msg(MSG::INFO)<<" nick in addVertexInfo "<<endreq;
  newVertex->setPosition( vertexPos );

  int NTrk = ListBaseTracks.size();  
  const Amg::Vector3D& gp(vertexPos);

  AmgSymMatrix(3) CovMtxV; 
  AmgSymMatrix(5) CovMtxP;
  
  CovMtxV = FillMatrix3x3(vertexCov);
  msg(MSG::INFO)<<" nick in makeVertex p1"<<endreq;
  msg(MSG::INFO)<<" CovMtxV has (0,0) "<<CovMtxV(0,0)<<" vertexCov(0) is "<< vertexCov[0]<<endreq;
  
  ////const Trk::RecVertex* tmpRecV = new Trk::RecVertex( vertex, CovMtxV, Ndf, Chi2 );
  
  std::vector<Trk::VxTrackAtVertex*>* tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
  std::vector<double> CovFull;
  msg(MSG::INFO)<<" nick in makeVertex p2"<<endreq;
  ///TEMPORARILY COMMENT THIS OUT - WE NEVER SEEM TO GET FULL COV MATRIX ANYWAY,
  ///AND THIS FUNCTION CRASHES WHEN RE-TRACKING IN THE SAME JOB
  StatusCode sc = m_fitSvc->VKalGetFullCov( (long int) NTrk, CovFull); 
  if (sc.isFailure()) msg(MSG::DEBUG)<<"Failed to get full covariance matrix"<<endreq;
  int covarExist=0; ///
  if( sc.isSuccess() ) covarExist=1;
  for(int ii=0; ii<NTrk ; ii++) {
    if(covarExist){ CovMtxP = FillMatrix5x5( CovFull , ii);}
    else          { FillUnitMatrix5x5(CovMtxP);}
    msg(MSG::INFO)<<" nick trkatvrt has "<<TrkAtVrt[ii][0]<<" "<<TrkAtVrt[ii][1]<<" "<<TrkAtVrt[ii][2]<<endreq;
    msg(MSG::INFO)<<" nick originaltrackparticle has "<<ListBaseTracks.at(ii)->p4().Px()<<" "<<ListBaseTracks.at(ii)->p4().Py()<<" "<<ListBaseTracks.at(ii)->p4().Pz()<<endreq;
    
    Trk::PerigeeSurface surface(gp);

    Trk::Perigee* tmpMeasPer  =  new Trk::Perigee( 0.,0.,
						   TrkAtVrt[ii][0], /// Phi
						   TrkAtVrt[ii][1], /// Theta
						   TrkAtVrt[ii][2], /// 1/p ///
						   surface, /// Trk::PerigeeSurface(gp),
						   &CovMtxP);
     

 
    Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( Chi2PerTrk[ii], tmpMeasPer ) ;
    tmpVTAV->push_back(tmpPointer);
    
    
    ElementLink<xAOD::TrackParticleContainer>  TEL(*trkColl, ListBaseTracks[ii]->index());   
    //    TEL.setElement(ListBaseTracks[ii]);
    newVertex->addTrackAtVertex(TEL,1.);
    //TEL.setStorableObject(*trkColl);

    /*
    Trk::LinkToTrack *ITL = new Trk::LinkToTrack(TEL);    //pointer to initial Trk
     //             ITL->setStorableObject( *trackTES );
    tmpPointer->setOrigTrack(ITL);                       //pointer to initial Trk

    */


    // Trk::TrackParameters* tmpMeasPer = new Trk::TrackParameters(0.,0., TrkAtVrt[ii][0],
    //								TrkAtVrt[ii][1],
    //TrkAtVrt[ii][2]);



    /*
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, const TrackParameters* initialPerigee);

    Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0., TrkAtVrt[ii][0],
						    TrkAtVrt[ii][1],
						    TrkAtVrt[ii][2],
						    gp,
						    CovMtxP );
    Trk::VxTrackAtVertex* trkatvrt = new Trk::VxTrackAtVertex( Chi2PerTrk[ii], tmpMeasPer );
    ///    ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( (xAOD::TrackParticle*) ListBaseTracks[ii]);
    ///  Trk::LinkToTrackParticle * ITL = new Trk::LinkToTrackParticle(TEL); //pointer to initial Trk 
    /// trkatvrt->setOrigTrack(ITL);
    tmpVTAV->push_back(trkatvrt);
    */
    msg(MSG::INFO)<<" nick in makeVertex p3"<<endreq;
  } 
  newVertex->setCovariancePosition(CovMtxV);
  
  // newVertex->auxdata<std::vector< Trk::VxTrackAtVertex* >* >("VxTrackAtVertex") = tmpVTAV;

  double vertexMassZeroHypo = calcMass(tmpVTAV,0.);
  double vertexMassPionHypo = calcMass(tmpVTAV,139.4);
  msg(MSG::INFO)<<"NIICK masses "<<vertexMassZeroHypo<< " "<<vertexMassPionHypo<<" "<<vertexMom.M()<<endreq;

  newVertex->auxdata<double>("mass") = vertexMassZeroHypo;
  newVertex->auxdata<double>("massPionHypo") = vertexMassPionHypo;
  newVertex->auxdata<int>("vtxCharge") = (int)Charge;
  newVertex->auxdata<std::vector<long int> >("trackIndices") = SelTrk;
  //  newVertex->auxdata<TLorentzVector>("vertexP4") = vertexMom;
  newVertex->auxdata<std::vector<double> >("chi2ForTrack") = Chi2PerTrk;
  
  return;

}

double 
RPVDispVrtVertexer::calcMass(std::vector<Trk::VxTrackAtVertex* >* vtxTracks, double trkMass) 
{
  TLorentzVector vtxP4(0,0,0,0);
  for (unsigned int i=0; i< vtxTracks->size(); ++i) 
    {
      const Trk::TrackParameters* perigeeParms = vtxTracks->at(i)->perigeeAtVertex();
      //   double phi = perigeeParms->parameters()[Trk::phi0];
      // double theta = perigeeParms->parameters()[Trk::theta];
      double qOverP = perigeeParms->parameters()[Trk::qOverP];
      double magP= fabs(1.0/qOverP);
      
      AmgVector(3) trkP = perigeeParms->momentum();
      std::cout<<" looook "<<magP<<" "<<sqrt(trkP(0)*trkP(0)+trkP(1)*trkP(1)+trkP(2)*trkP(2))<<std::endl;
      
      TLorentzVector trkP4(trkP(0), trkP(1), trkP(2), sqrt(trkMass*trkMass+trkP(0)*trkP(0)+trkP(1)*trkP(1)+trkP(2)*trkP(2) ) );
      std::cout<<" looook2 "<<trkP(0)<<" "<<trkP4.Px()<<std::endl;
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
  TLorentzVector  vtxMom = vertex->auxdata<TLorentzVector>("vertexP4");
  long int Charge = vertex->auxdata<int>("charge");
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
		Chi2,
		Ndf,
		trkIndices);
  return;
}




AmgSymMatrix(3)
RPVDispVrtVertexer::FillMatrix3x3(std::vector<double>& inputVec, int iTrk)
{

  AmgSymMatrix(3) CovMtx;
    /// Fill 3x3 matrix with vertex error
  if (inputVec.size() < 6) {
    msg(MSG::ERROR)<<"Tried to make a covariance matrix from a vector with not enough elements"<<endreq;
    FillUnitMatrix3x3(CovMtx);
    return CovMtx;
  }
  
  
  CovMtx(0,0)= inputVec[0];
  CovMtx(1,0)= inputVec[1]; CovMtx(0,1)= inputVec[1]; CovMtx(1,1)= inputVec[2];
  CovMtx(2,0)= inputVec[3]; CovMtx(0,2)= inputVec[3]; 
  CovMtx(2,1)= inputVec[4]; CovMtx(1,2)= inputVec[4]; CovMtx(2,2)= inputVec[5];
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
