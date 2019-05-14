from WMCore.Configuration import Configuration
config = Configuration()

import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

inputDataset = '/TprimeTprime_M-1400_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
isVLQsignal = True
requestName = 'FWLJMET_1Lep_'+date_str+'_rizki'
outputFolder = 'FWLJMET_crab_test'

config.section_("General")
config.General.requestName = requestName
config.General.workArea = 'crabSubmitLogs/'
config.General.transferLogs = True
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
if(isVLQsignal):config.JobType.scriptExe = 'crab_script.sh'
config.JobType.psetName = '../runFWLJMet_singleLep.py'
config.JobType.pyCfgParams = ['isMC=True','isTTbar=False','maxEvents=100']
config.JobType.maxMemoryMB = 2500 #MB
config.JobType.maxJobRuntimeMin = 2750 #minutes
config.JobType.numCores = 4 #use wisely if turned on.

config.section_("Data")
config.Data.inputDataset = inputDataset
config.Data.allowNonValidInputDataset = True
# config.Data.splitting = 'Automatic'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = False
config.Data.publication = False
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = requestName
config.Data.outLFNDirBase = '/store/group/lpcljm/'+outputFolder

config.section_("Site")
config.Site.storageSite = 'T3_US_FNALLPC'
