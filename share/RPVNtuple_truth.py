
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthWriteGeant.set_Value_and_Lock(False)
D3PDMakerFlags.TruthWritePartons.set_Value_and_Lock(True)
D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)
from TruthD3PDMaker.GenEventD3PDObject         import GenEventD3PDObject
from TruthD3PDAnalysis.truthParticleConfig        import truthParticleConfig
##from TruthD3PDMaker.TruthJetFilterConfig             import TruthJetFilterConfig
from TruthD3PDMaker.TruthParticleD3PDObject    import *
from EventCommonD3PDMaker.DRIndexAssociation  import DRIndexAssociation
from TrackD3PDMaker.TruthTrackD3PDObject import *
## Truth Vertices
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
from RPVDispVrt.RPVDispVrtConf import D3PD__TruthParticleTruthTrackAssociationTool
from RPVDispVrt.RPVDispVrtConf import D3PD__TruthTrackTruthParticleAssociationTool
from RPVDispVrt.RPVDispVrtConf import D3PD__TruthTrackTruthNeutralinoAssociationTool
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__TruthTrackFillerTool as TruthTrackFiller

truthKey="TruthEvent"
if DoAOD:
    truthKey="GEN_AOD"
    pass
def makeRPVTruthTrackD3PDObject (name, prefix, object_name,
                                 getter = None,
##                                 sgkey = 'GEN_AOD',
##                                 sgkey = 'TruthEvent',
                                 sgkey=truthKey,
                                 typename = 'McEventCollection',
                                 label = 'mcTrk_'):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__TruthTrackGetterTool
        getter = D3PD__TruthTrackGetterTool (name + '_Getter',
                                             SGKey = sgkey,
                                             TypeName = typename,
                                             Label = label,
                                             KeepOnlyPrimaryParticles = False,
                                             KeepOnlyChargedParticles = False,
                                             KeepV0 = True,
                                             TrueTrackPtCut = 0.,
                                             TrueTrackEtaCut = 99999.,
                                             MaxRAll = 2000000,
                                             MaxZAll = 2000000,
                                             MaxRPrimary = 2000,
                                             MaxZPrimary = 2000)
        pass
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter)

#################################################################################

### Truth particles (everything, but no perigee info)
truPart = make_SGDataVector_D3PDObject \
          ('TruthParticleContainer',
           D3PDMakerFlags.TruthParticlesSGKey(),
           'mc_')

def _truthPartAlgHook (c, prefix, sgkey,
                       seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                       *args, **kw):
    truthParticleConfig (seq = seq, prefix = prefix, sgkey = sgkey)
    return
truPart.defineHook (_truthPartAlgHook)

truPart.defineBlock (0, 'TruthKin',
                     EventCommonD3PDMaker.FourMomFillerTool,
                     WriteRect = True,
                     WriteE = True,
                     WriteM = True)

truPart.defineBlock (0, 'TruthInfo',
                     EventCommonD3PDMaker.TruthParticleFillerTool)

ProdVertexAssoc = SimpleAssociation \
                  (truPart,
                   EventCommonD3PDMaker.TruthParticleProdVertexAssociationTool,
                   level = 1,
                   prefix = 'prod_vx_',
                   blockname = 'ProdVert')
ProdVertexAssoc.defineBlock (1, 'ProdVertPos',
                             EventCommonD3PDMaker.GenVertexFillerTool)
from RPVDispVrt.RPVDispVrtConf import D3PD__TruthParticleEndVertexAssociationTool
EndVertexAssoc = SimpleAssociation \
                 (truPart,
                  D3PD__TruthParticleEndVertexAssociationTool,
                  level = 1,
                  prefix = 'end_vx_',
                  blockname = 'EndVert')
EndVertexAssoc.defineBlock (1, 'EnddVertPos',
                            EventCommonD3PDMaker.GenVertexFillerTool)

TruTrackAssoc = IndexAssociation \
                (truPart,
                 D3PD__TruthParticleTruthTrackAssociationTool,
                 prefix = "mcTrk_",
                 target="mcTrk_")

ChildAssoc = TruthParticleChildAssociation(
    parent = truPart,
    prefix = 'child_',
    target="mc_",
    level = 0 )


#TruthPartTrackPartDRAssoc = DRIndexAssociation \
#                             (parent=truPart,
#                              type_name='Rec::TrackParticleContainer',
#                              default_sgkey = "TrackParticleCandidate",
#                              default_drcut = 0.03,
#                              prefix = 'trk_',
#                              target = 'trk' )

def _RPVTruthParticleChildAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_RPVTruthParticleChildAssociation', None)
    if assoc:
        assoc.Target = prefix
        pass
#    assoc.Target="mc_"
    return
truPart.defineHook(_RPVTruthParticleChildAssocHook)

ParentAssoc = TruthParticleParentAssociation(
    parent = truPart,
    prefix = 'parent_',
    target="mc_",
    level = 0 )

def _RPVTruthParticleParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_parent_RPVTruthParticleParentAssociation', None)
    if assoc:
        assoc.Target = prefix
        pass
    return
truPart.defineHook(_RPVTruthParticleParentAssocHook)


## Track Truth (only status == 1)
truTrack = D3PDObject (makeRPVTruthTrackD3PDObject, 'mcTrk_',"RPVTruthTrackD3PDObject",None,truthKey)
truTrack.defineBlock (0, 'TruthTrackGenParticleInfo', GenParticleFiller, prefix='gen_', WriteE = True, WriteM = False)
truTrack.defineBlock (0, 'TruthTrackPerigee',  GenParticlePerigeeFiller, prefix='perigee_')
truTrack.defineBlock (0, 'TruthTrackParameters',  TruthTrackFiller)

##truTrack = D3PDObject(makeRPVTruthTrackD3PDObject,'mcTrk_',prefix='mcTrk_',label='mcTrk_')
##truTrack.defineBlock (0, 'TruthTrackParameters',  TruthTrackFiller)

TruTrkTruPartAssoc = IndexAssociation\
                     (truTrack,
                      D3PD__TruthTrackTruthParticleAssociationTool,
                      target="mc_",
                      prefix="mc_")

TruTrkTruChiAssoc = IndexAssociation\
                    (truTrack,
                     D3PD__TruthTrackTruthNeutralinoAssociationTool,
                     target="mc_",
                     prefix="mc_LSP_mother_")


from RPVDispVrt.RPVDispVrtConf import D3PD__RPVVrtTruthParticleAssociationTool
RPVVrtTPAssoc = IndexAssociation\
                (DisplacedVertexD3PDObject,
                 D3PD__RPVVrtTruthParticleAssociationTool,
                 target="mc_",
                 prefix="mc_")

from RPVDispVrt.RPVDispVrtConf import D3PD__GenParticleTrackParticleAssociationTool
TruTrkTrkAssoc = IndexAssociation\
                 (truTrack,
                  D3PD__GenParticleTrackParticleAssociationTool,
                  target="trk",
                  prefix="trk_")


if DoTruthJet:
    from TruthD3PDMaker.TruthJetD3PDObject import TruthJetD3PDObject
