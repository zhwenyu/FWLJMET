import os,sys

####INCOMPLETE !! -- May 17, 2019

signaldict = {}
for mass in [str( x *100) for x in xrange(1,19)]:
	if mass=='1500':
		signaldict['TpTp'+mass] = '/TprimeTprime_M-'+mass+'_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
	else:
		signaldict['TpTp'+mass] = '/TprimeTprime_M-'+mass+'_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'

for mass in [str( x* 100) for x in xrange(1,19)]:
	if mass=='1500':
		signaldict['BpBp'+mass] = '/BprimeBprime_M-'+mass+'_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v3/MINIAODSIM'
	else:
		signaldict['BpBp'+mass] = '/BprimeBprime_M-'+mass+'_TuneCP5_PSweights_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'

bkgdict = {}

bkgdict['WZ'] = '/WZTo3LNu_TuneCP5_13TeV-powheg-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'
bkgdict['ZZ'] = '/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12-v1/MINIAODSIM'


bkglist = [

	#WZTo3LNu
	'',
	#ZZTo4L
	'',
	#WWW 4F
	'',
	#WWZ
	'',
	#WZZ
	'',
	#ZZZ
	'',
	#TTWJetsToLNu
	'',
	#TTZToLLNuNu M-10
	'',
	
	#WW
	'',

	#DYJetsToLL M-50 --> Julie has this	
	#WJetsToLNu --> Julie has this
	#TT --> Julis has this

    ]


datadict2018 = {}

datadict2018['EGammaRun2018A']  = '/EGamma/Run2018A-17Sep2018-v2/MINIAOD'
datadict2018['EGammaRun2018B']  = '/EGamma/Run2018B-17Sep2018-v1/MINIAOD'
datadict2018['EGammaRun2018C']  = '/EGamma/Run2018C-17Sep2018-v1/MINIAOD'
datadict2018['EGammaRun2018D']  = '/EGamma/Run2018D-PromptReco-v2/MINIAOD'

datadict2018['DoubleMuonRun2018A']  = '/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD'
datadict2018['DoubleMuonRun2018B']  = '/DoubleMuon/Run2018B-17Sep2018-v1/MINIAOD'
datadict2018['DoubleMuonRun2018C']  = '/DoubleMuon/Run2018C-17Sep2018-v1/MINIAOD'
datadict2018['DoubleMuonRun2018D']  = '/DoubleMuon/Run2018D-PromptReco-v2/MINIAOD'

datadict2018['MuonEGRun2018A']  = '/MuonEG/Run2018A-17Sep2018-v1/MINIAOD'
datadict2018['MuonEGRun2018B']  = '/MuonEG/Run2018B-17Sep2018-v1/MINIAOD'
datadict2018['MuonEGRun2018C']  = '/MuonEG/Run2018C-17Sep2018-v1/MINIAOD'
datadict2018['MuonEGRun2018D']  = '/MuonEG/Run2018D-PromptReco-v2/MINIAOD'


