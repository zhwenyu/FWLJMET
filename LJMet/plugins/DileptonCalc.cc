#include "FWLJMET/LJMet/interface/DileptonCalc.h"


using namespace LHAPDF;

DileptonCalc::DileptonCalc()
{
}

int DileptonCalc::BeginJob(edm::ConsumesCollector && iC)
{

    std::cout << "["+GetName()+"]: "<< "initializing parameters" << std::endl;
    
    debug     = mPset.getParameter<bool>("debug");
    isMc      = mPset.getParameter<bool>("isMc");
    dataType  = mPset.getParameter<std::string>("dataType");
    
    
    //Triggers
    doTriggerStudy_ = mPset.getParameter<bool>("doTriggerStudy");
    triggersToken      = iC.consumes<edm::TriggerResults>(mPset.getParameter<edm::InputTag>("TriggerBits"));
    triggerObjectToken = iC.consumes<pat::TriggerObjectStandAloneCollection>(mPset.getParameter<edm::InputTag>("TriggerObjects"));

    //PV
    pvToken = iC.consumes<reco::VertexCollection>(mPset.getParameter<edm::InputTag>("pvSrc"));

    //Misc
    rhoJetsToken = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoJetsInputTag"));
    rhoJetsNCToken = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoJetsNCInputTag"));
    PFCandToken = iC.consumes<pat::PackedCandidateCollection>(mPset.getParameter<edm::InputTag>("PFparticlesCollection"));
    keepFullMChistory = mPset.getParameter<bool>("keepFullMChistory");
    if(debug) cout << "["+GetName()+"]: "<< "keepFullMChistory : " <<    keepFullMChistory << endl;
    
    //Electron    
    UseElMVA = mPset.getParameter<bool>("UseElMVA");
    
    //Jets
    AK8jetsToken             = iC.consumes<pat::JetCollection>(mPset.getParameter<edm::InputTag>("AK8jet_collection"));
	doNewJEC                 = mPset.getParameter<bool>("doNewJEC");
	JECup                    = mPset.getParameter<bool>("JECup");
	JECdown                  = mPset.getParameter<bool>("JECdown");
	JERup                    = mPset.getParameter<bool>("JERup");
	JERdown                  = mPset.getParameter<bool>("JERdown");
	JetMETCorr.Initialize(mPset); // REMINDER: THIS NEEDS --if(!isMc)JetMETCorr.SetFacJetCorr(event)-- somewhere before correcting jets for data, since data JEC is era dependent. !!

	//BTAG parameter initialization
	btagSfUtil.Initialize(mPset);
	
	//MET
	METnoHFtoken        = iC.consumes<std::vector<pat::MET>>(mPset.getParameter<edm::InputTag>("metnohf_collection"));
	METmodToken         = iC.consumes<std::vector<pat::MET>>(mPset.getParameter<edm::InputTag>("metmod_collection"));
	
	
	//Gen
    genParticlesToken   = iC.consumes<reco::GenParticleCollection>(mPset.getParameter<edm::InputTag>("genParticlesCollection"));
    genJetsToken        = iC.consumes<std::vector< reco::GenJet> >(mPset.getParameter<edm::InputTag>("genJetsCollection"));
	genToken            = iC.consumes<GenEventInfoProduct>(edm::InputTag("generator")); //Hardcoding.
	LHEToken            = iC.consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer")); //hardcoding.
	orlhew              = mPset.getParameter<bool>("OverrideLHEWeights");
	basePDFname         = mPset.getParameter<std::string>("basePDFname");
	newPDFname          = mPset.getParameter<std::string>("newPDFname");


    if (orlhew) {
	  std::cout << "["+GetName()+"]: "<< "Overriding LHE weights, using "<<newPDFname<<" as new and "<<basePDFname<<" as base PDF set." << std::endl;
	  LHAPDF::Info& cfg = LHAPDF::getConfig();
	  cfg.set_entry("Verbosity", 0);
	}
    else cout << "["+GetName()+"]: "<< "Writing LHE weights (no override)." << endl;

    
    return 0;
}

int DileptonCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{

    if(debug)std::cout << "Processing Event in DileptonCalc::AnalyzeEvent" << std::endl;    
    
    if(!isMc)JetMETCorr.SetFacJetCorr(event); // FOR JET CORRECTION INITIALIZATION

    AnalyzeDataType(event, selector);
    
    AnalyzeTriggers(event, selector);
    
    AnalyzePV(event, selector);
    
    AnalyzeElectron(event, selector);
    
    AnalyzeMuon(event, selector);
    
    AnalyzeGenJets(event, selector);

    AnalyzeJets(event, selector);

    AnalyzeAK8Jets(event, selector);

    AnalyzeMET(event, selector);
    
    AnalyzeGenInfo(event, selector);

    return 0;
}

void DileptonCalc::AnalyzeDataType(edm::Event const & event, BaseEventSelector * selector)
{

	//What is this for?? -- June 12, 2019

    //
    // _____ Primary dataset (from python cfg) _____________________
    //
    //
    int dataEE = 0;
    int dataEM = 0;
    int dataMM = 0;
    
    if      (dataType == "EE" or dataType == "ElEl") dataEE = 1;
    else if (dataType == "EM" or dataType == "ElMu") dataEM = 1;
    else if (dataType == "MM" or dataType == "MuMu") dataMM = 1;
    else if (dataType == "All" or dataType == "ALL") {
        dataEE = 1; dataEM = 1; dataMM = 1;
    }
    
    SetValue("dataEE", dataEE);
    SetValue("dataEM", dataEM);
    SetValue("dataMM", dataMM);

}

void DileptonCalc::AnalyzeTriggers(edm::Event const & event, BaseEventSelector * selector)
{

    //
    // _____ Get objects from the selector _____________________
    //
    std::vector<unsigned int>             const & vSelTriggers     = selector->GetSelTriggers();

    //
    // ____ Trigger ____________________________
    //

    int passEE = 0;
    int passEM = 0;
    int passMM = 0;
    
    if (vSelTriggers.size() == 3) {
        passEE = (int)vSelTriggers.at(0);
        passEM = (int)vSelTriggers.at(1);
        passMM = (int)vSelTriggers.at(2);
    }
    
    SetValue("trigEE", passEE);
    SetValue("trigEM", passEM);
    SetValue("trigMM", passMM);


    //double el
    bool HLT_Ele27_Ele37=false;
    bool HLT_DoubleEle25=false;
    bool HLT_DoubleEle27=false;
    bool HLT_DoubleEle33=false;

    bool HLT_DoubleEle24_eta2p1_WPTight_Gsf_v=false;
    bool HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v=false;
    bool HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v=false;
    bool HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v=false;
    bool HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v=false;

    //double mu
    bool HLT_Mu37_TkMu27=false;
    bool HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8=false;
    bool HLT_DoubleMu8_Mass8_PFHT350=false;
    bool HLT_DoubleMu4_Mass8_DZ_PFHT350=false;


    //cross
    bool HLT_Mu37Ele27=false;
    bool HLT_Mu27Ele37=false;
    
    bool HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ_v=false;
    bool HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v=false;
    bool HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_v=false;
    bool HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v=false;
    bool HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v=false;
    bool HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v=false;
    bool HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v=false;
    bool HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v=false;


    //single mu
    bool HLT_Mu8=false;
    bool HLT_Mu17=false;
    bool HLT_Mu19=false;
    bool HLT_Mu20=false;
    bool HLT_Mu27=false;
    bool HLT_Mu8_TrkIsoVVL=false;
    bool HLT_Mu17_TrkIsoVVL=false;
    bool HLT_Mu19_TrkIsoVVL=false;
    bool HLT_Mu3_PFJet40=false;
    bool HLT_IsoMu24=false;
    bool HLT_IsoTkMu24=false;
    bool HLT_IsoMu27=false;
    

    //single el
    bool HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v=false;
    bool HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v=false;
    bool HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v=false;

    bool HLT_Ele20_WPLoose_Gsf_v=false;
    bool HLT_Ele20_eta2p1_WPLoose_Gsf_v=false;
    bool HLT_Ele27_WPTight_Gsf_v=false;
    bool HLT_Ele32_WPTight_Gsf_v=false;
    bool HLT_Ele35_WPTight_Gsf_v=false;
    bool HLT_Ele35_WPTight_Gsf_L1EGMT_v=false;
    bool HLT_Ele38_WPTight_Gsf_v=false;
    bool HLT_Ele40_WPTight_Gsf_v=false;
    bool HLT_Ele32_WPTight_Gsf_L1DoubleEG_v=false;
    bool HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v=false;
    bool HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v=false;
    bool HLT_Ele28_HighEta_SC20_Mass55_v=false;

    bool HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v=false;
    bool HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v=false;
    bool HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v=false;

    if(doTriggerStudy_){

      edm::Handle<edm::TriggerResults> triggerBits;
      event.getByToken(triggersToken,triggerBits);
      const edm::TriggerNames &names = event.triggerNames(*triggerBits);
      
      
      for (unsigned int i=0; i!=triggerBits->size(); ++i) {
          std::string Path = names.triggerName(i);
          
          const unsigned int triggerIndex(i);
          if(triggerBits->accept(triggerIndex)){
            //electron paths
            if(Path.find("HLT_Ele27_Ele37_CaloIdL_MW_v") !=std::string::npos) HLT_Ele27_Ele37=true;
            if(Path.find("HLT_DoubleEle25_CaloIdL_MW_v") !=std::string::npos) HLT_DoubleEle25=true;
            if(Path.find("HLT_DoubleEle27_CaloIdL_MW_v") !=std::string::npos) HLT_DoubleEle27=true;
            if(Path.find("HLT_DoubleEle33_CaloIdL_MW_v") !=std::string::npos) HLT_DoubleEle33=true;

            if(Path.find("HLT_DoubleEle24_eta2p1_WPTight_Gsf_v") !=std::string::npos) HLT_DoubleEle24_eta2p1_WPTight_Gsf_v=true;
            if(Path.find("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v") !=std::string::npos) HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v=true;
            if(Path.find("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v") !=std::string::npos) HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v=true; 
            if(Path.find("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v") !=std::string::npos) HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v=true;
            if(Path.find("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v") !=std::string::npos) HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v=true;

            //Muon paths
            if(Path.find("HLT_Mu37_TkMu27_v")!=std::string::npos) HLT_Mu37_TkMu27=true;
            if(Path.find("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8")!=std::string::npos) HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8=true;
            if(Path.find("HLT_DoubleMu8_Mass8_PFHT350")!=std::string::npos) HLT_DoubleMu8_Mass8_PFHT350=true;
            if(Path.find("HLT_DoubleMu4_Mass8_DZ_PFHT350")!=std::string::npos) HLT_DoubleMu4_Mass8_DZ_PFHT350=true;

            //cross paths                
            if(Path.find("HLT_Mu37_Ele27_CaloIdL_MW_v")!=std::string::npos) HLT_Mu37Ele27=true;
            if(Path.find("HLT_Mu27_Ele37_CaloIdL_MW_v")!=std::string::npos) HLT_Mu27Ele37=true;

            if(Path.find("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ_v") !=std::string::npos) HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ_v=true;
            if(Path.find("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v") !=std::string::npos) HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v=true;
            if(Path.find("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_v") !=std::string::npos) HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_v=true;
            if(Path.find("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v") !=std::string::npos) HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v=true;
            if(Path.find("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v") !=std::string::npos) HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v=true;
            if(Path.find("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v") !=std::string::npos) HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v=true;
            if(Path.find("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v") !=std::string::npos) HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v=true;
            if(Path.find("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v") !=std::string::npos) HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v=true;


            //control triggers - mu
            if(Path.find("HLT_Mu8_v")!=std::string::npos) HLT_Mu8=true;
            if(Path.find("HLT_Mu17_v")!=std::string::npos) HLT_Mu17=true;
            if(Path.find("HLT_Mu19_v")!=std::string::npos) HLT_Mu19=true;
            if(Path.find("HLT_Mu20_v")!=std::string::npos) HLT_Mu20=true;
            if(Path.find("HLT_Mu27_v")!=std::string::npos) HLT_Mu27=true;
            if(Path.find("HLT_Mu8_TrkIsoVVL_v")!=std::string::npos) HLT_Mu8_TrkIsoVVL=true;
            if(Path.find("HLT_Mu17_TrkIsoVVL_v")!=std::string::npos) HLT_Mu17_TrkIsoVVL=true;
            if(Path.find("HLT_Mu19_TrkIsoVVL_v")!=std::string::npos) HLT_Mu19_TrkIsoVVL=true;
            if(Path.find("HLT_Mu3_PFJet40_v")!=std::string::npos) HLT_Mu3_PFJet40=true;
            if(Path.find("HLT_IsoMu24_v")!=std::string::npos) HLT_IsoMu24=true;
            if(Path.find("HLT_IsoTkMu24_v")!=std::string::npos) HLT_IsoTkMu24=true;
            if(Path.find("HLT_IsoMu27_v")!=std::string::npos) HLT_IsoMu27=true;


            //control triggers - el
            if(Path.find("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v")!=std::string::npos) HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v=true;
            if(Path.find("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v")!=std::string::npos) HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v=true;
            if(Path.find("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v")!=std::string::npos) HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v=true;

            if(Path.find("HLT_Ele20_WPLoose_Gsf_v")!=std::string::npos) HLT_Ele20_WPLoose_Gsf_v=true;
            if(Path.find("HLT_Ele20_eta2p1_WPLoose_Gsf_v")!=std::string::npos) HLT_Ele20_eta2p1_WPLoose_Gsf_v=true;
            if(Path.find("HLT_Ele27_WPTight_Gsf_v")!=std::string::npos) HLT_Ele27_WPTight_Gsf_v=true;
            if(Path.find("HLT_Ele32_WPTight_Gsf_v")!=std::string::npos) HLT_Ele32_WPTight_Gsf_v=true;
            if(Path.find("HLT_Ele35_WPTight_Gsf_v")!=std::string::npos) HLT_Ele35_WPTight_Gsf_v=true;
            if(Path.find("HLT_Ele35_WPTight_Gsf_L1EGMT_v")!=std::string::npos) HLT_Ele35_WPTight_Gsf_L1EGMT_v=true;
            if(Path.find("HLT_Ele38_WPTight_Gsf_v")!=std::string::npos) HLT_Ele38_WPTight_Gsf_v=true;
            if(Path.find("HLT_Ele40_WPTight_Gsf_v")!=std::string::npos) HLT_Ele40_WPTight_Gsf_v=true;
            if(Path.find("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v")!=std::string::npos) HLT_Ele32_WPTight_Gsf_L1DoubleEG_v=true;
            if(Path.find("HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v")!=std::string::npos) HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v=true;
            if(Path.find("HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v")!=std::string::npos) HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v=true;
            if(Path.find("HLT_Ele28_HighEta_SC20_Mass55_v")!=std::string::npos) HLT_Ele28_HighEta_SC20_Mass55_v=true;

            if(Path.find("HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v")!=std::string::npos) HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v=true;
            if(Path.find("HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v")!=std::string::npos) HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v=true;
            if(Path.find("HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v")!=std::string::npos) HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v=true;
          }
      }
    }

    //double el
    SetValue("HLT_Ele27_Ele37",HLT_Ele27_Ele37);
    SetValue("HLT_DoubleEle25",HLT_DoubleEle25);
    SetValue("HLT_DoubleEle27",HLT_DoubleEle27);
    SetValue("HLT_DoubleEle33",HLT_DoubleEle33);

    SetValue("HLT_DoubleEle24_eta2p1_WPTight_Gsf_v",HLT_DoubleEle24_eta2p1_WPTight_Gsf_v);
    SetValue("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v",HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350_v);
    SetValue("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v",HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350_v);
    SetValue("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v);
    SetValue("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v",HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v);

    //double mu
    SetValue("HLT_Mu37_TkMu27",HLT_Mu37_TkMu27);
    SetValue("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8",HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8);
    SetValue("HLT_DoubleMu8_Mass8_PFHT350",HLT_DoubleMu8_Mass8_PFHT350);
    SetValue("HLT_DoubleMu4_Mass8_DZ_PFHT350",HLT_DoubleMu4_Mass8_DZ_PFHT350);

    //cross
    SetValue("HLT_Mu37Ele27",HLT_Mu37Ele27);
    SetValue("HLT_Mu27Ele37",HLT_Mu27Ele37);

    SetValue("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ_v",HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ_v);
    SetValue("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v);
    SetValue("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_v",HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_v);
    SetValue("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v",HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v);
    SetValue("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v);
    SetValue("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v);
    SetValue("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v",HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v);
    SetValue("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v",HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v);


    //single mu
    SetValue("HLT_Mu8",HLT_Mu8);
    SetValue("HLT_Mu17",HLT_Mu17);
    SetValue("HLT_Mu19",HLT_Mu19);
    SetValue("HLT_Mu20",HLT_Mu20);
    SetValue("HLT_Mu27",HLT_Mu27);
    SetValue("HLT_Mu8_TrkIsoVVL",HLT_Mu8_TrkIsoVVL);
    SetValue("HLT_Mu17_TrkIsoVVL",HLT_Mu17_TrkIsoVVL);
    SetValue("HLT_Mu19_TrkIsoVVL",HLT_Mu19_TrkIsoVVL);
    SetValue("HLT_Mu3_PFJet40",HLT_Mu3_PFJet40);
    SetValue("HLT_IsoMu24",HLT_IsoMu24);
    SetValue("HLT_IsoTkMu24",HLT_IsoTkMu24);
    SetValue("HLT_IsoMu27",HLT_IsoMu27);

    //single el
    SetValue("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v);
    SetValue("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v);
    SetValue("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v",HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v);

    SetValue("HLT_Ele20_WPLoose_Gsf_v",HLT_Ele20_WPLoose_Gsf_v);
    SetValue("HLT_Ele20_eta2p1_WPLoose_Gsf_v",HLT_Ele20_eta2p1_WPLoose_Gsf_v);
    SetValue("HLT_Ele27_WPTight_Gsf_v",HLT_Ele27_WPTight_Gsf_v);
    SetValue("HLT_Ele32_WPTight_Gsf_v",HLT_Ele32_WPTight_Gsf_v);
    SetValue("HLT_Ele35_WPTight_Gsf_v",HLT_Ele35_WPTight_Gsf_v);
    SetValue("HLT_Ele35_WPTight_Gsf_L1EGMT_v",HLT_Ele35_WPTight_Gsf_L1EGMT_v);
    SetValue("HLT_Ele38_WPTight_Gsf_v",HLT_Ele38_WPTight_Gsf_v);
    SetValue("HLT_Ele40_WPTight_Gsf_v",HLT_Ele40_WPTight_Gsf_v);
    SetValue("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v",HLT_Ele32_WPTight_Gsf_L1DoubleEG_v);
    SetValue("HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v",HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v);
    SetValue("HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v",HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v);
    SetValue("HLT_Ele28_HighEta_SC20_Mass55_v",HLT_Ele28_HighEta_SC20_Mass55_v);

    SetValue("HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v",HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v);
    SetValue("HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v);
    SetValue("HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v",HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v);



}

void DileptonCalc::AnalyzePV(edm::Event const & event, BaseEventSelector * selector)
{

    //This currently not the same as calculated by MultiLepCalc ! Should this be modified? -- June 12, 2019 

    //Primary vertices
    edm::Handle<std::vector<reco::Vertex> > pvHandle;
    event.getByToken(pvToken, pvHandle);
    goodPVs = *(pvHandle.product());
    
    SetValue("nPV", (int)goodPVs.size());

}

void DileptonCalc::AnalyzeElectron(edm::Event const & event, BaseEventSelector * selector)
{

    //
    // _____ Get objects from the selector _____________________
    //
    std::vector<edm::Ptr<pat::Electron> > const & vSelElectrons    = selector->GetSelElectrons();

    //
    //_____ Electrons _________________________
    //
    
    //Four std::vector
    std::vector <double> elPt;
    std::vector <double> elEta;
    std::vector <double> elPhi;
    std::vector <double> elEnergy;
    
    //Quality criteria
    std::vector <double> elRelIso;
    std::vector <double> elMiniIsoEA;
    std::vector <double> elMiniIsoDB;
    std::vector <double> elMiniIsoSUSY;
    std::vector <double> elDxy;
    std::vector <int>    elNotConversion;
    std::vector <int>    elChargeConsistent;
    std::vector <int>    elIsEBEE;
    std::vector <int>    elQuality;
    std::vector <int>    elCharge;
    std::vector <int>    elGsfCharge;
    std::vector <int>    elCtfCharge;
    std::vector <int>    elScPixCharge;
    
    //ID requirement
    std::vector <double> elDeta;
    std::vector <double> elDphi;
    std::vector <double> elSihih;
    std::vector <double> elHoE;
    std::vector <double> elD0;
    std::vector <double> elSIP3D;
    std::vector <double> elDZ;
    std::vector <double> elOoemoop;
    std::vector <int>    elMHits;
    std::vector <int>    elVtxFitConv;

    std::vector <double> elMVAValue;
    std::vector <double> elIsMVATight90;
    std::vector <double> elIsMVATight80;
    std::vector <double> elIsMVALoose;
    std::vector <double> elMVAValue_iso;
    std::vector <double> elIsMVATightIso90;
    std::vector <double> elIsMVATightIso80;
    std::vector <double> elIsMVALooseIso;
    std::vector <double> elIsTight;
    std::vector <double> elIsMedium;
    std::vector <double> elIsLoose;
    std::vector <double> elIsVeto;
    
    //added CMSDAS variables
    std::vector <double> diElMass;
    std::vector <int> elCharge1;
    std::vector <int> elCharge2;

    //Extra info about isolation
    std::vector <double> elChIso;
    std::vector <double> elNhIso;
    std::vector <double> elPhIso;
    std::vector <double> elAEff;
    std::vector <double> elRhoIso;
    std::vector <double> elPUIso;
    
    //mother-information
    //Generator level information -- MC matching
    std::vector<double> elGen_Reco_dr;
    std::vector<int> elPdgId;
    std::vector<int> elStatus;
    std::vector<int> elMatched;
    std::vector<int> elNumberOfMothers;
    std::vector<double> elMother_pt;
    std::vector<double> elMother_eta;
    std::vector<double> elMother_phi;
    std::vector<double> elMother_energy;
    std::vector<int> elMother_id;
    std::vector<int> elMother_status;
    //Matched gen electron information:
    std::vector<double> elMatchedPt;
    std::vector<double> elMatchedEta;
    std::vector<double> elMatchedPhi;
    std::vector<double> elMatchedEnergy;
    std::vector<std::string> TriggerElectronFilters;
    std::vector<double> TriggerElectronPts;
    std::vector<double> TriggerElectronEtas;
    std::vector<double> TriggerElectronPhis;
    std::vector<double> TriggerElectronEnergies;
    
    edm::Handle<double> rhoHandle;
    event.getByToken(rhoJetsToken, rhoHandle);
    rhoIso = std::max(*(rhoHandle.product()), 0.0);

    //rho isolation from susy recommendation
    edm::Handle<double> rhoJetsNC;
    event.getByToken(rhoJetsNCToken , rhoJetsNC);
    double myRhoJetsNC = *rhoJetsNC;
    double _rhoNC = myRhoJetsNC;

    //pfcandidates for mini iso
    
    edm::Handle<pat::PackedCandidateCollection> packedPFCands;
    event.getByToken(PFCandToken, packedPFCands);
        

    //keep track of which electron we are looking at
    int ElIndex = 0;
    for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
      //Protect against electrons without tracks (should never happen, but just in case)
      if ((*iel)->gsfTrack().isNonnull() and (*iel)->gsfTrack().isAvailable()){
      
          //Four std::vector
          elPt     . push_back((*iel)->ecalDrivenMomentum().pt()); //Must check: why ecalDrivenMomentum?
          elEta    . push_back((*iel)->ecalDrivenMomentum().eta());
          elPhi    . push_back((*iel)->ecalDrivenMomentum().phi());
          elEnergy . push_back((*iel)->ecalDrivenMomentum().energy());       
        
          //if there are two electrons calculate invariant mass from two highest pt objects
          if(vSelElectrons.size()==2){
            for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iiel = vSelElectrons.begin(); iiel != vSelElectrons.end(); iiel++){
              //float mass = pow( (*iel)->energy() * (*iiel)->energy() - ( (*iel)->pt() * (*iiel)->pt() - (*iel)->pz() * (*iiel)->pz()), 0.5);
              if(iiel!=iel){
                TLorentzVector diElFourVec( (*iel)->px() + (*iiel)->px(),(*iel)->py() + (*iiel)->py(),(*iel)->pz() + (*iiel)->pz(), (*iel)->energy() + (*iiel)->energy());
                diElMass.push_back(diElFourVec.M());
                elCharge1.push_back( (*iel)->charge());
                elCharge2.push_back( (*iiel)->charge());
              }
            }
          }
          else{ 
            diElMass.push_back(-1);
            elCharge1.push_back(-99999);
            elCharge2.push_back(-99998);
          }

          //Isolation
          
          // WTF are these hardcoded values below ?! People should fucking stop hardcoding things without doc !! -- June 11, 2019.
          double scEta = (*iel)->superCluster()->eta();
          double AEff;
          if(fabs(scEta) >2.4) AEff = 0.1524;
          else if(fabs(scEta) >2.3) AEff = 0.1204;
          else if(fabs(scEta) >2.2) AEff = 0.1051;
          else if(fabs(scEta) >2.0) AEff = 0.0854;
          else if(fabs(scEta) >1.479) AEff = 0.1073;
          else if(fabs(scEta) >1.0) AEff = 0.1626;
          else AEff = 0.1566;
          
          reco::GsfElectron::PflowIsolationVariables pfIso = (*iel)->pfIsolationVariables();
          double chIso = pfIso.sumChargedHadronPt;
          double nhIso = pfIso.sumNeutralHadronEt;
          double phIso = pfIso.sumPhotonEt;
          double PUIso = pfIso.sumPUPt;
          double relIso = ( chIso + std::max(0.0, nhIso + phIso - rhoIso*AEff) ) / (*iel)->pt();
          
          elChIso  . push_back(chIso);
          elNhIso  . push_back(nhIso);
          elPhIso  . push_back(phIso);
          elPUIso  . push_back(PUIso);
          elAEff   . push_back(AEff);
          elRhoIso . push_back(rhoIso);
        
          elRelIso . push_back(relIso);
        
          //Conversion rejection
          int nLostHits = (*iel)->gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS);
          double dist   = (*iel)->convDist();
          double dcot   = (*iel)->convDcot();
          int notConv   = nLostHits == 0 and (fabs(dist) > 0.02 or fabs(dcot) > 0.02);
          //get three different charges
          elGsfCharge.push_back( (*iel)->gsfTrack()->charge());
          if( (*iel)->closestCtfTrackRef().isNonnull()) elCtfCharge.push_back((*iel)->closestCtfTrackRef()->charge());
          else elCtfCharge.push_back(-999);
          elScPixCharge.push_back((*iel)->scPixCharge());
          elCharge.push_back((*iel)->charge());
          elNotConversion . push_back(notConv);
                
          //IP: for some reason this is with respect to the first vertex in the collection
          if(goodPVs.size() > 0){
            elDxy.push_back((*iel)->gsfTrack()->dxy(goodPVs.at(0).position()));
            elDZ.push_back((*iel)->gsfTrack()->dz(goodPVs.at(0).position()));
          } else {
            elDxy.push_back(-999);
            elDZ.push_back(-999);
          }
          elChargeConsistent.push_back((*iel)->isGsfCtfScPixChargeConsistent());
          elIsEBEE.push_back(((*iel)->isEBEEGap()<<2) + ((*iel)->isEE()<<1) + (*iel)->isEB());
          elDeta.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
          elDphi.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());
          elSihih.push_back((*iel)->full5x5_sigmaIetaIeta());
          elHoE.push_back((*iel)->hcalOverEcal());
          elD0.push_back((*iel)->dB());
          pat::Electron::IpType elIP3d = (pat::Electron::IpType) 1;
          float sip3d = (*iel)->dB(elIP3d) / (*iel)->edB(elIP3d);
          elSIP3D.push_back(sip3d);
          elOoemoop.push_back(1.0/(*iel)->ecalEnergy() - (*iel)->eSuperClusterOverP()/(*iel)->ecalEnergy());
          elMHits.push_back((*iel)->gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS));
          elVtxFitConv.push_back((*iel)->passConversionVeto());


          //Trigger Matching - store 4-std::vector and filter information for all trigger objects deltaR matched to electrons
//           if(doTriggerStudy_){
//             
//             //read in trigger objects
//             edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
//             event.getByToken(triggerObjectToken,triggerObjects);
//             
//             edm::Handle<edm::TriggerResults> triggerBits;
//             event.getByToken(triggersToken,triggerBits);
//             const edm::TriggerNames &names = event.triggerNames(*triggerBits);
//             
//             //loop over them for deltaR matching
//             TLorentzVector trigObj;
//             pat::TriggerObjectStandAlone matchedObj;
//             std::vector<std::string> paths;
//             float closestDR = 10000.;
//             for( pat::TriggerObjectStandAlone obj : *triggerObjects){
//               obj.unpackPathNames(names);
//               float dR = mdeltaR( (*iel)->eta(), (*iel)->phi(), obj.eta(),obj.phi() );
//               if(dR < closestDR){
//                 closestDR = dR;
//                 trigObj.SetPtEtaPhiE((*iel)->pt(),(*iel)->eta(),(*iel)->phi(),(*iel)->energy());
//                 matchedObj=obj;
//               }
//             }
//             if(closestDR<0.5){
//               TriggerElectronPts.push_back(trigObj.Pt());
//               TriggerElectronEtas.push_back(trigObj.Eta());
//               TriggerElectronPhis.push_back(trigObj.Phi());
//               TriggerElectronEnergies.push_back(trigObj.Energy());
//               //std::cout<<"found matched trigger object!"<<std::endl;
//               //now store information about filters
//               for (unsigned h = 0; h < matchedObj.filterLabels().size(); ++h){
//                 std::string filter = matchedObj.filterLabels()[h];
//                 std::string Index = Form("MatchedIndex%i_",ElIndex);
//                 std::string hltFilter_wIndex = Index+filter;
//                 //std::cout<<hltFilter_wIndex<<std::endl;
//                 TriggerElectronFilters.push_back(hltFilter_wIndex);
//               }              
//             }
//             else{
//               TriggerElectronPts.push_back(-9999);
//               TriggerElectronEtas.push_back(-9999);
//               TriggerElectronPhis.push_back(-9999);
//               TriggerElectronEnergies.push_back(-9999);
//             }
//           }
          
        if (UseElMVA) {
            // See singleLepCalc for more IDs and usingV2
            elIsMVATight80.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wp80"));
            elIsMVATight90.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wp90"));
            elIsMVALoose.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wpLoose"));
            elMVAValue.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV2Values"));

            elIsMVATightIso80.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wp80"));
            elIsMVATightIso90.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wp90"));
            elIsMVALooseIso.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wpLoose"));
            elMVAValue_iso.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17IsoV2Values"));
        }
        
        elIsTight.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-tight"));
        elIsMedium.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-medium"));
        elIsLoose.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-loose"));
        elIsVeto.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-veto"));



        if(isMc && keepFullMChistory){
            //cout << "start\n";
            edm::Handle<reco::GenParticleCollection> genParticles;
            event.getByToken(genParticlesToken, genParticles);
            int matchId = findMatch(*genParticles, 11, (*iel)->eta(), (*iel)->phi());
            double closestDR = 10000.;
            //cout << "matchId "<<matchId <<endl;
            if (matchId>=0) {
              const reco::GenParticle & p = (*genParticles).at(matchId);
              closestDR = mdeltaR( (*iel)->eta(), (*iel)->phi(), p.eta(), p.phi());
              //cout << "closestDR "<<closestDR <<endl;
              if(closestDR < 0.3){
                elGen_Reco_dr.push_back(closestDR);
                elPdgId.push_back(p.pdgId());
                elStatus.push_back(p.status());
                elMatched.push_back(1);
                elMatchedPt.push_back( p.pt());
                elMatchedEta.push_back(p.eta());
                elMatchedPhi.push_back(p.phi());
                elMatchedEnergy.push_back(p.energy());
                int oldSize = elMother_id.size();
                fillMotherInfo(p.mother(), 0, elMother_id, elMother_status, elMother_pt, elMother_eta, elMother_phi, elMother_energy);
                elNumberOfMothers.push_back(elMother_id.size()-oldSize);
              }
            }
            if(closestDR >= 0.3){
              elNumberOfMothers.push_back(-1);
              elGen_Reco_dr.push_back(-1.0);
              elPdgId.push_back(-1);
              elStatus.push_back(-1);
              elMatched.push_back(0);
              elMatchedPt.push_back(-1000.0);
              elMatchedEta.push_back(-1000.0);
              elMatchedPhi.push_back(-1000.0);
              elMatchedEnergy.push_back(-1000.0);
            
            }
          
          
        }//closing the isMC checking criteria
      }
      //increment index
      ElIndex+=1;
    }
    
    //trigger info
    SetValue("TrigElPt",TriggerElectronPts);
    SetValue("TrigElEta",TriggerElectronEtas);
    SetValue("TrigElPhi",TriggerElectronPhis);
    SetValue("TrigElEnergy",TriggerElectronEnergies);
    SetValue("TrigElFilters",TriggerElectronFilters);

    //Four std::vector
    SetValue("elPt"     , elPt);
    SetValue("elEta"    , elEta);
    SetValue("elPhi"    , elPhi);
    SetValue("elEnergy" , elEnergy);

    //cmsdas variables
    SetValue("diElMass", diElMass);
    SetValue("elCharge1", elCharge1);
    SetValue("elCharge2", elCharge2);
    
    SetValue("elGsfCharge",elGsfCharge);
    SetValue("elCtfCharge",elCtfCharge);
    SetValue("elScPixCharge",elScPixCharge);
    SetValue("elCharge", elCharge);

    //Quality requirements
    SetValue("elRelIso" , elRelIso); //Isolation
    SetValue("elDxy"    , elDxy);    //Dxy
    SetValue("elNotConversion" , elNotConversion);  //Conversion rejection
    SetValue("elChargeConsistent", elChargeConsistent);
    SetValue("elIsEBEE", elIsEBEE);

//     SetValue("elQuality", elQuality); // Not used -- June 12, 2019.
//     SetValue("elMiniIsoEA",elMiniIsoEA); // Not used -- June 12, 2019.
//     SetValue("elMiniIsoDB",elMiniIsoDB); // Not used -- June 12, 2019.
//     SetValue("elMiniIsoSUSY",elMiniIsoSUSY); // Not used -- June 12, 2019.

    //this value not specific to electrons but we set it here
    SetValue("rhoNC",_rhoNC);

    //ID cuts
    SetValue("elDeta", elDeta);
    SetValue("elDphi", elDphi);
    SetValue("elSihih", elSihih);
    SetValue("elHoE", elHoE);
    SetValue("elD0", elD0);
    SetValue("elSIP3D",elSIP3D);
    SetValue("elDZ", elDZ);
    SetValue("elOoemoop", elOoemoop);
    SetValue("elMHits", elMHits);
    SetValue("elVtxFitConv", elVtxFitConv);


    SetValue("elMVAValue", elMVAValue);
    SetValue("elIsMVATight90", elIsMVATight90);
    SetValue("elIsMVATight80", elIsMVATight80);
    SetValue("elIsMVALoose", elIsMVALoose);

    SetValue("elMVAValue_iso", elMVAValue_iso);
    SetValue("elIsMVATightIso90",elIsMVATightIso90);
    SetValue("elIsMVATightIso80",elIsMVATightIso80);
    SetValue("elIsMVALooseIso",elIsMVALooseIso);

    SetValue("elIsTight", elIsTight);
    SetValue("elIsMedium", elIsMedium);
    SetValue("elIsLoose", elIsLoose);
    SetValue("elIsVeto", elIsVeto);


    //Extra info about isolation
    SetValue("elChIso" , elChIso);
    SetValue("elNhIso" , elNhIso);
    SetValue("elPhIso" , elPhIso);
    SetValue("elAEff"  , elAEff);
    SetValue("elRhoIso", elRhoIso);
    SetValue("elPUIso", elPUIso);
    
    //MC matching -- mother information
    SetValue("elNumberOfMothers", elNumberOfMothers);
    SetValue("elGen_Reco_dr", elGen_Reco_dr);
    SetValue("elPdgId", elPdgId);
    SetValue("elStatus", elStatus);
    SetValue("elMatched",elMatched);
    SetValue("elMother_pt", elMother_pt);
    SetValue("elMother_eta", elMother_eta);
    SetValue("elMother_phi", elMother_phi);
    SetValue("elMother_energy", elMother_energy);
    SetValue("elMother_status", elMother_status);
    SetValue("elMother_id", elMother_id);
    //Matched gen muon information:
    SetValue("elMatchedPt", elMatchedPt);
    SetValue("elMatchedEta", elMatchedEta);
    SetValue("elMatchedPhi", elMatchedPhi);
    SetValue("elMatchedEnergy", elMatchedEnergy);
    

}

void DileptonCalc::AnalyzeMuon(edm::Event const & event, BaseEventSelector * selector)
{

    //
    // _____ Get objects from the selector _____________________
    //
    std::vector<edm::Ptr<pat::Muon> >     const & vSelMuons        = selector->GetSelMuons();

    //
    //_____ Muons _____________________________
    //
    
    std::vector <int> muCharge;
    std::vector <bool> muGlobal;
    std::vector <bool> muTracker;
    std::vector <bool> muPF;
    //Four std::vector
    std::vector <double> muPt;
    std::vector <double> muEta;
    std::vector <double> muPhi;
    std::vector <double> muEnergy;
    
    //Quality criteria
    std::vector <double> muChi2;
    std::vector <double> muDxy;
    std::vector <double> muDz;
    std::vector <double> muSIP3D;
    std::vector <double> muRelIso;
    std::vector <double> muMiniIsoEA;
    std::vector <double> muMiniIsoDB;
    std::vector <double> muMiniIsoSUSY;
    
    std::vector <int> muNValMuHits;
    std::vector <int> muNMatchedStations;
    std::vector <int> muNValPixelHits;
    std::vector <int> muNTrackerLayers;
    
    //Extra info about isolation
    std::vector <double> muChIso;
    std::vector <double> muNhIso;
    std::vector <double> muGIso;
    std::vector <double> muPuIso;

    //ID info
    std::vector <int> muIsTight;
    std::vector<int> muIsLoose;
    
    //Generator level information -- MC matching
    std::vector<double> muGen_Reco_dr;
    std::vector<int> muPdgId;
    std::vector<int> muStatus;
    std::vector<int> muMatched;
    std::vector<int> muNumberOfMothers;
    std::vector<double> muMother_pt;
    std::vector<double> muMother_eta;
    std::vector<double> muMother_phi;
    std::vector<double> muMother_energy;
    std::vector<int> muMother_id;
    std::vector<int> muMother_status;
    //Matched gen muon information:
    std::vector<double> muMatchedPt;
    std::vector<double> muMatchedEta;
    std::vector<double> muMatchedPhi;
    std::vector<double> muMatchedEnergy;

    std::vector<std::string> TriggerMuonFilters;
    std::vector<double> TriggerMuonPts;
    std::vector<double> TriggerMuonEtas;
    std::vector<double> TriggerMuonPhis;
    std::vector<double> TriggerMuonEnergies;
    

    //make index for muons
    int MuIndex = 0;
    for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++){
        //Protect against muons without tracks (should never happen, but just in case)
        if ((*imu)->globalTrack().isNonnull() and (*imu)->globalTrack().isAvailable() and
            (*imu)->innerTrack().isNonnull()  and (*imu)->innerTrack().isAvailable())
            {
            
            
            //charge
            muCharge.push_back((*imu)->charge());
            
            //Four std::vector
            muPt     . push_back((*imu)->pt());
            muEta    . push_back((*imu)->eta());
            muPhi    . push_back((*imu)->phi());
            muEnergy . push_back((*imu)->energy());
            
            muIsTight.push_back((*imu)->passed(reco::Muon::CutBasedIdTight));
            muIsLoose.push_back((*imu)->passed(reco::Muon::CutBasedIdLoose));

            muGlobal.push_back((*imu)->isGlobalMuon());
            muTracker.push_back((*imu)->isTrackerMuon());
            muPF.push_back((*imu)->isPFMuon());
            
            //Chi2
            muChi2 . push_back((*imu)->globalTrack()->normalizedChi2());
            
            //new definition of iso based on muon pog page
            const reco::MuonPFIsolation pfIsolationR04 = (*imu)->pfIsolationR04();
            double chIso  = pfIsolationR04.sumChargedHadronPt;
            double nhIso  = pfIsolationR04.sumNeutralHadronEt;
            double gIso   = pfIsolationR04.sumPhotonEt;
            double puIso  = pfIsolationR04.sumPUPt;
            double relIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso)) / (*imu)->pt();
            muRelIso . push_back(relIso);

            muChIso . push_back(chIso);
            muNhIso . push_back(nhIso);
            muGIso  . push_back(gIso);
            muPuIso . push_back(puIso);

            float musip3d;
            pat::Muon::IpType muIP3d = (pat::Muon::IpType) 1;
            
            //IP: for some reason this is with respect to the first vertex in the collection
            if (goodPVs.size() > 0){
                muDxy . push_back((*imu)->dB());
                muDz  . push_back((*imu)->muonBestTrack()->dz(goodPVs.at(0).position()));
                //muon sip3d;
                musip3d = (*imu)->dB(muIP3d) / (*imu)->edB(muIP3d);

            } 
            else {
                muDxy . push_back(-999);
                muDz  . push_back(-999);
                musip3d=-999;
            }
            muSIP3D.push_back(musip3d);
            
            //Numbers of hits
            muNValMuHits       . push_back((*imu)->globalTrack()->hitPattern().numberOfValidMuonHits());
            muNMatchedStations . push_back((*imu)->numberOfMatchedStations());
            muNValPixelHits    . push_back((*imu)->innerTrack()->hitPattern().numberOfValidPixelHits());
            muNTrackerLayers   . push_back((*imu)->innerTrack()->hitPattern().trackerLayersWithMeasurement());
            
            
            //Trigger Matching - store 4-std::vector and filter information for all trigger objects deltaR matched to electrons
//             if(doTriggerStudy_){
//                 //read in trigger objects
//                 edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
//                 event.getByToken(triggerObjectToken,triggerObjects);
//                 
//                 edm::Handle<edm::TriggerResults> triggerBits;
//                 event.getByToken(triggersToken,triggerBits);
//                 const edm::TriggerNames &names = event.triggerNames(*triggerBits);
//                 
//                 //loop over them for deltaR matching
//                 TLorentzVector trigObj;
//                 pat::TriggerObjectStandAlone matchedObj;
//                 std::vector<std::string> paths;
//                 float closestDR = 10000.;
//                 for( pat::TriggerObjectStandAlone obj : *triggerObjects){
//                     obj.unpackPathNames(names);
//                     float dR = mdeltaR( (*imu)->eta(), (*imu)->phi(), obj.eta(),obj.phi() );
//                     if(dR < closestDR){
//                         closestDR = dR;
//                         trigObj.SetPtEtaPhiE((*imu)->pt(),(*imu)->eta(),(*imu)->phi(),(*imu)->energy());
//                         matchedObj=obj;
//                     }
//                 }
//                 if(closestDR<0.5){
//                     TriggerMuonPts.push_back(trigObj.Pt());
//                     TriggerMuonEtas.push_back(trigObj.Eta());
//                     TriggerMuonPhis.push_back(trigObj.Phi());
//                     TriggerMuonEnergies.push_back(trigObj.Energy());
//                     //std::cout<<"found muon matched trigger object!"<<std::endl;
//                     //now store information about filters
//                     for (unsigned int h = 0; h < matchedObj.filterLabels().size(); h++){
//                         std::string filter = matchedObj.filterLabels()[h];
//                         std::string Index = Form("MatchedIndex%i_",MuIndex);
//                         std::string hltFilter_wIndex = Index+filter;
//                         //std::cout<<hltFilter_wIndex<<std::endl;
//                         TriggerMuonFilters.push_back(hltFilter_wIndex);
//                     }              
//                 }
//                 else{
//                     TriggerMuonPts.push_back(-9999);
//                     TriggerMuonEtas.push_back(-9999);
//                     TriggerMuonPhis.push_back(-9999);
//                     TriggerMuonEnergies.push_back(-9999);
//                 }
//             }

            
            if(isMc && keepFullMChistory){
                edm::Handle<reco::GenParticleCollection> genParticles;
                event.getByToken(genParticlesToken, genParticles);
                int matchId = findMatch(*genParticles, 13, (*imu)->eta(), (*imu)->phi());
                double closestDR = 10000.;
                if (matchId>=0) {
                    const reco::GenParticle & p = (*genParticles).at(matchId);
                    closestDR = mdeltaR( (*imu)->eta(), (*imu)->phi(), p.eta(), p.phi());
                    if(closestDR < 0.3){
                        muGen_Reco_dr.push_back(closestDR);
                        muPdgId.push_back(p.pdgId());
                        muStatus.push_back(p.status());
                        muMatched.push_back(1);
                        muMatchedPt.push_back( p.pt());
                        muMatchedEta.push_back(p.eta());
                        muMatchedPhi.push_back(p.phi());
                        muMatchedEnergy.push_back(p.energy());
                        int oldSize = muMother_id.size();
                        fillMotherInfo(p.mother(), 0, muMother_id, muMother_status, muMother_pt, muMother_eta, muMother_phi, muMother_energy);
                        muNumberOfMothers.push_back(muMother_id.size()-oldSize);
                    }
                }
                if(closestDR >= 0.3){
                    muNumberOfMothers.push_back(-1);
                    muGen_Reco_dr.push_back(-1.0);
                    muPdgId.push_back(-1);
                    muStatus.push_back(-1);
                    muMatched.push_back(0);
                    muMatchedPt.push_back(-1000.0);
                    muMatchedEta.push_back(-1000.0);
                    muMatchedPhi.push_back(-1000.0);
                    muMatchedEnergy.push_back(-1000.0);
                    
                }
                
                
            }

        }
        
        //increment index
        MuIndex+=1;
    }

    //trigger info
    SetValue("TrigMuPt",TriggerMuonPts);
    SetValue("TrigMuEta",TriggerMuonEtas);
    SetValue("TrigMuPhi",TriggerMuonPhis);
    SetValue("TrigMuEnergy",TriggerMuonEnergies);
    SetValue("TrigMuFilters",TriggerMuonFilters);
    
    
    SetValue("muCharge", muCharge);
    SetValue("muGlobal", muGlobal);
    SetValue("muTracker",muTracker);
    SetValue("muPF",muPF);

    //Four std::vector
    SetValue("muPt"     , muPt);
    SetValue("muEta"    , muEta);
    SetValue("muPhi"    , muPhi);
    SetValue("muEnergy" , muEnergy);
  
    //muon ids
    SetValue("muIsTight", muIsTight);
    SetValue("muIsLoose",muIsLoose);
  
    //Quality criteria
    SetValue("muChi2"   , muChi2);
    SetValue("muDxy"    , muDxy);
    SetValue("muDz"     , muDz);
    SetValue("muSIP3D", muSIP3D);
    SetValue("muRelIso" , muRelIso);

//     SetValue("muMiniIsoEA", muMiniIsoEA); // Not used -- June 12, 2019.
//     SetValue("muMiniIsoDB", muMiniIsoDB); // Not used -- June 12, 2019.
//     SetValue("muMiniIsoSUSY", muMiniIsoSUSY); // Not used -- June 12, 2019.

    SetValue("muNValMuHits"       , muNValMuHits);
    SetValue("muNMatchedStations" , muNMatchedStations);
    SetValue("muNValPixelHits"    , muNValPixelHits);
    SetValue("muNTrackerLayers"   , muNTrackerLayers);
    
    //Extra info about isolation
    SetValue("muChIso", muChIso);
    SetValue("muNhIso", muNhIso);
    SetValue("muGIso" , muGIso);
    SetValue("muPuIso", muPuIso);
    
    //MC matching -- mother information
    SetValue("muGen_Reco_dr", muGen_Reco_dr);
    SetValue("muPdgId", muPdgId);
    SetValue("muStatus", muStatus);
    SetValue("muMatched",muMatched);
    SetValue("muMother_pt", muMother_pt);
    SetValue("muMother_eta", muMother_eta);
    SetValue("muMother_phi", muMother_phi);
    SetValue("muMother_energy", muMother_energy);
    SetValue("muMother_status", muMother_status);
    SetValue("muMother_id", muMother_id);
    SetValue("muNumberOfMothers", muNumberOfMothers);
    //Matched gen muon information:
    SetValue("muMatchedPt", muMatchedPt);
    SetValue("muMatchedEta", muMatchedEta);
    SetValue("muMatchedPhi", muMatchedPhi);
    SetValue("muMatchedEnergy", muMatchedEnergy);


}

void DileptonCalc::AnalyzeGenJets(edm::Event const & event, BaseEventSelector * selector)
{

    //
    //_____GenJets_____________________________
    //

    if(isMc){
      edm::Handle<std::vector<reco::GenJet> > genJets;
      
      event.getByToken(genJetsToken,genJets);
    
      std::vector<double> genJetPt;
      std::vector<double> genJetEta;
      std::vector<double> genJetPhi;
      std::vector<double> genJetEnergy;
      std::vector<double> genJetMass;
      
      for(std::vector<reco::GenJet>::const_iterator ijet = genJets->begin(); ijet != genJets->end(); ijet++){
          
          genJetPt.push_back(ijet->pt());
          genJetEta.push_back(ijet->eta());
          genJetPhi.push_back(ijet->phi());
          genJetEnergy.push_back(ijet->energy());
          genJetMass.push_back(ijet->mass());
          //cout<<"setting gen jet info"<<endl;
          
      }
      
      SetValue("genJetPt", genJetPt);
      SetValue("genJetEta", genJetEta);
      SetValue("genJetPhi", genJetPhi);
      SetValue("genJetEnergy", genJetEnergy);
      SetValue("genJetMass", genJetMass);
    }

}

void DileptonCalc::AnalyzeJets(edm::Event const & event, BaseEventSelector * selector)
{

    //
    // _____ Get objects from the selector _____________________
    //
    std::vector<edm::Ptr<pat::Jet> >      const & vSelJets         = selector->GetSelJets();
    std::vector<pat::Jet>                 const & vSelCleanedJets  = selector->GetSelCleanedJets();            


    //
    //_____ Jets ______________________________
    //


    //for jet correction
    unsigned int syst;
    bool reCorrectJet = doNewJEC;
    if (JECup){syst=1;}
    else if (JECdown){syst=2;}
    else if (JERup){syst=3;}
    else if (JERdown){syst=4;}
    else syst = 0; //nominal

    //Get AK4 Jets
    //Four std::vector
    std::vector <double> AK4JetPt;
    std::vector <double> AK4JetEta;
    std::vector <double> AK4JetPhi;
    std::vector <double> AK4JetEnergy;
    
    std::vector <int>    AK4JetTBag;
    std::vector <double> AK4JetRCN;
    
    bool isAK8 = false;
    for (std::vector<edm::Ptr<pat::Jet> >::const_iterator ijet = vSelJets.begin(); ijet != vSelJets.end(); ijet++){
        
        //Four std::vector
        TLorentzVector lv = JetMETCorr.correctJet(*ijet, event, rhoJetsToken, isAK8, reCorrectJet, syst);
                
        AK4JetPt     . push_back(lv.Pt());
        AK4JetEta    . push_back(lv.Eta());
        AK4JetPhi    . push_back(lv.Phi());
        AK4JetEnergy . push_back(lv.Energy());
        
        AK4JetTBag   . push_back(btagSfUtil.isJetTagged(*ijet, lv, event, isMc));
        AK4JetRCN    . push_back(((*ijet)->chargedEmEnergy()+(*ijet)->chargedHadronEnergy()) / ((*ijet)->neutralEmEnergy()+(*ijet)->neutralHadronEnergy()));
    }
    
    //Four std::vector
    SetValue("AK4JetPt"     , AK4JetPt);
    SetValue("AK4JetEta"    , AK4JetEta);
    SetValue("AK4JetPhi"    , AK4JetPhi);
    SetValue("AK4JetEnergy" , AK4JetEnergy);
    
    SetValue("AK4JetTBag"   , AK4JetTBag);
    SetValue("AK4JetRCN"    , AK4JetRCN);

    //Get cleaned AK4 Jets
    //Four std::vector
    std::vector <double> cleanedAK4JetPt;
    std::vector <double> cleanedAK4JetPtScaleUp;
    std::vector <double> cleanedAK4JetPtScaleDown;
    std::vector <double> cleanedAK4JetPtSmearUp;
    std::vector <double> cleanedAK4JetPtSmearDown;
    std::vector <double> cleanedAK4JetEta;
    std::vector <double> cleanedAK4JetPhi;
    std::vector <double> cleanedAK4JetEnergy;
    
    std::vector <int>    cleanedAK4JetTBag;
    std::vector <double> cleanedAK4JetRCN;
    
    for (std::vector<pat::Jet>::const_iterator ijet = vSelCleanedJets.begin(); ijet != vSelCleanedJets.end(); ijet++){
      //no need to correct so just push back quantities from jet directly
      //get scaled values for MC only
      if(isMc){
                    TLorentzVector jecUP   = JetMETCorr.correctJet(*ijet, event, rhoJetsToken, isAK8, reCorrectJet, 1);
                    TLorentzVector jecDOWN = JetMETCorr.correctJet(*ijet, event, rhoJetsToken, isAK8, reCorrectJet, 2);
                    TLorentzVector jerUP   = JetMETCorr.correctJet(*ijet, event, rhoJetsToken, isAK8, reCorrectJet, 3);
                    TLorentzVector jerDOWN = JetMETCorr.correctJet(*ijet, event, rhoJetsToken, isAK8, reCorrectJet, 4);
                    cleanedAK4JetPtScaleUp.   push_back(jecUP.Pt());
                    cleanedAK4JetPtScaleDown. push_back(jecDOWN.Pt());
                    cleanedAK4JetPtSmearUp.   push_back(jerUP.Pt());
                    cleanedAK4JetPtSmearDown. push_back(jerDOWN.Pt());
      }
      cleanedAK4JetPt     . push_back((*ijet).pt());
      cleanedAK4JetEta    . push_back((*ijet).eta());
      cleanedAK4JetPhi    . push_back((*ijet).phi());
      cleanedAK4JetEnergy . push_back((*ijet).energy());
      
      TLorentzVector jetP4; jetP4.SetPtEtaPhiE((*ijet).pt(), (*ijet).eta(), (*ijet).phi(), (*ijet).energy() );
      
      cleanedAK4JetTBag   . push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc));
      cleanedAK4JetRCN    . push_back(((*ijet).chargedEmEnergy()+(*ijet).chargedHadronEnergy()) / ((*ijet).neutralEmEnergy()+(*ijet).neutralHadronEnergy()));
    }
    
    //Four std::vector
    SetValue("cleanedAK4JetPt"     , cleanedAK4JetPt);
    SetValue("cleanedAK4JetPtScaleUp"     , cleanedAK4JetPtScaleUp);
    SetValue("cleanedAK4JetPtScaleDown"     , cleanedAK4JetPtScaleDown);
    SetValue("cleanedAK4JetPtSmearUp"     , cleanedAK4JetPtSmearUp);
    SetValue("cleanedAK4JetPtSmearDown" , cleanedAK4JetPtSmearDown);
    SetValue("cleanedAK4JetEta"    , cleanedAK4JetEta);
    SetValue("cleanedAK4JetPhi"    , cleanedAK4JetPhi);
    SetValue("cleanedAK4JetEnergy" , cleanedAK4JetEnergy);
    
    SetValue("cleanedAK4JetTBag"   , cleanedAK4JetTBag);
    SetValue("cleanedAK4JetRCN"    , cleanedAK4JetRCN);

}

void DileptonCalc::AnalyzeAK8Jets(edm::Event const & event, BaseEventSelector * selector)
{


    //
    //_____ AK8 Jets ______________________________
    //


    //Get AK8 jets for W's
    edm::Handle<std::vector<pat::Jet> > AK8Jets;
    event.getByToken(AK8jetsToken, AK8Jets);
    
    //Four std::vector
    std::vector <double> AK8JetPt;
    std::vector <double> AK8JetEta;
    std::vector <double> AK8JetPhi;
    std::vector <double> AK8JetEnergy;
    
    std::vector <double> AK8JetCSV;
    
    //Identity
    std::vector <int> AK8JetIndex;
    std::vector <int> AK8JetnDaughters;
    
    //Mass
    std::vector <double> AK8JetTrimmedMass;
    std::vector <double> AK8JetPrunedMass;
    std::vector <double> AK8JetFilteredMass;
    std::vector <double> AK8JetSoftDropMass;
    
    //nsubjettiness
    std::vector<double> AK8JetTau1;
    std::vector<double> AK8JetTau2;
    std::vector<double> AK8JetTau3;


    //Daughter four std::vector and index -- THESE ARE CURRENTLY IDENTICAL TO THOSE FOR TOP DAUGHTERS BECAUSE THE JET IS ALWAYS THE SLIMMED AK8JET
    std::vector <double> AK8DaughterPt;
    std::vector <double> AK8DaughterEta;
    std::vector <double> AK8DaughterPhi;
    std::vector <double> AK8DaughterEnergy;
    
    std::vector <int> AK8DaughterMotherIndex;
    
    for (std::vector<pat::Jet>::const_iterator ijet = AK8Jets->begin(); ijet != AK8Jets->end(); ijet++){
        
        int index = (int)(ijet-AK8Jets->begin());
        
        //Four std::vector
        AK8JetPt     . push_back(ijet->pt());
        AK8JetEta    . push_back(ijet->eta());
        AK8JetPhi    . push_back(ijet->phi());
        AK8JetEnergy . push_back(ijet->energy());
        
        AK8JetCSV    . push_back(ijet->bDiscriminator( "combinedSecondaryVertexBJetTags")); // Does this need to be updated??
        
        //Identity
        AK8JetIndex      . push_back(index);
        AK8JetnDaughters . push_back((int)ijet->numberOfDaughters());
        //Mass
        AK8JetTrimmedMass . push_back(-999);
        AK8JetPrunedMass . push_back(-999);
        AK8JetFilteredMass . push_back(-999);
        AK8JetSoftDropMass . push_back(-999);
        //nsubjettiness
        AK8JetTau1.push_back(-999);
        AK8JetTau2.push_back(-999);
        AK8JetTau3.push_back(-999);
        //cout<<"set n subjettiness"<<endl;
        for (size_t ui = 0; ui < ijet->numberOfDaughters(); ui++){
            AK8DaughterPt     . push_back(ijet->daughter(ui)->pt());
            AK8DaughterEta    . push_back(ijet->daughter(ui)->eta());
            AK8DaughterPhi    . push_back(ijet->daughter(ui)->phi());
            AK8DaughterEnergy . push_back(ijet->daughter(ui)->energy());
            
            AK8DaughterMotherIndex . push_back(index);
        }
    }
    
    //Four std::vector
    SetValue("AK8JetPt"     , AK8JetPt);
    SetValue("AK8JetEta"    , AK8JetEta);
    SetValue("AK8JetPhi"    , AK8JetPhi);
    SetValue("AK8JetEnergy" , AK8JetEnergy);
    
    SetValue("AK8JetCSV"    , AK8JetCSV);
    
    //Identity
    SetValue("AK8JetIndex"      , AK8JetIndex);
    SetValue("AK8JetnDaughters" , AK8JetnDaughters);
    
    //Mass
    SetValue("AK8JetTrimmedMass"     , AK8JetTrimmedMass);
    SetValue("AK8JetPrunedMass"     , AK8JetPrunedMass);
    SetValue("AK8JetFilteredMass"     , AK8JetFilteredMass);
    SetValue("AK8JetSoftDropMass"     , AK8JetSoftDropMass);

    
    //Daughter four std::vector and index
    SetValue("AK8DaughterPt"     , AK8DaughterPt);
    SetValue("AK8DaughterEta"    , AK8DaughterEta);
    SetValue("AK8DaughterPhi"    , AK8DaughterPhi);
    SetValue("AK8DaughterEnergy" , AK8DaughterEnergy);
    
    SetValue("AK8DaughterMotherIndex" , AK8DaughterMotherIndex);


}

void DileptonCalc::AnalyzeMET(edm::Event const & event, BaseEventSelector * selector)
{

    //
    // _____ Get objects from the selector _____________________
    //
    std::vector<edm::Ptr<pat::Jet>> const & vAllJets = selector->GetAllJets();
    edm::Ptr<pat::MET>              const & pMet     = selector->GetMet();
            

    //
    //_____ MET ______________________________
    //

	//for jet correction
	bool reCorrectJet = doNewJEC;
	unsigned int syst;
	if (JECup){syst=1;}
	else if (JECdown){syst=2;}
	else if (JERup){syst=3;}
	else if (JERdown){syst=4;}
	else syst = 0; //nominal


    double _met = -9999.0;
    double _met_phi = -9999.0;
    // Corrected MET
    double _corr_met = -9999.0;
    double _corr_met_phi = -9999.0;
    
    if(pMet.isNonnull() && pMet.isAvailable()) {
        _met = pMet->p4().pt();
        _met_phi = pMet->p4().phi();
        TLorentzVector corrMET = JetMETCorr.correctMet(*pMet, event, rhoJetsToken, vAllJets, reCorrectJet, syst); // Clint Richardson used vSelCleanedJets??? June 12, 2019 
        if(corrMET.Pt()>0) {
            _corr_met = corrMET.Pt();
            _corr_met_phi = corrMET.Phi();
        }
        
    }
    SetValue("met", _met);
    SetValue("met_phi", _met_phi);
    SetValue("corr_met", _corr_met);
    SetValue("corr_met_phi", _corr_met_phi);


    //METNOHF
    bool useHF;
    useHF = false;
    double _metnohf = -9999.0;
    double _metnohf_phi = -9999.0;
    double _corr_metnohf = -9999.0;
    double _corr_metnohf_phi = -9999.0;
    edm::Handle<std::vector<pat::MET> > METnoHF;
    if(event.getByToken(METnoHFtoken, METnoHF)){
      edm::Ptr<pat::MET> metnohf = edm::Ptr<pat::MET>( METnoHF, 0);

      if(metnohf.isNonnull() && metnohf.isAvailable()) {
        _metnohf = metnohf->p4().pt();
        _metnohf_phi = metnohf->p4().phi();

        TLorentzVector corrMETNOHF = JetMETCorr.correctMet(*metnohf, event, rhoJetsToken, vAllJets, doNewJEC, syst, useHF); // Clint Richardson used vSelCleanedJets??? June 12, 2019
        if(corrMETNOHF.Pt()>0) {
	  _corr_metnohf = corrMETNOHF.Pt();
	  _corr_metnohf_phi = corrMETNOHF.Phi();
        }
      }
    }
    SetValue("metnohf", _metnohf);
    SetValue("metnohf_phi", _metnohf_phi);
    SetValue("corr_metnohf", _corr_metnohf);
    SetValue("corr_metnohf_phi", _corr_metnohf_phi);

    //METMOD
    useHF = false;
    double _metmod = -9999.0;
    double _metmod_phi = -9999.0;
    double _corr_metmod = -9999.0;
    double _corr_metmod_phi = -9999.0;
    edm::Handle<std::vector<pat::MET> > METmod;
    if(event.getByToken(METmodToken, METmod)){
      edm::Ptr<pat::MET> metmod = edm::Ptr<pat::MET>( METmod, 0);

      if(metmod.isNonnull() && metmod.isAvailable()) {
        _metmod = metmod->p4().pt();
        _metmod_phi = metmod->p4().phi();

        TLorentzVector corrMETMOD = JetMETCorr.correctMet(*metmod, event, rhoJetsToken, vAllJets, doNewJEC, syst, useHF); // Clint Richardson used vSelCleanedJets??? June 12, 2019
        if(corrMETMOD.Pt()>0) {
	  _corr_metmod = corrMETMOD.Pt();
	  _corr_metmod_phi = corrMETMOD.Phi();
        }
      }
    }

    SetValue("metmod", _metmod);
    SetValue("metmod_phi", _metmod_phi);
    SetValue("corr_metmod", _corr_metmod);
    SetValue("corr_metmod_phi", _corr_metmod_phi);

}

void DileptonCalc::AnalyzeGenInfo(edm::Event const & event, BaseEventSelector * selector)
{

    //
    //_____ Gen Info ______________________________
    //
    
    //Four std::vector
    std::vector <double> genPt;
    std::vector <double> genEta;
    std::vector <double> genPhi;
    std::vector <double> genEnergy;
    
    //Identity
    std::vector <int> genID;
    std::vector <int> genIndex;
    std::vector <int> genStatus;
    std::vector <int> genMotherID;
    std::vector <int> genMotherIndex;
    std::vector<bool> genIsFromTau;
    std::vector<bool> genIsPrompt;
    std::vector<bool> genPMotherHasC;
    std::vector<bool> genPMotherHasB;
    std::vector<int> genPMother;

    double LHEweightorig = 0;

    //event weights
    std::vector<double> evtWeightsMC;
    float MCWeight=1;
    std::vector<double> LHEweights;
    std::vector<int> LHEweightids;

    std::vector <int> NewPDFids;
    std::vector <double> NewPDFweights;
    std::vector <double> NewPDFweightsBase;

    if (isMc){
      //load info for event weight
      edm::Handle<GenEventInfoProduct> genEvtInfo;
      event.getByToken(genToken, genEvtInfo );

      std::vector<double> evtWeights = genEvtInfo->weights();
      double theWeight = genEvtInfo->weight();
      
      evtWeightsMC=evtWeights;
      MCWeight = theWeight;
      
      if (orlhew) {

          float x1 = genEvtInfo->pdf()->x.first;
          float x2 = genEvtInfo->pdf()->x.second;
          double Q = genEvtInfo->pdf()->scalePDF;
          int id1 = genEvtInfo->pdf()->id.first;
          int id2 = genEvtInfo->pdf()->id.second;
          //std::cout<<"x1 x2 Q id1 id2"<<std::endl;
          //std::cout<<x1<<" "<<x2<<" "<<Q<<" "<<id1<<" "<<id2<<std::endl;

          //Initialize PDF sets
          LHAPDF::PDF* basepdf1 = LHAPDF::mkPDF(basePDFname,0);
          const LHAPDF::GridPDF& pdf1 = * dynamic_cast<const LHAPDF::GridPDF*>(basepdf1);
  
          // calculate central PDFs for generator set,
          double pdf1_gen = pdf1.xfxQ(id1, x1, Q);
          double pdf2_gen = pdf1.xfxQ(id2, x2, Q);
          //std::cout<<"pdf1_gen = "<<pdf1_gen<<" pdf2_gen = "<<pdf2_gen<<std::endl;
          delete basepdf1;
  
          const LHAPDF::PDFSet newset(newPDFname);
          const size_t nmem = newset.size();
          const std::vector<LHAPDF::PDF*> newpdfs = newset.mkPDFs();
          for (size_t i = 0; i<nmem; i++) {
            const LHAPDF::GridPDF& pdf2 = * dynamic_cast<const LHAPDF::GridPDF*>(newpdfs[i]);

            double pdf1_new = pdf2.xfxQ(id1, x1, Q);
            double pdf2_new = pdf2.xfxQ(id2, x2, Q);
            NewPDFweights.push_back((pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen));
            NewPDFweightsBase.push_back(pdf1_gen*pdf2_gen);
            NewPDFids.push_back(315000+i);

            delete (newpdfs[i]);
          }
        }

      //weights for mc uncertainties
      edm::Handle<LHEEventProduct> EvtHandle;
      if(event.getByToken(LHEToken,EvtHandle)){
            
                    // Storing LHE weights https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW
                    // for MC@NLO renormalization and factorization scale. 
                    // ID numbers 1001 - 1009. (muR,muF) = 
                    // 0 = 1001: (1,1)    3 = 1004: (2,1)    6 = 1007: (0.5,1)  
                    // 1 = 1002: (1,2)    4 = 1005: (2,2)   7 = 1008: (0.5,2)  
                    // 2 = 1003: (1,0.5)  5 = 1006: (2,0.5) 8 = 1009: (0.5,0.5)
                    // for PDF variations: ID numbers > 2000
                    
                    LHEweightorig = EvtHandle->originalXWGTUP();

                    std::string weightidstr;
                    int weightid;
                    if(EvtHandle->weights().size() > 0){  
                      for(unsigned int i = 0; i < EvtHandle->weights().size(); i++){
                              weightidstr = EvtHandle->weights()[i].id;
                              weightid = std::stoi(weightidstr);
                              LHEweights.push_back(EvtHandle->weights()[i].wgt/EvtHandle->originalXWGTUP());
                              LHEweightids.push_back(weightid);
                      }
                    }
      }

      //load genparticles collection
      edm::Handle<reco::GenParticleCollection> genParticles;
      event.getByToken(genParticlesToken, genParticles);
      
      
      for(size_t i = 0; i < genParticles->size(); i++){
      
          const reco::GenParticle & p = (*genParticles).at(i);
          
          bool keep=false;

          //attempt using mc gen particle flags
          if(p.status()==1) keep = true; //all stable out going particles
          else if( p.isPromptDecayed() ) keep = true; //keep prompt particles which have undergone decay (e.g. B-mesons)
          else if( p.statusFlags().isPrompt()) keep = true; // keep all prompt particles in case I want to do quark matching
          else if( p.isDirectPromptTauDecayProductFinalState()) keep = true; //log save leptons from tau decays since they are 'prompt' for us
          

          if(!keep) continue;    

          bool promptMotherHasB = false;
          bool promptMotherHasC = false;

          reco::GenParticle* mother = 0;
          if(p.status()==1){
            mother = (reco::GenParticle*) p.mother();
            while(mother){
                if(mother->isPromptDecayed()) break;
                else{ mother = (reco::GenParticle*) mother->mother();}
            }
          }

          if(mother){
            HepPDT::ParticleID heppdtid(mother->pdgId());
            promptMotherHasB = heppdtid.hasBottom();
            promptMotherHasC = heppdtid.hasCharm();
          }

          genPMotherHasB.push_back(promptMotherHasB);
          genPMotherHasC.push_back(promptMotherHasC);
          if(mother) genPMother.push_back(mother->pdgId());
          else       genPMother.push_back(-999);
          //Four std::vector
          genPt     . push_back(p.pt());
          genEta    . push_back(p.eta());
          genPhi    . push_back(p.phi());
          genEnergy . push_back(p.energy());
          
          //Identity
          genID          . push_back(p.pdgId());
          genIndex         . push_back((int) i);
          genStatus        . push_back(p.status());
          genIsPrompt      . push_back(p.statusFlags().isPrompt());
          genIsFromTau     . push_back(p.isDirectPromptTauDecayProductFinalState()); //save whether or not a particle came from a prompt tau
          //mother info - stores mother pdgID if mother exists or -999 if mother doesn't exist. This way the entries will always be aligned with main gen particle entries
          if(p.mother()) genMotherID.push_back((p.mother())->pdgId());
          else genMotherID.push_back(-999);
          //genMotherIndex   . push_back(mInd);
          
        }//End loop over gen particles
    }  //End MC-only if
    
    // Four std::vector
    SetValue("genPt"    , genPt);
    SetValue("genEta"   , genEta);
    SetValue("genPhi"   , genPhi);
    SetValue("genEnergy", genEnergy);
    
    // Identity
    SetValue("genID"         , genID);
    SetValue("genIndex"      , genIndex);
    SetValue("genStatus"     , genStatus);
    SetValue("genMotherID"   , genMotherID);
    SetValue("genMotherIndex", genMotherIndex);
    SetValue("genIsPrompt"   , genIsPrompt);
    SetValue("genIsFromPromptTau", genIsFromTau);
    SetValue("evtWeightsMC", evtWeightsMC);
    SetValue("MCWeight",MCWeight);
    SetValue("LHEweightorig",LHEweightorig);
    SetValue("LHEWeights",LHEweights);
    SetValue("LHEWeightIDs",LHEweightids);
    SetValue("NewPDFids", NewPDFids);
    SetValue("NewPDFweights", NewPDFweights);
    SetValue("NewPDFweightsBase", NewPDFweightsBase);
    SetValue("genPMotherHasC",genPMotherHasC);
    SetValue("genPMotherHasB",genPMotherHasB);
    SetValue("genPMother",genPMother);

}

// ----------------
// Helper functions
// ----------------

int DileptonCalc::findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi)
{
    float dRtmp = 1000;
    float closestDR = 10000.;
    int closestGenPart = -1;
    
    for(size_t j = 0; j < genParticles.size(); ++ j) {
        const reco::GenParticle & p = (genParticles).at(j);
        dRtmp = mdeltaR( eta, phi, p.eta(), p.phi());
        if ( dRtmp < closestDR && abs(p.pdgId()) == idToMatch){// && dRtmp < 0.3) {
            closestDR = dRtmp;
            closestGenPart = j;
        }//end of requirement for matching
    }//end of gen particle loop 
    return closestGenPart;
}

double DileptonCalc::mdeltaR(double eta1, double phi1, double eta2, double phi2)
{
    return std::sqrt(deltaR2 (eta1, phi1, eta2, phi2));
}

void DileptonCalc::fillMotherInfo(const reco::Candidate *mother, 
                                  int i, 
                                  std::vector <int> & momid, 
                                  std::vector <int> & momstatus, 
                                  std::vector<double> & mompt, 
                                  std::vector<double> & mometa, 
                                  std::vector<double> & momphi, 
                                  std::vector<double> & momenergy)
{
    if(mother) {
        momid.push_back(mother->pdgId());
        momstatus.push_back(mother->status());
        mompt.push_back(mother->pt());
        mometa.push_back(mother->eta());
        momphi.push_back(mother->phi());
        momenergy.push_back(mother->energy());
        if(i<10)fillMotherInfo(mother->mother(), i+1, momid, momstatus, mompt, mometa, momphi, momenergy);
    }
    
    
} 
