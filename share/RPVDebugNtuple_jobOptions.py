theApp.HistogramPersistency = 'ROOT'
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "RPVDEBUG DATAFILE='debugNtuple.root' TYPE='ROOT' OPT='RECREATE'" ]
theApp.Dlls += [ 'RootHistCnv' ]
