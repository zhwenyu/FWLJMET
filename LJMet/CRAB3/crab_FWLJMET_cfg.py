from WMCore.Configuration import Configuration
config = Configuration()

import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

#import os
#userName = os.environ['USER']

#inputDataset = '/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
#isVLQsignal = False
#requestName = 'FWLJMET_1Lep_'+date_str+'_'+userName
#outputFolder = 'FWLJMET_crab_test'
#isMC = True
#isVLQsignal = False
#if 'Tprime' in inputDataset or 'Bprime' in inputDataset: isVLQsignal=True
####################
### SET YOUR STRINGS
####################
cmsRun_config  = '../runFWLJMet_multiLep_multipleTree.py'
inputDataset   = '/TprimeTprime_M-1400_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
requestName    = 'FWLJMET_3Lep_'+date_str+'_rizki'
outputFolder   = 'FWLJMET_crab_test'
Json_for_data  = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt" #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis#DATA
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

#for VLQ signal this will run using crab_script.sh which will reset the env var in order to access LHApdf outside of CMSSW
if(isVLQsignal):
	config.JobType.scriptExe = 'crab_script.sh'

#cmsRun params
if(isMC):
	config.JobType.pyCfgParams = ['isMC=True']
else:
	config.JobType.pyCfgParams = ['isMC=False']

if(isTTbar):
	config.JobType.pyCfgParams += ['isTTbar=True']

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
