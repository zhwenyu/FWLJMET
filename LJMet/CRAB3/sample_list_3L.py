import os,sys

####INCOMPLETE !! -- May 17, 2019

signaldict = {}
signaldict['TpTp1800'] = '/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM'

samplelist = [

	# TT
   '/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1600_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1400_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1300_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1200_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM',
   '/TprimeTprime_M-1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM',

]

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


datalist2018 = {}

datalist2018['EGammaRun2018A']  = '/EGamma/Run2018A-22Jun2018-v1/MINIAOD',
datalist2018['DoubleMuonRun2018A']  = '/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD'
datalist2018['MuonEGRun2018A']  = '/MuonEG/Run2018A-PromptReco-v3/MINIAOD'

datalistRunA = [
'/EGamma/Run2018A-22Jun2018-v1/MINIAOD',
'/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD',
'/MuonEG/Run2018A-PromptReco-v3/MINIAOD'
]
datalistRunB = [
'/EGamma/Run2018B-17Sep2018-v1/MINIAOD',
'/EGamma/Run2018B-26Sep2018-v1/MINIAOD',
'/DoubleMuon/Run2018B-17Sep2018-v1/MINIAOD',
'/MuonEG/Run2018B-PromptReco-v1/MINIAOD', #chose this because it had more files
]
datalistRunC = [
'/EGamma/Run2018C-17Sep2018-v1/MINIAOD',
'/DoubleMuon/Run2018C-17Sep2018-v1/MINIAOD',
'/MuonEG/Run2018C-PromptReco-v3/MINIAOD',
]
datalistRunD = [
'/EGamma/Run2018D-PromptReco-v2/MINIAOD',
'/DoubleMuon/Run2018D-PromptReco-v2/MINIAOD',
'/MuonEG/Run2018D-PromptReco-v2/MINIAOD',
]


