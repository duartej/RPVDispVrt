from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class RPVDispVrtFlags(JobPropertyContainer):
    """ RPV/LL DV flag/job property  container."""
jobproperties.add_Container(RPVDispVrtFlags)

RPVDVFlags=jobproperties.RPVDispVrtFlags



class InputFileName(JobProperty):
    statusOn = True
    allowedTypes = ["str"]
    StoredValue="dummy.root"
    
RPVDVFlags.add_JobProperty(InputFileName)

class OutputFileName(JobProperty):
    statusOn = True
    allowedTypes = ["str"]
    StoredValue="DispVtxD3PD.root"

RPVDVFlags.add_JobProperty(OutputFileName)

class DoTRTPixCut(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue=True
RPVDVFlags.add_JobProperty(DoTRTPixCut)    

class DoTRTPixPTCut(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue=True
RPVDVFlags.add_JobProperty(DoTRTPixPTCut)    

class DoFinalMerge(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue=True
RPVDVFlags.add_JobProperty(DoFinalMerge)    
