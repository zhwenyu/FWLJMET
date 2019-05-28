import os,sys


signaldict = {}

##VLQ TT
signaldict['TpTp1800'] = '/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1700'] = '/TprimeTprime_M-1700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1600'] = '/TprimeTprime_M-1600_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1500'] = '/TprimeTprime_M-1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1400'] = '/TprimeTprime_M-1400_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1300'] = '/TprimeTprime_M-1300_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1200'] = '/TprimeTprime_M-1200_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1100'] = '/TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['TpTp1000'] = '/TprimeTprime_M-1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM'
signaldict['TpTp700'] = '/TprimeTprime_M-700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM'

##VLQ BB
signaldict['BpBp1800'] = '/BprimeBprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1700'] = '/BprimeBprime_M-1700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
signaldict['BpBp1600'] = '/BprimeBprime_M-1600_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1500'] = '/BprimeBprime_M-1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1400'] = '/BprimeBprime_M-1400_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1300'] = '/BprimeBprime_M-1300_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1200'] = '/BprimeBprime_M-1200_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1100'] = '/BprimeBprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp1000'] = '/BprimeBprime_M-1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
signaldict['BpBp900']  = '/BprimeBprime_M-900_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'

bkgdict = {}

bkgdict['WZ'] = '/WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
bkgdict['ZZ'] = '/ZZTo4L_13TeV_powheg_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
bkgdict['WWW'] = '/WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
bkgdict['WWZ'] = '/WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
bkgdict['WZZ'] = '/WZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
bkgdict['ZZZ'] = '/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
bkgdict['TTW'] = '/TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
bkgdict['TTZ'] = '/TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
#bkgdict['WW'] = ''

ttbarbkgdict = {}

datadict = {}

datadict['DoubleEGRun2018B']  = '/DoubleEG/Run2017B-31Mar2018-v1/MINIAOD'
datadict['DoubleEGRun2018C']  = '/DoubleEG/Run2017C-31Mar2018-v1/MINIAOD'
datadict['DoubleEGRun2018D']  = '/DoubleEG/Run2017D-31Mar2018-v1/MINIAOD'
datadict['DoubleEGRun2018E']  = '/DoubleEG/Run2017E-31Mar2018-v1/MINIAOD'
datadict['DoubleEGRun2018F']  = '/DoubleEG/Run2017F-31Mar2018-v1/MINIAOD'

datadict['DoubleMuonRun2018B']  = '/DoubleMuon/Run2017B-31Mar2018-v1/MINIAOD'
datadict['DoubleMuonRun2018C']  = '/DoubleMuon/Run2017C-31Mar2018-v1/MINIAOD'
datadict['DoubleMuonRun2018D']  = '/DoubleMuon/Run2017D-31Mar2018-v1/MINIAOD'
datadict['DoubleMuonRun2018E']  = '/DoubleMuon/Run2017E-31Mar2018-v1/MINIAOD'
datadict['DoubleMuonRun2018F']  = '/DoubleMuon/Run2017F-31Mar2018-v1/MINIAOD'

datadict['MuonEGRun2018B']  = '/MuonEG/Run2017B-31Mar2018-v1/MINIAOD'
datadict['MuonEGRun2018C']  = '/MuonEG/Run2017C-31Mar2018-v1/MINIAOD'
datadict['MuonEGRun2018D']  = '/MuonEG/Run2017D-31Mar2018-v1/MINIAOD'
datadict['MuonEGRun2018E']  = '/MuonEG/Run2017E-31Mar2018-v1/MINIAOD'
datadict['MuonEGRun2018F']  = '/MuonEG/Run2017F-31Mar2018-v1/MINIAOD'


