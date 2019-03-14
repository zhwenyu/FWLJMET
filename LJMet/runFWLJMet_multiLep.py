import FWCore.ParameterSet.Config as cms
import os


relBase = os.environ['CMSSW_BASE']

process = cms.Process("LJMET")

## MessageLogger
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 20

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

## Maximal Number of Events
MAXEVENTS = 200
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(MAXEVENTS) )

## Source / Input
isMC=True
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/00000/8EA8FE89-254F-E811-835E-0090FAA58BF4.root',
    )
)

OUTFILENAME = 'TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8'
# TFileService
process.TFileService = cms.Service("TFileService", fileName = cms.string(OUTFILENAME+'_FWLJMET_MC.root'))


# Output Module Configuration (expects a path 'p')
# process.out = cms.OutputModule("PoolOutputModule",
#                                fileName = cms.untracked.string(OUTFILENAME+'_postReco_MC.root'),
#                                #SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                                #outputCommands = cms.untracked.vstring('keep *')
#                                )


## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v17', '')
if isMC == False: process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_v11')
print 'Using global tag', process.GlobalTag.globaltag


## Produce new slimmedElectrons with V2 IDs
from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                       runVID=True,
                       era='2017-Nov17ReReco')


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

            isMc  = cms.bool(isMC),

			# Trigger cuts
			trigger_cut  = cms.bool(True),
            HLTcollection= cms.InputTag("TriggerResults","","HLT"),
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

			# PV cuts
			pv_cut     = cms.bool(True),
			pvSelector = cms.PSet( # taken from https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/PhysicsTools/SelectorUtils/python/pvSelector_cfi.py
				NPV     = cms.int32(1),
				pvSrc   = cms.InputTag('offlineSlimmedPrimaryVertices'),
				minNdof = cms.double(4.0),
				maxZ    = cms.double(24.0),
				maxRho  = cms.double(2.0)
				),

			# MET filter - https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
			metfilters      = cms.bool(True),
			flag_tag        = cms.InputTag('TriggerResults::PAT'),
			METfilter_extra = cms.InputTag("ecalBadCalibReducedMINIAODFilter"),

			#MET cuts
			met_cuts       = cms.bool(False),
			min_met        = cms.double(20.0),
			max_met        = cms.double(99999999999.0),
			met_collection = cms.InputTag('slimmedMETs'),


            PFparticlesCollection  = cms.InputTag("packedPFCandidates"),
            rhoJetsNCInputTag            = cms.InputTag("fixedGridRhoFastjetCentralNeutral",""),

            #Muon
            muon_cuts                = cms.bool(True),
            muonsCollection          = cms.InputTag("slimmedMuons"),
            min_muon                 = cms.int32(0), #not implemented in src code
            muon_minpt               = cms.double(20.0),
            muon_maxeta              = cms.double(2.4),
            muon_useMiniIso          = cms.bool(True),
            loose_muon_minpt         = cms.double(20.0),
            loose_muon_maxeta        = cms.double(2.4),
            muon_dxy                 = cms.double(0.2),
            muon_dz                  = cms.double(0.5),
            loose_muon_dxy           = cms.double(999999.),
            loose_muon_dz            = cms.double(999999.),

            # Muon -- Unused parameters but could be use again
            muon_relIso              = cms.double(0.2),
            loose_muon_relIso        = cms.double(0.4),

            # Electon
            electron_cuts            = cms.bool(True),
            electronsCollection      = cms.InputTag("slimmedElectrons"),
#             electronsCollection    = cms.InputTag("slimmedElectrons::LJMET"), #Egamma ID V2
            min_electron             = cms.int32(0), #not implemented in src code
            electron_minpt           = cms.double(20.0),
            electron_maxeta          = cms.double(2.4),
            electron_useMiniIso      = cms.bool(True),
            electron_miniIso         = cms.double(0.1),
            loose_electron_miniIso   = cms.double(0.4),
            loose_electron_minpt     = cms.double(20.0),
            loose_electron_maxeta    = cms.double(2.4),
            UseElMVA                 = cms.bool(True),
#             UseElIDV1                = cms.bool(False),
            UseElIDV1                = cms.bool(True),
            
            #nLeptons
            minLooseLeptons_cut = cms.bool(True), #inclusive Loose.
            minLooseLeptons     = cms.int32(2),
            maxLooseLeptons_cut = cms.bool(False),
            maxLooseLeptons     = cms.int32(9999),
            minLeptons_cut      = cms.bool(False),
            minLeptons          = cms.int32(2),
            maxLeptons_cut      = cms.bool(False),
            maxLeptons          = cms.int32(9999),
            
            #Jets
            jet_collection           = cms.InputTag('slimmedJets'),
            AK8jet_collection        = cms.InputTag('slimmedJetsAK8'),
            JECup                    = cms.bool(False),
            JECdown                  = cms.bool(False),
            JERup                    = cms.bool(False),
            JERdown                  = cms.bool(False),
            doNewJEC                 = cms.bool(True),
            doLepJetCleaning         = cms.bool(True),
            CleanLooseLeptons        = cms.bool(True),
            LepJetDR                 = cms.double(0.4),
            LepJetDRAK8              = cms.double(0.8),
            jet_cuts                 = cms.bool(True),
            jet_minpt                = cms.double(30.0),
            jet_maxeta               = cms.double(2.5),
            jet_minpt_AK8            = cms.double(200.0),
            jet_maxeta_AK8           = cms.double(2.4),
            min_jet                  = cms.int32(1),
            max_jet                  = cms.int32(9999),
            leading_jet_pt           = cms.double(30.0),
			# Jet corrections are read from txt files which need updating!
			JEC_txtfile = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt'),
			JERSF_txtfile = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_SF_AK4PFchs.txt'),
			JER_txtfile = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK4PFchs.txt'),
			JERAK8_txtfile = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK8PFPuppi.txt'),
			MCL1JetPar               = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt'),
			MCL2JetPar               = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt'),
			MCL3JetPar               = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt'),
			MCL1JetParAK8            = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFPuppi.txt'),
			MCL2JetParAK8            = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFPuppi.txt'),
			MCL3JetParAK8            = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFPuppi.txt'),
			DataL1JetPar             = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt'),
			DataL2JetPar             = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt'),
			DataL3JetPar             = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt'),
			DataResJetPar            = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt'),
			DataL1JetParAK8          = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt'),
			DataL2JetParAK8          = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt'),
			DataL3JetParAK8          = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt'),
			DataResJetParAK8         = cms.string(relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFPuppi.txt'),
			
			
			#Btag
			btag_cuts                = cms.bool(False), #not implemented
			btagOP                   = cms.string('MEDIUM'),
			bdisc_min                = cms.double(0.4941),
			DeepCSVfile              = cms.string(relBase+'/src/FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
			DeepCSVSubjetfile        = cms.string(relBase+'/src/FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
			BTagUncertUp             = cms.bool(False), # no longer needed
			BTagUncertDown           = cms.bool(False), # no longer needed
			MistagUncertUp           = cms.bool(False), # no longer needed
			MistagUncertDown          = cms.bool(False), # no longer needed
			
			

            ),


	MultiLepCalc = cms.PSet( # name has to match the calculator name as registered in Calc.cc

            debug                  = cms.bool(True),
            isMc                   = cms.bool(isMC),
            saveLooseLeps          = cms.bool(True),  
            keepFullMChistory      = cms.bool(isMC),

            rhoJetsNCInputTag            = cms.InputTag("fixedGridRhoFastjetCentralNeutral",""), #this is for muon
            genParticlesCollection = cms.InputTag("prunedGenParticles"), 
            PFparticlesCollection  = cms.InputTag("packedPFCandidates"),

            rhoJetsInputTag            = cms.InputTag("fixedGridRhoFastjetAll"), #this is for electron. Why is it different compared to muon?
            
        	UseElMVA                 = cms.bool(True),
#             UseElIDV1                = cms.bool(False),
            UseElIDV1                = cms.bool(True),


	),


)



# Configure a path and endpath to run the producer and output modules
process.p = cms.Path(
#     process.egammaPostRecoSeq *
    process.ljmet
)

# process.ep = cms.EndPath(process.out)
