#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RPVDispVrt/RPVDispVrt.h"
DECLARE_ALGORITHM_FACTORY( RPVDispVrt )
//#include "RPVDispVrt/RPVMCTruHists.h"
//DECLARE_ALGORITHM_FACTORY( RPVMCTruHists )
#include "RPVDispVrt/RPVDispVrtTestAlg.h"
DECLARE_ALGORITHM_FACTORY( RPVDispVrtTestAlg )
#include "RPVDispVrt/DVCompatibilizer.h"
DECLARE_ALGORITHM_FACTORY( DVCompatibilizer )
#include "RPVDispVrt/RPVDispVrtTrackSelector.h"
DECLARE_TOOL_FACTORY( RPVDispVrtTrackSelector )
#include "RPVDispVrt/RPVDispVrtVertexer.h"
DECLARE_TOOL_FACTORY( RPVDispVrtVertexer )
#include "RPVDispVrt/RPVDispVrtVertexSelector.h"
DECLARE_TOOL_FACTORY( RPVDispVrtVertexSelector )
#include "RPVDispVrt/RPVDispVrtVertexCleanup.h"
DECLARE_TOOL_FACTORY( RPVDispVrtVertexCleanup )
//#include "RPVDispVrt/RPVVrtD3PDFillerTool.h"
//DECLARE_TOOL_FACTORY( RPVDispVrtSecondPass )
//#include "RPVDispVrt/RPVDispVrtSecondPass.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RPVVrtD3PDFillerTool)
//#include "RPVDispVrt/TruthParticleTruthTrackAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleTruthTrackAssociationTool)
//#include "RPVDispVrt/TruthTrackTruthParticleAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTrackTruthParticleAssociationTool)
//#include "RPVDispVrt/TruthTrackTruthNeutralinoAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTrackTruthNeutralinoAssociationTool)
//#include "RPVDispVrt/TruthParticleEndVertexAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthParticleEndVertexAssociationTool)
//#include "RPVDispVrt/RPVVrtTruthParticleAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RPVVrtTruthParticleAssociationTool)
//#include "RPVDispVrt/RPVVrtMuonDistAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RPVVrtMuonDistAssociationTool)
//#include "RPVDispVrt/GenParticleTrackParticleAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, GenParticleTrackParticleAssociationTool)
//#include "RPVDispVrt/RPVVrtElectronDistAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RPVVrtElectronDistAssociationTool)
//#include "RPVDispVrt/RPVVrtPhotonDistAssociationTool.h"
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RPVVrtPhotonDistAssociationTool)


DECLARE_FACTORY_ENTRIES(RPVDispVrt) {
  DECLARE_ALGORITHM( RPVDispVrt )
    //    DECLARE_ALGORITHM( RPVMCTruHists )
    DECLARE_ALGORITHM( RPVDispVrtTestAlg )
    DECLARE_ALGORITHM( DVCompatibilizer )
    DECLARE_TOOL( RPVDispVrtTrackSelector)
    DECLARE_TOOL( RPVDispVrtVertexer)
    DECLARE_TOOL( RPVDispVrtVertexSelector)
    DECLARE_TOOL( RPVDispVrtVertexCleanup)
    //DECLARE_TOOL( RPVDispVrtSecondPass)
    //DECLARE_NAMESPACE_TOOL( D3PD, RPVVrtD3PDFillerTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, TruthParticleTruthTrackAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, TruthTrackTruthParticleAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, TruthTrackTruthNeutralinoAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, TruthParticleEndVertexAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, RPVVrtTruthParticleAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, GenParticleTrackParticleAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, RPVVrtMuonDistAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, RPVVrtElectronDistAssociationTool)
    //DECLARE_NAMESPACE_TOOL( D3PD, RPVVrtPhotonDistAssociationTool)
}

//Notes:
//
//1. The argument to the DECLARE_FACTORY_ENTRIES(XXX) is the name of the
//   component library (libXXX.so).
//2. Each Algorithm that is contained in the library should be declared
//   by the DECLARE_ALGORITHM() statement.
//3. Each Service that is contained in the library should be declared
//   by the DECLARE_SERVICE() statement.
//
// See Athena User Guide for more information


//DECLARE_ALGORITHM_FACTORY( RPVDispVrt )
//DECLARE_FACTORY_ENTRIES(RPVDispVrt) {
//    DECLARE_ALGORITHM( RPVDispVrt )
//}
