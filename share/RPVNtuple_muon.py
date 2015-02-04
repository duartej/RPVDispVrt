from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from EventCommonD3PDMaker.DRIndexAssociation  import DRIndexAssociation
from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject

MuonTrackPartDRAssoc = DRIndexAssociation \
    (parent=MuonD3PDObject,
     type_name='Rec::TrackParticleContainer',
     default_sgkey = "TrackParticleCandidate",
     default_drcut = 0.03,
     prefix = 'trk_',
     target = 'trk' )


if DoTruth:
    from TruthD3PDMaker.MCTruthClassifierConfig import D3PDMCTruthClassifier

    if not DoAOD:
        D3PDMCTruthClassifier.McEventCollection="TruthEvent"
        pass



    TrackPartMuonDRAssoc = DRIndexAssociation \
                           (parent=TrackWithTruthD3PDObject,
                            type_name='Analysis::MuonContainer',
                            default_sgkey = D3PDMakerFlags.MuonSGKey(),
                            default_drcut = 0.03,
                            prefix = 'mu_',
                            target = 'mu_' )
    MuonTruthTrackAssoc = IndexAssociation \
                          (MuonD3PDObject,
                           MuonD3PDMaker.MuonGenParticleAssociationTool,
                           Classifier = D3PDMCTruthClassifier,
                           prefix = 'mcTrk_',
                           target = 'mcTrk_',
                           DRVar = 'dr')    
#    MuonTruthTrackAssoc = IndexAssociation \
#                          (MuonD3PDObject,
#                           MuonD3PDMaker.MuonGenParticleAssociationTool,
#                           target = 'mcTrk_',
#                           prefix = 'mcTrk_',
#                           DRVar = 'dr' )
#    MuonTruthTrackAssoc = DRIndexAssociation \
#                          (parent=MuonD3PDObject,
#                           type_name='GenParticleContainer',
#                           default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
#                           default_drcut = 0.03,
#                           prefix = 'mc_',
#                           target = 'mc_')
#    MuonTruthPartDRAssoc = DRIndexAssociation \
#                          (parent=MuonD3PDObject,
#                           type_name='TruthParticleContainer',
#                           default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
#                           default_drcut = 0.03,
#                           prefix = 'mc_',
#                           target = 'mc_')

else:
    TrackPartMuonDRAssoc = DRIndexAssociation \
    (parent=TrackParticleD3PDObject,
     type_name='Analysis::MuonContainer',
     default_sgkey = D3PDMakerFlags.MuonSGKey(),
     default_drcut = 0.03,
     prefix = 'mu_',
     target = 'mu_' )


from RPVDispVrt.RPVDispVrtConf import D3PD__RPVVrtMuonDistAssociationTool
RPVVrtTPAssoc = IndexMultiAssociation\
    (DisplacedVertexD3PDObject,
     D3PD__RPVVrtMuonDistAssociationTool,
     target="mu_",
     prefix="mu_")


