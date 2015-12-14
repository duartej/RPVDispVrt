#include "RPVDispVrt/RPVDispVrt.h"
#include "RPVDispVrt/RPVDispVrtTrackSelector.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "RPVDispVrt/RPVDispVrtVertexer.h"
#include "RPVDispVrt/RPVDispVrtVertexCleanup.h"
#include "RPVDispVrt/RPVDispVrtVertexSelector.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"

/// --------------------------------------------------------------------
/// Constructor

RPVDispVrt::RPVDispVrt(const std::string& name,
			     ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_trkTool("RPVDispVrtTrackSelector",this),
  m_vtxTool("RPVDispVrtVertexer",this),
  m_vtxClean("RPVDispVrtVertexCleanup",this),
  m_vtxSelector("RPVDispVrtVertexSelector",this)
{
  declareProperty("InputTrackCollection", m_trackCollName="InDetTrackParticles");
  declareProperty("InputPrimaryVertex", m_primVtxName="PrimaryVertices");
  declareProperty("OutputVertexCollection", m_secVtxName="RPVSecVertices");
  declareProperty("TrackSelectorTool",m_trkTool,"RPVDispVrtTrackSelector");
  declareProperty("VertexerTool",m_vtxTool,"RPVDispVrtVertexer");
  declareProperty("VertexCleanupTool",m_vtxClean,"RPVDispVrtVertexCleanup");
  declareProperty("VertexSelectorTool",m_vtxSelector,"RPVDispVrtVertexSelector");
  declareProperty("RandomizeTracks",m_randomizeTracks=false);
  declareProperty("UseAllPrimaryVertices",m_useAllPrimVtx=false);
  declareProperty("SelectForMaterialMap",m_materialMapOnly=false);
  declareProperty("SelectGT2Trk",m_gt2trkOnly=false);

}

/// --------------------------------------------------------------------
/// Initialize

StatusCode RPVDispVrt::initialize() {

  msg(MSG::DEBUG) << "RPVDispVrt::initialize" << endreq;

  /** Get Tools */
  if (m_trkTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved tool " << m_trkTool << endreq;

  if (m_vtxTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_vtxTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved tool " << m_vtxTool << endreq;
  
  if (m_vtxClean.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Can't find RPVDispVrtVertexCleanup" << endreq;
    return StatusCode::SUCCESS; 
  } else {
    msg(MSG::DEBUG) << "RPVDispVrtVertexCleanup found" << endreq;
  }

  //if (m_vtxSelector.retrieve().isFailure()) {
  //  msg(MSG::ERROR) << "Can't find RPVDispVrtVertexSelector" << endreq;
  //  return StatusCode::SUCCESS; 
  //} else {
  //  msg(MSG::DEBUG) << "RPVDispVrtVertexSelector found" << endreq;
  // }

  //if (m_doSecondPassVertex) {
  //  if (m_spvTool.retrieve().isFailure()) {
  //    msg(MSG::FATAL) << "Failed to retrieve tool " << m_spvTool << endreq;
  //   return StatusCode::FAILURE;
  //  } else
  //   msg(MSG::DEBUG) << "Retrieved tool " << m_spvTool << endreq;
  //  }


  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute

StatusCode RPVDispVrt::execute() {

  msg(MSG::DEBUG)<<"in RPVDispVrt::execute()"<<endreq;

  /// return StatusCode::SUCCESS;

  const xAOD::VertexContainer* primVertices(0);
  StatusCode sc = evtStore()->retrieve(primVertices,m_primVtxName);
  if (sc.isFailure()) 
    msg(MSG::ERROR)<<"Failed to retrieve Primary Vertex collection "<<m_primVtxName<<endreq;
  else msg(MSG::DEBUG)<<"retrieved Primary Vertex collection size "<<primVertices->size()<<endreq;  

  if (primVertices->size() == 0) {
    msg(MSG::WARNING)<<"Primary vertex not found, will skip this event"<<endreq;
    return StatusCode::SUCCESS;
  }
  const xAOD::Vertex* pVtx = primVertices->at(0);
  
  /// create output container
  
  xAOD::VertexContainer* secVertices = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer* secVerticesAux = new xAOD::VertexAuxContainer();
  secVertices->setStore(secVerticesAux);
  
  /// test - copied from tutorial
  /*  
  const xAOD::JetContainer* jets(0);
  sc = evtStore()->retrieve(jets,"AntiKt4LCTopoJets");
  if (sc.isFailure()) msg(MSG::ERROR)<<"Unable to retrieve jet collection."<<endreq;
  
  xAOD::JetContainer* goodJets = new xAOD::JetContainer; //creates a new jet container to hold the subset
  xAOD::JetAuxContainer* goodJetsAux = new xAOD::JetAuxContainer;
  
  goodJets->setStore( goodJetsAux ); //gives it a new associated aux container
  for(auto jet_itr=jets->begin() ; jet_itr != jets->end(); ++jet_itr ) {
    ATH_MSG_INFO( "  jet eta = " << (*jet_itr)->eta() );
    if(fabs((*jet_itr)->eta()) > 2.5) continue; //example jet selection
    
    xAOD::Jet* newJet = new xAOD::Jet; //create a new jet object
    // Create private auxstore for the object, copying all values from old jet
    newJet->makePrivateStore(**jet_itr);
    goodJets->push_back(newJet);
    newJet->auxdata<double>("myDouble") = 5.; //example decoration
    //myDecoration(*newJet) = 5.0;
    
  } // end for loop over jets
 
   //example record to storegate: you must record both the container and the auxcontainer
  CHECK( evtStore()->record(goodJets,"GoodJets") );
  CHECK( evtStore()->record(goodJetsAux,"GoodJetsAux.") ); 
  */ 
  
  /// retrieve TrackParticleContainer 
  const xAOD::TrackParticleContainer* trkColl(0);
  sc = evtStore()->retrieve(trkColl,m_trackCollName);
  if (sc.isFailure()) msg(MSG::ERROR)<<"Failed to retrieve TrackParticle collection"<<endreq;
  else msg(MSG::DEBUG)<<"retrieved TrackParticleContainer size "<<trkColl->size()<<endreq;

  //////////////////////////////////////////

  xAOD::TrackParticleContainer* selectedTrkColl = new xAOD::TrackParticleContainer();
  xAOD::TrackParticleAuxContainer* selectedTrkCollAux = new xAOD::TrackParticleAuxContainer();
  selectedTrkColl->setStore(selectedTrkCollAux);

  xAOD::TrackParticleContainer::const_iterator i_trk  = trkColl->begin();

  int trkCount = 0;
  std::vector<int> indices;
  indices.clear();

  for (; i_trk != trkColl->end() ; ++i_trk) {
    const xAOD::TrackParticle*  trk = (*i_trk);
    //    xAOD::TrackParticle*  trk = const_cast<xAOD::TrackParticle*>(*i_trk);
    if (m_useAllPrimVtx) {
      if (m_trkTool->decision(*trk,primVertices)) {
	msg(MSG::INFO)<<"        --> SelectedTrackIndex   "<<selectedTrkColl->size()
		      << ", TrackParticleIndex = " << std::distance(trkColl->begin(), i_trk) << endreq;
	indices.push_back(trkCount);
	xAOD::TrackParticle* newTrk = new xAOD::TrackParticle;
	newTrk->makePrivateStore(*trk);
	//	trk->auxdata<bool>("isDVselected")=true;
	selectedTrkColl->push_back(newTrk);
      } //else
	// {
	//	trk->auxdata<bool>("isDVselected")=false;
	// }
    } else {
      if (m_trkTool->decision(*trk,pVtx)) {
	msg(MSG::INFO)<<"        --> SelectedTrackIndex   "<<selectedTrkColl->size()
		      << ", TrackParticleIndex = " << std::distance(trkColl->begin(), i_trk) << endreq;
	////	trk->auxdata<bool>("isDVselected")=true;	
	indices.push_back(trkCount);
	xAOD::TrackParticle* newTrk = new xAOD::TrackParticle;
	newTrk->makePrivateStore(*trk);
	//	trk->auxdata<bool>("isDVselected")=true;
	selectedTrkColl->push_back(newTrk);
	//	selectedTrkColl->push_back(const_cast<xAOD::TrackParticle*>(trk));	
	//      selectedTrkColl->push_back(*trk);
	//      selectedTrkColl.push_back(trk);
      } //else 
	//{
	//trk->auxdata<bool>("isDVselected")=false;
	// }
    }
    trkCount++;
  }

  msg(MSG::INFO)<<"Size of selected track collection is "<<selectedTrkColl->size()<<endreq;

  sc = m_vtxTool->findVertices(secVertices,selectedTrkColl);
  if (sc.isFailure()) msg(MSG::ERROR)<<"Making vertices had some problem"<<endreq;
  else msg(MSG::INFO)<<"Found "<<secVertices->size()<<" vertices"<<endreq;
   
  sc = m_vtxClean->cleanupVertices(secVertices,selectedTrkColl);
  if (sc.isFailure()) msg(MSG::ERROR)<<"Cleanup vertices had some problem"<<endreq;

  msg(MSG::INFO)<<"Number of Secondary Vertices : "<<secVertices->size()<<endreq;

  for (unsigned int i=0; i<secVertices->size(); ++i) {
   
    AmgVector(3) vertexPos;
    vertexPos=secVertices->at(i)->position();

    msg(MSG::INFO)<<" vertexMassPionHypo = "<< secVertices->at(i)->auxdata<double>("massPionHypo")/1000
		  <<" GeV, (x, y, z) = ( "  <<vertexPos(0)<<", "<<vertexPos(1)<<", "<<vertexPos(2)<<" )"<<endreq;

    msg(MSG::INFO)<<"After cleanup trackIndices :"<< secVertices->at(i)->auxdata<std::vector<long int> >("trackIndices")
		  <<endreq;    
    //    msg(MSG::INFO)<<"After cleanup trackIndices :"<< secVertices->at(i)->nTrackParticles()
    //		  <<endreq;    
    
    //    for(int j=0;j<secVertices->at(i)->nTrackParticles();j++){
    //      std::cout << "hiotono Track  "<<secVertices->at(i)->auxdata< std::vector< Trk::VxTrackAtVertex > >("vxTrackAtVertex").at(j) << std::endl;
    //    }
  }

  //  for(int ii=0; ii<SelTrk.size() ; ii++) {
  //    msg(MSG::INFO)<<"track "<<SelTrk.at(ii)<<" : TrkAtVrt (Phi, Theta, 1/p) = ("
  //		  <<TrkAtVrt[ii][0]<<", "<<TrkAtVrt[ii][1]<<", "<<TrkAtVrt[ii][2]<<")"<<endreq;
  //  }
  //  msg(MSG::INFO)<<" vertexMassPionHypo = "<<newVertex->auxdata<double>("massPionHypo")/1000<<" GeV, (x, y, z) = ( "
  //		<<vertex(0)<<", "<<vertex(1)<<", "<<vertex(2)<<" )"<<endreq;

  /*
    if (m_materialMapOnly) secVertices = m_vtxSelector->selectForMaterialMap(secVertices);
  if (m_gt2trkOnly) secVertices = m_vtxSelector->selectGT2Trk(secVertices);
  */

  //-----End of post-processing-----//

  ///  sc = evtStore()->record(secVertices,m_secVtxName);
  sc = evtStore()->record(secVertices,"RPVSecVertices");
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record RPVVrt collection"<<endreq;
  } else {
    msg(MSG::INFO)<<"Recording RPVVrtContainer with "<<secVertices->size()<<" vertices"<<endreq;
  }
  sc = evtStore()->record(secVerticesAux,"RPVSecVerticesAux.");
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record RPVVrt auxcollection"<<endreq;
  }

  sc = evtStore()->record(selectedTrkColl,"RPVSelectedTracks");
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record RPVSelectedTracks"<<endreq;
  } 
  sc = evtStore()->record(selectedTrkCollAux,"RPVSelectedTracksAux.");
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record RPVSelectedTracksAux"<<endreq;
  } 
  return StatusCode::SUCCESS;
}

StatusCode RPVDispVrt::finalize() {

  return StatusCode::SUCCESS;
}

