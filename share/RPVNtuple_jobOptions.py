from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.MuonSGKey.set_Value_and_Lock("MuidMuonCollection")

from RPVDispVrt.RPVDispVrtFlags import RPVDVFlags

## Output file name
outFileName = RPVDVFlags.OutputFileName()


doLightweight = False

DoJet=True
DoMET=True
DoTruth = rec.doTruth() and not doLightweight
DoTruthJet=False
DoBeamSpot= False #not doLightweight
DoTrigger= not doLightweight
DoHitsOnTracks=False
DoAOD="AOD" in athenaCommonFlags.FilesInput.get_Value()[0]

DoElectron=True #EGAMMA
DoPhoton=True #EGAMMA
DoMuon=True 

#--------------------------------------------------------------
# Configure the D3PDMaker
#--------------------------------------------------------------

import MinBiasD3PDMaker
import TrackD3PDMaker
import MuonD3PDMaker
import MissingETD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
import egammaD3PDMaker #EGAMMA

################################################################
## Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

if DoBeamSpot:
## BeamSpot
##--------------------------------------------------------------
## Configure Beam spot service
##--------------------------------------------------------------

    from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource == 'data':
        include("InDetBeamSpotService/BeamCondSvc.py")
        conddb.addOverride("/Indet/Beampos", minbiasD3PDflags.BSDBFolderName())
        pass
    from MinBiasD3PDMaker.MinBiasD3PDMakerConf import MinBiasD3PD__MinBiasD3PDPreProcessing
    MinBiasD3PDPreProcessingAlg = MinBiasD3PD__MinBiasD3PDPreProcessing( name = "MinBiasD3PDPreProcessingAlg",
                                                                         OutputLevel = INFO )
    topSequence += MinBiasD3PDPreProcessingAlg    
    from MinBiasD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject

##################################################################
## Track and vertex info

include("RPVDispVrt/RPVNtuple_track.py")

##################################################################
## truth info

if DoTruth:
    include("RPVDispVrt/RPVNtuple_truth.py")
    pass

################################################################
## add muon info

if DoMuon:
    include("RPVDispVrt/RPVNtuple_muon.py")

################################################################
## add jet info

if DoJet:
    include("RPVDispVrt/RPVNtuple_jet.py")
##    import QcdD3PDMaker
################################################################
## add met info

if DoMET:
    include("RPVDispVrt/RPVNtuple_MET.py")

################################################################
## add electron info

if DoElectron:
    include("RPVDispVrt/RPVNtuple_electron.py")

################################################################
## add photon info

if DoPhoton:
    include("RPVDispVrt/RPVNtuple_photon.py")

################################################################

## add trigger info

if DoTrigger:
    include("RPVDispVrt/RPVNtuple_trigger.py")
    
##################### put it all together #######################

## D3PD Maker alg
alg = D3PDMakerCoreComps.MakerAlg('DispVtxTree',
                                  topSequence,
                                  outFileName)

alg += EventInfoD3PDObject(10)

if DoBeamSpot:
    alg += BeamSpotD3PDObject(1)
if DoMET:
    alg += RefFinalMETD3PDObject(0,exclude=metExcludes)                  
if DoMuon:
    alg += MuonD3PDObject(10)    
if DoJet:
    alg += JetD3PDObject(0,sgkey="AntiKt4TopoEMJets",include=jetIncludes,prefix="AntiKt4Jet_")
    alg += JetD3PDObject(0,sgkey="AntiKt6TopoEMJets",include=jetIncludes,prefix="AntiKt6Jet_")
    alg += EventShapeD3PDObject(0, 'rho')
if DoElectron:
	alg += ElectronD3PDObject(10)
if DoPhoton:
	alg += PhotonD3PDObject(10)
if DoTruth:
    alg += truPart(10)
    alg += truTrack(10)
    alg += TruthVertexD3PDObject(10)
    alg += GenEventD3PDObject (1)

    alg += PrimaryVertexD3PDObject(9)
    alg += DisplacedVertexD3PDObject(9)      
    alg += TrackWithTruthD3PDObject(10)
    if DoTruthJet:
        alg+= TruthJetD3PDObject(10,sgkey="AntiKt4TruthJets")
        pass
else:
    if not doLightweight:
        alg += TrackParticleD3PDObject(8)
    alg += PrimaryVertexD3PDObject(8)
    alg += DisplacedVertexD3PDObject(8)   
    pass
### HACK - there must be an easier way to do this, no????
if not doLightweight:
    alg.trk_Filler.trk_Filler_D3PD__TrackParticleTrackSummaryAssociationTool.trk_Filler_D3PD__TrackParticleTrackSummaryAssociationTool_trk_TrackSummary.IDSharedHits=True


if DoAOD and DoTruth:
    alg.mc_Filler.mc_Filler_D3PD__TruthParticleTruthTrackAssociationTool.Associator.GenParticleContainer="GEN_AOD"

 ############# adding all EF muons ###################
if DoTrigger:
    from TrigMuonD3PDMaker.TrigMuonEFInfoD3PDObject import TrigMuonEFInfoD3PDObject
    alg += TrigMuonEFInfoD3PDObject(10) 

if DoTrigger and DoElectron:
    from TrigEgammaD3PDMaker.EFElectronD3PDObject import EFElectronD3PDObject
    # from TrigEgammaD3PDMaker.TrigElectronD3PDObject import TrigElectronD3PDObject # L2 objects
    alg += EFElectronD3PDObject(10)
    # alg += TrigElectronD3PDObject(10) # L2 objects

if DoTrigger and DoPhoton:
    from TrigEgammaD3PDMaker.EFPhotonD3PDObject import EFPhotonD3PDObject
    # from TrigEgammaD3PDMaker.TrigPhotonD3PDObject import TrigPhotonD3PDObject # L2 objects
    alg += EFPhotonD3PDObject(10) 	
