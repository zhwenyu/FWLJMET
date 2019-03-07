import FWCore.ParameterSet.Config as cms

process = cms.Process("LJMET")

## MessageLogger
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 20

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

## Maximal Number of Events
MAXEVENTS = 100
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(MAXEVENTS) )

## Source / Input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/00000/8EA8FE89-254F-E811-835E-0090FAA58BF4.root',
    )
)

OUTFILENAME = 'ljmet_TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8_test_producer_MC.root'
# TFileService
process.TFileService = cms.Service("TFileService", fileName = cms.string(OUTFILENAME))
## Output Module Configuration (expects a path 'p')
# process.out = cms.OutputModule("PoolOutputModule",
#                                fileName = cms.untracked.string(OUTFILENAME),
#                                #SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                                #outputCommands = cms.untracked.vstring('keep *')
#                                )


process.ljmet = cms.EDAnalyzer(
	'LJMet',

	debug         = cms.bool(True),
	selector      = cms.string('TestSelector'),
	exclude_calcs = cms.vstring(
			'DummyCalc',	
	),
	
	test_selector = cms.PSet(
	
		debug  = cms.bool(True),

		HLTcollection      = cms.InputTag("TriggerResults","","HLT"),
		muonsCollection     = cms.InputTag("slimmedMuons"),
		electronsCollection = cms.InputTag("slimmedElectrons"),
		
		HLTtargets    = cms.vstring(
    	#MuMu        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',    #exists in 2017  (PreScaled!)        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',  #exists in 2017 (PreScaled!)        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v',                      
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',           
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',        
    	#ElEl
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v',  #exists in 2017    
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v', #exists in 2017        
		#MuEl
        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017 
		'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017 
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017  
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017
        #for trig efficiency
        'HLT_IsoMu24_v',
        'HLT_IsoTkMu24_v',
        'HLT_IsoMu27_v',
        'HLT_Ele27_WPTight_Gsf_v',
        'HLT_Ele35_WPTight_Gsf_v',
        ),
        
        minLeptons = cms.int32(3),

        min_muPt   = cms.double(20.), 
        max_muEta  = cms.double(2.4), 
        min_elPt   = cms.double(20.), 
        max_elEta  = cms.double(2.4), 
        
	),
	
	TestCalc = cms.PSet( # name has to match the calculator name as registered in Calc.cc
	
		debug  = cms.bool(True),

		electronsCollection = cms.InputTag("slimmedElectrons"),
	
	),

	DummyCalc = cms.PSet( # name has to match the calculator name as registered in Calc.cc	

		debug  = cms.bool(True),
	
	),

	
)


#added by rizki - this part is not necessary for FWLJMET !
process.skimMiniAOD = cms.EDFilter(
	"SkimMiniAOD",
	debug					= cms.bool(True),
	
	filter_by_minLeptons	= cms.bool(True),
	muonCollection 			= cms.InputTag("slimmedMuons"),
	electronCollection		= cms.InputTag("slimmedElectrons"),
	minLeptons				= cms.int32(3),

	filter_by_HLT			= cms.bool(True),
	HLT						= cms.InputTag("TriggerResults","","HLT"),
    HLTpaths 				= cms.vstring(
    	#MuMu        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',    #exists in 2017  (PreScaled!)        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',  #exists in 2017 (PreScaled!)        
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v',                      
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',           
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',        
    	#ElEl
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v',  #exists in 2017    
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v', #exists in 2017        
		#MuEl
        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017 
		'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017 
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017  
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017
        #for trig efficiency
        'HLT_IsoMu24_v',
        'HLT_IsoTkMu24_v',
        'HLT_IsoMu27_v',
        'HLT_Ele27_WPTight_Gsf_v',
        'HLT_Ele35_WPTight_Gsf_v',
        ),
	
	)


# Configure a path and endpath to run the producer and output modules
process.p = cms.Path(
	process.ljmet
#     *process.skimMiniAOD - this is not necessary for FWLJMET !
)
#process.ep = cms.EndPath(process.out)