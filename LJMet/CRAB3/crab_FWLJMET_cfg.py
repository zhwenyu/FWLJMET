from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'FWLJMET_TTTT_rizki'
config.General.workArea = 'crabSubmitLogs/'
config.General.transferLogs = True
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../runFWLJMet_singleLep.py'
config.JobType.pyCfgParams = ['isMC=True','isTTbar=False','maxEvents=-1']

config.section_("Data")
config.Data.inputDataset = '/TTTT_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
config.Data.allowNonValidInputDataset = True
config.Data.splitting = 'FileBased'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 1
config.Data.ignoreLocality = False
config.Data.publication = False
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'FWLJMET_TTTT_04112019_rizki_TESTDELETEME'
config.Data.outLFNDirBase = '/store/group/lpcljm/FWLJMET_crab_test/'

config.section_("Site")
config.Site.storageSite = 'T3_US_FNALLPC'
