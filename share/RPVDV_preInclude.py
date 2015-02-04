###
# job options for extended setup, extension processor accepting failed track extensions and basic cuts.

## to run on express stream ESD, need to set following variable to True
## in order to avoid a clash of StoreGate keys.



doExpressStream=False
from AthenaCommon.AthenaCommonFlags import jobproperties
from RecExConfig.RecFlags import rec
rec.doESD.set_Value_and_Lock(True)
rec.doInDet.set_Value_and_Lock(True)
rec.doEgamma.set_Value_and_Lock(True)
rec.doLowPt.set_Value_and_Lock(True)
rec.doJiveXML.set_Value_and_Lock(False)

rec.doTau.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(True)

rec.doTrigger.set_Value_and_Lock(True)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(False)

readESD=True
jobproperties.Rec.readESD=readESD


#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and readESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = False 
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
redoPatternRecoAndTracking = True
# --- redo primary vertexing (will be set to true later automatically if you redid the tracking and want to redo the TrackParticle creation)
reDoPrimaryVertexing = True 
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = True 
# --- redo conversion finding
reDoConversions = False
# --- redo V0 finding
reDoV0Finder = False

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from RPVDispVrt.RPVDispVrtFlags import *
athenaCommonFlags.FilesInput = [
    RPVDVFlags.InputFileName()
##    "/r02/atlas/nbarlow/DV_ESD/mc12_114006.ESD.pool.root"
    ] 

print "inputFile is ", RPVDVFlags.InputFileName()

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'MyAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

from AthenaCommon.DetFlags import DetFlags 
# --- switch on Detector bits
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Print()



from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
InDetFlags.preProcessing = redoPatternRecoAndTracking
InDetFlags.doPRDFormation = False                    # those two will be (later) automatically false if
InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false


InDetFlags.doNewTracking     = redoPatternRecoAndTracking
InDetFlags.doBackTracking     = False #False ### redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = False ###redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = True # False ### redoPatternRecoAndTracking

InDetFlags.doiPatRec         = False
InDetFlags.doxKalman         = False
InDetFlags.doLowPt           = False
InDetFlags.doHighD0          = True
InDetFlags.dontRemakeESDTracks=True
InDetFlags.doCTBTracking     = False


InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics or reDoV0Finder
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = reDoPrimaryVertexing
InDetFlags.doParticleCreation = reDoParticleCreation
InDetFlags.doConversions      = reDoConversions
InDetFlags.doSecVertexFinder  = False
InDetFlags.doV0Finder         = reDoV0Finder
InDetFlags.doTrkNtuple        = False # no trk val ntuple needed, got d3pd.
InDetFlags.doPixelTrkNtuple   = False 
InDetFlags.doSctTrkNtuple     = False 
InDetFlags.doTrtTrkNtuple     = False 
InDetFlags.doPixelClusterNtuple = False
InDetFlags.doSctClusterNtuple   = False
InDetFlags.doTrtDriftCircleNtuple = False
InDetFlags.doVtxNtuple        = False 
InDetFlags.doConvVtxNtuple    = False 
InDetFlags.doV0VtxNtuple      = False 
InDetFlags.doRefit            = doRefitTracks
InDetFlags.doLowBetaFinder    = False
InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)
InDetFlags.doStatistics = True

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts

InDetNewTrackingCutsHighD0 = ConfiguredNewTrackingCuts("HighD0")
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__minPT= 500. * Units.MeV
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxPT= 1. * Units.TeV
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxEta= 5
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxPrimaryImpact=300.0 * Units.mm
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxZImpact=1500.0 * Units.mm
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxSecondaryImpact=300.0 * Units.mm
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__minSecondaryPt=500.0 * Units.MeV
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__minClusters=7
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__minSiNotShared=5
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxShared=2
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__maxTracksPerSharedPRD=2
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__seedFilterLevel=1
InDetNewTrackingCutsHighD0._ConfiguredNewTrackingCuts__minPixel=0

InDetNewTrackingCutsHighD0.printInfo()



###############################################################################
##now redo muon reco
##############################################################################
from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
from MuonRecExample.DataFiles import DataInfo,DataFilesInfoMap
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags


## possibly turn on?
muonRecFlags.doMoore = True
muonRecFlags.doStandalone=False
muonCombinedRecFlags.doMuidMuonCollection = True
muonCombinedRecFlags.doMuidCombined = True
muonCombinedRecFlags.doCombinedFit= True
muonCombinedRecFlags.doMuidStandalone = True
muonCombinedRecFlags.doMuonCollection= True
muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(False) 

muonRecFlags.doMuonboy = False
muonRecFlags.calibMoore=False
muonRecFlags.calibMuonboy=False
muonRecFlags.calibNtupleSegments=False
muonRecFlags.calibNtupleTracks=False
muonRecFlags.calibNtupleTrigger=False


muonCombinedRecFlags.doMboyCombined = False
muonCombinedRecFlags.doMuTagIMO = False
muonCombinedRecFlags.doLArMuId = False
muonCombinedRecFlags.doMuGirl = False
##muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(False) # uncomment only for data.
muonCombinedRecFlags.doMuidCombinedForward = False
muonCombinedRecFlags.doMuTag = False
muonCombinedRecFlags.doCaloMuonCollection= False
muonCombinedRecFlags.doCaloTrkMuId= False

muonCombinedRecFlags.doCombinedFitForward= False
muonCombinedRecFlags.doForwardMuons= False
muonCombinedRecFlags.doMuGirlLowBeta= False
muonCombinedRecFlags.doMuGirlLowBetaMuonCollection= False

muonCombinedRecFlags.doStacoMuonCollection= False
muonCombinedRecFlags.doStatisticalCombination= False
muonCombinedRecFlags.doStatisticalCombinationForward= False

###############################################################################
##now redo electron reco
##############################################################################

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(True)
# jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
# jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
# jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)

from egammaRec.egammaRecFlags import egammaRecFlags
egammaRecFlags.doEgammaCaloSeeded = True # Default=True
egammaRecFlags.doEgammaTrackSeeded = True # Default=True
egammaRecFlags.doEgammaForwardSeeded = True # Default=True
egammaRecFlags.doConversionAODRecovery = True # Default=True
egammaRecFlags.doAODRender = True # Default=True
egammaRecFlags.useOldPhotonLogic = True # Default=True
egammaRecFlags.doTopoCaloSeeded = True # Default=True
egammaRecFlags.useBremTracks = True # Default=True
egammaRecFlags.cellContainerName = False # Default=False
egammaRecFlags.makeNewConvContainer = True # Default=True
egammaRecFlags.useSofteTopo = True # Default=True

##############################################################################

### require trigger filter?
#include("RPVDispVrt/RPVFilter_jobOptions.py")

print "-=-=-=- rec flacs -=-=-=-="
print rec

print "-=-=-=- recAlgs flacs -=-=-=-="
print recAlgs

###### run reconstruction
###include("RecExCommon/RecExCommon_topOptions.py")
###
###
###topSequence.InDetCopyAlg.AliasName='NewTracks'
###if rec.doTruth():
###    topSequence.DetailedTrackTruthMaker.TrackCollectionName='NewTracks'
###    topSequence.InDetParticleTruthMaker.tracksName='NewTracks'
###    pass
###topSequence.InDetPriVxFinder.TracksName='NewTracks'
###topSequence.InDetParticleCreator_PRD_Association.TracksName=['NewTracks']
###topSequence.InDetParticleCreation.TracksName='NewTracks'
###
###
###topSequence.InDetTrackCollectionMerger.TracksLocation=['Tracks','ExtendedHighD0Tracks'] # do not want to use TRTStandaloneTracks here. They are useless... Change this if one want to do reco without 2nd step.
###
####### FOR EXPRESS STREAM
###if doExpressStream:
###    topSequence.InDetTrackCollectionMerger.OutputTracksLocation="MergedTracks"
###    topSequence.InDetCopyAlg.CollectionName='MergedTracks'
###    pass
###
###topSequence.MuidCombined.InDetTracksLocation="NewTracks"
###
###include("RPVDispVrt/RPVVertexing_jobOptions.py")
###include("RPVDispVrt/RPVNtuple_jobOptions.py")
###
###print "FINAL SEQUENCE",topSequence
###
###theApp.EvtMax=10
###
