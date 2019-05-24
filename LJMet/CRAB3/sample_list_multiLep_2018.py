import os,sys


signaldict = {}

##VLQ TT
signaldict['TpTp1800'] = '/TprimeTprime_M-1800_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1700'] = '/TprimeTprime_M-1700_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1600'] = '/TprimeTprime_M-1600_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1500'] = '/TprimeTprime_M-1500_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
signaldict['TpTp1400'] = '/TprimeTprime_M-1400_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1300'] = '/TprimeTprime_M-1300_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1200'] = '/TprimeTprime_M-1200_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1100'] = '/TprimeTprime_M-1100_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['TpTp1000'] = '/TprimeTprime_M-1000_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
#signaldict['TpTp900'] = '/TprimeTprime_M-900_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM' (excluded)

##VLQ BB
signaldict['BpBp1800'] = '/BprimeBprime_M-1800_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1700'] = '/BprimeBprime_M-1700_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1600'] = '/BprimeBprime_M-1600_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v3/MINIAODSIM'
signaldict['BpBp1500'] = '/BprimeBprime_M-1500_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1400'] = '/BprimeBprime_M-1400_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1300'] = '/BprimeBprime_M-1300_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1200'] = '/BprimeBprime_M-1200_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1100'] = '/BprimeBprime_M-1100_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
signaldict['BpBp1000'] = '/BprimeBprime_M-1000_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
#signaldict['BpBp900'] = '/BprimeBprime_M-900_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM' (excluded)

bkgdict = {}

bkgdict['WZ'] = '/WZTo3LNu_TuneCP5_13TeV-powheg-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['ZZ'] = '/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['WWW'] = '/WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['WWZ'] = '/WWZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['WZZ'] = '/WZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['ZZZ'] = '/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['TTW'] = '/TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['TTZ'] = '/TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
#bkgdict['WW'] = ''

ttbarbkgdict = {}

datadict = {}

datadict['EGammaRun2018A']  = '/EGamma/Run2018A-17Sep2018-v2/MINIAOD'
datadict['EGammaRun2018B']  = '/EGamma/Run2018B-17Sep2018-v1/MINIAOD'
datadict['EGammaRun2018C']  = '/EGamma/Run2018C-17Sep2018-v1/MINIAOD'
datadict['EGammaRun2018D']  = '/EGamma/Run2018D-PromptReco-v2/MINIAOD'

datadict['DoubleMuonRun2018A']  = '/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD'
datadict['DoubleMuonRun2018B']  = '/DoubleMuon/Run2018B-17Sep2018-v1/MINIAOD'
datadict['DoubleMuonRun2018C']  = '/DoubleMuon/Run2018C-17Sep2018-v1/MINIAOD'
datadict['DoubleMuonRun2018D']  = '/DoubleMuon/Run2018D-PromptReco-v2/MINIAOD'

datadict['MuonEGRun2018A']  = '/MuonEG/Run2018A-17Sep2018-v1/MINIAOD'
datadict['MuonEGRun2018B']  = '/MuonEG/Run2018B-17Sep2018-v1/MINIAOD'
datadict['MuonEGRun2018C']  = '/MuonEG/Run2018C-17Sep2018-v1/MINIAOD'
datadict['MuonEGRun2018D']  = '/MuonEG/Run2018D-PromptReco-v2/MINIAOD'


