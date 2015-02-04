D3PDMakerFlags.PhotonSGKey="PhotonCollection"
from egammaD3PDMaker.PhotonD3PDObject                import PhotonD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                import PhotonClusterAssoc
from egammaD3PDMaker.egammaTriggerBitsD3PDObject     import egammaTriggerBitsD3PDObject
from D3PDMakerCoreComps.IndexAssociation			 import IndexAssociation
from EventCommonD3PDMaker.DRIndexAssociation 		 import DRIndexAssociation
	
# electronIncludes = ['Kinematics', 'Charge','Author', 'Iso','TrkMatch','Retaphi','Trk', 'IDHits','BeamSpotImpact','Pointing','ClusterKin','ClusterTime','PAUVariables', 'PAU', 'EFIndex']
# electronIncludes = ['Kinematics','Charge','Author','Iso','TrkMatch','Retaphi','Trk','IDHits','BeamSpotImpact','Pointing','ClusterKin','ClusterTime','PAUVariables','PAU','EFIndex','TruthClassification','IsEM','Conversion0','Samplings','IsEMCuts','Layer3Shape','ConvFlags','Conversion','FwdEVars','Truth','TruthBrem','HadLeakage','Layer1Shape','Layer2Shape','Layer1ShapeExtra']

PhotonTrackPartDRAssoc = DRIndexAssociation \
    (parent=PhotonD3PDObject,
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


if DoTruth:
    TrackPartPhotonDRAssoc = DRIndexAssociation \
                           (parent=TrackWithTruthD3PDObject,
                            type_name='PhotonContainer',
                            default_sgkey = D3PDMakerFlags.PhotonSGKey(),
                            default_drcut = 0.03,
                            prefix = 'ph_',
                            target = 'ph_' )
    PhotonTruthTrackAssoc = IndexAssociation \
                          (PhotonD3PDObject,
                           egammaD3PDMaker.egammaGenParticleAssociationTool,
                           Classifier = D3PDMCTruthClassifier,
                           target = 'mcTrk_',
                           prefix = 'mcTrk_',
                           DRVar = 'dr' )
    PhotonTruthPartDRAssoc = DRIndexAssociation \
                          (parent=PhotonD3PDObject,
                           type_name='TruthParticleContainer',
                           default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
                           default_drcut = 0.03,
                           prefix = 'mc_',
                           target = 'mc_')

else:
    TrackPartPhotonDRAssoc = DRIndexAssociation \
    (parent=TrackParticleD3PDObject,
     type_name='PhotonContainer',
     default_sgkey = D3PDMakerFlags.PhotonSGKey(),
     default_drcut = 0.03,
     prefix = 'ph_',
     target = 'ph_' )
	 
from RPVDispVrt.RPVDispVrtConf import D3PD__RPVVrtPhotonDistAssociationTool
RPVVrtTPAssoc = IndexMultiAssociation\
    (DisplacedVertexD3PDObject,
     D3PD__RPVVrtPhotonDistAssociationTool,
     target="ph_",
     prefix="ph_")
