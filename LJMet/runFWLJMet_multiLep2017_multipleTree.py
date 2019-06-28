import FWCore.ParameterSet.Config as cms
import os


relBase = os.environ['CMSSW_BASE']


## PARSE ARGUMENTS
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.register('isMC', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is MC')
options.register('isTTbar', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is TTbar')
options.register('isVLQsignal', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is VLQ Signal')

## SET DEFAULT VALUES
## ATTENTION: THESE DEFAULT VALUES ARE SET FOR VLQ SIGNAL ! isMC=True, isTTbar=False, isVLQsignal=True 
options.isMC = ISMC
options.isTTbar = ISTTBAR
options.isVLQsignal = ISVLQSIGNAL
options.inputFiles = [
    'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1400_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/50000/F82DC089-5591-E811-9210-6C3BE5B58198.root'
    ]
options.maxEvents = 100
options.parseArguments()

isMC= options.isMC
isTTbar = options.isTTbar
isVLQsignal = options.isVLQsignal

#Check arguments
print options

## LJMET
process = cms.Process("LJMET")

## MessageLogger
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 20

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

## Check memory and timing - TURN OFF when not testing!
import FWCore.ParameterSet.Config as cms
def customise(process):

    #Adding SimpleMemoryCheck service:
    process.SimpleMemoryCheck=cms.Service("SimpleMemoryCheck",
                                          ignoreTotal=cms.untracked.int32(1),
                                          oncePerEventMode=cms.untracked.bool(True))
    #Adding Timing service:
    process.Timing=cms.Service("Timing")

    #Add these 3 lines to put back the summary for timing information at the end of the logfile
    #(needed for TimeReport report)
    if hasattr(process,'options'):
        process.options.wantSummary = cms.untracked.bool(True)
    else:
        process.options = cms.untracked.PSet(
            wantSummary = cms.untracked.bool(True)
        )

    return(process)
# customise(process)

## Multithreading option
process.options.numberOfThreads=cms.untracked.uint32(4)
process.options.numberOfStreams=cms.untracked.uint32(0)

## Maximal Number of Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(cms.untracked.vstring(options.inputFiles),
    )
)

OUTFILENAME = "DATASET"
process.TFileService = cms.Service("TFileService", fileName = cms.string(OUTFILENAME+'.root'))

# OUTFILENAME = "cmsRun" #This could be better !
# if(isMC):
#         POSTFIX = 'MC'
# else:
#         POSTFIX = 'DATA'
# POSTFIX+='_3Lep'
# ## TFileService
# process.TFileService = cms.Service("TFileService", fileName = cms.string(OUTFILENAME+'_FWLJMET_'+POSTFIX+'.root'))


## Output Module Configuration (expects a path 'p')
# process.out = cms.OutputModule("PoolOutputModule",
#                                fileName = cms.untracked.string(OUTFILENAME+'_postReco_MC.root'),
#                                #SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                                #outputCommands = cms.untracked.vstring('keep *')
#                                )


################################
## MC Weights Analyzer
################################
process.mcweightanalyzer = cms.EDAnalyzer(
    "WeightAnalyzer",
    overrideLHEweight = cms.bool(isVLQsignal),
    basePDFname = cms.string("NNPDF31_nnlo_as_0118_nf_4"),
    newPDFname = cms.string("NNPDF31_nnlo_as_0118_nf_4_mc_hessian"),
    )

################################
## Trigger filter
################################
import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
# accept if any path succeeds (explicit)
process.filter_any_explicit = hlt.hltHighLevel.clone(
    HLTPaths = [
                        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v*',  #exists in 2017
                        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*', #exists in 2017

                        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*',   #exists in 2017
                        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ*', #exists in 2017
                        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v*', #exists in 2017
                        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*',  #exists in 2017

                        #for trig efficiency
                        'HLT_Ele27_WPTight_Gsf_v*',
                        'HLT_Ele35_WPTight_Gsf_v*',

                        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*',   #exists in 2017
                        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ*', #exists in 2017
                        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v*', #exists in 2017
                        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*',  #exists in 2017

                        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v*',    #exists in 2017  (PreScaled!)
                        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v*',  #exists in 2017 (PreScaled!)
                        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v*',
                        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v*',
                        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v*',
                        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v*',

                        #for trig efficiency
                        'HLT_IsoMu24_v*',
                        'HLT_IsoTkMu24_v*',
                        'HLT_IsoMu27_v*',
    ],
    throw = False
    )


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
from Configuration.AlCa.GlobalTag import GlobalTag # See https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v17', '')
if not isMC: process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_v11')
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
# from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
# from RecoBTag.MXNet.pfDeepBoostedJet_cff import *
# 
# updateJetCollection(
#    process,
#    jetSource = cms.InputTag('slimmedJetsAK8'),
#    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
#    svSource = cms.InputTag('slimmedSecondaryVertices'),
#    rParam = 0.8,
#    jetCorrections = ('AK8PFPuppi', cms.vstring(['L2Relative', 'L3Absolute']), 'None'),
#    btagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags',
#                          'pfDeepBoostedJetTags:probTbcq', 'pfDeepBoostedJetTags:probTbqq',
#                          'pfDeepBoostedJetTags:probWcq', 'pfDeepBoostedJetTags:probWqq',
#                          'pfDeepBoostedJetTags:probZbb', 'pfDeepBoostedJetTags:probZcc', 'pfDeepBoostedJetTags:probZqq',
#                          'pfDeepBoostedJetTags:probHbb', 'pfDeepBoostedJetTags:probHcc', 'pfDeepBoostedJetTags:probHqqqq',
#                          'pfDeepBoostedJetTags:probQCDbb', 'pfDeepBoostedJetTags:probQCDcc',
#                          'pfDeepBoostedJetTags:probQCDb', 'pfDeepBoostedJetTags:probQCDc',
#                          'pfDeepBoostedJetTags:probQCDothers',
#                          'pfDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:WvsQCD',
#                          'pfDeepBoostedDiscriminatorsJetTags:ZvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD',
#                          'pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:H4qvsQCD',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probTbcq', 'pfMassDecorrelatedDeepBoostedJetTags:probTbqq',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probWcq', 'pfMassDecorrelatedDeepBoostedJetTags:probWqq',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probZbb', 'pfMassDecorrelatedDeepBoostedJetTags:probZcc', 'pfMassDecorrelatedDeepBoostedJetTags:probZqq',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probHbb', 'pfMassDecorrelatedDeepBoostedJetTags:probHcc', 'pfMassDecorrelatedDeepBoostedJetTags:probHqqqq',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probQCDbb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDcc',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probQCDb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDc',
#                          'pfMassDecorrelatedDeepBoostedJetTags:probQCDothers',
#                          'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD',
#                          'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD',
#                          'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:bbvsLight', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ccvsLight'],
#    postfix = 'AK8Puppi',
#    printWarning = False
#    )

################################################################################################
#### Establish references between PATified fat jets and subjets using the BoostedJetMerger
################################################################################################
# process.updatedJetsAK8PuppiSoftDropPacked = cms.EDProducer("BoostedJetMerger",
#                                                            jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
#                                                            subjetSrc=cms.InputTag('slimmedJetsAK8PFPuppiSoftDropPacked','SubJets')
#                                                            )
################################
#### Pack fat jets with subjets
################################
# process.packedJetsAK8Puppi = cms.EDProducer("JetSubstructurePacker",
#                                             jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
#                                             distMax = cms.double(0.8),
#                                             fixDaughters = cms.bool(False),
#                                             algoTags = cms.VInputTag(cms.InputTag("updatedJetsAK8PuppiSoftDropPacked")),
#                                             algoLabels =cms.vstring('SoftDropPuppi')
# )


##############################################
#run QGTagger code again to calculate jet axis1  (HOT Tagger) - https://github.com/susy2015/TopTagger/tree/master/TopTagger#instructions-for-saving-tagger-results-to-nanoaod-with-cmssw_9_4_11
##############################################
# updateJetCollection(
#     process,
#     jetSource = cms.InputTag('slimmedJets'),
# )
# process.load('RecoJets.JetProducers.QGTagger_cfi')
# # patAlgosToolsTask.add(process.QGTagger)
# process.QGTagger.srcJets = cms.InputTag('slimmedJets')
# process.updatedPatJets.userData.userFloats.src += ['QGTagger:ptD','QGTagger:axis1','QGTagger:axis2']
# process.updatedPatJets.userData.userInts.src += ['QGTagger:mult']


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

## For MET filter
if(isMC): MET_filt_flag_tag        = 'TriggerResults::PAT'
else:     MET_filt_flag_tag        = 'TriggerResults::RECO'

## For Jet corrections
doNewJEC                 = True
JECup                    = False
JECdown                  = False
JERup                    = False
JERdown                  = False
doAllJetSyst             = False #this determines whether to save JEC/JER up/down in one job. Default is currently false. Mar 19,2019.
JEC_txtfile              = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt'
JERSF_txtfile            = 'FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_SF_AK4PFchs.txt'
JER_txtfile              = 'FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK4PFchs.txt'
JERAK8_txtfile           = 'FWLJMET/LJMet/data/Fall17V3/Fall17_V3_MC_PtResolution_AK8PFPuppi.txt'
MCL1JetPar               = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt'
MCL2JetPar               = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt'
MCL3JetPar               = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt'
MCL1JetParAK8            = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFPuppi.txt'
MCL2JetParAK8            = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFPuppi.txt'
MCL3JetParAK8            = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFPuppi.txt'
DataL1JetPar             = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt'
DataL2JetPar             = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt'
DataL3JetPar             = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt'
DataResJetPar            = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt'
DataL1JetParAK8          = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt'
DataL2JetParAK8          = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt'
DataL3JetParAK8          = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt'
DataResJetParAK8         = 'FWLJMET/LJMet/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFPuppi.txt'

## El MVA ID
UseElIDV1_ = False #False means using ElIDV2

## TriggerPaths (for ljmet): 
hlt_path_el  = cms.vstring(
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v',  #exists in 2017
        'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v', #exists in 2017

        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017
        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017

        #for trig efficiency
        'HLT_Ele27_WPTight_Gsf_v',
        'HLT_Ele35_WPTight_Gsf_v',
        )
hlt_path_mu = cms.vstring(
        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',   #exists in 2017
        'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ', #exists in 2017
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v', #exists in 2017
        'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  #exists in 2017

        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',    #exists in 2017  (PreScaled!)
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',  #exists in 2017 (PreScaled!)
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v',
        'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v',
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',
        'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',

        #for trig efficiency
        'HLT_IsoMu24_v',
        'HLT_IsoTkMu24_v',
        'HLT_IsoMu27_v',
        )


#Selector/Calc config
MultiLepSelector_cfg = cms.PSet(

            debug  = cms.bool(False),

            isMc  = cms.bool(isMC),

            # Trigger cuts
            trigger_cut  = cms.bool(True),
            HLTcollection= cms.InputTag("TriggerResults","","HLT"),
            dump_trigger = cms.bool(False),

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
            min_met        = cms.double(20.0),
            max_met        = cms.double(99999999999.0),
            met_collection = cms.InputTag('slimmedMETs'),
            rhoJetsInputTag = cms.InputTag("fixedGridRhoFastjetAll"), #for jetmetcorrection

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
            # electronsCollection      = cms.InputTag("slimmedElectrons"), #slimmedElectrons::LJMET" #for Egamma ID V2
            electronsCollection      = cms.InputTag("slimmedElectrons::LJMET"), #slimmedElectrons::LJMET" #for Egamma ID V2
            min_electron             = cms.int32(0), #not implemented in src code
            electron_minpt           = cms.double(20.0),
            electron_maxeta          = cms.double(2.4),
            electron_useMiniIso      = cms.bool(True),
            electron_miniIso         = cms.double(0.1),
            loose_electron_miniIso   = cms.double(0.4),
            loose_electron_minpt     = cms.double(20.0),
            loose_electron_maxeta    = cms.double(2.4),
            UseElMVA                 = cms.bool(True),
            UseElIDV1                = cms.bool(UseElIDV1_), #False means using ElIDV2
            # UseElIDV1                = cms.bool(False), #False means using ElIDV2

            #nLeptons
            minLooseLeptons_cut = cms.bool(True), #inclusive Loose.
            minLooseLeptons     = cms.int32(3),
            maxLooseLeptons_cut = cms.bool(False),
            maxLooseLeptons     = cms.int32(9999),
            minLeptons_cut      = cms.bool(False),
            minLeptons          = cms.int32(3),
            maxLeptons_cut      = cms.bool(False),
            maxLeptons          = cms.int32(9999),

            # Jets
            jet_collection           = cms.InputTag('slimmedJets'),
            # jet_collection           = cms.InputTag('updatedPatJets::LJMET'), #if using updated jets
            AK8jet_collection        = cms.InputTag('slimmedJetsAK8'),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doLepJetCleaning         = cms.bool(True),
            CleanLooseLeptons        = cms.bool(True), #This needs to be well thought of depending on saving loose leptons or not and make sure treatment is the same for MC/Data!!
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
            # Jet corrections are read from txt files
            doNewJEC                 = cms.bool(doNewJEC),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.FileInPath(JEC_txtfile),
            JERSF_txtfile            = cms.FileInPath(JERSF_txtfile),
            JER_txtfile              = cms.FileInPath(JER_txtfile),
            JERAK8_txtfile           = cms.FileInPath(JERAK8_txtfile),
            MCL1JetPar               = cms.FileInPath(MCL1JetPar),
            MCL2JetPar               = cms.FileInPath(MCL2JetPar),
            MCL3JetPar               = cms.FileInPath(MCL3JetPar),
            MCL1JetParAK8            = cms.FileInPath(MCL1JetParAK8),
            MCL2JetParAK8            = cms.FileInPath(MCL2JetParAK8),
            MCL3JetParAK8            = cms.FileInPath(MCL3JetParAK8),
            DataL1JetPar             = cms.FileInPath(DataL1JetPar),
            DataL2JetPar             = cms.FileInPath(DataL2JetPar),
            DataL3JetPar             = cms.FileInPath(DataL3JetPar),
            DataResJetPar            = cms.FileInPath(DataResJetPar),
            DataL1JetParAK8          = cms.FileInPath(DataL1JetParAK8),
            DataL2JetParAK8          = cms.FileInPath(DataL2JetParAK8),
            DataL3JetParAK8          = cms.FileInPath(DataL3JetParAK8),
            DataResJetParAK8         = cms.FileInPath(DataResJetParAK8),


            #Btag
            btag_cuts                = cms.bool(False), #not implemented
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.FileInPath('FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.FileInPath('FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
            BTagUncertUp             = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            BTagUncertDown           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertUp           = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.
            MistagUncertDown          = cms.bool(False), # no longer needed, but can still be utilized. Keep false as default.

            )
if isMC:
    MultiLepSelector_cfg.mctrigger_path_el = hlt_path_el
    MultiLepSelector_cfg.mctrigger_path_mu = hlt_path_mu
    MultiLepSelector_cfg.trigger_path_el = cms.vstring('')
    MultiLepSelector_cfg.trigger_path_mu = cms.vstring('')
else:
    MultiLepSelector_cfg.mctrigger_path_el = cms.vstring('')
    MultiLepSelector_cfg.mctrigger_path_mu = cms.vstring('')
    MultiLepSelector_cfg.trigger_path_el = hlt_path_el
    MultiLepSelector_cfg.trigger_path_mu = hlt_path_mu

MultiLepCalc_cfg = cms.PSet(

            debug                  = cms.bool(False),
            isMc                   = cms.bool(isMC),
            saveLooseLeps          = cms.bool(not isMC),
            keepFullMChistory      = cms.bool(isMC),

            rhoJetsNCInputTag      = cms.InputTag("fixedGridRhoFastjetCentralNeutral",""), #this is for muon
            genParticlesCollection = cms.InputTag("prunedGenParticles"),
            PFparticlesCollection  = cms.InputTag("packedPFCandidates"),

            rhoJetsInputTag            = cms.InputTag("fixedGridRhoFastjetAll"), #this is for electron. Why is it different compared to muon?
            UseElMVA                 = cms.bool(True), #True means save MVA values, False means not saving.
            UseElIDV1                = cms.bool(UseElIDV1_), #False means using ElIDV2.

            # Jet corrections needs to be passed here again if Calc uses jet correction
            doNewJEC                 = cms.bool(doNewJEC),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.FileInPath(JEC_txtfile),
            JERSF_txtfile            = cms.FileInPath(JERSF_txtfile),
            JER_txtfile              = cms.FileInPath(JER_txtfile),
            JERAK8_txtfile           = cms.FileInPath(JERAK8_txtfile),
            MCL1JetPar               = cms.FileInPath(MCL1JetPar),
            MCL2JetPar               = cms.FileInPath(MCL2JetPar),
            MCL3JetPar               = cms.FileInPath(MCL3JetPar),
            MCL1JetParAK8            = cms.FileInPath(MCL1JetParAK8),
            MCL2JetParAK8            = cms.FileInPath(MCL2JetParAK8),
            MCL3JetParAK8            = cms.FileInPath(MCL3JetParAK8),
            DataL1JetPar             = cms.FileInPath(DataL1JetPar),
            DataL2JetPar             = cms.FileInPath(DataL2JetPar),
            DataL3JetPar             = cms.FileInPath(DataL3JetPar),
            DataResJetPar            = cms.FileInPath(DataResJetPar),
            DataL1JetParAK8          = cms.FileInPath(DataL1JetParAK8),
            DataL2JetParAK8          = cms.FileInPath(DataL2JetParAK8),
            DataL3JetParAK8          = cms.FileInPath(DataL3JetParAK8),
            DataResJetParAK8         = cms.FileInPath(DataResJetParAK8),

            #For accessing METnoHF, and METmod
            metnohf_collection = cms.InputTag('slimmedMETsNoHF'),
            metmod_collection = cms.InputTag('slimmedMETsModifiedMET'),

            #Gen stuff
            saveGenHT          = cms.bool(False),
            genJetsCollection  = cms.InputTag("slimmedGenJets"),
            OverrideLHEWeights = cms.bool(isVLQsignal), #TRUE FOR SIGNALS, False otherwise
            basePDFname        = cms.string('NNPDF31_nnlo_as_0118_nf_4'),
            newPDFname         = cms.string('NNPDF31_nnlo_as_0118_nf_4_mc_hessian'),
            keepPDGID          = cms.vuint32(1, 2, 3, 4, 5, 6, 21, 11, 12, 13, 14, 15, 16, 24),
            keepMomPDGID       = cms.vuint32(6, 24),
            keepPDGIDForce     = cms.vuint32(6,6),
            keepStatusForce    = cms.vuint32(62,22),
            cleanGenJets       = cms.bool(True),

            #Btagging - Btag info needs to be passed here again if Calc uses Btagging.
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.FileInPath('FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.FileInPath('FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
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
            puppiCorrPath      = cms.FileInPath('FWLJMET/LJMet/data/PuppiSoftdropMassCorr/weights/puppiCorr.root'),

            rhoJetsInputTag          = cms.InputTag("fixedGridRhoFastjetAll"), #this is for electron. Why is it different compared to muon?

            # Jet recorrections needs to be passed here again if Calc uses jet correction
            doNewJEC                 = cms.bool(doNewJEC),
            JECup                    = cms.bool(JECup),
            JECdown                  = cms.bool(JECdown),
            JERup                    = cms.bool(JERup),
            JERdown                  = cms.bool(JERdown),
            doAllJetSyst             = cms.bool(doAllJetSyst),
            JEC_txtfile              = cms.FileInPath(JEC_txtfile),
            JERSF_txtfile            = cms.FileInPath(JERSF_txtfile),
            JER_txtfile              = cms.FileInPath(JER_txtfile),
            JERAK8_txtfile           = cms.FileInPath(JERAK8_txtfile),
            MCL1JetPar               = cms.FileInPath(MCL1JetPar),
            MCL2JetPar               = cms.FileInPath(MCL2JetPar),
            MCL3JetPar               = cms.FileInPath(MCL3JetPar),
            MCL1JetParAK8            = cms.FileInPath(MCL1JetParAK8),
            MCL2JetParAK8            = cms.FileInPath(MCL2JetParAK8),
            MCL3JetParAK8            = cms.FileInPath(MCL3JetParAK8),
            DataL1JetPar             = cms.FileInPath(DataL1JetPar),
            DataL2JetPar             = cms.FileInPath(DataL2JetPar),
            DataL3JetPar             = cms.FileInPath(DataL3JetPar),
            DataResJetPar            = cms.FileInPath(DataResJetPar),
            DataL1JetParAK8          = cms.FileInPath(DataL1JetParAK8),
            DataL2JetParAK8          = cms.FileInPath(DataL2JetParAK8),
            DataL3JetParAK8          = cms.FileInPath(DataL3JetParAK8),
            DataResJetParAK8         = cms.FileInPath(DataResJetParAK8),

            #Btagging - Btag info needs to be passed here again if Calc uses Btagging.
            btagOP                   = cms.string('MEDIUM'),
            bdisc_min                = cms.double(0.4941), # THIS HAS TO MATCH btagOP !
            applyBtagSF              = cms.bool(True), #This is implemented by BTagSFUtil.cc
            DeepCSVfile              = cms.FileInPath('FWLJMET/LJMet/data/DeepCSV_94XSF_V3_B_F.csv'),
            DeepCSVSubjetfile        = cms.FileInPath('FWLJMET/LJMet/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
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

    dnnFile = cms.FileInPath('FWLJMET/LJMet/data/BEST_mlp.json'),

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
    taggerCfgFile    = cms.FileInPath('TopTagger/TopTagger/data/TopTaggerCfg-DeepResolved_DeepCSV_GR_noDisc_Release_v1.0.0/TopTagger.cfg'),
    discriminatorCut = cms.double(0.5),
    saveAllTopCandidates = cms.bool(False)

    )

## nominal
process.ljmet = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        ttree_name    = cms.string('ljmet'),
        verbosity     = cms.int32(0),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        # 'TTbarMassCalc',
                        # 'DeepAK8Calc',
                        # 'HOTTaggerCalc',
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
        # TTbarMassCalc = cms.PSet(TTbarMassCalc_cfg),
        # DeepAK8Calc    = cms.PSet(), #current ljmet wants all calc to send a PSet, event if its empty.
        # HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg),
        # BestCalc      = cms.PSet(BestCalc_cfg),

        )


## JECup - reset bools for all calcs/selectors that use JEC
MultiLepSelector_cfg.JECup = cms.bool(True)
MultiLepCalc_cfg.JECup     = cms.bool(True)
JetSubCalc_cfg.JECup       = cms.bool(True)
process.ljmet_JECup = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        ttree_name    = cms.string('ljmet_JECup'),
        verbosity     = cms.int32(0),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        # 'TTbarMassCalc',
                        # 'DeepAK8Calc',
                        # 'HOTTaggerCalc',
                        #'BestCalc',
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
        HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg),
        BestCalc      = cms.PSet(BestCalc_cfg),

        )


##JECdown - reset bools for all calcs/selectors that use JEC
MultiLepSelector_cfg.JECup   = cms.bool(False)
MultiLepCalc_cfg.JECup       = cms.bool(False)
JetSubCalc_cfg.JECup         = cms.bool(False)
MultiLepSelector_cfg.JECdown = cms.bool(True)
MultiLepCalc_cfg.JECdown     = cms.bool(True)
JetSubCalc_cfg.JECdown       = cms.bool(True)
process.ljmet_JECdown = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        ttree_name    = cms.string('ljmet_JECdown'),
        verbosity     = cms.int32(0),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        # 'TTbarMassCalc',
                        # 'DeepAK8Calc',
                        # 'HOTTaggerCalc',
                        #'BestCalc',
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
        HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg),
        BestCalc      = cms.PSet(BestCalc_cfg),

        )

##JERup - reset bools for all calcs/selectors that use JEC
MultiLepSelector_cfg.JECup   = cms.bool(False)
MultiLepCalc_cfg.JECup       = cms.bool(False)
JetSubCalc_cfg.JECup         = cms.bool(False)
MultiLepSelector_cfg.JECdown = cms.bool(False)
MultiLepCalc_cfg.JECdown     = cms.bool(False)
JetSubCalc_cfg.JECdown       = cms.bool(False)
MultiLepSelector_cfg.JERup   = cms.bool(True)
MultiLepCalc_cfg.JERup       = cms.bool(True)
JetSubCalc_cfg.JERup         = cms.bool(True)
process.ljmet_JERup = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        ttree_name    = cms.string('ljmet_JERup'),
        verbosity     = cms.int32(0),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        # 'TTbarMassCalc',
                        # 'DeepAK8Calc',
                        # 'HOTTaggerCalc',
                        #'BestCalc',
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
        HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg),
        BestCalc      = cms.PSet(BestCalc_cfg),

        )

##JERup - reset bools for all calcs/selectors that use JEC
MultiLepSelector_cfg.JECup   = cms.bool(False)
MultiLepCalc_cfg.JECup       = cms.bool(False)
JetSubCalc_cfg.JECup         = cms.bool(False)
MultiLepSelector_cfg.JECdown = cms.bool(False)
MultiLepCalc_cfg.JECdown     = cms.bool(False)
JetSubCalc_cfg.JECdown       = cms.bool(False)
MultiLepSelector_cfg.JERup   = cms.bool(False)
MultiLepCalc_cfg.JERup       = cms.bool(False)
JetSubCalc_cfg.JERup         = cms.bool(False)
MultiLepSelector_cfg.JERdown = cms.bool(True)
MultiLepCalc_cfg.JERdown     = cms.bool(True)
JetSubCalc_cfg.JERdown       = cms.bool(True)
process.ljmet_JERdown = cms.EDAnalyzer(
        'LJMet',

        debug         = cms.bool(False),
        ttree_name    = cms.string('ljmet_JERdown'),
        verbosity     = cms.int32(0),
        selector      = cms.string('MultiLepSelector'),
        include_calcs = cms.vstring(
                        'MultiLepCalc',
                        'TpTpCalc',
                        'CommonCalc',
                        'JetSubCalc',
                        # 'TTbarMassCalc',
                        # 'DeepAK8Calc',
                        # 'HOTTaggerCalc',
                        #'BestCalc',
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
        HOTTaggerCalc = cms.PSet(HOTTaggerCalc_cfg),
        BestCalc      = cms.PSet(BestCalc_cfg),

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

    process.p = cms.Path(
                         process.mcweightanalyzer *
                         process.filter_any_explicit *
                         process.fullPatMetSequenceModifiedMET *
                         process.prefiringweight *
                         process.egammaPostRecoSeq *
                         #process.updatedJetsAK8PuppiSoftDropPacked *
                         #process.packedJetsAK8Puppi *
                         #process.QGTagger *
                         process.ecalBadCalibReducedMINIAODFilter *
                         process.ttbarcat *
                         process.ljmet *#(ntuplizer) 
                         process.ljmet_JECup *#(ntuplizer) 
                         process.ljmet_JECdown * #(ntuplizer) 
                         process.ljmet_JERup *#(ntuplizer) 
                         process.ljmet_JERdown #(ntuplizer) 
                         )

elif(isMC):
    process.p = cms.Path(
       process.mcweightanalyzer *
       process.filter_any_explicit *
       process.fullPatMetSequenceModifiedMET *
       process.prefiringweight *
       process.egammaPostRecoSeq *
       #process.updatedJetsAK8PuppiSoftDropPacked *
       #process.packedJetsAK8Puppi *
       #process.QGTagger *
       process.ecalBadCalibReducedMINIAODFilter *
       process.ljmet *#(ntuplizer) 
       process.ljmet_JECup *#(ntuplizer) 
       process.ljmet_JECdown *#(ntuplizer) 
       process.ljmet_JERup *#(ntuplizer) 
       process.ljmet_JERdown #(ntuplizer) 
    )
else: #Data
    process.p = cms.Path(
       process.filter_any_explicit *
       process.fullPatMetSequenceModifiedMET *
       process.prefiringweight *
       process.egammaPostRecoSeq *
       #process.updatedJetsAK8PuppiSoftDropPacked *
       #process.packedJetsAK8Puppi *
       #process.QGTagger *
       process.ecalBadCalibReducedMINIAODFilter *
       process.ljmet #(ntuplizer) 
    )

process.p.associate(patAlgosToolsTask)


# process.ep = cms.EndPath(process.out)
# process.ep = cms.EndPath(process.out,patAlgosToolsTask)
# process.scedule = cms.Schedule(
#     process.p,
#     process.outpath)

