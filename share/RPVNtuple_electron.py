D3PDMakerFlags.ElectronSGKey="ElectronCollection"
from egammaD3PDMaker.ElectronD3PDObject              import ElectronD3PDObject
from egammaD3PDMaker.ElectronD3PDObject              import ElectronClusterAssoc
from egammaD3PDMaker.egammaTriggerBitsD3PDObject     import egammaTriggerBitsD3PDObject
from D3PDMakerCoreComps.IndexAssociation			 import IndexAssociation
from EventCommonD3PDMaker.DRIndexAssociation 		 import DRIndexAssociation
	


# electronIncludes = ['Kinematics', 'Charge','Author', 'Iso','TrkMatch','Retaphi','Trk', 'IDHits','BeamSpotImpact','Pointing','ClusterKin','ClusterTime','PAUVariables', 'PAU', 'EFIndex']
# electronIncludes = ['Kinematics','Charge','Author','Iso','TrkMatch','Retaphi','Trk','IDHits','BeamSpotImpact','Pointing','ClusterKin','ClusterTime','PAUVariables','PAU','EFIndex','TruthClassification','IsEM','Conversion0','Samplings','IsEMCuts','Layer3Shape','ConvFlags','Conversion','FwdEVars','Truth','TruthBrem','HadLeakage','Layer1Shape','Layer2Shape','Layer1ShapeExtra']


ElectronTrackPartDRAssoc = DRIndexAssociation \
    (parent=ElectronD3PDObject,
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
    TrackPartElectronDRAssoc = DRIndexAssociation \
                           (parent=TrackWithTruthD3PDObject,
                            type_name='ElectronContainer',
                            default_sgkey = D3PDMakerFlags.ElectronSGKey(),
                            default_drcut = 0.03,
                            prefix = 'el_',
                            target = 'el_' )
    ElectronTruthTrackAssoc = IndexAssociation \
                          (ElectronD3PDObject,
                           egammaD3PDMaker.egammaGenParticleAssociationTool,
                           Classifier = D3PDMCTruthClassifier,
                           target = 'mcTrk_',
                           prefix = 'mcTrk_',
                           DRVar = 'dr' )
    ElectronTruthPartDRAssoc = DRIndexAssociation \
                          (parent=ElectronD3PDObject,
                           type_name='TruthParticleContainer',
                           default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
                           default_drcut = 0.03,
                           prefix = 'mc_',
                           target = 'mc_')

else:
    TrackPartElectronDRAssoc = DRIndexAssociation \
    (parent=TrackParticleD3PDObject,
     type_name='ElectronContainer',
     default_sgkey = D3PDMakerFlags.ElectronSGKey(),
     default_drcut = 0.03,
     prefix = 'el_',
     target = 'el_' )
	 
from RPVDispVrt.RPVDispVrtConf import D3PD__RPVVrtElectronDistAssociationTool
RPVVrtTPAssoc = IndexMultiAssociation\
    (DisplacedVertexD3PDObject,
     D3PD__RPVVrtElectronDistAssociationTool,
     target="el_",
     prefix="el_")
