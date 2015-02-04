## Reco Tracks
from TrackD3PDMaker.TrackD3PDMakerFlags import *
TrackD3PDFlags.storeVertexPurity.set_Value_and_Lock(False)
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails.set_Value_and_Lock(1)

TrackD3PDFlags.storeVertexTrackAssociation.set_Value_and_Lock(True)

if doLightweight:
    TrackD3PDFlags.storeVertexTrackIndexAssociation.set_Value_and_Lock(False)

if DoHitsOnTracks:
    TrackD3PDFlags.storePixelHitsOnTrack.set_Value_and_Lock(True)
    TrackD3PDFlags.storeSCTHitsOnTrack.set_Value_and_Lock(True)
##    TrackD3PDFlags.storeTRTHitsOnTrack.set_Value_and_Lock(True)
    TrackD3PDFlags.storePixelOutliersOnTrack.set_Value_and_Lock(True)
    TrackD3PDFlags.storeSCTOutliersOnTrack.set_Value_and_Lock(True)
##    TrackD3PDFlags.storeTRTOutliersOnTrack.set_Value_and_Lock(True)        
    TrackD3PDFlags.storePixelHolesOnTrack.set_Value_and_Lock(True)
    TrackD3PDFlags.storeSCTHolesOnTrack.set_Value_and_Lock(True)
##    TrackD3PDFlags.storeTRTHolesOnTrack.set_Value_and_Lock(True)    
    pass

from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
## Reco Vertex
from TrackD3PDMaker.VertexD3PDObject import *
from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
## CTP RDO
##from MinBiasD3PDMaker.CtpRdoD3PDObject import CtpRdoD3PDObject
from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject

from RPVDispVrt.RPVDispVrtConf import D3PD__RPVVrtD3PDFillerTool

##################################################################
def BuildRPVVrtD3PDObject(_prefix='dispVtx_',
                          _label='dispVtx',
                          _sgkey=D3PDMakerFlags.VertexSGKey(),
                          trackTarget='trk',
                          trackPrefix='trk_',
                          trackType='Rec::TrackParticleContainer'):

    object = make_SGDataVector_D3PDObject ('RPVVrtContainer',
                                           _sgkey,
                                           _prefix)
                                           

    DefineVertexD3PDObject(object,
                           trackTarget,
                           trackPrefix,
                           trackType,
                           flags=TrackD3PDFlags)


    return object

################################################################


PrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='primVtx_',
                                                _label='primVtx',
                                                _sgkey='VxPrimaryCandidate',
                                                trackTarget='trk',
                                                trackPrefix='trk_',
                                                trackType='Rec::TrackParticleContainer')



DisplacedVertexD3PDObject = BuildRPVVrtD3PDObject(_prefix='dispVtx_',
                                                _label='dispVtx',
                                                  _sgkey='SecondaryVertices',
                                                  trackTarget='trk',
                                                  trackPrefix='trk_',
                                                  trackType='Rec::TrackParticleContainer')

DisplacedVertexD3PDObject.defineBlock(1, 'RPVExtra',
                                      D3PD__RPVVrtD3PDFillerTool)


#$$$if DoTruth:
#$$$    TrackWithTruthD3PDObject= TrackD3PDObject(_label='trk',
#$$$                                              _prefix='trk_',
#$$$                                              _sgkey=D3PDMakerFlags.TrackSGKey(),
#$$$                                              typeName='Rec::TrackParticleContainer',
#$$$                                              truthTarget='mcTrk_',
#$$$                                              truthPrefix='mcTrk_',
#$$$                                              detailedTruthPrefix='detailed_mc_',
#$$$##                                              trkTrackSGKey='Tracks',
#$$$                                              truthMapKey='TrackParticleTruthCollection',
#$$$                                              detailedTruthMapKey='DetailedTrackTruth')

if DoTruth:
    TrackWithTruthD3PDObject= TrackD3PDObject(_label='trk',
                                              _prefix='trk_',
                                              _sgkey=D3PDMakerFlags.TrackSGKey(),
                                              typeName='Rec::TrackParticleContainer',
                                              truthTarget='mcTrk_',
                                              truthPrefix='mcTrk_',
                                              detailedTruthPrefix='detailed_mc_',
                                              SGKeyForTruth="TrackParticleCandidate",#D3PDMakerFlags.TrackSGKey(),
                                              ##                                              trkTrackSGKey='Tracks',
                                              truthMapKey='TrackParticleTruthCollection',
                                              detailedTruthMapKey='DetailedTrackTruth')
