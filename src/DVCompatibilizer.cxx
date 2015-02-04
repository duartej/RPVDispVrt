#include "RPVDispVrt/DVCompatibilizer.h"
#include "RPVDispVrt/RPVDispVrt.h"
#include "RPVDispVrt/RPVDispVrtTrackSelector.h"
#include "RPVDispVrt/RPVDispVrtVertexer.h"
#include "RPVDispVrt/RPVDispVrtVertexCleanup.h"
#include "RPVDispVrt/RPVDispVrtVertexSelector.h"
#include "RPVDispVrt/RPVDispVrtSecondPass.h"
#include "TTree.h"

/// --------------------------------------------------------------------
/// Constructor

DVCompatibilizer::DVCompatibilizer(const std::string& name,
                 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("InputVertices", m_inputVertices="SecondaryVertices");
  declareProperty("OutputVertices", m_outputVertices="DispVrtVertices");
}

/// --------------------------------------------------------------------
/// Initialize

StatusCode DVCompatibilizer::initialize() {

  msg(MSG::DEBUG) << "DVCompatibilizer::initialize" << endreq;

  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute

StatusCode DVCompatibilizer::execute() {

  msg(MSG::DEBUG)<<"in DVCompatibilizer::execute()"<<endreq;


  RPVVrtContainer* in(0);
  StatusCode sc = evtStore()->retrieve(in,m_inputVertices);
  if (sc.isFailure())
    msg(MSG::ERROR)<<"Failed to retrieve input vertex collection "<<m_inputVertices<<endreq;
  else msg(MSG::DEBUG)<<"retrieved input vertex collection size "<<in->size()<<endreq;

  if (in->size() == 0) {
    msg(MSG::WARNING)<<"DV vertices not found, will skip this event"<<endreq;
    return StatusCode::SUCCESS;
  }

  /// create output container
  VxContainer* out = new VxContainer();

  // now clone our vertices

  RPVVrtContainer::iterator vIt = in->begin();
  RPVVrtContainer::iterator vEnd = in->end();

  for (; vIt != vEnd; ++vIt){
    Trk::VxCandidate* cand = *vIt;
    Trk::VxCandidate* vx = new Trk::VxCandidate(*cand);
    vx->setVertexType(Trk::SecVtx);
    out->push_back(vx);
  }


  sc = evtStore()->record(out,m_outputVertices);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record VxCandidate collection made from RPVVrtContainer"<<endreq;
  } else {
    msg(MSG::DEBUG)<<"Recording VxCandidate collection made from RPVVrtContainer with "<<out->size()<<" vertices"<<endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode DVCompatibilizer::finalize() {

  return StatusCode::SUCCESS;
}

