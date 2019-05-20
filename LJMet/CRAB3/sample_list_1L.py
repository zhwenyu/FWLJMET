import os,sys

   # <<< EWK bcks >>>
	#WZTo3LNu
	#WWW 4F
	#WWZ
	#WZZ
	#ZZZ
	#WW
	#ZZTo4L

    #s-channel
    #t-channel
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

#DYJetsToLL M-50
#bkgdict['DYM50'] = '/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM' (inclusive)
#bkgdict['DYM50HT70to100'] = '/DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM' (below acceptance)
#bkgdict['DYM50HT100to200'] = '/DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM' (below acceptance)
bkgdict['DYM50HT200to400'] = '/DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
bkgdict['DYM50HT400to600'] = '/DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext2-v3/MINIAODSIM'
bkgdict['DYM50HT600to800'] = '/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
bkgdict['DYM50HT800to1200'] = '/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
bkgdict['DYM50HT1200to2500'] = '/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'
bkgdict['DYM50HT2500toInf'] = '/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM'

#WJetsToLNu
#bkgdict['WJets'] = '/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM' (inclusive)
#bkgdict['WJetsHT70to100'] = '/WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM' (below acceptance)
#bkgdict['WJetsHT100to200'] = '/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM' (below acceptance)
bkgdict['WJetsHT200to400'] = '/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
bkgdict['WJetsHT400to600'] = '/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
bkgdict['WJetsHT600to800'] = '/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
bkgdict['WJetsHT800to1200'] = '/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
bkgdict['WJetsHT1200to2500'] = '/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
bkgdict['WJetsHT2500toInf'] = '/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'

#TTWJets
bkgdict['TTW'] = '/TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM'
#TTZ
bkgdict['TTZM10'] = '/TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM'
bkgdict['TTZM1to10'] = '/TTZToLL_M-1to10_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'

# <<< SM single top bck >>>
#tW
bkgdict['ST_tW_antitop'] = '/ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v1/MINIAODSIM'
bkgdict['ST_tW_top'] = '/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v1/MINIAODSIM'


ttbarbkgdict = {}

# <<< SM ttbar bck >>>
#(only TT_Mtt-700to1000 is available for Autumn2018, so use inclusive samples) - 20 May, 2019
ttbarbkgdict['TTToHadronic'] = '/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
ttbarbkgdict['TTToSemiLeptonic'] = '/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
ttbarbkgdict['TTTo2L2Nu'] = '/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'


datadict2018 = {}

datadict2018['EGammaRun2018A']  = '/EGamma/Run2018A-17Sep2018-v2/MINIAOD'
datadict2018['EGammaRun2018B']  = '/EGamma/Run2018B-17Sep2018-v1/MINIAOD'
datadict2018['EGammaRun2018C']  = '/EGamma/Run2018C-17Sep2018-v1/MINIAOD'
datadict2018['EGammaRun2018D']  = '/EGamma/Run2018D-PromptReco-v2/MINIAOD'

datadict2018['SingleMuonRun2018A']  = '/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD'
datadict2018['SingleMuonRun2018B']  = '/DoubleMuon/Run2018B-17Sep2018-v1/MINIAOD'
datadict2018['SingleMuonRun2018C']  = '/DoubleMuon/Run2018C-17Sep2018-v1/MINIAOD'
datadict2018['SingleMuonRun2018D']  = '/DoubleMuon/Run2018D-PromptReco-v2/MINIAOD'



