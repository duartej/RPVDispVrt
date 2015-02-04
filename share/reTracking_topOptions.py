###
# job options for extended setup, extension processor accepting failed track extensions and basic cuts.

from RecExConfig.RecFlags import rec
rec.doESD.set_Value_and_Lock(True)
##rec.doESD.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(True)
rec.doEgamma.set_Value_and_Lock(False)
rec.doLowPt.set_Value_and_Lock(True)
#rec.doWriteESD.set_Value_and_Lock(True)
rec.doJiveXML.set_Value_and_Lock(False)

rec.doTau.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(True)
#rec.doMuonStandalone.set_Value_and_Lock(True)
rec.doTrigger.set_Value_and_Lock(True)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(False)

readESD = True
rec.readESD=readESD
#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and readESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = False 
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
# CLA changed:
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

athenaCommonFlags.FilesInput = [
    '/home/nbarlow/ESD/mc11_7TeV.114006.Pythia_RPV_vtx2_LSP500_filtDL.recon.ESD.e825_s1310_s1300_r3043_tid617262_00/ESD.617262._000041.pool.root.1'
#    "/home/nbarlow/ESD/mc11_7TeV.114006.Pythia_RPV_vtx2_LSP500_filtDL.recon.ESD.e825_s1310_s1300_r2730_tid516244_00/ESD.516244._000024.pool.root.2"
#    	"/home/nbarlow/ESD/data11_7TeV.00183407.physics_JetTauEtmiss.merge.DESDM_RPVLL.r2603_p659_p662_tid494253_00/DESDM_RPVLL.494253._000022.pool.root.1"
#    	"/home/nbarlow/ESD/group.perf-idtracking.mc11_valid.114006.Pythia_RPV_vtx2_LSP500_filtDL.e825_s1310_s1300_d578.pu0.17p0p2p10.cutLevel4.v1.110919115344_EXT1/group.perf-idtracking.33771_004507.EXT1._00017.ESD.root"
        
#        "/usera/brochu/work/RPV/data/user.fbrochu.001047.EXT0._00001.ESD.pool.root"
#	"/tmp/brochu/ESD.194314._001099.pool.root"
#	"/atlas/brochu/TrackingValidation/mc11_valid/group.perf-idtracking.mc11_valid.114006.Pythia_RPV_vtx2_LSP500_filtDL.e825_s1310_s1300_d578.pu0.17p0p2p10.cutLevel4.v1.110919115344_EXT1/group.perf-idtracking.33771_004507.EXT1._00017.ESD.root"
#	"/atlas/brochu/TrackingValidation/mc11_valid/mc11_valid.114006.Pythia_RPV_vtx2_LSP500_filtDL.recon.ESD.e825_s1310_s1300_r2666/ESD.505778._000002.pool.root.1"
#    "/home/nbarlow/ESD/data11_7TeV.00184130.physics_Muons.merge.DESDM_RPVLL.r2603_p659_tid491965_00/DESDM_RPVLL.491965._000132.pool.root.1"
#    "/home/nbarlow/ESD/data11_7TeV.00184022.physics_JetTauEtmiss.merge.DESDM_TRACK.r2603_p659_p662_tid492817_00/DESDM_TRACK.492817._000020.pool.root.8"
#    "/home/nbarlow/ESD/mc11valid_ttbar.ESD.pool.root"
#    '/home/nbarlow/ESD/mc10_7TeV.114006.Pythia_RPV_vtx2_LSP500_filtDL.recon.ESD.e574_s933_s946_r1801_tid209795_00/ESD.209795._000001.pool.root.1'
#    '/home/nbarlow/AOD/data10_7TeV.00161379.physics_MinBias.merge.AOD.r1647_p306_p307_tid187847_00/AOD.187847._000001.pool.root.1'
    ] 

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

InDetFlags.doiPatRec         = False
InDetFlags.doxKalman         = False
InDetFlags.doLowPt           = True
InDetFlags.doCTBTracking      = False
InDetFlags.doBackTracking     = redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
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

##### TEST FOR USING PRIMARY VERTEXING TO FIND DVs
##InDetFlags.primaryVertexCutSetup="StartUp"
##InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
### END TEST


from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
#from ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
#print "SiSpacePointsSeedMaker",SiSpacePointsSeedMaker

InDetNewTrackingCutsLowPt = ConfiguredNewTrackingCuts("LowPt")
#InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minPT= 100. * Units.GeV
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minPT= 500. * Units.MeV
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxPT= 1. * Units.TeV
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxEta= 5
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxPrimaryImpact=300.0 * Units.mm
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxZImpact=1500.0 * Units.mm
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxSecondaryImpact=300.0 * Units.mm
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minSecondaryPt=500.0 * Units.MeV
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minClusters=7
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minSiNotShared=5
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxShared=2
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxTracksPerSharedPRD=2
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__seedFilterLevel=1
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__minPixel=0

InDetNewTrackingCutsLowPt.printInfo()



#=#
#=################################################################################
#=###now redo muon reco
#=###############################################################################
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doMuGirl = False
muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(True)

###
###
###from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
###from MuonRecExample.DataFiles import DataInfo,DataFilesInfoMap
###from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
###
###muonRecFlags.doMuonboy = False
###muonRecFlags.doMoore = True
####muonRecFlags.doMuGirl = False
###muonCombinedRecFlags.doMuidMuonCollection = True
###
###muonCombinedRecFlags.doMuidStandalone = True
###muonCombinedRecFlags.doMuidCombined = True
###muonCombinedRecFlags.doMuTagIMO = False
###muonCombinedRecFlags.doLArMuId = False
###muonCombinedRecFlags.doMuGirl = False
###muonCombinedRecFlags.doCombinedMuonFit.set_Value_and_Lock(True) # uncomment only for data.
###
### require trigger filter?
#include("RPVDispVrt/RPVFilter_jobOptions.py")

### run reconstruction
include("RecExCommon/RecExCommon_topOptions.py")
# correcting Nick's mistake.
ToolSvc.InDetSpSeedsMaker.maxdImpactPPS=1.7
print "RESTORED: first pass seeds maker",ToolSvc.InDetSpSeedsMaker

ToolSvc.InDetTRT_ExtensionTool.MinNumberPIXclusters=0
ToolSvc.InDetTRT_ExtensionTool.MinNumberSCTclusters  = 4
ToolSvc.InDetTRT_ExtensionTool.maxImpactParameter    = 500.
ToolSvc.InDetTRT_ExtensionTool.ZVertexHalfWidth      = 500.
print ToolSvc.InDetTRT_ExtensionTool

topSequence.InDetTrackCollectionMerger.TracksLocation=['ExtendedTracks', 'ResolvedTRTSeededTracks', 'ExtendedLowPtTracks'] # do not want to use TRTStandaloneTracks here. They are useless... Change this if one want to do reco without 2nd step.
print "MODIFIED", topSequence.InDetTrackCollectionMerger

include("RPVDispVrt/RPVVertexing_jobOptions.py")
include("RPVDispVrt/RPVNtuple_jobOptions.py")

print ToolSvc.RPVDispVrtTrackSelector
print topSequence.RPVDispVrt

ToolSvc.MuTagInDetDetailedTrackSelectorTool.IPd0Max=300
ToolSvc.MuTagInDetDetailedTrackSelectorTool.IPz0Max=1500
ToolSvc.MuTagInDetDetailedTrackSelectorTool.d0MaxPreselection=300
ToolSvc.MuTagInDetDetailedTrackSelectorTool.nHitPix=0
ToolSvc.MuTagInDetDetailedTrackSelectorTool.nHitSct=0
ToolSvc.MuTagInDetDetailedTrackSelectorTool.nHitSi=7
ToolSvc.MuTagInDetDetailedTrackSelectorTool.pTMin=1000.
ToolSvc.StacoInDetDetailedTrackSelectorTool.IPd0Max=300
ToolSvc.StacoInDetDetailedTrackSelectorTool.IPz0Max=1500
ToolSvc.StacoInDetDetailedTrackSelectorTool.d0MaxPreselection=300
ToolSvc.StacoInDetDetailedTrackSelectorTool.nHitPix=0
ToolSvc.StacoInDetDetailedTrackSelectorTool.nHitSct=0
ToolSvc.StacoInDetDetailedTrackSelectorTool.nHitSi=7
ToolSvc.StacoInDetDetailedTrackSelectorTool.pTMin=1000.


print "FINAL SEQUENCE",topSequence
print "FINAL ALGTOOLS", ToolSvc

theApp.EvtMax=10
#athenaCommonFlags.SkipEvents=1970
#ServiceMgr.EventSelector.SkipEvents=990
