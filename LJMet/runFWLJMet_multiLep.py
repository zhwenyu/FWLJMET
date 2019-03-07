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



process.ljmet = cms.EDAnalyzer(
	'LJMet',

	debug         = cms.bool(True),
	selector      = cms.string('MultiLepSelector'),
	include_calcs = cms.vstring(
			'MultiLepCalc',
	),
	exclude_calcs = cms.vstring(
			'TestCalc',
			'DummyCalc',
	),

	MultiLepSelector = cms.PSet( # name has to match the name as registered in BeginJob of  EventSelector.cc

            debug  = cms.bool(True),

            isMc  = cms.bool(True),
            
            HLTcollection       = cms.InputTag("TriggerResults","","HLT"),
            muonsCollection     = cms.InputTag("slimmedMuons"),
            electronsCollection = cms.InputTag("slimmedElectrons"),
            
			# Trigger cuts
			trigger_cut  = cms.bool(True),
			dump_trigger = cms.bool(True),

			mctrigger_path_el = cms.vstring(        
				'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v',  #exists in 2017    
				'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v', #exists in 2017        

				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017 
				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017 
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017  
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017

				#for trig efficiency
				'HLT_Ele27_WPTight_Gsf_v',
				'HLT_Ele35_WPTight_Gsf_v',
				),
			mctrigger_path_mu = cms.vstring(
				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017 
				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017 
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017  
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017

				'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',    #exists in 2017  (PreScaled!)        
				'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',  #exists in 2017 (PreScaled!)        
				'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v',                      
				'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',           
				'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',        

				#for trig efficiency
				'HLT_IsoMu24_v',
				'HLT_IsoTkMu24_v',
				'HLT_IsoMu27_v',
				),

			trigger_path_el = cms.vstring(''),
			trigger_path_mu = cms.vstring(''),   
            

            minLeptons = cms.int32(3),

            min_muPt   = cms.double(20.),
            max_muEta  = cms.double(2.4),
            min_elPt   = cms.double(20.),
            max_elEta  = cms.double(2.4),

            ),

	MultiLepCalc = cms.PSet( # name has to match the calculator name as registered in Calc.cc

	    debug  = cms.bool(True),
            electronsCollection = cms.InputTag("slimmedElectrons"),

	),


)



# Configure a path and endpath to run the producer and output modules
process.p = cms.Path(
	process.ljmet
)
