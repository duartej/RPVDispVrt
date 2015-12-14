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
  if( evtStore()->retrieve(secVertices,"RPVSecVertices") != StatusCode::SUCCESS )
  {
      ATH_MSG_ERROR("Failed to retrieve SecondaryVertices collection");
  }
  else
  {
     ATH_MSG_INFO("retrieved SecondaryVertices size "<<secVertices->size());  
  }

  for(unsigned int i = 0; i < secVertices->size(); ++i)
  {
      const xAOD::Vertex* vert = secVertices->at(i);
     
      ATH_MSG_INFO("[" << i << "] vertex with position (x,y,z) =" 
              <<vert->x() << "," << vert->y() << "," 
              <<vert->z() << ") [mm]" );
      ATH_MSG_INFO(" and mass "<<vert->auxdata<float>("mass"));
      ATH_MSG_INFO(" and the particleLinks.size()" << vert->trackParticleLinks().size() <<
           " Made of valid links?"  );
      for(unsigned int k = 0; k < vert->trackParticleLinks().size() ; ++k)
      {
          ATH_MSG_INFO("   [" << k << "]-track link, pointer: " << vert->trackParticle(k) );
      }
  }
  return StatusCode::SUCCESS;
}

StatusCode RPVDispVrtTestAlg::finalize() 
{

  return StatusCode::SUCCESS;
}

