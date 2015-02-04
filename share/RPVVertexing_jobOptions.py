## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from RPVDispVrt.RPVDispVrtFlags import RPVDVFlags

from RPVDispVrt.RPVDispVrtConf import RPVDispVrt
topSequence+=RPVDispVrt()
###topSequence.RPVDispVrt.DoDebugNtuple=False
##topSequence.RPVDispVrt.DebugNtupleFileName="RPVDEBUG"
##topSequence.RPVDispVrt.RandomizeTracks=False
topSequence.RPVDispVrt.UseAllPrimaryVertices=False
topSequence.RPVDispVrt.SelectForMaterialMap=False
topSequence.RPVDispVrt.SelectGT2Trk=False
topSequence.RPVDispVrt.OutputLevel=DEBUG
##topSequence.RPVDispVrt.doSecondPassVertexing=False

#from RPVDispVrt.RPVDispVrtConf import RPVDispVrtVertexer
#ToolSvc += RPVDispVrtVertexer()
#ToolSvc.RPVDispVrtVertexer.OutputLevel=INFO
#ToolSvc.RPVDispVrtVertexer.OutputLevel=DEBUG

#from RPVDispVrt.RPVDispVrtConf import RPVDispVrtVertexSelector
#ToolSvc+= RPVDispVrtVertexSelector()
#ToolSvc.RPVDispVrtVertexer.VertexSelectorTool = ToolSvc.RPVDispVrtVertexSelector
#ToolSvc.RPVDispVrtVertexSelector.VtxChi2Cut = 5.0

#=#from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
#=#AtlasExtrapolator = AtlasExtrapolator('AtlasExtrapolator')
#=#ToolSvc+=AtlasExtrapolator
#=#
#=#from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
#=#InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
#=#	                                      Extrapolator        = ToolSvc.AtlasExtrapolator,
#=#	                                      IterationNumber     = 30,
#=#	                                      MagFieldSvc         = ToolSvc.AtlasMagneticFieldTool,
#=#	                                      AtlasMagFieldSvc    = 'MagFieldAthenaSvc'
#=#					     )
#=#ToolSvc +=  InclusiveVxFitterTool
#=#InclusiveVxFitterTool.OutputLevel = INFO
#=#ToolSvc.RPVDispVrtVertexer.VertexFitterTool=InclusiveVxFitterTool

from RPVDispVrt.RPVDispVrtConf import RPVDispVrtTrackSelector
ToolSvc += RPVDispVrtTrackSelector()
ToolSvc.RPVDispVrtTrackSelector.TrkChi2Cut = 50.0
ToolSvc.RPVDispVrtTrackSelector.TrkPtCut = 1000.0
ToolSvc.RPVDispVrtTrackSelector.CutSctHits = 2 #nonzero : automatic removal of TRTStandAlone tracks
ToolSvc.RPVDispVrtTrackSelector.CutPixelHits = 0
ToolSvc.RPVDispVrtTrackSelector.CutSharedHits = 1
ToolSvc.RPVDispVrtTrackSelector.DstFromBeamCut = 2.0 #d0
ToolSvc.RPVDispVrtTrackSelector.TRTPixCut = RPVDVFlags.DoTRTPixCut()


### track killing for systematics studies
#ToolSvc.RPVDispVrtTrackSelector.RandomlyKillTracksFracLowEta = 0.03
#ToolSvc.RPVDispVrtTrackSelector.RandomlyKillTracksFracHighEta = 0.043

#from RPVDispVrt.RPVDispVrtConf import RPVDispVrtVertexCleanup
#ToolSvc+= RPVDispVrtVertexCleanup()
#ToolSvc.RPVDispVrtVertexCleanup.VertexMergeSignifCut = 3.0
#ToolSvc.RPVDispVrtVertexCleanup.DoFinalVertexMergeDistance=RPVDVFlags.DoFinalMerge()
#ToolSvc.RPVDispVrtVertexCleanup.DoFinalVertexMergeSignif=False

#ToolSvc.RPVDispVrtVertexCleanup.VertexerTool = ToolSvc.RPVDispVrtVertexer

#topSequence.RPVDispVrt.TrackSelectorTool = ToolSvc.RPVDispVrtTrackSelector
#topSequence.RPVDispVrt.VertexerTool = ToolSvc.RPVDispVrtVertexer
#topSequence.RPVDispVrt.VertexCleanupTool = ToolSvc.RPVDispVrtVertexCleanup

#from RPVDispVrt.RPVDispVrtConf import RPVDispVrtTestAlg
#topSequence+=RPVDispVrtTestAlg()
