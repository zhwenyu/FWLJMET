from WMCore.Configuration import Configuration
config = Configuration()

import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

inputDataset = INPUT
requestName = REQNAME
outputFolder = OUTFOLDER
isMC = ISMC
isVLQsignal = False
if 'Tprime' in inputDataset or 'Bprime' in inputDataset: isVLQsignal=True


config.section_("General")
config.General.requestName = requestName
config.General.workArea = 'crabSubmitLogs/'
config.General.transferLogs = True
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../runFWLJMet_singleLep.py'
if(isVLQsignal): config.JobType.scriptExe = 'crab_script.sh'
if(isMC):config.JobType.pyCfgParams = ['isMC=True']
else:config.JobType.pyCfgParams = ['isMC=False']
config.JobType.maxMemoryMB = 4000 #MB
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
if(isMC==False):config.Data.lumiMask = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt" #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis#DATA
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = requestName
config.Data.outLFNDirBase = '/store/group/lpcljm/'+outputFolder

config.section_("Site")
config.Site.storageSite = 'T3_US_FNALLPC'
