#--------------------------------------------------------------
# Job configuration
#--------------------------------------------------------------

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
ServiceMgr.MessageSvc.defaultLimit  = 9999999

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#athenaCommonFlags.FilesInput = [ '/tmp/nbarlow/ESD.092064._000001.pool.root.1']
athenaCommonFlags.FilesInput = [
    #    '/home/nbarlow/AOD/mc09_7TeV.109279.J3_pythia_jetjet_1muon.recon.AOD.e534_s765_s767_r1302_tid137138_00/AOD.137138._000290.pool.root.1'
    "/home/nbarlow/ESD/group.perf-idtracking.mc11_valid.114006.Pythia_RPV_vtx2_LSP500_filtDL.e825_s1310_s1300_d578.pu0.17p0p2p10.cutLevel4.v1.110919115344_EXT1/group.perf-idtracking.33771_004507.EXT1._00017.ESD.root"
#    '/home/nbarlow/ESD/mc10_7TeV.114006.Pythia_RPV_vtx2_LSP500_filtDL.recon.ESD.e574_s933_s946_r1801_tid209795_00/ESD.209795._000001.pool.root.1'
#    '/home/nbarlow/AOD/data10_7TeV.00161379.physics_MinBias.merge.AOD.r1647_p306_p307_tid187847_00/AOD.187847._000001.pool.root.1'
    ]

#athenaCommonFlags.FilesInput = [ '/tmp/nbarlow/myESD_MuonswBeam_156xy.pool.root']
#athenaCommonFlags.FilesInput = ['/tmp/nbarlow/data10_7TeV.00152166.physics_MinBias.recon.ESD.r1297_tid135296_00/ESD.135296._000480.pool.root.1']

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'RPVDispVrtAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

from RecExConfig.RecFlags import rec

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags

include ("RecExCommon/RecExCommon_topOptions.py")

## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#### optional pre-filtering before running the vertexing etc.
#include("RPVDispVrt/RPVFilter_jobOptions.py")
### vertexing
include("RPVDispVrt/RPVVertexing_jobOptions.py")
### ntuple to debug vertexing code
#include("RPVDispVrt/RPVDebugNtuple_jobOptions.py")
### D3PD-making code
include("RPVDispVrt/RPVNtuple_jobOptions.py")


## Configure input
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
theApp.EvtMax = 10
ServiceMgr.EventSelector.SkipEvents=0
