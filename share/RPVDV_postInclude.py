###
# job options for extended setup, extension processor accepting failed track extensions and basic cuts.

## to run on express stream ESD, need to set following variable to True
## in order to avoid a clash of StoreGate keys.

topSequence.InDetCopyAlg.AliasName='NewTracks'
if rec.doTruth():
    topSequence.DetailedTrackTruthMaker.TrackCollectionName='NewTracks'
    topSequence.InDetParticleTruthMaker.tracksName='NewTracks'
    pass
topSequence.InDetPriVxFinder.TracksName='NewTracks'
topSequence.InDetParticleCreator_PRD_Association.TracksName=['NewTracks']
topSequence.InDetParticleCreation.TracksName='NewTracks'


topSequence.InDetTrackCollectionMerger.TracksLocation=['Tracks','ExtendedHighD0Tracks'] # do not want to use TRTStandaloneTracks here. They are useless... Change this if one want to do reco without 2nd step.

#### FOR EXPRESS STREAM
if doExpressStream:
    topSequence.InDetTrackCollectionMerger.OutputTracksLocation="MergedTracks"
    topSequence.InDetCopyAlg.CollectionName='MergedTracks'
    pass

topSequence.MuidCombined.InDetTracksLocation="NewTracks"

include("RPVDispVrt/RPVVertexing_jobOptions.py")
include("RPVDispVrt/RPVNtuple_jobOptions.py")

print "FINAL SEQUENCE",topSequence


