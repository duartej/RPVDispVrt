#from PrimaryDPDMaker.TriggerFilter import TriggerFilter
#topSequence += TriggerFilter( "DVAnaTriggerFilter",
#                              trigger = "EF_mu40_MSonly" )

from AthenaCommon.AlgSequence import AthSequencer
seq=AthSequencer("AthFilterSeq")

from GoodRunsListsUser.GoodRunsListsUserConf import *
seq += TriggerSelectorAlg('TriggerAlg1')

seq.TriggerAlg1.TriggerSelection = 'EF_mu40_MSonly_barrel_medium'
#seq.TriggerAlg1.OutputLevel=DEBUG
