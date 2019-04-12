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
MAXEVENTS = 100
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(MAXEVENTS) )

## Source / Input
isMC=True
#isMC=False
isTTbar = True
if(isMC):
    #INFILE='root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/00000/8EA8FE89-254F-E811-835E-0090FAA58BF4.root'
    INFILE='root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TTTT_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/70000/0ED34A55-DD52-E811-91CC-E0071B73B6B0.root',
    if (isTTbar):
        INFILE='root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/50000/5E7E4AA9-0743-E811-999A-0CC47A7C35A8.root'
else:
    INFILE='root://cmsxrootd.fnal.gov//store/data/Run2017F/DoubleEG/MINIAOD/09May2018-v1/10000/444E03EB-B75F-E811-AFBA-F01FAFD8F16A.root'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        INFILE,
    )
)

if(isMC):
        # OUTFILENAME = 'TprimeTprime_M-1100_TuneCP5_13TeV-madgraph-pythia8'
        OUTFILENAME = 'TTTT_TuneCP5_PSweights_13TeV-amcatnlo-pythia8'
        if (isTTbar):
            OUTFILENAME = 'TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8'
        POSTFIX = 'MC'
else:
        OUTFILENAME = 'DoubleEG_Run2017F'
        POSTFIX = 'DATA'
## TFileService
process.TFileService = cms.Service("TFileService", fileName = cms.string(OUTFILENAME+'_FWLJMET_'+POSTFIX+'.root'))


## Output Module Configuration (expects a path 'p')
# process.out = cms.OutputModule("PoolOutputModule",
#                                fileName = cms.untracked.string(OUTFILENAME+'_postReco_MC.root'),
#                                #SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                                outputCommands = cms.untracked.vstring('keep *')
#                                )

################################
## For updateJetCollection
################################
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask
patAlgosToolsTask = getPatAlgosToolsTask(process)
# process.outpath = cms.EndPath(process.out, patAlgosToolsTask)


## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v17', '')
if isMC == False: process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_v11')
print 'Using global tag', process.GlobalTag.globaltag



################################################
## Produce new slimmedElectrons with V2 IDs - https://twiki.cern.ch/twiki/bin/view/CMS/EgammaMiniAODV2
################################################
from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                       runVID=True,
                       era='2017-Nov17ReReco')


################################################
## Produce modified MET with the ECAL noise fix
################################################
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD(
    process,
    isData = not isMC,
    fixEE2017 = True,
    fixEE2017Params = {'userawPt': True, 'ptThreshold':50.0, 'minEtaThreshold':2.65, 'maxEtaThreshold': 3.139},
    postfix = "ModifiedMET"
    )

################################
## Rerun the ecalBadCalibFilter
################################
process.load('RecoMET.METFilters.ecalBadCalibFilter_cfi')

baddetEcallist = cms.vuint32(
    [872439604,872422825,872420274,872423218,
     872423215,872416066,872435036,872439336,
     872420273,872436907,872420147,872439731,
     872436657,872420397,872439732,872439339,
     872439603,872422436,872439861,872437051,
     872437052,872420649,872421950,872437185,
     872422564,872421566,872421695,872421955,
     872421567,872437184,872421951,872421694,
     872437056,872437057,872437313,872438182,
     872438951,872439990,872439864,872439609,
     872437181,872437182,872437053,872436794,
     872436667,872436536,872421541,872421413,
     872421414,872421031,872423083,872421439]
)

process.ecalBadCalibReducedMINIAODFilter = cms.EDFilter(
    "EcalBadCalibFilter",
    EcalRecHitSource = cms.InputTag("reducedEgamma:reducedEERecHits"),
    ecalMinEt        = cms.double(50.),
    baddetEcal    = baddetEcallist,
    taggingMode = cms.bool(True),
    debug = cms.bool(False)
)


################################
## Produce DeepAK8 jet tags
################################
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
from RecoBTag.MXNet.pfDeepBoostedJet_cff import *

updateJetCollection(
   process,
   jetSource = cms.InputTag('slimmedJetsAK8'),
   pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
   svSource = cms.InputTag('slimmedSecondaryVertices'),
   rParam = 0.8,
   jetCorrections = ('AK8PFPuppi', cms.vstring(['L2Relative', 'L3Absolute']), 'None'),
   btagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags',
                         'pfDeepBoostedJetTags:probTbcq', 'pfDeepBoostedJetTags:probTbqq',
                         'pfDeepBoostedJetTags:probWcq', 'pfDeepBoostedJetTags:probWqq',
                         'pfDeepBoostedJetTags:probZbb', 'pfDeepBoostedJetTags:probZcc', 'pfDeepBoostedJetTags:probZqq',
                         'pfDeepBoostedJetTags:probHbb', 'pfDeepBoostedJetTags:probHcc', 'pfDeepBoostedJetTags:probHqqqq',
                         'pfDeepBoostedJetTags:probQCDbb', 'pfDeepBoostedJetTags:probQCDcc',
                         'pfDeepBoostedJetTags:probQCDb', 'pfDeepBoostedJetTags:probQCDc',
                         'pfDeepBoostedJetTags:probQCDothers',
                         'pfDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:WvsQCD',
                         'pfDeepBoostedDiscriminatorsJetTags:ZvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD',
                         'pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:H4qvsQCD',
                         'pfMassDecorrelatedDeepBoostedJetTags:probTbcq', 'pfMassDecorrelatedDeepBoostedJetTags:probTbqq',
                         'pfMassDecorrelatedDeepBoostedJetTags:probWcq', 'pfMassDecorrelatedDeepBoostedJetTags:probWqq',
                         'pfMassDecorrelatedDeepBoostedJetTags:probZbb', 'pfMassDecorrelatedDeepBoostedJetTags:probZcc', 'pfMassDecorrelatedDeepBoostedJetTags:probZqq',
                         'pfMassDecorrelatedDeepBoostedJetTags:probHbb', 'pfMassDecorrelatedDeepBoostedJetTags:probHcc', 'pfMassDecorrelatedDeepBoostedJetTags:probHqqqq',
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDbb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDcc',
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDc',
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDothers',
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD',
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD',
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:bbvsLight', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ccvsLight'],
   postfix = 'AK8Puppi',
   printWarning = False
   )

################################################################################################
#### Establish references between PATified fat jets and subjets using the BoostedJetMerger
################################################################################################
process.updatedJetsAK8PuppiSoftDropPacked = cms.EDProducer("BoostedJetMerger",
                                                           jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
                                                           subjetSrc=cms.InputTag('slimmedJetsAK8PFPuppiSoftDropPacked','SubJets')
                                                           )
################################
#### Pack fat jets with subjets
################################
process.packedJetsAK8Puppi = cms.EDProducer("JetSubstructurePacker",
                                            jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
                                            distMax = cms.double(0.8),
                                            fixDaughters = cms.bool(False),
                                            algoTags = cms.VInputTag(cms.InputTag("updatedJetsAK8PuppiSoftDropPacked")),
                                            algoLabels =cms.vstring('SoftDropPuppi')
)


##############################################
#run QGTagger code again to calculate jet axis1  (HOT Tagger) - https://github.com/susy2015/TopTagger/tree/master/TopTagger#instructions-for-saving-tagger-results-to-nanoaod-with-cmssw_9_4_11
##############################################
updateJetCollection(
    process,
    jetSource = cms.InputTag('slimmedJets'),
)
process.load('RecoJets.JetProducers.QGTagger_cfi')
# patAlgosToolsTask.add(process.QGTagger)
process.QGTagger.srcJets = cms.InputTag('slimmedJets')
process.updatedPatJets.userData.userFloats.src += ['QGTagger:ptD','QGTagger:axis1','QGTagger:axis2']
process.updatedPatJets.userData.userInts.src += ['QGTagger:mult']


################################
## Produce L1 Prefiring probabilities - https://twiki.cern.ch/twiki/bin/viewauth/CMS/L1ECALPrefiringWeightRecipe
################################
from PhysicsTools.PatUtils.l1ECALPrefiringWeightProducer_cfi import l1ECALPrefiringWeightProducer
process.prefiringweight = l1ECALPrefiringWeightProducer.clone(
    DataEra = cms.string("2017BtoF"),
    UseJetEMPt = cms.bool(False),
    PrefiringRateSystematicUncty = cms.double(0.2),
    SkipWarnings = False)



################################################
### LJMET
################################################

#For MET filter
if(isMC): MET_filt_flag_tag        = 'TriggerResults::PAT'
else:     MET_filt_flag_tag        = 'TriggerResults::RECO'

#For Jet corrections
doNewJEC                 = True
JECup                    = False
JECdown                  = False
JERup                    = False
JERdown                  = False
doAllJetSyst             = False #this determines whether to save JER/JER up/down in one job. Default is currently false. Mar 19,2019.
JEC_txtfile              = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt'
JERSF_txtfile            = relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_SF_AK4PFchs.txt'
JER_txtfile              = relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK4PFchs.txt'
JERAK8_txtfile           = relBase+'/src/FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK8PFPuppi.txt'
MCL1JetPar               = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt'
MCL2JetPar               = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt'
MCL3JetPar               = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt'
MCL1JetParAK8            = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFPuppi.txt'
MCL2JetParAK8            = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFPuppi.txt'
MCL3JetParAK8            = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFPuppi.txt'
DataL1JetPar             = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt'
DataL2JetPar             = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt'
DataL3JetPar             = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt'
DataResJetPar            = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt'
DataL1JetParAK8          = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt'
DataL2JetParAK8          = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt'
DataL3JetParAK8          = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt'
DataResJetParAK8         = relBase+'/src/FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFPuppi.txt'


MultiLepSelector_cfg = cms.PSet(

            debug  = cms.bool(False),

            isMc  = cms.bool(isMC),

            # Trigger cuts
            trigger_cut  = cms.bool(True),
            HLTcollection= cms.InputTag("TriggerResults","","HLT"),
            dump_trigger = cms.bool(False),
            mctrigger_path_el = cms.vstring(
                        #'digitisation_step',
                        'HLT_Ele35_WPTight_Gsf',
                        'HLT_Ele38_WPTight_Gsf',
                        'HLT_Ele40_WPTight_Gsf',
                        'HLT_Ele28_eta2p1_WPTight_Gsf_HT150',
                        'HLT_Ele15_IsoVVVL_PFHT450_PFMET50',
                        'HLT_Ele15_IsoVVVL_PFHT450',
                        'HLT_Ele50_IsoVVVL_PFHT450',
                        'HLT_Ele15_IsoVVVL_PFHT600',
                        'HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165',
                        'HLT_Ele115_CaloIdVT_GsfTrkIdT'

                        'HLT_Ele32_WPTight_Gsf',
                        'HLT_Ele32_WPTight_Gsf_L1DoubleEG',
                        'HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned',
                        ),
            mctrigger_path_mu = cms.vstring(
                        #'digitisation_step',
                        'HLT_IsoMu24',
                        'HLT_IsoMu24_eta2p1',
                        'HLT_IsoMu27',
                        'HLT_IsoMu30',
                        'HLT_Mu50',
                        'HLT_TkMu50',
                        'HLT_Mu55',
                        'HLT_Mu15_IsoVVVL_PFHT450_PFMET50',
                        'HLT_Mu15_IsoVVVL_PFHT450',
                        'HLT_Mu50_IsoVVVL_PFHT450',
                        'HLT_Mu15_IsoVVVL_PFHT600',

                        'HLT_IsoTkMu24',
                        'HLT_IsoMu24_2p1',
                        ),
            trigger_path_el = cms.vstring(''), #currently set to be the same as mc in the src code
            trigger_path_mu = cms.vstring(''), #currently set to be the same as mc in the src code

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
            flag_tag        = cms.InputTag(MET_filt_flag_tag),
            METfilter_extra = cms.InputTag("ecalBadCalibReducedMINIAODFilter"),

            # MET cuts
            met_cuts       = cms.bool(True),
            min_met        = cms.double(30.0),
            max_met        = cms.double(99999999999.0),
            met_collection = cms.InputTag('slimmedMETs'),
            rhoJetsInputTag = cms.InputTag("fixedGridRhoFastjetAll"), #for jetmetcorrection

            PFparticlesCollection  = cms.InputTag("packedPFCandidates"),
            rhoJetsNCInputTag            = cms.InputTag("fixedGridRhoFastjetCentralNeutral",""),

            #Muon
            muon_cuts                = cms.bool(True),
            muonsCollection          = cms.InputTag("slimmedMuons"),
            min_muon                 = cms.int32(0), #not implemented in src code
            muon_minpt               = cms.double(25.0),
            muon_maxeta              = cms.double(2.4),
            muon_useMiniIso          = cms.bool(True),
            loose_muon_minpt         = cms.double(10.0),
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
            # electronsCollection      = cms.InputTag("slimmedElectrons"), #slimmedElectrons::LJMET" #for Egamma ID V2
            electronsCollection      = cms.InputTag("slimmedElectrons::LJMET"), #slimmedElectrons::LJMET" #for Egamma ID V2
            min_electron             = cms.int32(0), #not implemented in src code
            electron_minpt           = cms.double(25.0),
            electron_maxeta          = cms.double(2.5),
            electron_useMiniIso      = cms.bool(True),
            electron_miniIso         = cms.double(0.1),
            loose_electron_miniIso   = cms.double(0.4),
            loose_electron_minpt     = cms.double(10.0),
            loose_electron_maxeta    = cms.double(2.5),
            UseElMVA                 = cms.bool(True),
            # UseElIDV1                = cms.bool(True), #False means using ElIDV2
            UseElIDV1                = cms.bool(False), #False means using ElIDV2

            #nLeptons
            minLooseLeptons_cut = cms.bool(False), #inclusive Loose.
            minLooseLeptons     = cms.int32(0),
            maxLooseLeptons_cut = cms.bool(False),
            maxLooseLeptons     = cms.int32(9999),
            minLeptons_cut      = cms.bool(True),
            minLeptons          = cms.int32(1),
            maxLeptons_cut      = cms.bool(True),
            maxLeptons          = cms.int32(1),

            # Jets
            # jet_collection           = cms.InputTag('slimmedJets'),
            jet_collection           = cms.InputTag('updatedPatJets::LJMET'),
            AK8jet_collection        = cms.InputTag('slimmedJetsAK8'),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doLepJetCleaning         = cms.bool(True),
            CleanLooseLeptons        = cms.bool(True),
            LepJetDR                 = cms.double(0.4),
            LepJetDRAK8              = cms.double(0.8),
            jet_cuts                 = cms.bool(True),
            jet_minpt                = cms.double(20.0),
            jet_maxeta               = cms.double(3.0),
            jet_minpt_AK8            = cms.double(170.0),
            jet_maxeta_AK8           = cms.double(2.4),
            min_jet                  = cms.int32(1),
            max_jet                  = cms.int32(4000),
            leading_jet_pt           = cms.double(20.0),
            # Jet corrections are read from txt files
            doNewJEC                 = cms.bool(doNewJEC),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.string(JEC_txtfile),
            JERSF_txtfile            = cms.string(JERSF_txtfile),
            JER_txtfile              = cms.string(JER_txtfile),
            JERAK8_txtfile           = cms.string(JERAK8_txtfile),
            MCL1JetPar               = cms.string(MCL1JetPar),
            MCL2JetPar               = cms.string(MCL2JetPar),
            MCL3JetPar               = cms.string(MCL3JetPar),
            MCL1JetParAK8            = cms.string(MCL1JetParAK8),
            MCL2JetParAK8            = cms.string(MCL2JetParAK8),
            MCL3JetParAK8            = cms.string(MCL3JetParAK8),
            DataL1JetPar             = cms.string(DataL1JetPar),
            DataL2JetPar             = cms.string(DataL2JetPar),
            DataL3JetPar             = cms.string(DataL3JetPar),
            DataResJetPar            = cms.string(DataResJetPar),
            DataL1JetParAK8          = cms.string(DataL1JetParAK8),
            DataL2JetParAK8          = cms.string(DataL2JetParAK8),
            DataL3JetParAK8          = cms.string(DataL3JetParAK8),
            DataResJetParAK8         = cms.string(DataResJetParAK8),


            #Btag
            btag_cuts                = cms.bool(False), #not implemented
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.string(relBase+'/src/FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.string(relBase+'/src/FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
            BTagUncertUp             = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            BTagUncertDown           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertUp           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertDown          = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.

            )

MultiLepCalc_cfg = cms.PSet(

            debug                  = cms.bool(False),
            isMc                   = cms.bool(isMC),
            saveLooseLeps          = cms.bool(True),
            keepFullMChistory      = cms.bool(isMC),

            rhoJetsNCInputTag      = cms.InputTag("fixedGridRhoFastjetCentralNeutral",""), #this is for muon
            genParticlesCollection = cms.InputTag("prunedGenParticles"),
            PFparticlesCollection  = cms.InputTag("packedPFCandidates"),

            rhoJetsInputTag            = cms.InputTag("fixedGridRhoFastjetAll"), #this is for electron. Why is it different compared to muon?
            UseElMVA                 = cms.bool(True),
            UseElIDV1                = cms.bool(True), #False means using ElIDV2.

            # Jet corrections needs to be passed here again if Calc uses jet correction
            doNewJEC                 = cms.bool(doNewJEC),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.string(JEC_txtfile),
            JERSF_txtfile            = cms.string(JERSF_txtfile),
            JER_txtfile              = cms.string(JER_txtfile),
            JERAK8_txtfile           = cms.string(JERAK8_txtfile),
            MCL1JetPar               = cms.string(MCL1JetPar),
            MCL2JetPar               = cms.string(MCL2JetPar),
            MCL3JetPar               = cms.string(MCL3JetPar),
            MCL1JetParAK8            = cms.string(MCL1JetParAK8),
            MCL2JetParAK8            = cms.string(MCL2JetParAK8),
            MCL3JetParAK8            = cms.string(MCL3JetParAK8),
            DataL1JetPar             = cms.string(DataL1JetPar),
            DataL2JetPar             = cms.string(DataL2JetPar),
            DataL3JetPar             = cms.string(DataL3JetPar),
            DataResJetPar            = cms.string(DataResJetPar),
            DataL1JetParAK8          = cms.string(DataL1JetParAK8),
            DataL2JetParAK8          = cms.string(DataL2JetParAK8),
            DataL3JetParAK8          = cms.string(DataL3JetParAK8),
            DataResJetParAK8         = cms.string(DataResJetParAK8),

            #For accessing METnoHF, and METmod
            metnohf_collection = cms.InputTag('slimmedMETsNoHF'),
            metmod_collection = cms.InputTag('slimmedMETsModifiedMET'),

            #Gen stuff
            saveGenHT          = cms.bool(False),
            genJetsCollection  = cms.InputTag("slimmedGenJets"),
            OverrideLHEWeights = cms.bool(True),
            basePDFname        = cms.string('NNPDF31_nnlo_as_0118_nf_4'),
            newPDFname         = cms.string('NNPDF31_lo_as_0118'),
            keepPDGID          = cms.vuint32(1, 2, 3, 4, 5, 6, 21, 11, 12, 13, 14, 15, 16, 24),
            keepMomPDGID       = cms.vuint32(6, 24),
            keepPDGIDForce     = cms.vuint32(6,6),
            keepStatusForce    = cms.vuint32(62,22),
            cleanGenJets       = cms.bool(True),

            #Btagging - Btag info needs to be passed here again if Calc uses Btagging.
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.string(relBase+'/src/FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.string(relBase+'/src/FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
            BTagUncertUp             = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            BTagUncertDown           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertUp           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertDown          = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.

            )

TpTpCalc_cfg = cms.PSet(

            genParticlesCollection = cms.InputTag("prunedGenParticles"),

            )

JetSubCalc_cfg = cms.PSet(

            debug        = cms.bool(False),
            isMc         = cms.bool(isMC),

            genParticles       = cms.InputTag("prunedGenParticles"),

            kappa              = cms.double(0.5), #for Jet Charge calculation
            killHF             = cms.bool(False),
            puppiCorrPath      = cms.string(relBase+'/src/FWLJMET/LJMet/data/PuppiSoftdropMassCorr/weights/puppiCorr.root'),

            rhoJetsInputTag          = cms.InputTag("fixedGridRhoFastjetAll"), #this is for electron. Why is it different compared to muon?

            # Jet recorrections needs to be passed here again if Calc uses jet correction
            doNewJEC                 = cms.bool(doNewJEC),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.string(JEC_txtfile),
            JERSF_txtfile            = cms.string(JERSF_txtfile),
            JER_txtfile              = cms.string(JER_txtfile),
            JERAK8_txtfile           = cms.string(JERAK8_txtfile),
            MCL1JetPar               = cms.string(MCL1JetPar),
            MCL2JetPar               = cms.string(MCL2JetPar),
            MCL3JetPar               = cms.string(MCL3JetPar),
            MCL1JetParAK8            = cms.string(MCL1JetParAK8),
            MCL2JetParAK8            = cms.string(MCL2JetParAK8),
            MCL3JetParAK8            = cms.string(MCL3JetParAK8),
            DataL1JetPar             = cms.string(DataL1JetPar),
            DataL2JetPar             = cms.string(DataL2JetPar),
            DataL3JetPar             = cms.string(DataL3JetPar),
            DataResJetPar            = cms.string(DataResJetPar),
            DataL1JetParAK8          = cms.string(DataL1JetParAK8),
            DataL2JetParAK8          = cms.string(DataL2JetParAK8),
            DataL3JetParAK8          = cms.string(DataL3JetParAK8),
            DataResJetParAK8         = cms.string(DataResJetParAK8),

            #Btagging - Btag info needs to be passed here again if Calc uses Btagging.
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.string(relBase+'/src/FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.string(relBase+'/src/FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
            BTagUncertUp             = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            BTagUncertDown           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertUp           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertDown          = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.

            )

TTbarMassCalc_cfg = cms.PSet(

        genParticles = cms.InputTag("prunedGenParticles"),
        genTtbarId = cms.InputTag("categorizeGenTtbar:genTtbarId")
        )

BestCalc_cfg = cms.PSet(

    dnnFile = cms.string(relBase+'/src/FWLJMET/LJMet/data/BEST_mlp.json'),

    numSubjetsMin = cms.int32(2),
    numDaughtersMin = cms.int32(3),
    jetSoftDropMassMin = cms.double(10.0),
    jetPtMin = cms.double(170.0),
    radiusSmall = cms.double(0.4),
    radiusLarge = cms.double(0.8),
    reclusterJetPtMin = cms.double(20.0),
    jetChargeKappa = cms.double(0.6),
    maxJetSize = cms.int32(4),
    )

HOTTaggerCalc_cfg = cms.PSet(

    ak4PtCut         = cms.double(20),
    qgTaggerKey      = cms.string('QGTagger'),
    deepCSVBJetTags  = cms.string('pfDeepCSVJetTags'),
    CvsBCJetTags     = cms.string('pfCombinedCvsBJetTags'),
    CvsLCJetTags     = cms.string('pfCombinedCvsLJetTags'),
    bTagKeyString    = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
    taggerCfgFile    = cms.string(relBase+'/src/TopTagger/TopTagger/data/TopTaggerCfg-DeepResolved_DeepCSV_GR_noDisc_Release_v1.0.0/TopTagger.cfg'),
    discriminatorCut = cms.double(0.5),
    saveAllTopCandidates = cms.bool(False)

    )

process.ljmet = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        verbosity     = cms.int32(1),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        'TTbarMassCalc',
                        'DeepAK8Calc',
                        'HOTTaggerCalc',
                        # 'BestCalc', #NOT WORKING at the moment, April 5, 2019.--Rizki.
        ),
        exclude_calcs = cms.vstring(
                        'TestCalc',
                        'DummyCalc',
        ),

        # name has to match the name as registered in BeginJob of  EventSelector.cc
        MultiLepSelector = cms.PSet(MultiLepSelector_cfg),

        # Calc cfg name has to match the name as registered in Calc.cc
        MultiLepCalc  = cms.PSet(MultiLepCalc_cfg),
        TpTpCalc      = cms.PSet(TpTpCalc_cfg),
        CommonCalc    = cms.PSet(), #current ljmet wants all calc to send a PSet, event if its empty.
        JetSubCalc    = cms.PSet(JetSubCalc_cfg),
        TTbarMassCalc = cms.PSet(TTbarMassCalc_cfg),
        DeepAK8Calc    = cms.PSet(), #current ljmet wants all calc to send a PSet, event if its empty.
        HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg)
        # BestCalc      = cms.PSet(BestCalc_cfg),

)


################################################
### PROCESS PATH
################################################

# Configure a path and endpath to run the producer and output modules

# ----------------------- GenHFHadronMatcher -----------------                                                
if (isTTbar):
    process.load("PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff")

    from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
    process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(
        particles = cms.InputTag("prunedGenParticles")
        )
    from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
    process.genJetFlavourInfos = ak4JetFlavourInfos.clone(
        jets = cms.InputTag("slimmedGenJets")
        )
    from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
    process.matchGenBHadron = matchGenBHadron.clone(
        genParticles = cms.InputTag("prunedGenParticles"),
        jetFlavourInfos = "genJetFlavourInfos"
        )
    from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenCHadron
    process.matchGenCHadron = matchGenCHadron.clone(
        genParticles = cms.InputTag("prunedGenParticles"),
        jetFlavourInfos = "genJetFlavourInfos"
        )
    process.load("TopQuarkAnalysis.TopTools.GenTtbarCategorizer_cfi")
    process.categorizeGenTtbar.genJets = cms.InputTag("slimmedGenJets")

    process.ttbarcat = cms.Sequence(
        process.selectedHadronsAndPartons * process.genJetFlavourInfos * process.matchGenBHadron 
        * process.matchGenCHadron* ## gen HF flavour matching            
        process.categorizeGenTtbar  ## return already a categorization id for tt                  
        )

    process.p = cms.Path(process.fullPatMetSequenceModifiedMET * 
                         process.prefiringweight * 
                         process.egammaPostRecoSeq * 
                         process.updatedJetsAK8PuppiSoftDropPacked * 
                         process.packedJetsAK8Puppi *
                         process.QGTagger *
                         process.ecalBadCalibReducedMINIAODFilter *
                         process.ttbarcat *
                         process.ljmet #(ntuplizer)
                         )

else:
    process.p = cms.Path(
       process.fullPatMetSequenceModifiedMET *
       process.prefiringweight *
       process.egammaPostRecoSeq *
       process.updatedJetsAK8PuppiSoftDropPacked *
       process.packedJetsAK8Puppi *
       process.QGTagger *
       process.ecalBadCalibReducedMINIAODFilter *
       process.ljmet #(ntuplizer)
    )

process.p.associate(patAlgosToolsTask)

#from Configuration.EventContent.EventContent_cff import MINIAODSIMEventContent
# process.out.outputCommands.append('drop *_*_*_LJMET')
# process.out.outputCommands.append('keep *_prefiringweight*_*_LJMET')
# process.out.outputCommands.append('keep *_slimmedElectrons_*_LJMET')
# process.out.outputCommands.append('keep *_packedJetsAK8Puppi*_*_LJMET')
# process.out.outputCommands.append('keep *_*_SubJets_*')
# process.out.outputCommands.append('keep *_updatedPatJets_*_LJMET')
# process.out.outputCommands.append('keep *_updatedPatJets*_tagInfos_LJMET')
# # process.out.outputCommands.append('keep *_QGTagger_*_LJMET')
# process.out.outputCommands.append('keep *_slimmedMETsModifiedMET_*_LJMET')
# process.out.outputCommands.append('keep *_ecalBadCalibReducedMINIAODFilter_*_LJMET')
# process.out.outputCommands.append('keep int_categorize*_*_LJMET'),


# process.ep = cms.EndPath(process.out)
# process.ep = cms.EndPath(process.out,patAlgosToolsTask)
# process.scedule = cms.Schedule(
#     process.p,
#     process.outpath)

