#include "RPVDispVrt/RPVDispVrtTestAlg.h"
#include "xAODTracking/VertexContainer.h"

/// --------------------------------------------------------------------
/// Constructor

RPVDispVrtTestAlg::RPVDispVrtTestAlg(const std::string& name,
			     ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

/// --------------------------------------------------------------------
/// Initialize

StatusCode RPVDispVrtTestAlg::initialize() {

  msg(MSG::INFO) << "RPVDispVrtTestAlg::initialize" << endreq;

  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute

StatusCode RPVDispVrtTestAlg::execute() {
  msg(MSG::DEBUG)<<"in RPVDispVrtTestAlg::execute()"<<endreq;

  const xAOD::VertexContainer* secVertices(0);
  StatusCode sc = evtStore()->retrieve(secVertices,"RPVSecVertices");
  if (sc.isFailure()) 
    msg(MSG::ERROR)<<"Failed to retrieve SecondaryVertices collection"<<endreq;
  else msg(MSG::INFO)<<"retrieved SecondaryVertices size "<<secVertices->size()<<endreq;  

  if (secVertices->size() > 0) {
    const xAOD::Vertex* vert = secVertices->at(0);
    
    msg(MSG::INFO)<<"found vertex with position "<<vert->position().x()<<endreq;
    msg(MSG::INFO)<<" and mass "<<vert->auxdata<double>("mass")<<endreq;
    
  }
  return StatusCode::SUCCESS;
}

StatusCode RPVDispVrtTestAlg::finalize() {

  return StatusCode::SUCCESS;
}

