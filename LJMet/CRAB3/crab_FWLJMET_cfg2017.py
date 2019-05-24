from WMCore.Configuration import Configuration
config = Configuration()

import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

import os
userName = os.environ['USER']

####################
### SET YOUR STRINGS
####################
cmsRun_config  = '../runFWLJMet_singleLep_multipleTree.py'
inputDataset   = '/TprimeTprime_M-1400_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'
requestName    = 'FWLJMET_1Lep_'+date_str+'_'+userName
outputFolder   = 'FWLJMET_crab_test'
Json_for_data  = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PromptReco/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSONtxt'
isMC           = True
isVLQsignal    = False
isTTbar        = False

if 'Tprime' in inputDataset or 'Bprime' in inputDataset: isVLQsignal=True

##############
### GENERAL
##############
config.section_("General")
config.General.requestName = requestName
config.General.workArea = 'crabSubmitLogs/'
config.General.transferLogs = True
config.General.transferOutputs = True


##############
### JobType
##############
config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = cmsRun_config

#cmsRun params
if(isMC):
	config.JobType.pyCfgParams = ['isMC=True']
else:
	config.JobType.pyCfgParams = ['isMC=False']

if(isTTbar):
	config.JobType.pyCfgParams += ['isTTbar=True']

#for VLQ signal this will run using crab_script.sh which will reset the env var in order to access LHApdf outside of CMSSW
if(isVLQsignal):
	config.JobType.scriptExe = 'crab_script.sh'
else:
	config.JobType.pyCfgParams += ['isVLQsignal=False']
	
# runtime, memory, cores
if(isMC):
	config.JobType.maxJobRuntimeMin = 2750 #minutes
config.JobType.maxMemoryMB = 4000 #MB
config.JobType.numCores = 4 #use wisely if turned on.

##############
### DATA
##############
config.section_("Data")
config.Data.inputDataset = inputDataset
config.Data.allowNonValidInputDataset = True
if(isMC):
	config.Data.splitting = 'FileBased'
	config.Data.unitsPerJob = 1
else:
	config.Data.splitting = 'Automatic'
	config.Data.lumiMask = Json_for_data
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = False
config.Data.publication = False
# This string is used to construct the output dataset name : /store/user/lpcljm/<outputFolder>/<inpuDataset>/<requestName>/<someCRABgeneratedNumber>/<someCRABgeneratedNumber>/
config.Data.outputDatasetTag = requestName
config.Data.outLFNDirBase = '/store/group/lpcljm/'+outputFolder

config.section_("Site")
config.Site.storageSite = 'T3_US_FNALLPC'
