#include "RPVDispVrt/RPVDispVrtTrackSelector.h"
#include "xAODTracking/TrackParticle.h"
///#include "TrkEventPrimitives/ErrorMatrix.h"
#include <iostream>

//constructor
RPVDispVrtTrackSelector::RPVDispVrtTrackSelector
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_trackToVertexTool("Reco::TrackToVertex")
{    

  declareInterface< RPVDispVrtTrackSelector  >( this );
  declareProperty("TrkChi2Cut",         m_TrkChi2Cut=5.);
  declareProperty("TrkPtCut",           m_TrkPtCut=500.); 
  declareProperty("CutSctHits",         m_CutSctHits=2  );
  declareProperty("CutPixelHits",       m_CutPixelHits=0  );
  declareProperty("CutSiliconHits",     m_CutSiliconHits=0  );
  declareProperty("CutPixelHoles",      m_CutPixelHoles=4  );
  declareProperty("CutBLayHits",        m_CutBLayHits=0  );
  declareProperty("CutSharedHits",      m_CutSharedHits=0 );
  declareProperty("DstFromBeamCut",     m_DstFromBeamCut=2.);
  declareProperty("RandomlyKillTracksFracLowEta",m_killTrackFracLowEta=0.);
  declareProperty("RandomlyKillTracksFracHighEta",m_killTrackFracHighEta=0.);
  declareProperty("CutTrkQuality",m_TrkQualityCut=false);
  declareProperty("CutTRT",m_TRTCut=false);
  declareProperty("TRTPixCut",m_TRTPixCut=false);

}

//destructor  
RPVDispVrtTrackSelector::~RPVDispVrtTrackSelector() {
}

StatusCode 
RPVDispVrtTrackSelector::initialize() {

  StatusCode sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode 
RPVDispVrtTrackSelector::finalize() {
 

  return StatusCode::SUCCESS;
}

bool RPVDispVrtTrackSelector::decision(const xAOD::TrackParticle& track, const xAOD::VertexContainer* vertexColl) const 
{
  bool passes = true;
  
  xAOD::VertexContainer::const_iterator vxIt = vertexColl->begin();
  for (; vxIt != vertexColl->end(); ++vxIt) {
    const xAOD::Vertex* pVtx = (*vxIt);
    passes &= decision(track, pVtx);
  }
  
  return passes;
}

bool RPVDispVrtTrackSelector::passesTRTPixCut(const xAOD::TrackParticle& track) const
{
    uint8_t nPix = 0;
    bool gotPix = track.summaryValue(nPix,xAOD::numberOfPixelHits);
    if (!gotPix) {
      msg(MSG::WARNING)<<"couldn't retrieve nPix"<<endreq;
    }
    uint8_t nTRT = 0;
    bool gotTRT = track.summaryValue(nPix,xAOD::numberOfTRTHits);
    if (!gotTRT) {
      msg(MSG::WARNING)<<"couldn't retrieve nTRT"<<endreq;
    }
    if (((int)nTRT == 0) && ((int)nPix < 2)) return false;
    
  
    return true;
}



bool RPVDispVrtTrackSelector::decision(const xAOD::TrackParticle& track,const xAOD::Vertex* vertex) const
{

  if ((m_killTrackFracLowEta > 0.) || (m_killTrackFracHighEta > 0.)) {
    /// initialize random seed: 
    srand ( time(NULL) );
    float rndm = (rand() % 1000)/1000.0;

    if ((fabs(track.eta()) < 1.0) && ( rndm < m_killTrackFracLowEta)) return false;
    if ((fabs(track.eta()) > 1.0) && ( rndm < m_killTrackFracHighEta)) return false;
  }

  if (vertex==0) {
    msg(MSG::ERROR)<<"vertex is null!"<<endreq;
    return false;
  }

  const Trk::Perigee* perigeeAtPV = m_trackToVertexTool->perigeeAtVertex(track, vertex->position());
  double d0 = perigeeAtPV->parameters()[Trk::d0];

  if(m_TRTPixCut){
    if(!passesTRTPixCut(track)) return false;
  }

    uint8_t nPix = 0;
    bool gotPix = track.summaryValue(nPix,xAOD::numberOfPixelHits);
    if (!gotPix) {
      msg(MSG::WARNING)<<"couldn't retrieve nPix"<<endreq;
    }
    uint8_t nSCT = 0;
    bool gotSCT = track.summaryValue(nSCT,xAOD::numberOfSCTHits);
    if (!gotSCT) {
      msg(MSG::WARNING)<<"couldn't retrieve nSCT"<<endreq;
    }
    uint8_t nTRT = 0;
    bool gotTRT = track.summaryValue(nTRT,xAOD::numberOfTRTHits);
    if (!gotTRT) {
      msg(MSG::WARNING)<<"couldn't retrieve nTRT"<<endreq;
    }
    uint8_t nBLayer = 0;
    bool gotBLayer = track.summaryValue(nBLayer,xAOD::numberOfBLayerHits);
    if (!gotBLayer) {
      msg(MSG::WARNING)<<"couldn't retrieve nBLayer"<<endreq;
    }

    uint8_t nPixSharedHits = 0;
    bool gotPixSharedHits = track.summaryValue(nPixSharedHits,xAOD::numberOfPixelSharedHits);
    if (!gotPixSharedHits) {
      msg(MSG::WARNING)<<"couldn't retrieve nPixSharedHits"<<endreq;
    }

    uint8_t nSCTSharedHits = 0;
    bool gotSCTSharedHits = track.summaryValue(nSCTSharedHits,xAOD::numberOfSCTSharedHits);
    if (!gotSCTSharedHits) {
      msg(MSG::WARNING)<<"couldn't retrieve nSCTSharedHits"<<endreq;
    }

    uint8_t nPixHoles = 0;
    bool gotPixHoles = track.summaryValue(nPixHoles,xAOD::numberOfPixelHoles);
    if (!gotPixHoles) {
      msg(MSG::WARNING)<<"couldn't retrieve nPixHoles"<<endreq;
    }


    msg(MSG::INFO)<<"Numbers of hits "<<(int)nPix<<" "<<(int)nSCT<<" "<<(int)nTRT<<" "<<(int)nPixSharedHits<<" "<<(int)nSCTSharedHits<<" "<<(int)nPixHoles<<endreq;
    

  if (track.pt() < m_TrkPtCut) return false;
  if (track.chiSquared() / track.numberDoF() > m_TrkChi2Cut) return false;
  

  if ((int)nPix < m_CutPixelHits) return false;
  if ((int)nSCT < m_CutSctHits) return false;
  if ((int)nBLayer < m_CutBLayHits) return false;
  if ((int)nPixSharedHits + (int)nSCTSharedHits > m_CutSharedHits) return false;
  if ((int)nPix + (int)nSCT < m_CutSiliconHits) return false;
  if ((int)nPixHoles >  m_CutPixelHoles) return false;
  
 

  if (fabs(d0) < m_DstFromBeamCut) return false;
  /// if it got to here, it passed all cuts.
  return true;

  
}



