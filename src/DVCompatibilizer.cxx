#include "RPVDispVrt/DVCompatibilizer.h"
//#include "RPVDispVrt/RPVVrt.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "RPVDispVrt/RPVDispVrtTrackSelector.h"
#include "RPVDispVrt/RPVDispVrtVertexer.h"
#include "RPVDispVrt/RPVDispVrtVertexCleanup.h"
#include "RPVDispVrt/RPVDispVrtVertexSelector.h"
//#include "RPVDispVrt/RPVDispVrtSecondPass.h"
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


  //RPVVrtContainer* in(0);
  xAOD::VertexContainer * in = 0;
  StatusCode sc = evtStore()->retrieve(in,m_inputVertices);
  if (sc.isFailure())
    msg(MSG::ERROR)<<"Failed to retrieve input vertex collection "<<m_inputVertices<<endreq;
  else msg(MSG::DEBUG)<<"retrieved input vertex collection size "<<in->size()<<endreq;

  if (in->size() == 0) {
    msg(MSG::WARNING)<<"DV vertices not found, will skip this event"<<endreq;
    return StatusCode::SUCCESS;
  }

  /// create output container
  xAOD::VertexContainer* out = new xAOD::VertexContainer();

  // now clone our vertices
  xAOD::VertexContainer::iterator vIt = in->begin();
  xAOD::VertexContainer::iterator vEnd = in->end();

  for (; vIt != vEnd; ++vIt){
    xAOD::Vertex* cand = *vIt;
    xAOD::Vertex* vx = new xAOD::Vertex(*cand);
    vx->setVertexType(xAOD::VxType::VertexType::SecVtx);
    out->push_back(vx);
  }


  sc = evtStore()->record(out,m_outputVertices);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<<"Failed to record VertexCandidate collection made from xAOD::VertexContainer"<<endreq;
  } else {
    msg(MSG::DEBUG)<<"Recording VertexCandidate collection made from xAOD::VertexContainer with "<<out->size()<<" vertices"<<endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode DVCompatibilizer::finalize() {

  return StatusCode::SUCCESS;
}

