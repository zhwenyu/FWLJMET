#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "FWLJMET/LJMet/interface/MiniIsolation.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include <TRandom3.h>
#include <boost/algorithm/string.hpp>

#include "FWLJMET/LJMet/interface/BTagSFUtil.h"
#include "FWLJMET/LJMet/interface/BtagHardcodedConditions.h"
#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"


using namespace std;

class MultiLepEventSelector : public BaseEventSelector { //adapted from singleLepEventSelector.cc

public:


    MultiLepEventSelector();
    ~MultiLepEventSelector();


    // executes before loop over events
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );

    // executes after loop over events
    virtual void EndJob();


protected:

    //Misc
    bool bFirstEntry;
    bool debug;
    bool isMc;

    //Trigger
    bool trigger_cut;
    bool dump_trigger;
    std::vector<std::string> mctrigger_path_el;
    std::vector<std::string> mctrigger_path_mu;
    std::vector<std::string> trigger_path_el;
    std::vector<std::string> trigger_path_mu;

    //PV
    bool pv_cut;
    boost::shared_ptr<PVSelector> pvSel_;

    //MET Filter
    bool   metfilters;

    //MET
    bool   met_cuts;
    double min_met;
    double max_met;

    //Muon
    bool   muon_cuts;
    int    min_muon;
    double muon_minpt;
    double muon_maxeta;
    bool   muon_useMiniIso;
    double muon_dxy;
    double muon_dz;
    double muon_relIso;
    double loose_muon_minpt;
    double loose_muon_maxeta;
    double loose_muon_dxy;
    double loose_muon_dz;
    double loose_muon_relIso;

    //Electron
    bool   electron_cuts;
    int    min_electron;
    double electron_minpt;
    double electron_maxeta;
    bool   electron_useMiniIso;
    double electron_miniIso;
    double loose_electron_miniIso;
    double loose_electron_minpt;
    double loose_electron_maxeta;
    bool   UseElMVA;
    bool   UseElIDV1;

    //nLepton
    bool minLeptons_cut;
    int  minLeptons;
    bool maxLeptons_cut;
    int  maxLeptons;
    bool minLooseLeptons_cut;
    int  minLooseLeptons;
    bool maxLooseLeptons_cut;
    int  maxLooseLeptons;

    //Jets
    bool   JECup;
    bool   JECdown;
    bool   JERup;
    bool   JERdown;
    bool   doNewJEC;
    bool   doLepJetCleaning;
    bool   CleanLooseLeptons;
    double LepJetDR;
    double LepJetDRAK8;
    bool   jet_cuts;
    double jet_minpt;
    double jet_maxeta;
    double jet_minpt_AK8;
    double jet_maxeta_AK8;
    int    min_jet;
    int    max_jet;
    double leading_jet_pt;
    std::string JEC_txtfile;
    std::string JERSF_txtfile;
    std::string JER_txtfile;
    std::string JERAK8_txtfile;

    //Tokens
    edm::EDGetTokenT<edm::TriggerResults>            triggersToken;
    edm::EDGetTokenT<reco::VertexCollection>         PVToken;
    edm::EDGetTokenT<edm::TriggerResults>            METfilterToken;
    edm::EDGetTokenT<bool>                           METfilterToken_extra;
    edm::EDGetTokenT<std::vector<pat::MET> >         METtoken;
    edm::EDGetTokenT<pat::MuonCollection>            muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>        electronsToken;
    edm::EDGetTokenT<pat::PackedCandidateCollection> PFCandToken;
    edm::EDGetTokenT<double>                         rhoJetsNC_Token;
	edm::EDGetTokenT<double>                         rhoJetsToken;
    edm::EDGetTokenT<pat::JetCollection>             jetsToken;
    edm::EDGetTokenT<pat::JetCollection>             AK8jetsToken;


    //Separate methods for each selction for organization
    bool TriggerSelection  (edm::Event const & event);
    bool PVSelection       (edm::Event const & event);
    bool METfilter         (edm::Event const & event);
    void MuonSelection     (edm::Event const & event);
    void ElectronSelection (edm::Event const & event);
    bool LeptonsSelection  (edm::Event const & event, pat::strbitset & ret);
    bool JetSelection      (edm::Event const & event, pat::strbitset & ret);
    void AK8JetSelection   (edm::Event const & event);
    bool METSelection      (edm::Event const & event);


private:


    //NOTE: theres no really good reason why there are in private or protected. Not planning on having child classes of this class.

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // NOTE: EVERYTHING BELOW NEEDS TO BE EITHER REORGANIZED/REWRITTEN - start
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------



    //JETMET correction helper methods - Ideally THESE NEEDS TO BE ON A SEPARATE DEDICATED JET CORRENTION CLASS, or something like that.
    //ISSUE: These methods currently needs to be accessed by calculators. So here is not the place to put it. But right now as a first pass they are put here.
    void JECbyIOV(edm::EventBase const & event); // -->> THIS IS A HACK. NEEDS TO BE REWRITTEN AND NOT HAVE THIS HERE.
    TLorentzVector correctJet(const pat::Jet & jet,
                              edm::Event const & event,
                              bool isMc,
                              edm::EDGetTokenT<double> rhoJetsToken,
                              bool doAK8Corr=false,
                              bool reCorrectJet=false,
                              unsigned int syst=0);
    pat::Jet correctJetReturnPatJet(const pat::Jet & jet,
                                    edm::Event const & event,
                                    bool isMc,
                                    edm::EDGetTokenT<double> rhoJetsToken,
                                    bool doAK8Corr=false,
                                    bool reCorrectJet=false,
                                    unsigned int syst=0);
    TLorentzVector correctMet(const pat::MET & met, edm::Event const & event, unsigned int syst = 0, bool useHF = true);
    TLorentzVector correctJetForMet(const pat::Jet & jet, edm::Event const & event, unsigned int syst = 0);

    //JET correction methods variables/parameters/object type definitions - Ideally THESE NEEDS TO BE ON A SEPARATE DEDICATED JET CORRENTION CLASS, or something like that. - but on top of that this needs to be optimized and rewritten.

    int mNCorrJets; // used in correctJet method

    TRandom3 JERrand;

    std::string MCL1JetPar;
    std::string MCL2JetPar;
    std::string MCL3JetPar;
    std::string MCL1JetParAK8;
    std::string MCL2JetParAK8;
    std::string MCL3JetParAK8;
    std::string DataL1JetPar;
    std::string DataL2JetPar;
    std::string DataL3JetPar;
    std::string DataResJetPar;
    std::string DataL1JetParAK8;
    std::string DataL2JetParAK8;
    std::string DataL3JetParAK8;
    std::string DataResJetParAK8;
    std::string DataL1JetParByIOV;
    std::string DataL2JetParByIOV;
    std::string DataL3JetParByIOV;
    std::string DataResJetParByIOV;
    std::string DataL1JetParByIOVAK8;
    std::string DataL2JetParByIOVAK8;
    std::string DataL3JetParByIOVAK8;
    std::string DataResJetParByIOVAK8;

    JME::JetResolution resolution;
    JME::JetResolution resolutionAK8;
    JME::JetResolutionScaleFactor resolution_SF;

    JetCorrectionUncertainty *jecUnc;

    JetCorrectorParameters *L3JetPar;
    JetCorrectorParameters *L2JetPar;
    JetCorrectorParameters *L1JetPar;
    JetCorrectorParameters *L3JetParAK8;
    JetCorrectorParameters *L2JetParAK8;
    JetCorrectorParameters *L1JetParAK8;
    JetCorrectorParameters *L3JetPar_B;
    JetCorrectorParameters *L2JetPar_B;
    JetCorrectorParameters *L1JetPar_B;
    JetCorrectorParameters *L3JetParAK8_B;
    JetCorrectorParameters *L2JetParAK8_B;
    JetCorrectorParameters *L1JetParAK8_B;
    JetCorrectorParameters *L3JetPar_C;
    JetCorrectorParameters *L2JetPar_C;
    JetCorrectorParameters *L1JetPar_C;
    JetCorrectorParameters *L3JetParAK8_C;
    JetCorrectorParameters *L2JetParAK8_C;
    JetCorrectorParameters *L1JetParAK8_C;
    JetCorrectorParameters *L3JetPar_DE;
    JetCorrectorParameters *L2JetPar_DE;
    JetCorrectorParameters *L1JetPar_DE;
    JetCorrectorParameters *L3JetParAK8_DE;
    JetCorrectorParameters *L2JetParAK8_DE;
    JetCorrectorParameters *L1JetParAK8_DE;
    JetCorrectorParameters *L3JetPar_F;
    JetCorrectorParameters *L2JetPar_F;
    JetCorrectorParameters *L1JetPar_F;
    JetCorrectorParameters *L3JetParAK8_F;
    JetCorrectorParameters *L2JetParAK8_F;
    JetCorrectorParameters *L1JetParAK8_F;
    JetCorrectorParameters *ResJetPar;
    JetCorrectorParameters *ResJetParAK8;

    FactorizedJetCorrector *JetCorrector;
    FactorizedJetCorrector *JetCorrectorAK8;

    JetCorrectorParameters *ResJetPar_B;
    JetCorrectorParameters *ResJetParAK8_B;
    FactorizedJetCorrector *JetCorrector_B;
    FactorizedJetCorrector *JetCorrectorAK8_B;

    JetCorrectorParameters *ResJetPar_C;
    JetCorrectorParameters *ResJetParAK8_C;
    FactorizedJetCorrector *JetCorrector_C;
    FactorizedJetCorrector *JetCorrectorAK8_C;

    JetCorrectorParameters *ResJetPar_DE;
    JetCorrectorParameters *ResJetParAK8_DE;
    FactorizedJetCorrector *JetCorrector_DE;
    FactorizedJetCorrector *JetCorrectorAK8_DE;

    JetCorrectorParameters *ResJetPar_F;
    JetCorrectorParameters *ResJetParAK8_F;
    FactorizedJetCorrector *JetCorrector_F;
    FactorizedJetCorrector *JetCorrectorAK8_F;

    //BTAG helper method
    bool isJetTagged(const pat::Jet &jet,
                     TLorentzVector correctedJet_lv,
                     edm::Event const & event,
                     bool isMc,
                     bool applySF = true,
                     int shiftflag = 0,
                     bool subjetflag = false);

    //BTAG variables/parameters/object type definitions
    bool        btag_cuts;
    double      bdisc_min;
    std::string DeepCSVfile;
    std::string DeepCSVSubjetfile;
    std::string btagOP;
    double bTagCut;
    BTagSFUtil mBtagSfUtil;
    BtagHardcodedConditions mBtagCond;
    bool        BTagUncertUp;
    bool        BTagUncertDown;
    bool        MistagUncertUp;
    bool        MistagUncertDown;
    BTagCalibration       calib;
    BTagCalibration       calibsj;
    BTagCalibrationReader reader;
    BTagCalibrationReader readerSJ;

    int mNBtagSfCorrJets; // used in isJetTagged method

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // NOTE: EVERYTHING ABOVE NEEDS TO BE EITHER REORGANIZED/REWRITTEN - end
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------


};


static int reg = LjmetFactory::GetInstance()->Register(new MultiLepEventSelector(), "MultiLepSelector");


MultiLepEventSelector::MultiLepEventSelector()
{
}

MultiLepEventSelector::~MultiLepEventSelector()
{
}

void MultiLepEventSelector::BeginJob( const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC)
{

    std::cout << mLegend << "initializing MultiLepEventSelector selection" << std::endl;

    BaseEventSelector::BeginJob(iConfig,(edm::ConsumesCollector &&)iC);

    std::string thisSelectorName = GetName();
    const edm::ParameterSet& selectorConfig = iConfig.getParameterSet( thisSelectorName ) ;

    debug               = selectorConfig.getParameter<bool>("debug");
    isMc                = selectorConfig.getParameter<bool>("isMc");
    bFirstEntry         = true; //in case anything needs a first entry bool.

    //Trigger
    triggersToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    trigger_cut         = selectorConfig.getParameter<bool>("trigger_cut");
    dump_trigger        = selectorConfig.getParameter<bool>("dump_trigger");
    mctrigger_path_el   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_el");
    mctrigger_path_mu   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_mu");
    trigger_path_el     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_el");
    trigger_path_mu     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_mu");

    //PV
    const edm::ParameterSet& PVconfig = selectorConfig.getParameterSet("pvSelector") ;
    PVToken                           = iC.consumes<reco::VertexCollection>(PVconfig.getParameter<edm::InputTag>("pvSrc"));
    pv_cut                            = selectorConfig.getParameter<bool>("pv_cut");
    pvSel_                            = boost::shared_ptr<PVSelector>( new PVSelector(PVconfig) );

    //MET filter
    METfilterToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("flag_tag"));
    METfilterToken_extra = iC.consumes<bool>(selectorConfig.getParameter<edm::InputTag>("METfilter_extra"));
    metfilters           = selectorConfig.getParameter<bool>("metfilters");

    //Muon
    muonsToken               = iC.consumes<pat::MuonCollection>(selectorConfig.getParameter<edm::InputTag>("muonsCollection"));;
    muon_cuts                = selectorConfig.getParameter<bool>("muon_cuts");
    min_muon                 = selectorConfig.getParameter<int>("min_muon");
    muon_minpt               = selectorConfig.getParameter<double>("muon_minpt");
    muon_maxeta              = selectorConfig.getParameter<double>("muon_maxeta");
    muon_useMiniIso          = selectorConfig.getParameter<bool>("muon_useMiniIso");
    loose_muon_minpt         = selectorConfig.getParameter<double>("loose_muon_minpt");
    loose_muon_maxeta        = selectorConfig.getParameter<double>("loose_muon_maxeta");
    muon_dxy                 = selectorConfig.getParameter<double>("muon_dxy");
    muon_dz                  = selectorConfig.getParameter<double>("muon_dz");
    loose_muon_dxy           = selectorConfig.getParameter<double>("loose_muon_dxy");
    loose_muon_dz            = selectorConfig.getParameter<double>("loose_muon_dz");

    muon_relIso              = selectorConfig.getParameter<double>("muon_relIso");
    loose_muon_relIso        = selectorConfig.getParameter<double>("loose_muon_relIso");

    //Electron
    electronsToken           = iC.consumes<pat::ElectronCollection>(selectorConfig.getParameter<edm::InputTag>("electronsCollection"));
    electron_cuts            = selectorConfig.getParameter<bool>("electron_cuts");
    min_electron             = selectorConfig.getParameter<int>("min_electron");
    electron_minpt           = selectorConfig.getParameter<double>("electron_minpt");
    electron_maxeta          = selectorConfig.getParameter<double>("electron_maxeta");
    electron_useMiniIso      = selectorConfig.getParameter<bool>("electron_useMiniIso");
    electron_miniIso         = selectorConfig.getParameter<double>("electron_miniIso");
    loose_electron_miniIso   = selectorConfig.getParameter<double>("loose_electron_miniIso");
    loose_electron_minpt     = selectorConfig.getParameter<double>("loose_electron_minpt");
    loose_electron_maxeta    = selectorConfig.getParameter<double>("loose_electron_maxeta");
    UseElMVA                 = selectorConfig.getParameter<bool>("UseElMVA");
    UseElIDV1                = selectorConfig.getParameter<bool>("UseElIDV1");

    //nLepton
    minLooseLeptons_cut      = selectorConfig.getParameter<bool>("minLooseLeptons_cut");
    minLooseLeptons          = selectorConfig.getParameter<int>("minLooseLeptons");
    maxLooseLeptons_cut      = selectorConfig.getParameter<bool>("maxLooseLeptons_cut");
    maxLooseLeptons          = selectorConfig.getParameter<int>("maxLooseLeptons");
    minLeptons_cut           = selectorConfig.getParameter<bool>("minLeptons_cut");
    minLeptons               = selectorConfig.getParameter<int>("minLeptons");
    maxLeptons_cut           = selectorConfig.getParameter<bool>("maxLeptons_cut");
    maxLeptons               = selectorConfig.getParameter<int>("maxLeptons");

    //Jets
    jetsToken                = iC.consumes<pat::JetCollection>(selectorConfig.getParameter<edm::InputTag>("jet_collection")),
    AK8jetsToken             = iC.consumes<pat::JetCollection>(selectorConfig.getParameter<edm::InputTag>("AK8jet_collection")),
    JECup                    = selectorConfig.getParameter<bool>("JECup"),
    JECdown                  = selectorConfig.getParameter<bool>("JECdown"),
    JERup                    = selectorConfig.getParameter<bool>("JERup"),
    JERdown                  = selectorConfig.getParameter<bool>("JERdown"),
    doNewJEC                 = selectorConfig.getParameter<bool>("doNewJEC"),
    doLepJetCleaning         = selectorConfig.getParameter<bool>("doLepJetCleaning"),
    CleanLooseLeptons        = selectorConfig.getParameter<bool>("CleanLooseLeptons"),
    LepJetDR                 = selectorConfig.getParameter<double>("LepJetDR"),
    LepJetDRAK8              = selectorConfig.getParameter<double>("LepJetDRAK8"),
    jet_cuts                 = selectorConfig.getParameter<bool>("jet_cuts"),
    jet_minpt                = selectorConfig.getParameter<double>("jet_minpt"),
    jet_maxeta               = selectorConfig.getParameter<double>("jet_maxeta"),
    jet_minpt_AK8            = selectorConfig.getParameter<double>("jet_minpt_AK8"),
    jet_maxeta_AK8           = selectorConfig.getParameter<double>("jet_maxeta_AK8"),
    min_jet                  = selectorConfig.getParameter<int>("min_jet"),
    max_jet                  = selectorConfig.getParameter<int>("max_jet"),
    leading_jet_pt           = selectorConfig.getParameter<double>("leading_jet_pt"),
    JEC_txtfile              = selectorConfig.getParameter<std::string>("JEC_txtfile"),
    JERSF_txtfile            = selectorConfig.getParameter<std::string>("JERSF_txtfile"),
    JER_txtfile              = selectorConfig.getParameter<std::string>("JER_txtfile"),
    JERAK8_txtfile           = selectorConfig.getParameter<std::string>("JERAK8_txtfile"),


    //MET
    METtoken           = iC.consumes<std::vector<pat::MET> >(selectorConfig.getParameter<edm::InputTag>("met_collection"));
    met_cuts           = selectorConfig.getParameter<bool>("met_cuts");
    min_met            = selectorConfig.getParameter<double>("min_met");
    max_met            = selectorConfig.getParameter<double>("max_met");

    //Misc
    PFCandToken          = iC.consumes<pat::PackedCandidateCollection>(selectorConfig.getParameter<edm::InputTag>("PFparticlesCollection"));
    rhoJetsNC_Token      = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsNCInputTag"));
    rhoJetsToken      = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsInputTag"));


    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // NOTE: EVERYTHING BELOW NEEDS TO BE EITHER REORGANIZED/REWRITTEN - start
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------

    //JET CORRECTION  initialization

    MCL1JetPar               = selectorConfig.getParameter<std::string>("MCL1JetPar"),
    MCL2JetPar               = selectorConfig.getParameter<std::string>("MCL2JetPar"),
    MCL3JetPar               = selectorConfig.getParameter<std::string>("MCL3JetPar"),
    MCL1JetParAK8            = selectorConfig.getParameter<std::string>("MCL1JetParAK8"),
    MCL2JetParAK8            = selectorConfig.getParameter<std::string>("MCL2JetParAK8"),
    MCL3JetParAK8            = selectorConfig.getParameter<std::string>("MCL3JetParAK8"),
    DataL1JetPar             = selectorConfig.getParameter<std::string>("DataL1JetPar"),
    DataL2JetPar             = selectorConfig.getParameter<std::string>("DataL2JetPar"),
    DataL3JetPar             = selectorConfig.getParameter<std::string>("DataL3JetPar"),
    DataResJetPar            = selectorConfig.getParameter<std::string>("DataResJetPar"),
    DataL1JetParAK8          = selectorConfig.getParameter<std::string>("DataL1JetParAK8"),
    DataL2JetParAK8          = selectorConfig.getParameter<std::string>("DataL2JetParAK8"),
    DataL3JetParAK8          = selectorConfig.getParameter<std::string>("DataL3JetParAK8"),
    DataResJetParAK8         = selectorConfig.getParameter<std::string>("DataResJetParAK8"),
    DataL1JetParByIOV        = DataL1JetPar;
    DataL2JetParByIOV        = DataL2JetPar;
    DataL3JetParByIOV        = DataL3JetPar;
    DataResJetParByIOV       = DataResJetPar;
    DataL1JetParByIOVAK8     = DataL1JetParAK8;
    DataL2JetParByIOVAK8     = DataL2JetParAK8;
    DataL3JetParByIOVAK8     = DataL3JetParAK8;
    DataResJetParByIOVAK8    = DataResJetParAK8;

    if ( isMc ) jecUnc = new JetCorrectionUncertainty(JEC_txtfile);

    resolution = JME::JetResolution(JER_txtfile);
    resolutionAK8 = JME::JetResolution(JERAK8_txtfile);
    resolution_SF = JME::JetResolutionScaleFactor(JERSF_txtfile);

    std::vector<JetCorrectorParameters> vPar;
    std::vector<JetCorrectorParameters> vParAK8;
    std::vector<JetCorrectorParameters> vPar_B;
    std::vector<JetCorrectorParameters> vParAK8_B;
    std::vector<JetCorrectorParameters> vPar_C;
    std::vector<JetCorrectorParameters> vParAK8_C;
    std::vector<JetCorrectorParameters> vPar_DE;
    std::vector<JetCorrectorParameters> vParAK8_DE;
    std::vector<JetCorrectorParameters> vPar_F;
    std::vector<JetCorrectorParameters> vParAK8_F;

    if ( isMc ) {
      // Create the JetCorrectorParameter objects, the order does not matter.

      L3JetPar  = new JetCorrectorParameters(MCL3JetPar);
      L2JetPar  = new JetCorrectorParameters(MCL2JetPar);
      L1JetPar  = new JetCorrectorParameters(MCL1JetPar);

      L3JetParAK8  = new JetCorrectorParameters(MCL3JetParAK8);
      L2JetParAK8  = new JetCorrectorParameters(MCL2JetParAK8);
      L1JetParAK8  = new JetCorrectorParameters(MCL1JetParAK8);
      // Load the JetCorrectorParameter objects into a std::vector,
      // IMPORTANT: THE ORDER MATTERS HERE !!!!
      vPar.push_back(*L1JetPar);
      vPar.push_back(*L2JetPar);
      vPar.push_back(*L3JetPar);

      vParAK8.push_back(*L1JetParAK8);
      vParAK8.push_back(*L2JetParAK8);
      vParAK8.push_back(*L3JetParAK8);

      if (doNewJEC) std::cout << mLegend << "Applying new jet energy corrections" << std::endl;
      else std::cout << mLegend << "NOT applying new jet energy corrections - ARE YOU SURE?" << std::endl;

      //NOTE: these statement are there originaly from oldLJMet, but its causing segfault !!!
//       delete JetCorrector;
//       delete JetCorrectorAK8;

      JetCorrector = new FactorizedJetCorrector(vPar);
      JetCorrectorAK8 = new FactorizedJetCorrector(vParAK8);

    }
    else if ( !isMc ) {
      // Create the JetCorrectorParameter objects, the order does not matter.

      std::string strB;
      std::string strC;
      std::string strDE;
      std::string strF;

      strB  = DataL1JetPar;
      strC  = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF  = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL1JetParByIOV_B = strB;
      std::string DataL1JetParByIOV_C = strC;
      std::string DataL1JetParByIOV_DE = strDE;
      std::string DataL1JetParByIOV_F = strF;

      strB = DataL2JetPar;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL2JetParByIOV_B = strB;
      std::string DataL2JetParByIOV_C = strC;
      std::string DataL2JetParByIOV_DE = strDE;
      std::string DataL2JetParByIOV_F = strF;

      strB = DataL3JetPar;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL3JetParByIOV_B = strB;
      std::string DataL3JetParByIOV_C = strC;
      std::string DataL3JetParByIOV_DE = strDE;
      std::string DataL3JetParByIOV_F = strF;

      strB = DataResJetPar;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataResJetParByIOV_B = strB;
      std::string DataResJetParByIOV_C = strC;
      std::string DataResJetParByIOV_DE = strDE;
      std::string DataResJetParByIOV_F = strF;

      strB = DataL1JetParAK8;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL1JetParAK8ByIOV_B = strB;
      std::string DataL1JetParAK8ByIOV_C = strC;
      std::string DataL1JetParAK8ByIOV_DE = strDE;
      std::string DataL1JetParAK8ByIOV_F = strF;

      strB = DataL2JetParAK8;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL2JetParAK8ByIOV_B = strB;
      std::string DataL2JetParAK8ByIOV_C = strC;
      std::string DataL2JetParAK8ByIOV_DE = strDE;
      std::string DataL2JetParAK8ByIOV_F = strF;

      strB = DataL3JetParAK8;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataL3JetParAK8ByIOV_B = strB;
      std::string DataL3JetParAK8ByIOV_C = strC;
      std::string DataL3JetParAK8ByIOV_DE = strDE;
      std::string DataL3JetParAK8ByIOV_F = strF;

      strB = DataResJetParAK8;
      strC = strB; boost::replace_first(strC,"B_V","C_V");
      strDE = strB; boost::replace_first(strDE,"B_V","DE_V");
      strF = strB; boost::replace_first(strF,"B_V","F_V");
      std::string DataResJetParAK8ByIOV_B = strB;
      std::string DataResJetParAK8ByIOV_C = strC;
      std::string DataResJetParAK8ByIOV_DE = strDE;
      std::string DataResJetParAK8ByIOV_F = strF;

      ResJetPar_B = new JetCorrectorParameters(DataResJetParByIOV_B);
      L3JetPar_B  = new JetCorrectorParameters(DataL3JetParByIOV_B);
      L2JetPar_B  = new JetCorrectorParameters(DataL2JetParByIOV_B);
      L1JetPar_B  = new JetCorrectorParameters(DataL1JetParByIOV_B);
      ResJetParAK8_B = new JetCorrectorParameters(DataResJetParAK8ByIOV_B);
      L3JetParAK8_B  = new JetCorrectorParameters(DataL3JetParAK8ByIOV_B);
      L2JetParAK8_B  = new JetCorrectorParameters(DataL2JetParAK8ByIOV_B);
      L1JetParAK8_B  = new JetCorrectorParameters(DataL1JetParAK8ByIOV_B);

      ResJetPar_C = new JetCorrectorParameters(DataResJetParByIOV_C);
      L3JetPar_C  = new JetCorrectorParameters(DataL3JetParByIOV_C);
      L2JetPar_C  = new JetCorrectorParameters(DataL2JetParByIOV_C);
      L1JetPar_C  = new JetCorrectorParameters(DataL1JetParByIOV_C);
      ResJetParAK8_C = new JetCorrectorParameters(DataResJetParAK8ByIOV_C);
      L3JetParAK8_C  = new JetCorrectorParameters(DataL3JetParAK8ByIOV_C);
      L2JetParAK8_C  = new JetCorrectorParameters(DataL2JetParAK8ByIOV_C);
      L1JetParAK8_C  = new JetCorrectorParameters(DataL1JetParAK8ByIOV_C);

      ResJetPar_DE = new JetCorrectorParameters(DataResJetParByIOV_DE);
      L3JetPar_DE  = new JetCorrectorParameters(DataL3JetParByIOV_DE);
      L2JetPar_DE  = new JetCorrectorParameters(DataL2JetParByIOV_DE);
      L1JetPar_DE  = new JetCorrectorParameters(DataL1JetParByIOV_DE);
      ResJetParAK8_DE = new JetCorrectorParameters(DataResJetParAK8ByIOV_DE);
      L3JetParAK8_DE  = new JetCorrectorParameters(DataL3JetParAK8ByIOV_DE);
      L2JetParAK8_DE  = new JetCorrectorParameters(DataL2JetParAK8ByIOV_DE);
      L1JetParAK8_DE  = new JetCorrectorParameters(DataL1JetParAK8ByIOV_DE);

      ResJetPar_F = new JetCorrectorParameters(DataResJetParByIOV_F);
      L3JetPar_F  = new JetCorrectorParameters(DataL3JetParByIOV_F);
      L2JetPar_F  = new JetCorrectorParameters(DataL2JetParByIOV_F);
      L1JetPar_F  = new JetCorrectorParameters(DataL1JetParByIOV_F);
      ResJetParAK8_F = new JetCorrectorParameters(DataResJetParAK8ByIOV_F);
      L3JetParAK8_F  = new JetCorrectorParameters(DataL3JetParAK8ByIOV_F);
      L2JetParAK8_F  = new JetCorrectorParameters(DataL2JetParAK8ByIOV_F);
      L1JetParAK8_F  = new JetCorrectorParameters(DataL1JetParAK8ByIOV_F);

      // Load the JetCorrectorParameter objects into a std::vector,
      // IMPORTANT: THE ORDER MATTERS HERE !!!!
      vPar_B.push_back(*L1JetPar_B);
      vPar_B.push_back(*L2JetPar_B);
      vPar_B.push_back(*L3JetPar_B);
      vPar_B.push_back(*ResJetPar_B);
      vParAK8_B.push_back(*L1JetParAK8_B);
      vParAK8_B.push_back(*L2JetParAK8_B);
      vParAK8_B.push_back(*L3JetParAK8_B);
      vParAK8_B.push_back(*ResJetParAK8_B);

      vPar_C.push_back(*L1JetPar_C);
      vPar_C.push_back(*L2JetPar_C);
      vPar_C.push_back(*L3JetPar_C);
      vPar_C.push_back(*ResJetPar_C);
      vParAK8_C.push_back(*L1JetParAK8_C);
      vParAK8_C.push_back(*L2JetParAK8_C);
      vParAK8_C.push_back(*L3JetParAK8_C);
      vParAK8_C.push_back(*ResJetParAK8_C);

      vPar_DE.push_back(*L1JetPar_DE);
      vPar_DE.push_back(*L2JetPar_DE);
      vPar_DE.push_back(*L3JetPar_DE);
      vPar_DE.push_back(*ResJetPar_DE);
      vParAK8_DE.push_back(*L1JetParAK8_DE);
      vParAK8_DE.push_back(*L2JetParAK8_DE);
      vParAK8_DE.push_back(*L3JetParAK8_DE);
      vParAK8_DE.push_back(*ResJetParAK8_DE);

      vPar_F.push_back(*L1JetPar_F);
      vPar_F.push_back(*L2JetPar_F);
      vPar_F.push_back(*L3JetPar_F);
      vPar_F.push_back(*ResJetPar_F);
      vParAK8_F.push_back(*L1JetParAK8_F);
      vParAK8_F.push_back(*L2JetParAK8_F);
      vParAK8_F.push_back(*L3JetParAK8_F);
      vParAK8_F.push_back(*ResJetParAK8_F);


      //NOTE: these statement are there originaly from oldLJMet, but its causing segfault !!
//       delete JetCorrector_B;
//       delete JetCorrector_C;
//       delete JetCorrector_DE;
//       delete JetCorrector_F;
//       delete JetCorrectorAK8_B;
//       delete JetCorrectorAK8_C;
//       delete JetCorrectorAK8_DE;
//       delete JetCorrectorAK8_F;

      JetCorrector_B = new FactorizedJetCorrector(vPar_B);
      JetCorrectorAK8_B = new FactorizedJetCorrector(vParAK8_B);
      JetCorrector_C = new FactorizedJetCorrector(vPar_C);
      JetCorrectorAK8_C = new FactorizedJetCorrector(vParAK8_C);
      JetCorrector_DE = new FactorizedJetCorrector(vPar_DE);
      JetCorrectorAK8_DE = new FactorizedJetCorrector(vParAK8_DE);
      JetCorrector_F = new FactorizedJetCorrector(vPar_F);
      JetCorrectorAK8_F = new FactorizedJetCorrector(vParAK8_F);

    }

    //BTAG parameter initialization
    btag_cuts          = selectorConfig.getParameter<bool>("btag_cuts"),
    bdisc_min          = selectorConfig.getParameter<double>("bdisc_min"),
    DeepCSVfile        = selectorConfig.getParameter<std::string>("DeepCSVfile"),
    DeepCSVSubjetfile  = selectorConfig.getParameter<std::string>("DeepCSVSubjetfile"),
    btagOP             = selectorConfig.getParameter<std::string>("btagOP"),
    BTagUncertUp       = selectorConfig.getParameter<bool>("BTagUncertUp"),
    BTagUncertDown     = selectorConfig.getParameter<bool>("BTagUncertDown"),
    MistagUncertUp     = selectorConfig.getParameter<bool>("MistagUncertUp"),
    MistagUncertDown   = selectorConfig.getParameter<bool>("MistagUncertDown"),

    bTagCut = bdisc_min;
    std::cout << mLegend << "b-tag check: DeepCSV "<<btagOP<<" > "<<bdisc_min<<std::endl;
    std::cout << mLegend << "b-tag files: " << DeepCSVfile << ", " << DeepCSVSubjetfile << std::endl;
    calib   = BTagCalibration("deepcsv",DeepCSVfile);
    calibsj = BTagCalibration("deepcsvsj",DeepCSVSubjetfile);
    if(btagOP == "LOOSE"){
      reader   = BTagCalibrationReader(BTagEntry::OP_LOOSE, "central", {"up","down"});
      readerSJ = BTagCalibrationReader(BTagEntry::OP_LOOSE, "central", {"up","down"});
    }else if(btagOP == "TIGHT"){
      reader   = BTagCalibrationReader(BTagEntry::OP_TIGHT, "central", {"up","down"});
    }else{
      reader   = BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up","down"});
      readerSJ = BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up","down"});
    }
    reader.load(calib, BTagEntry::FLAV_B, "comb");
    reader.load(calib, BTagEntry::FLAV_C, "comb");
    reader.load(calib, BTagEntry::FLAV_UDSG, "incl");
    readerSJ.load(calibsj, BTagEntry::FLAV_B, "lt");
    readerSJ.load(calibsj, BTagEntry::FLAV_C, "lt");
    readerSJ.load(calibsj, BTagEntry::FLAV_UDSG, "incl");


    // ---------------------------------------------------------------
    // ---------------------------------------------------------------
    // NOTE: EVERYTHING ABOVE NEEDS TO BE EITHER REORGANIZED/REWRITTEN - end
    // ---------------------------------------------------------------
    // ---------------------------------------------------------------


    //-----------------------
    // Define and Set cuts
    //-----------------------

    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");
    push_back("Trigger");
    push_back("Primary Vertex");
    push_back("MET filters");
    push_back("Min Loose Leptons");
    push_back("Max Loose Leptons");
    push_back("Min Leptons");
    push_back("Max Leptons");
    push_back("Min jet multiplicity");
    push_back("Max jet multiplicity");
    push_back("Leading jet pt");
    push_back("MET");
    push_back("All cuts");

    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Trigger",trigger_cut);
    set("Primary Vertex",pv_cut);
    set("MET filters", metfilters);
    set("Min Loose Leptons",minLooseLeptons_cut);
    set("Max Loose Leptons",maxLooseLeptons_cut);
    set("Min Leptons",minLeptons_cut);
    set("Max Leptons",maxLeptons_cut);
    if (jet_cuts){
        set("Min jet multiplicity", min_jet);
        set("Max jet multiplicity", max_jet);
        set("Leading jet pt", leading_jet_pt);
        }
    else{
        set("Min jet multiplicity", false);
        set("Max jet multiplicity", false);
        set("Leading jet pt", false);
        }
    set("MET", met_cuts);
    set("All cuts",true);


} // end of BeginJob()

bool MultiLepEventSelector::operator()( edm::Event const & event, pat::strbitset & ret) // EVENT SELECTOR!
{
  if(debug)std::cout << "Processing Event in MultiLepEventSelector::operator()" << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;
  if(debug)std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;

  while(1){ // standard infinite while loop trick to avoid nested ifs

    passCut(ret, "No selection");

    if( TriggerSelection(event) ) passCut(ret, "Trigger");
    else break;

    if( PVSelection(event) )      passCut(ret, "Primary Vertex");
    else break;

    if( METfilter(event) )        passCut(ret, "MET filters");
    else break;

    //Collect selected leptons
    MuonSelection(event);
    ElectronSelection(event);
    if( ! LeptonsSelection(event, ret) ) break;

    //Collect jets
    if( ! JetSelection(event, ret) ) break;

    //Collect AK8 jets
    AK8JetSelection(event);

    if( METSelection(event) )     passCut(ret, "MET"); // this needs to be after jets.
    else break;


    passCut(ret, "All cuts");
    break;

  } // end of while loop


  bFirstEntry = false;

  return (bool)ret;

  setIgnored(ret);

  return false;
}// end of operator()


void MultiLepEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
  //
  // Compute analysis-specific quantities in the event,
  // return via event content
  //

  //   ec.SetValue("pi", 3.14);


  return;
}

void MultiLepEventSelector::EndJob()
{

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// NOTE: EVERYTHING BELOW NEEDS TO BE EITHER REORGANIZED/REWRITTEN - start
// ---------------------------------------------------------------
// ---------------------------------------------------------------

//NOTE: about pointers, try using smart pointers which deletes itself when out of scope, or something like that.

//NOTE: these statement are there originaly from oldLJMet, but its causing segfault !!
//       delete ResJetPar;
//       delete L3JetPar;
//       delete L2JetPar;
//       delete L1JetPar;
//       delete ResJetParAK8;
//       delete L3JetParAK8;
//       delete L2JetParAK8;
//       delete L1JetParAK8;
//
//       delete ResJetPar_B;
//       delete L3JetPar_B;
//       delete L2JetPar_B;
//       delete L1JetPar_B;
//       delete ResJetParAK8_B;
//       delete L3JetParAK8_B;
//       delete L2JetParAK8_B;
//       delete L1JetParAK8_B;
//       delete ResJetPar_C;
//       delete L3JetPar_C;
//       delete L2JetPar_C;
//       delete L1JetPar_C;
//       delete ResJetParAK8_C;
//       delete L3JetParAK8_C;
//       delete L2JetParAK8_C;
//       delete L1JetParAK8_C;
//       delete ResJetPar_DE;
//       delete L3JetPar_DE;
//       delete L2JetPar_DE;
//       delete L1JetPar_DE;
//       delete ResJetParAK8_DE;
//       delete L3JetParAK8_DE;
//       delete L2JetParAK8_DE;
//       delete L1JetParAK8_DE;
//       delete ResJetPar_F;
//       delete L3JetPar_F;
//       delete L2JetPar_F;
//       delete L1JetPar_F;
//       delete ResJetParAK8_F;
//       delete L3JetParAK8_F;
//       delete L2JetParAK8_F;
//       delete L1JetParAK8_F;
//
//       delete jecUnc;

//       delete JetCorrector;
//       delete JetCorrectorAK8;
//       delete JetCorrector_B;
//       delete JetCorrectorAK8_B;
//       delete JetCorrector_C;
//       delete JetCorrectorAK8_C;
//       delete JetCorrector_DE;
//       delete JetCorrectorAK8_DE;
//       delete JetCorrector_F;
//       delete JetCorrectorAK8_F;

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// NOTE: EVERYTHING ABOVE NEEDS TO BE EITHER REORGANIZED/REWRITTEN - end
// ---------------------------------------------------------------
// ---------------------------------------------------------------


}

bool MultiLepEventSelector::TriggerSelection(edm::Event const & event)
{

	//
	//_____ Trigger cuts __________________________________
	//

	bool passTrig = false;

	if ( considerCut("Trigger") ) {

		if(debug)std::cout << "\t" <<"TriggerSelection:"<< std::endl;

		edm::Handle< edm::TriggerResults > triggersHandle;
		event.getByToken(triggersToken,triggersHandle);
		const edm::TriggerNames trigNames = event.triggerNames(*triggersHandle);

		unsigned int _tSize = triggersHandle->size();

		bool passTrigElMC = false;
		bool passTrigMuMC = false;
		bool passTrigElData = false;
		bool passTrigMuData = false;

		// dump trigger names
		if (bFirstEntry && dump_trigger){
			std::cout << "\t" << "Dumping available HLT paths in dataset:"<<std::endl;
			for (unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				std::cout << trigName <<std::endl;
			}
		}
		if (debug) std::cout<< "\t\t" <<"The event FIRED the following registered trigger(s) in LJMet: "<<std::endl;

		mvSelTriggersEl.clear();
		mvSelMCTriggersEl.clear();
		mvSelTriggersMu.clear();
		mvSelMCTriggersMu.clear();

		int passTrigEl = 0;
		if (debug) std::cout<< "\t" <<"	In MC El trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < mctrigger_path_el.size() && mctrigger_path_el.at(0)!="" ; ipath++){
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if (trigName.find(mctrigger_path_el.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))) {
					passTrigEl = 1;
					mvSelMCTriggersEl[mctrigger_path_el.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
				else mvSelMCTriggersEl[mctrigger_path_el.at(ipath)] = 0;
			}
		}
		if (passTrigEl>0) passTrigElMC = true;

		int passTrigMu = 0;
		if (debug) std::cout<< "\t" <<"	In MC Mu trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < mctrigger_path_mu.size() && mctrigger_path_mu.at(0)!="" ; ipath++){
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if ( trigName.find(mctrigger_path_mu.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
					passTrigMu = 1;
					mvSelMCTriggersMu[mctrigger_path_mu.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
				else mvSelMCTriggersMu[mctrigger_path_mu.at(ipath)] = 0;
			}
		}
		if (passTrigMu>0) passTrigMuMC = true;

		//Loop over each data channel separately
		passTrigEl = 0;
		if (debug) std::cout<< "\t" <<"	In Data El trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < trigger_path_el.size() && trigger_path_el.at(0)!="" ; ipath++){
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if ( trigName.find(trigger_path_el.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
					passTrigEl = 1;
					mvSelTriggersEl[trigger_path_el.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
				else mvSelTriggersEl[trigger_path_el.at(ipath)] = 0;
			}
		}
		if (passTrigEl>0) passTrigElData = true;

		passTrigMu = 0;
		if (debug) std::cout<< "\t" <<"	In Data Mu trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < trigger_path_mu.size() && trigger_path_mu.at(0)!="" ; ipath++){
			for(unsigned int i=0; i<_tSize; i++){
			std::string trigName = trigNames.triggerName(i);
			if ( trigName.find(trigger_path_mu.at(ipath)) == std::string::npos) continue;
			if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
				passTrigMu = 1;
				mvSelTriggersMu[trigger_path_mu.at(ipath)] = 1;
				if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
			}
			else mvSelTriggersMu[trigger_path_mu.at(ipath)] = 0;
			}
		}
		if (passTrigMu>0) passTrigMuData = true;

		if (isMc && (passTrigMuMC||passTrigElMC) ) passTrig = true;
		if (!isMc && (passTrigMuData||passTrigElData) ) passTrig = true;


	}
	else{

		if(debug)std::cout << "\t" <<"IGNORING TriggerSelection."<< std::endl;

		passTrig = true;

	} // end of trigger cuts

	return passTrig;

}

bool MultiLepEventSelector::PVSelection(edm::Event const & event)
{

    bool pass = false;

    //
    //_____ Primary Vertex cuts __________________________________
    //
    vSelPVs.clear();
    if ( considerCut("Primary Vertex") ) {

    	if(debug)std::cout << "\t" <<"PVSelection:"<< std::endl;

    	if ( (*pvSel_)(event) ){
    		pass = true;
    		vSelPVs = pvSel_->GetSelectedPvs(); //reference: https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/PhysicsTools/SelectorUtils/interface/PVSelector.h
    		if(debug)std::cout << "\t" << "\t" <<"num of selected PV = "<< pvSel_->GetNpv()<< std::endl;
    	}
    	else{
    		if(debug)std::cout << "\t" << "\t" <<"No selected PV."<< std::endl;
    	}

    }
    else{

         if(debug)std::cout << "\t" <<"IGNORING PVSelection"<< std::endl;

	 pass = true;

    }

    return pass;

}

bool MultiLepEventSelector::METfilter(edm::Event const & event)
{

	bool pass = false;

	//
	//_____ MET Filters __________________________________
	//
	//
	if (considerCut("MET filters")) {

	  if(debug)std::cout << "\t" <<"METFilterSelection:"<< std::endl;

	  edm::Handle<edm::TriggerResults > PatTriggerResults;
	  event.getByToken( METfilterToken, PatTriggerResults );
	  const edm::TriggerNames patTrigNames = event.triggerNames(*PatTriggerResults);

	  bool goodvertpass = false;
	  bool globaltighthalopass = false;
	  bool hbhenoisepass = false;
	  bool hbhenoiseisopass = false;
	  bool ecaldeadcellpass = false;
	  bool badpfmuonpass = false;
	  bool badchargedcandpass = false;
	  bool eebadscpass = false;
	  bool eebadcalibpass = false;


	  for (unsigned int i=0; i<PatTriggerResults->size(); i++){
	    if (patTrigNames.triggerName(i) == "Flag_goodVertices") goodvertpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_globalSuperTightHalo2016Filter") globaltighthalopass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_HBHENoiseFilter") hbhenoisepass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_HBHENoiseIsoFilter") hbhenoiseisopass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_EcalDeadCellTriggerPrimitiveFilter") ecaldeadcellpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_BadPFMuonFilter") badpfmuonpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_BadChargedCandidateFilter") badchargedcandpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_eeBadScFilter") eebadscpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_ecalBadCalibFilter") eebadcalibpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	  }

	  // Rerun ecalBadCalibReducedMINIAODFilter if possible
	  edm::Handle<bool> passecalBadCalibFilterUpdate;
	  if(event.getByToken( METfilterToken_extra , passecalBadCalibFilterUpdate)){
	      eebadcalibpass = *passecalBadCalibFilterUpdate;
	  }

	  if( hbhenoisepass &&
	      hbhenoiseisopass &&
	      globaltighthalopass &&
	      ecaldeadcellpass &&
	      (isMc || eebadscpass) &&
	      goodvertpass &&
	      badpfmuonpass &&
	      badchargedcandpass &&
	      eebadcalibpass)
	  {
	    if(debug)std::cout << "\t\t" <<"Passes MET Filter selection."<< std::endl;
	    pass=true;
	  }
	  else{

	    if(debug)std::cout << "\t" <<"Fails MET Filter selection."<< std::endl;

	  }

	}
	else{

	  if(debug)std::cout << "\t" <<"IGNORING MET Filter selection"<< std::endl;

	  pass = true;

	}

    return pass;

}

void MultiLepEventSelector::MuonSelection(edm::Event const & event)
{

	//======================================================
	//
	//_____ Muon cuts ________________________________
	//
	//

	int _n_muons  = 0;
	int nSelMuons = 0;
	int nSelLooseMuons = 0;

	//get muons
	edm::Handle< pat::MuonCollection > 	muonsHandle;
	event.getByToken(muonsToken, muonsHandle);

	vSelMuons.clear();
	vSelLooseMuons.clear();

	if ( muon_cuts ) { if(debug)std::cout << "\t" <<"Applying MuonSelection"<< std::endl;}
	else { if(debug)std::cout << "\t" <<"NOT applying MuonSelection"<< std::endl;}

	for (std::vector<pat::Muon>::const_iterator _imu = muonsHandle->begin(); _imu != muonsHandle->end(); _imu++){


		if ( muon_cuts ) {

		  bool pass = false;
		  bool pass_loose = false;

		  if (debug) std::cout<< "\t\t" << "pt    = " << _imu->pt() << std::endl; //DEBUG - rizki
		  if (debug) std::cout<< "\t\t" << "|eta| = " << fabs(_imu->eta()) << std::endl; //DEBUG - rizki
		  if (debug) std::cout<< "\t\t" << "phi = " << _imu->phi() << std::endl; //DEBUG - rizki

		  double chIso = (*_imu).pfIsolationR04().sumChargedHadronPt;
		  double nhIso = (*_imu).pfIsolationR04().sumNeutralHadronEt;
		  double gIso  = (*_imu).pfIsolationR04().sumPhotonEt;
		  double puIso = (*_imu).pfIsolationR04().sumPUPt;

		  double pt    = (*_imu).pt() ;
		  double pfIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso))/pt;

		  //muon cuts

		  //check loose first
		  while(1){

		    if ( _imu->pt()<loose_muon_minpt )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_muon_minpt_loose"<< std::endl;

		    if ( fabs(_imu->eta())> loose_muon_maxeta )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_muon_maxeta_loose"<< std::endl;


		    if ( ! (*_imu).passed(reco::Muon::CutBasedIdLoose) )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_CutBasedID_loose" << std::endl;

		    if (muon_useMiniIso){
		    	if ( ! (*_imu).passed(reco::Muon::MiniIsoLoose) ) break;
		    	if(debug)std::cout<< "\t\t\t" << "pass_muon_miniIso_loose"<< std::endl;
		    }
		    else{
		    	if ( pfIso > loose_muon_relIso) break;
		    	if(debug)std::cout<< "\t\t\t" << "pass_muon_relIso_loose "<< std::endl;
		    }

		    if (vSelPVs.size() > 0){

		      if ( fabs((*_imu).muonBestTrack()->dxy((*vSelPVs[0]).position())) > loose_muon_dxy )break;
		      if(debug)std::cout<< "\t\t\t" << "pass_muon_dxy_loose"<< std::endl;

		      if ( fabs((*_imu).muonBestTrack()->dz((*vSelPVs[0]).position())) > loose_muon_dz )break;
		      if(debug)std::cout<< "\t\t\t" << "pass_muon_dz_loose" << std::endl;

		    }

		    pass_loose = true; // success
		    if(debug)std::cout<< "\t\t\t" << "----> pass_loose"<< std::endl;
		    break;
		  }

		  if ( pass_loose ){
		    // save loose muon
		    vSelLooseMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
		    ++nSelLooseMuons;
		  }
		  else continue; //skip to next muon if fail loose

		  //check tight
		  while(1){

		    if ( _imu->pt()<muon_minpt )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_muon_minpt"<< std::endl;

		    if ( fabs(_imu->eta())>muon_maxeta )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_muon_maxeta"<< std::endl;


		    if ( ! (*_imu).passed(reco::Muon::CutBasedIdTight) )break;
		    if(debug)std::cout<< "\t\t\t" << "pass_CutBasedID" << std::endl;

		    if (muon_useMiniIso){
		    	if ( ! (*_imu).passed(reco::Muon::MiniIsoTight) ) break;
		    	if(debug)std::cout<< "\t\t\t" << "pass_muon_miniIso"<< std::endl;
		    }
		    else{
		    	if ( pfIso > muon_relIso) break;
		    	if(debug)std::cout<< "\t\t\t" << "pass_muon_relIso"<< std::endl;
		    }


		    if (vSelPVs.size() > 0){

		      if ( fabs((*_imu).muonBestTrack()->dxy((*vSelPVs[0]).position())) > muon_dxy )break;
		      if(debug)std::cout<< "\t\t\t" << "pass_muon_dxy"<< std::endl;

		      if ( fabs((*_imu).muonBestTrack()->dz((*vSelPVs[0]).position())) > muon_dz )break;
		      if(debug)std::cout<< "\t\t\t" << "pass_muon_dz" << std::endl;
		    }

		    pass = true; // success
		    if(debug)std::cout<< "\t\t\t" << "----> pass"<< std::endl;
		    break;

		  }

		  if ( pass ){
		    // savet tight muon
		    vSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
		    ++nSelMuons;
		  }



		} // end of muon cuts
		else{

		  // save ALL muons
		  vSelLooseMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
		  ++nSelLooseMuons;
		  vSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
		  ++nSelMuons;
		}

		_n_muons++;

	} // end of the muon loop

	if (debug) std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
	if (debug) std::cout<< "\t\t"<< "nSelLooseMuons          = " << nSelLooseMuons << " out of "<< muonsHandle->size() << std::endl; // DEBUG - rizki
	if (debug) std::cout<< "\t\t"<< "nSelMuons               = " << nSelMuons << " out of "<< muonsHandle->size() << std::endl; // DEBUG - rizki
	if (debug) std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki


}

void MultiLepEventSelector::ElectronSelection(edm::Event const & event)
{

	//
	//_____ Electron cuts __________________________________
	//
	// loop over electrons

	int _n_electrons  = 0;
	int nSelElectrons = 0;
	int nSelLooseElectrons = 0;

	//get electrons
	edm::Handle< pat::ElectronCollection > electronsHandle;
	event.getByToken(electronsToken, electronsHandle);

	vSelElectrons.clear();
	vSelLooseElectrons.clear();

	//packed pf candidates and rho source needed miniIso
	edm::Handle<pat::PackedCandidateCollection> packedPFCandsHandle;
	event.getByToken(PFCandToken, packedPFCandsHandle);

	//rho isolation from susy recommendation
	edm::Handle<double> rhoJetsNC_Handle;
	event.getByToken(rhoJetsNC_Token, rhoJetsNC_Handle);
	double myRhoJetsNC = *rhoJetsNC_Handle;

	if ( electron_cuts ) { if(debug)std::cout << "\t" <<"Applying ElectronSelection"<< std::endl;}
	else { if(debug)std::cout << "\t" <<"NOT applying ElectronSelection"<< std::endl;}


	for (std::vector<pat::Electron>::const_iterator _iel = electronsHandle->begin(); _iel != electronsHandle->end(); _iel++){

		if ( electron_cuts ) {

		  bool pass_loose = false;
		  bool pass = false;

		  if (debug) std::cout << "\t\t" << "pt                                          = " << _iel->pt() << std::endl; //DEBUG - rizki
		  if (debug) std::cout << "\t\t" << "|eta| ( ->superCluster()->eta(), ->eta() )  = " << fabs(_iel->superCluster()->eta()) << ", " << fabs(_iel->eta()) << std::endl; //DEBUG - rizki
		  if (debug) std::cout << "\t\t" << "phi ( ->superCluster()->phi(), ->phi() )    = " << _iel->superCluster()->phi() << ", " << _iel->phi() << std::endl; //DEBUG - rizki


		  //electron cuts

		  //check loose first
		  while(1){

			if (_iel->pt()<loose_electron_minpt) break;
			if(debug)std::cout << "\t\t\t" << "pass_electron_minpt_loose" <<std::endl;

			if ( fabs(_iel->superCluster()->eta())>loose_electron_maxeta )break;
			if(debug)std::cout << "\t\t\t" << "pass_electron_maxeta_loose" <<std::endl;

			if ((*_iel).isEBEEGap()) break; //why is this different from the tight selection version?
			if(debug)std::cout << "\t\t\t" << "pass_electron_isEBEEGap_loose" <<std::endl;

			if ( UseElMVA ) {

			  bool mvapass = false;

			  if(UseElIDV1) mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V1-wpLoose");
			  else mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V2-wpLoose");

			  if (!mvapass) break;
			  if(debug)std::cout << "\t\t\t" << "pass_electron_mva_loose" <<std::endl;

			  if(electron_useMiniIso){

				pat::Electron* elptr = new pat::Electron(*_iel);
				float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCandsHandle, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

				if(miniIso > loose_electron_miniIso){delete elptr;  break;}
				if(debug)std::cout << "\t\t\t" << "pass_electron_useMiniIso_loose" <<std::endl;
				delete elptr;
			  }
			}
			else {
			  bool cutbasedpass = false;
			  if(UseElIDV1) cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V1-loose");
			  else cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V2-loose");

			  if(!cutbasedpass) break;
			  if(debug)std::cout << "\t\t\t" << "pass_electron_cutbased_loose" <<std::endl;
			}

			pass_loose = true; // success
			if(debug)std::cout << "\t\t\t" << "----> pass_loose" <<std::endl;
			break;
		  }

		  if ( pass_loose ){
			// save loose electron
			vSelLooseElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
			++nSelLooseElectrons;
		  }
		  else continue; //skip to next el if fail loose


		  //check tight
		  while(1){

			if (_iel->pt()<electron_minpt) break;
			if(debug)std::cout << "\t\t\t" << "pass_electron_minpt" <<std::endl;

			if ( fabs(_iel->superCluster()->eta())>electron_maxeta ) break;
			if(debug)std::cout << "\t\t\t" << "pass_electron_maxeta" <<std::endl;

			if (!( fabs(_iel->superCluster()->eta())<1.4442 || fabs(_iel->superCluster()->eta())>1.566 ))break;
			if(debug)std::cout << "\t\t\t" << "pass_electron_isEBEEGap" <<std::endl;

			if ( UseElMVA ) {

			  bool mvapass = false;
			  if(UseElIDV1) mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V1-wp90");
			  else mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V2-wp90");

			  if (!mvapass) break;
			  if(debug)std::cout << "\t\t\t" << "pass_electron_mva" <<std::endl;


			  if(electron_useMiniIso){

				pat::Electron* elptr = new pat::Electron(*_iel);
				//Attention: Don't we need to update to the official CMSSW MiniIsolation.cc rather than some old file? --Rizki Mar 12, 2019.
				float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCandsHandle, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

				if(miniIso > electron_miniIso){delete elptr;  break;}
				if(debug)std::cout << "\t\t\t" << "pass_electron_useMiniIso" <<std::endl;
				delete elptr;
			  }
			}
			else {

			  bool cutbasedpass = false;
			  if(UseElIDV1) cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V1-tight");
			  else cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V2-tight");

			  if(!cutbasedpass) break;
			  if(debug)std::cout << "\t\t\t" << "pass_electron_cutbased" <<std::endl;
			}

			pass = true; // success
			if(debug)std::cout << "\t\t\t" << "----> pass" <<std::endl;
			break;
		  }

		  if ( pass ){
			// save tight electron
			vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
			++nSelElectrons;
		  }

		} // end of electron cuts
		else{

		  // save ALL electrons
		  vSelLooseElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
		  ++nSelLooseElectrons;
		  vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
		  ++nSelElectrons;
		}

		_n_electrons++;

	} // end of the electron loop


	if (debug)std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "nSelLooseElectrons         = " << nSelLooseElectrons << " out of " << electronsHandle->size() <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "nSelElectrons              = " << nSelElectrons << " out of " << electronsHandle->size() <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki


}

bool MultiLepEventSelector::LeptonsSelection(edm::Event const & event, pat::strbitset & ret)
{
	bool pass = false;

	if(debug)std::cout << "\t" <<"Leptons Selection:"<< std::endl;

	//Check for selected lepton requirement
	if(debug)std::cout << "\t" << "\t" << "nSelLooseMu = "<< vSelLooseMuons.size() << " nSelLooseEl = "<< vSelLooseElectrons.size() << std::endl;
	if(debug)std::cout << "\t" << "\t" << "nSelMu      = "<< vSelMuons.size() << " nSelEl      = "<< vSelElectrons.size() << std::endl;

	unsigned int nLeps      = vSelMuons.size() + vSelElectrons.size();
	unsigned int nLooseLeps = vSelLooseMuons.size() + vSelLooseElectrons.size();

	bool pass_minLooseLeptons = false;
	bool pass_maxLooseLeptons = false;
	bool pass_minLeptons      = false;
	bool pass_maxLeptons      = false;

	if(considerCut("Min Loose Leptons")){
		if ( nLooseLeps >= (unsigned int)minLooseLeptons){
			pass_minLooseLeptons = true;
			passCut(ret, "Min Loose Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_minLooseLeptons"<<std::endl;
		}
	}

	if(considerCut("Max Loose Leptons")){
		if ( nLooseLeps <= (unsigned int)maxLooseLeptons){
			pass_maxLooseLeptons = true;
			passCut(ret, "Max Loose Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_maxLooseLeptons"<<std::endl;
		}
	}


	if(considerCut("Min Leptons")){
		if ( nLeps >= (unsigned int)minLeptons){
			pass_minLeptons = true;
			passCut(ret, "Min Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_minLeptons"<<std::endl;
		}
	}

	if(considerCut("Max Leptons")){
		if ( nLeps <= (unsigned int)maxLeptons){
			pass_maxLeptons = true;
			passCut(ret, "Max Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_maxLeptons"<<std::endl;
		}
	}

	if(ignoreCut("Min Loose Leptons")) pass_minLooseLeptons = true;
	if(ignoreCut("Max Loose Leptons")) pass_maxLooseLeptons = true;
	if(ignoreCut("Min Leptons")) pass_minLeptons = true;
	if(ignoreCut("Max Leptons")) pass_maxLeptons = true;

	if( pass_minLooseLeptons && pass_maxLooseLeptons && pass_minLeptons && pass_maxLeptons) pass=true;


	return pass;
}

bool MultiLepEventSelector::JetSelection(edm::Event const & event, pat::strbitset & ret)
{

  //
  // jet loop
  //
  //
  if(debug)std::cout << "\t" <<"Jet Selection:"<< std::endl;


  if(!isMc) JECbyIOV(event);


  edm::Handle<std::vector<pat::Jet> > jetsHandle;
  event.getByToken( jetsToken, jetsHandle );

  int _n_good_jets = 0;
  int _n_jets = 0;
  int nBtagJets = 0;
  double _leading_jet_pt = 0.0;

  vAllJets.clear();
  vSelJets.clear();
  vSelCorrJets.clear();
/*  mvCorrJets_jesup.clear();
  mvCorrJets_jesdn.clear();
  mvCorrJets_jerup.clear();
  mvCorrJets_jerdn.clear();*/
  vSelCorrJetsWithBTags.clear();
  vSelBtagJets.clear();


  std::vector<edm::Ptr<pat::Muon>> cleaningMuons;
  cleaningMuons = vSelMuons;
  if(CleanLooseLeptons) cleaningMuons = vSelLooseMuons;

  std::vector<edm::Ptr<pat::Electron>> cleaningElectrons;
  cleaningElectrons = vSelElectrons;
  if(CleanLooseLeptons) cleaningElectrons = vSelLooseElectrons;

  //for jet correction
  bool isAK8 = false;
  bool reCorrectJet = doNewJEC;
  unsigned int syst;
  if (JECup){syst=1;}
  else if (JECdown){syst=2;}
  else if (JERup){syst=3;}
  else if (JERdown){syst=4;}
  else syst = 0; //nominal

  for (std::vector<pat::Jet>::const_iterator _ijet = jetsHandle->begin();_ijet != jetsHandle->end(); ++_ijet){

    bool _pass = false;
    bool _passpf = false;
    bool _isTagged = false;
    bool _cleaned = false;

    TLorentzVector jetP4;
/*    TLorentzVector jetP4_jesup;
    TLorentzVector jetP4_jesdn;
    TLorentzVector jetP4_jerup;
    TLorentzVector jetP4_jerdn;
*/
    pat::Jet tmpJet = _ijet->correctedJet(0);
    pat::Jet corrJet = *_ijet;


    if ( doLepJetCleaning ){
      if (debug) std::cout << "LepJetCleaning: Checking Overlap" << std::endl;

      for(unsigned int imu = 0; imu < cleaningMuons.size(); imu++){
		if ( deltaR(cleaningMuons[imu]->p4(),_ijet->p4()) < LepJetDR) {
		  std::vector<reco::CandidatePtr> muDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningMuons[imu]->numberOfSourceCandidatePtrs(); ++isrc ){
			if (cleaningMuons[imu]->sourceCandidatePtr(isrc).isAvailable()) {
			  muDaughters.push_back( cleaningMuons[imu]->sourceCandidatePtr(isrc) );
			}
		  }
		  if (debug) {
			std::cout << "         Muon : pT = " << cleaningMuons[imu]->pt() << " eta = " << cleaningMuons[imu]->eta() << " phi = " << cleaningMuons[imu]->phi() << std::endl;
			std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
		  }

		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
			for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
			  if ( (*_i_const).key() == muDaughters[muI].key() ) {
				tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );
				if (debug) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
				jetP4 = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJetReturnPatJet METHOD, CAN CALCULATE P4 FROM OUTPUT OF correctJetReturnPatJet RATHER THAN DOING EXACTLY EVERYTHING correctJetReturnPatJet IS DOING. FIX!!!! -- MAR 15,2019
/*				if (mbPar["doAllJetSyst"]) {
				  jetP4_jesup = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,1);
				  jetP4_jesdn = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,2);
				  jetP4_jerup = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,3);
				  jetP4_jerdn = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,4);
				}*/
				corrJet = correctJetReturnPatJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJet METHOD ! FIX !!!
				if (debug) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << std::endl;
				_cleaned = true;
				muDaughters.erase( muDaughters.begin()+muI );
				break;
			  }
			}
		  }
		}
      }

      for(unsigned int iel = 0; iel < cleaningElectrons.size(); iel++){
		if ( deltaR(cleaningElectrons[iel]->p4(),_ijet->p4()) < LepJetDR){
		  std::vector<reco::CandidatePtr> elDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningElectrons[iel]->numberOfSourceCandidatePtrs(); ++isrc ){
			if (cleaningElectrons[iel]->sourceCandidatePtr(isrc).isAvailable()) {
			  elDaughters.push_back( cleaningElectrons[iel]->sourceCandidatePtr(isrc) );
			}
		  }
		  if (debug) {
			std::cout << "     Electron : pT = " << cleaningElectrons[iel]->pt() << " eta = " << cleaningElectrons[iel]->eta() << " phi = " << cleaningElectrons[iel]->phi() << std::endl;
			std::cout << "      Raw Jet : pT = " << _ijet->correctedJet(0).pt() << " eta = " << _ijet->correctedJet(0).eta() << " phi = " << _ijet->correctedJet(0).phi() << std::endl;
		  }
		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
			for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
			  if ( (*_i_const).key() == elDaughters[elI].key() ) {
				tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );
				if (debug) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
				jetP4 = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJetReturnPatJet METHOD, CAN CALCULATE P4 FROM OUTPUT OF correctJetReturnPatJet RATHER THAN DOING EXACTLY EVERYTHING correctJetReturnPatJet IS DOING. FIX!!!! -- MAR 15,2019
/*				if (mbPar["doAllJetSyst"]) {
				  jetP4_jesup = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,1);
				  jetP4_jesdn = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,2);
				  jetP4_jerup = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,3);
				  jetP4_jerdn = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,4);
				}*/
				corrJet = correctJetReturnPatJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJet METHOD ! FIX!!!! -- MAR 15,2019
				if (debug) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << std::endl;
				_cleaned = true;
				elDaughters.erase( elDaughters.begin()+elI );
				break;
			  }
			}
		  }
		}
      }
    }

    if (!_cleaned) {
      jetP4 = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst);
/*      if (mbPar["doAllJetSyst"]) {
		jetP4_jesup = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,1);
		jetP4_jesdn = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,2);
		jetP4_jerup = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,3);
		jetP4_jerdn = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet,4);
      }*/
      corrJet = correctJetReturnPatJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet, syst);
    }

    _isTagged = isJetTagged(*_ijet, jetP4, event, isMc);

    // jet cuts  //NOTE: THIS IDEALLY SHOULDN'T BE HARD CODED -- Mar 13, 2019
    while(1){

      // PF Jet ID
      if (fabs(_ijet->correctedJet(0).eta()) < 2.4 &&
	  _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
	  _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
	  _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1 &&
	  _ijet->correctedJet(0).chargedHadronEnergyFraction() > 0 &&
	  _ijet->correctedJet(0).chargedMultiplicity() > 0
	  ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.4 &&
	       fabs(_ijet->correctedJet(0).eta()) < 2.7 &&
	       _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
	       _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1
	       ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.7 &&
	       fabs(_ijet->correctedJet(0).eta()) < 3.0 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() > 0.02 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.99 &&
	       _ijet->correctedJet(0).neutralMultiplicity() > 2
	       ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 3.0 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 &&
	       _ijet->correctedJet(0).neutralHadronEnergyFraction() > 0.02 &&
	       _ijet->correctedJet(0).neutralMultiplicity() > 10
	       ){ }
      else break; // fail

      _passpf = true;

      if ( jetP4.Pt() > jet_minpt ){ }
      else break; // fail


      if ( fabs(jetP4.Eta()) < jet_maxeta ){ }
      else break; // fail

      _pass = true;
      break;
    }

    pair<TLorentzVector,bool> jetwithtag;
    jetwithtag.first = jetP4;
    jetwithtag.second = _isTagged;

    if ( _pass ){


      // save all the good jets
      ++_n_good_jets;
      vSelJets.push_back(edm::Ptr<pat::Jet>( jetsHandle, _n_jets));
      vSelCorrJets.push_back(corrJet);
/*      if (mbPar["doAllJetSyst"]) {
		mvCorrJets_jesup.push_back(jetP4_jesup);
		mvCorrJets_jesdn.push_back(jetP4_jesdn);
		mvCorrJets_jerup.push_back(jetP4_jerup);
		mvCorrJets_jerdn.push_back(jetP4_jerdn);
      }
      else {
		mvCorrJets_jesup.push_back(jetP4);
		mvCorrJets_jesdn.push_back(jetP4);
		mvCorrJets_jerup.push_back(jetP4);
		mvCorrJets_jerdn.push_back(jetP4);
      }*/
      vSelCorrJetsWithBTags.push_back(jetwithtag);

      if (jetP4.Pt() > _leading_jet_pt) _leading_jet_pt = jetP4.Pt();

      if (_isTagged) {
		++nBtagJets;
		// save all the good b-tagged jets
		vSelBtagJets.push_back(edm::Ptr<pat::Jet>( jetsHandle, _n_jets));
      }
    }

    // save all the pf jets regardless of pt or eta
    // needed for MET corrections with JER/JES uncertainty
    if (_passpf)  vAllJets.push_back(edm::Ptr<pat::Jet>( jetsHandle, _n_jets));

    ++_n_jets;

  } // end of loop over jets


  // SELECT EVENTS BASED ON JETS

  bool pass_jet = false;

  while(1){

	  if ( jet_cuts ) {

		  if ( ignoreCut("Min jet multiplicity") || _n_good_jets >= cut("Min jet multiplicity",int()) ) passCut(ret, "Min jet multiplicity");
		  else break;

		  if ( ignoreCut("Max jet multiplicity") || _n_good_jets <= cut("Max jet multiplicity",int()) ) passCut(ret, "Max jet multiplicity");
		  else break;

		  if ( ignoreCut("Leading jet pt") ||  _leading_jet_pt >= cut("Leading jet pt",double()) ) passCut(ret, "Leading jet pt");
		  else break;

		  if(debug) std::cout << "\t\t\t" << "pass_jet"<<std::endl;

	  }
	  else if(debug) std::cout << "\t\t\t" << "IGNORING jet_cuts "<<std::endl;

	  pass_jet = true ;
	  break;

  }

  return pass_jet;


}

void MultiLepEventSelector::AK8JetSelection(edm::Event const & event)
{

  if(debug)std::cout << "\t" <<" AK8 Jet Selection:"<< std::endl;

  ////////////////////////////////////////////////////////////////////////////////////////
  // AK8 jets
  ////////////////////////////////////////////////////////////////////////////////////////

  edm::Handle<std::vector<pat::Jet> > AK8jetsHandle;
  event.getByToken( AK8jetsToken, AK8jetsHandle );

  int _n_good_jets_AK8 = 0;
  int _n_jets_AK8 = 0;

  vSelCorrJets_AK8.clear();

  std::vector<edm::Ptr<pat::Muon>> cleaningMuons;
  cleaningMuons = vSelMuons;
  if(CleanLooseLeptons) cleaningMuons = vSelLooseMuons;

  std::vector<edm::Ptr<pat::Electron>> cleaningElectrons;
  cleaningElectrons = vSelElectrons;
  if(CleanLooseLeptons) cleaningElectrons = vSelLooseElectrons;

  bool isAK8 = true;
  bool reCorrectJet = doNewJEC;

  for (std::vector<pat::Jet>::const_iterator _ijet = AK8jetsHandle->begin();_ijet != AK8jetsHandle->end(); ++_ijet){

    if(_ijet->pt() < 170) continue;
    if(_ijet->correctedJet(0).pt() < 170) continue;

    bool _pass = false;
    bool _cleaned = false;

    TLorentzVector jetP4;

    pat::Jet tmpJet = _ijet->correctedJet(0);
    pat::Jet corrJet = *_ijet;

    if ( doLepJetCleaning){
      if (debug) std::cout << " AK8 LepJetCleaning: Checking Overlap" << std::endl;

      for(unsigned int imu = 0; imu < cleaningMuons.size(); imu++){
		if ( deltaR(cleaningMuons[imu]->p4(),_ijet->p4()) < LepJetDRAK8) { //0.8 ){
		  std::vector<reco::CandidatePtr> muDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningMuons[imu]->numberOfSourceCandidatePtrs(); ++isrc ){
			if (cleaningMuons[imu]->sourceCandidatePtr(isrc).isAvailable()) {
			  muDaughters.push_back( cleaningMuons[imu]->sourceCandidatePtr(isrc) );
			}
		  }
		  if (debug) {
			std::cout << "         Muon : pT = " << cleaningMuons[imu]->pt() << " eta = " << cleaningMuons[imu]->eta() << " phi = " << cleaningMuons[imu]->phi() << " mass = " << cleaningMuons[imu]->mass() << std::endl;
			std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << " mass = " << _ijet->mass() << std::endl;
		  }

		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
			for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
			  if ( (*_i_const).key() == muDaughters[muI].key() ) {
				tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );
				if (debug) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << " mass = " << tmpJet.mass() << std::endl;
				jetP4 = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
				corrJet = correctJetReturnPatJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
				if (debug) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << " mass = " << jetP4.M() << std::endl;
				_cleaned = true;
				muDaughters.erase( muDaughters.begin()+muI );
				break;
			  }
			}
		  }
		}
      }

      for(unsigned int iel = 0; iel < cleaningElectrons.size(); iel++){
		if ( deltaR(cleaningElectrons[iel]->p4(),_ijet->p4()) < LepJetDRAK8){ //0.6 ){
		  std::vector<reco::CandidatePtr> elDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningElectrons[iel]->numberOfSourceCandidatePtrs(); ++isrc ){
			if (cleaningElectrons[iel]->sourceCandidatePtr(isrc).isAvailable()) {
			  elDaughters.push_back( cleaningElectrons[iel]->sourceCandidatePtr(isrc) );
			}
		  }
		  if (debug) {
			std::cout << "     Electron : pT = " << cleaningElectrons[iel]->pt() << " eta = " << cleaningElectrons[iel]->eta() << " phi = " << cleaningElectrons[iel]->phi() << " mass = " << cleaningElectrons[iel]->mass() << std::endl;
			std::cout << "      Raw Jet : pT = " << _ijet->correctedJet(0).pt() << " eta = " << _ijet->correctedJet(0).eta() << " phi = " << _ijet->correctedJet(0).phi() << " mass = " << _ijet->correctedJet(0).mass() << std::endl;
		  }
		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
			for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
			  if ( (*_i_const).key() == elDaughters[elI].key() ) {
				tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );
				if (debug) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << " mass = " << tmpJet.mass() << std::endl;
				jetP4 = correctJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
				corrJet = correctJetReturnPatJet(tmpJet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
				if (debug) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << " mass = " << jetP4.M() << std::endl;
				_cleaned = true;
				elDaughters.erase( elDaughters.begin()+elI );
				break;
			  }
			}
		  }
		}
      }
    }

    if (!_cleaned) {
      jetP4 = correctJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
      corrJet = correctJetReturnPatJet(*_ijet, event, isMc, rhoJetsToken, isAK8, reCorrectJet);
    }

    // jet cuts //NOTE: THIS IDEALLY SHOULDN'T BE HARD CODED -- Mar 14, 2019
    while(1){

      // PF Jet ID
      if (fabs(_ijet->correctedJet(0).eta()) < 2.4 &&
	  _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
	  _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
	  //_ijet->userFloat("patPuppiJetSpecificProducer:puppiMultiplicity") > 1 &&
	  _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1 &&
	  _ijet->correctedJet(0).chargedHadronEnergyFraction() > 0 &&
	  _ijet->correctedJet(0).chargedMultiplicity() > 0
	  ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.4 &&
	       fabs(_ijet->correctedJet(0).eta()) < 2.7 &&
	       _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
	       //_ijet->userFloat("patPuppiJetSpecificProducer:puppiMultiplicity") > 1
	       _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1
	       ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.7 &&
	       fabs(_ijet->correctedJet(0).eta()) < 3.0 &&
	       _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.99
	       ){ }
      else if (fabs(_ijet->correctedJet(0).eta()) >= 3.0 &&
	       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 &&
	       _ijet->correctedJet(0).neutralHadronEnergyFraction() > 0.02 &&
	       //_ijet->userFloat("patPuppiJetSpecificProducer:neutralPuppiMultiplicity") > 2 &&
	       //_ijet->userFloat("patPuppiJetSpecificProducer:neutralPuppiMultiplicity") < 15
	       _ijet->correctedJet(0).neutralMultiplicity() > 2 &&
	       _ijet->correctedJet(0).neutralMultiplicity() < 15
	       ){ }
      else break; // fail

      if ( jetP4.Pt() > jet_minpt_AK8 ){ }
      else break; // fail


      if ( fabs(jetP4.Eta()) < jet_maxeta_AK8 ){ }
      else break; // fail


      if ( jetP4.M() >= 0 ){ }
      else{
		std::cout << mLegend << " Warning : Jet mass < 0! Skipping..." << std::endl;
		break; // fail
      }

      _pass = true;
      break;
    }

    if ( _pass ){

      // save all the good jets
      ++_n_good_jets_AK8;
      vSelCorrJets_AK8.push_back(corrJet);

    }

    ++_n_jets_AK8;

  } // end of loop over AK8 jets


}

bool MultiLepEventSelector::METSelection(edm::Event const & event)
{

	bool pass = false;

	//
	//_____ MET cuts __________________________________
	//
	//
	if (considerCut("MET")) {

	  if (debug) std::cout<<"\t" <<"MET Selection:"<< std::endl;

	  edm::Handle<std::vector<pat::MET> > mhMet;
	  event.getByToken( METtoken, mhMet );
	  pMet = edm::Ptr<pat::MET>( mhMet, 0);

	  // pfMet
	  bool passMinMET = false;
	  bool passMaxMET = false;
	  if ( pMet.isNonnull() && pMet.isAvailable() ) {
	    pat::MET const & met = mhMet->at(0);
	    TLorentzVector corrMET = correctMet(met, event);

	    if (debug) std::cout<<"\t\t" <<"MET = " << corrMET.Pt()<< std::endl;

	    if ( corrMET.Pt() > min_met ) passMinMET=true;
	    if ( corrMET.Pt() < max_met ) passMaxMET=true;

	    if (passMinMET && passMaxMET){
	      pass = true;
	      if (debug) std::cout<<"\t\t\t" <<"---> PASSES MET Selection. " << std::endl;
	    }
	  }
	}
	else{

	  if(debug)std::cout << "\t" <<"IGNORING MET selection"<< std::endl;

	  pass = true;

	}

	return pass;

}

/*
bool MultiLepEventSelector::EXAMPLESelection(edm::Event const & event)
{

	bool pass = false;

	//
    //_____ Example cuts __________________________________
    //
    if ( considerCut("example cuts") ) {

    	if(debug)std::cout << "\t" <<"ExampleSelection:"<< std::endl;

    	if ( condition ){
    		pass = true;
    	}

    }
	   else{

		if(debug)std::cout << "\t" <<"IGNORING ExampleSelection"<< std::endl;

		pass = true;

	   } // end cuts

    return pass;

}
*/


// ---------------------------------------------------------------
// ---------------------------------------------------------------
// NOTE: EVERYTHING BELOW NEEDS TO BE EITHER REORGANIZED/REWRITTEN - start
// ---------------------------------------------------------------
// ---------------------------------------------------------------

//JET CORRECTION HELPER METHODS
void MultiLepEventSelector::JECbyIOV(edm::EventBase const & event)
{
/*
 *This function takes an event, looks up the correct JEC file, and produces the correct JetCorrector for JEC corrections.
 *JEC is run number dependent.
 *This first gets the run number for the event
 *It then pulls in the corersponding spring16_V10* file
 *Then uses that.
 *
 * This is called in singleLepEventSelector
 * */

  int iRun   = event.id().run();

  if(iRun <= 299330){ JetCorrector = JetCorrector_B; JetCorrectorAK8 = JetCorrectorAK8_B;}
  else if(iRun <= 302029){ JetCorrector = JetCorrector_C; JetCorrectorAK8 = JetCorrectorAK8_C;}
  else if(iRun <= 304827){ JetCorrector = JetCorrector_DE; JetCorrectorAK8 = JetCorrectorAK8_DE;}
  else{ JetCorrector = JetCorrector_F; JetCorrectorAK8 = JetCorrectorAK8_F;}

}

TLorentzVector MultiLepEventSelector::correctJet(const pat::Jet & jet,
                                                 edm::Event const & event,
                                                 bool isMc,
                                                 edm::EDGetTokenT<double> rhoJetsToken,
                                                 bool doAK8Corr,
                                                 bool reCorrectJet,
                                                 unsigned int syst)
{

  // JES and JES systematics
  pat::Jet correctedJet;
  if (reCorrectJet) correctedJet = jet.correctedJet(0);                 //copy original jet
  else correctedJet = jet;                                 //copy default corrected jet

  double ptscale = 1.0;
  double unc = 1.0;
  double pt = correctedJet.pt();
  double correction = 1.0;

  edm::Handle<double> rhoHandle;
  event.getByToken(rhoJetsToken, rhoHandle);
  double rho = std::max(*(rhoHandle.product()), 0.0);

  if ( isMc ){

    if (reCorrectJet) {
      // We need to undo the default corrections and then apply the new ones

      double pt_raw = jet.correctedJet(0).pt();
      if (doAK8Corr){
		JetCorrectorAK8->setJetEta(jet.eta());
		JetCorrectorAK8->setJetPt(pt_raw);
		JetCorrectorAK8->setJetA(jet.jetArea());
		JetCorrectorAK8->setRho(rho);

		try{
		  correction = JetCorrectorAK8->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }
      else{
		JetCorrector->setJetEta(jet.eta());
		JetCorrector->setJetPt(pt_raw);
		JetCorrector->setJetA(jet.jetArea());
		JetCorrector->setRho(rho);

		try{
		  correction = JetCorrector->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }

      correctedJet.scaleEnergy(correction);
      pt = correctedJet.pt();

    }

    Variation JERsystematic = Variation::NOMINAL;
    if( syst==3) JERsystematic = Variation::UP;
    if( syst==4) JERsystematic = Variation::DOWN;

    JME::JetParameters parameters;
    parameters.setJetPt(pt);
    parameters.setJetEta(correctedJet.eta());
    parameters.setRho(rho);
    double res = 0.0;
    if(doAK8Corr) res = resolutionAK8.getResolution(parameters);
    else res = resolution.getResolution(parameters);
    double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

    const reco::GenJet * genJet = jet.genJet();
    bool smeared = false;
    if(genJet){
      double deltaPt = fabs(genJet->pt() - pt);
      double deltaR = reco::deltaR(genJet->p4(),correctedJet.p4());
      if (deltaR < ((doAK8Corr) ? 0.4 : 0.2) && deltaPt <= 3*pt*res){
		double gen_pt = genJet->pt();
		double reco_pt = pt;
		double deltapt = (reco_pt - gen_pt) * factor;
		ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
		smeared = true;
      }
    }
    if (!smeared && factor>0) {
      JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
      ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
    }

    if (  syst==1 || syst==2) {
      jecUnc->setJetEta(jet.eta());
      jecUnc->setJetPt(pt*ptscale);

      if ( syst==1) {
	try{
	  unc = jecUnc->getUncertainty(true);
	}
	catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
	  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	  unc = 0.0;
	}
	unc = 1 + unc;
      }
      else {
		try{
		  unc = jecUnc->getUncertainty(false);
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		  unc = 0.0;
		}
		unc = 1 - unc;
      }

      if (pt*ptscale < 10.0 && ( syst==1)) unc = 2.0;
      if (pt*ptscale < 10.0 && ( syst==2)) unc = 0.01;

    }

    correctedJet.scaleEnergy(unc*ptscale);

  }
  else if (!isMc) {

    if (reCorrectJet) {

      double pt_raw = jet.correctedJet(0).pt();
      // We need to undo the default corrections and then apply the new ones

      if (doAK8Corr){
		JetCorrectorAK8->setJetEta(jet.eta());
		JetCorrectorAK8->setJetPt(pt_raw);
		JetCorrectorAK8->setJetA(jet.jetArea());
		JetCorrectorAK8->setRho(rho);

		try{
		  correction = JetCorrectorAK8->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }

      else{
		JetCorrector->setJetEta(jet.eta());
		JetCorrector->setJetPt(pt_raw);
		JetCorrector->setJetA(jet.jetArea());
		JetCorrector->setRho(rho);

		try{
		  correction = JetCorrector->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}

      }
      correctedJet.scaleEnergy(correction);
      pt = correctedJet.pt();


    }
  }

  TLorentzVector jetP4;
  jetP4.SetPtEtaPhiM(correctedJet.pt(), correctedJet.eta(),correctedJet.phi(), correctedJet.mass() );
  //std::cout<<"jet pt: "<<jetP4.Pt()<<" eta: "<<jetP4.Eta()<<" phi: "<<jetP4.Phi()<<" energy: "<<jetP4.E()<<std::endl;


  // sanity check - save correction of the first jet
  if (mNCorrJets==0){
    double _orig_pt = jet.pt();
    if (fabs(_orig_pt)<0.000000001){
      _orig_pt = 0.000000001;
    }
    SetHistValue("jes_correction", jetP4.Pt()/_orig_pt);
    ++mNCorrJets;
  }
  //if (jetP4.Pt()>30.) std::cout<<"JEC Ratio (new/old) = "<<jetP4.Pt()/jet.pt()<<"     -->    corrected pT / eta = "<<jetP4.Pt()<<" / "<<jetP4.Eta()<<std::endl;

  return jetP4;
}

pat::Jet MultiLepEventSelector::correctJetReturnPatJet(const pat::Jet & jet,
                                                       edm::Event const & event,
                                                       bool isMc,
                                                       edm::EDGetTokenT<double> rhoJetsToken,
                                                       bool doAK8Corr,
                                                       bool reCorrectJet,
                                                       unsigned int syst)
{

  // JES and JES systematics
  pat::Jet correctedJet;
  if (reCorrectJet) correctedJet = jet.correctedJet(0);                 //copy original jet
  else correctedJet = jet;                                 //copy default corrected jet

  double ptscale = 1.0;
  double unc = 1.0;
  double pt = correctedJet.pt();
  double correction = 1.0;

  edm::Handle<double> rhoHandle;
  event.getByToken(rhoJetsToken, rhoHandle);
  double rho = std::max(*(rhoHandle.product()), 0.0);

  if ( isMc ){

    if (reCorrectJet) {
      // We need to undo the default corrections and then apply the new ones

      double pt_raw = jet.correctedJet(0).pt();

      if (doAK8Corr){
		JetCorrectorAK8->setJetEta(jet.eta());
		JetCorrectorAK8->setJetPt(pt_raw);
		JetCorrectorAK8->setJetA(jet.jetArea());
		JetCorrectorAK8->setRho(rho);

		try{
		  correction = JetCorrectorAK8->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }

      else{
		JetCorrector->setJetEta(jet.eta());
		JetCorrector->setJetPt(pt_raw);
		JetCorrector->setJetA(jet.jetArea());
		JetCorrector->setRho(rho);

		try{
		  correction = JetCorrector->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }

      correctedJet.scaleEnergy(correction);
      pt = correctedJet.pt();

    }

    Variation JERsystematic = Variation::NOMINAL;
    if( syst==3) JERsystematic = Variation::UP;
    if( syst==4) JERsystematic = Variation::DOWN;

    JME::JetParameters parameters;
    parameters.setJetPt(pt);
    parameters.setJetEta(correctedJet.eta());
    parameters.setRho(rho);
    double res = 0.0;
    if(doAK8Corr) res = resolutionAK8.getResolution(parameters);
    else res = resolution.getResolution(parameters);
    double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

    const reco::GenJet * genJet = jet.genJet();
    bool smeared = false;
    if(genJet){
      double deltaPt = fabs(genJet->pt() - pt);
      double deltaR = reco::deltaR(genJet->p4(),correctedJet.p4());
      if (deltaR < ((doAK8Corr) ? 0.4 : 0.2) && deltaPt <= 3*pt*res){
		double gen_pt = genJet->pt();
		double reco_pt = pt;
		double deltapt = (reco_pt - gen_pt) * factor;
		ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
		smeared = true;
      }
    }
    if (!smeared && factor>0) {
      JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
      ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
    }

    if (  syst==1 || syst==2) {
      jecUnc->setJetEta(jet.eta());
      jecUnc->setJetPt(pt*ptscale);

      if ( syst==1) {
	try{
	  unc = jecUnc->getUncertainty(true);
	}
	catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
	  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	  unc = 0.0;
	}
	unc = 1 + unc;
      }
      else {
	try{
	  unc = jecUnc->getUncertainty(false);
	}
	catch(...){
	  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	  unc = 0.0;
	}
	unc = 1 - unc;
      }

      if (pt*ptscale < 10.0 && ( syst==1)) unc = 2.0;
      if (pt*ptscale < 10.0 && ( syst==2)) unc = 0.01;

    }

    correctedJet.scaleEnergy(unc*ptscale);

  }
  else if (!isMc) {

    if (reCorrectJet) {

      double pt_raw = jet.correctedJet(0).pt();
      // We need to undo the default corrections and then apply the new ones

      if (doAK8Corr){
		JetCorrectorAK8->setJetEta(jet.eta());
		JetCorrectorAK8->setJetPt(pt_raw);
		JetCorrectorAK8->setJetA(jet.jetArea());
		JetCorrectorAK8->setRho(rho);

		try{
		  correction = JetCorrectorAK8->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }

      else{
		JetCorrector->setJetEta(jet.eta());
		JetCorrector->setJetPt(pt_raw);
		JetCorrector->setJetA(jet.jetArea());
		JetCorrector->setRho(rho);

		try{
		  correction = JetCorrector->getCorrection();
		}
		catch(...){
		  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		}
      }
      correctedJet.scaleEnergy(correction);
      pt = correctedJet.pt();

    }
  }

  return correctedJet;
}

TLorentzVector MultiLepEventSelector::correctMet(const pat::MET & met, edm::Event const & event, unsigned int syst, bool useHF)
{
    double correctedMET_px = met.uncorPx();
    double correctedMET_py = met.uncorPy();
    if ( doNewJEC ) {
        for (std::vector<edm::Ptr<pat::Jet> >::const_iterator ijet = vAllJets.begin();
             ijet != vAllJets.end(); ++ijet) {
            if (!useHF && fabs((**ijet).eta())>2.6) continue;
            TLorentzVector lv = correctJetForMet(**ijet, event, syst);
            correctedMET_px += lv.Px();
            correctedMET_py += lv.Py();
        }
    }
    else {
        correctedMET_px = met.px();
        correctedMET_py = met.py();
    }

    correctedMET_p4.SetPxPyPzE(correctedMET_px, correctedMET_py, 0, sqrt(correctedMET_px*correctedMET_px+correctedMET_py*correctedMET_py));

    // sanity check histogram
    double _orig_met = met.pt();
    if (fabs(_orig_met) < 1.e-9) {
        _orig_met = 1.e-9;
    }
    SetHistValue("met_correction", correctedMET_p4.Pt()/_orig_met);

    return correctedMET_p4;
}

TLorentzVector MultiLepEventSelector::correctJetForMet(const pat::Jet & jet, edm::Event const & event, unsigned int syst)
{

    TLorentzVector jetP4, offJetP4;
    jetP4.SetPtEtaPhiM(0.000001,1.,1.,0.000001);

    if ( jet.chargedEmEnergyFraction() + jet.neutralEmEnergyFraction() > 0.90 ) {
        return jetP4-jetP4;
    }

    pat::Jet correctedJet = jet.correctedJet(0);                 //copy original jet

    jetP4.SetPtEtaPhiM(correctedJet.pt(),correctedJet.eta(),correctedJet.phi(),correctedJet.mass());

    const std::vector<reco::CandidatePtr> & cands = jet.daughterPtrVector();
    for ( std::vector<reco::CandidatePtr>::const_iterator cand = cands.begin();
        cand != cands.end(); ++cand ) {
        const reco::PFCandidate *pfcand = dynamic_cast<const reco::PFCandidate *>(cand->get());
        const reco::Candidate *mu = (pfcand != 0 ? ( pfcand->muonRef().isNonnull() ? pfcand->muonRef().get() : 0) : cand->get());
        if ( mu != 0 && (mu->isGlobalMuon() || mu->isStandAloneMuon()) ) {
	    TLorentzVector muonP4;
        muonP4.SetPtEtaPhiM((*cand)->pt(),(*cand)->eta(),(*cand)->phi(),(*cand)->mass());
	    jetP4 -= muonP4;
        }
    }
    offJetP4 = jetP4;

    double ptscale = 1.0;
    double unc = 1.0;
    double pt = correctedJet.pt();
    std::vector<float> corrVec;

    edm::Handle<double> rhoHandle;
    event.getByToken(rhoJetsToken, rhoHandle);
    double rho = std::max(*(rhoHandle.product()), 0.0);

    if ( isMc ){

        JetCorrector->setJetEta(correctedJet.eta());
  		JetCorrector->setJetPt(pt);
        JetCorrector->setJetA(jet.jetArea());
  		JetCorrector->setRho(rho);

        try{
    	    corrVec = JetCorrector->getSubCorrections();
        }
  		catch(...){
    	    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    	    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    	    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
        }

        jetP4 *= corrVec[corrVec.size()-1];
        offJetP4 *= corrVec[0];
        pt = jetP4.Pt();

        Variation JERsystematic = Variation::NOMINAL;
        if(JERup || syst==3) JERsystematic = Variation::UP;
        if(JERdown || syst==4) JERsystematic = Variation::DOWN;

	JME::JetParameters parameters;
	parameters.setJetPt(pt);
	parameters.setJetEta(jetP4.Eta());
	parameters.setRho(rho);
	double res = 0.0;
	res = resolution.getResolution(parameters);
	double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

        const reco::GenJet * genJet = jet.genJet();
        bool smeared = false;
	if(genJet){
	  TLorentzVector genP4;
	  genP4.SetPtEtaPhiE(genJet->pt(),genJet->eta(),genJet->phi(),genJet->energy());
	  double deltaPt = fabs(genJet->pt() - pt);
	  double deltaR = jetP4.DeltaR(genP4);
	  if (deltaR < 0.2 && deltaPt <= 3*pt*res){
	    double gen_pt = genJet->pt();
	    double reco_pt = pt;
	    double deltapt = (reco_pt - gen_pt) * factor;
	    ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
	    smeared = true;
	  }
	}
        if (!smeared && factor>0) {
          JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
          ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
        }

        if ( JECup || JECdown || syst==1 || syst==2) {
            jecUnc->setJetEta(jetP4.Eta());
            jecUnc->setJetPt(jetP4.Pt()*ptscale);

            if (JECup || syst==1) {
	      try{
		unc = jecUnc->getUncertainty(true);
	      }
	      catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
		std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		unc = 0.0;
	      }
	      unc = 1 + unc;
            }
            else {
	      try{
		unc = jecUnc->getUncertainty(false);
	      }
	      catch(...){
		std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
		std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
		std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
		unc = 0.0;
	      }
	      unc = 1 - unc;
            }

            if (jetP4.Pt()*ptscale < 10.0 && (JECup || syst==1)) unc = 2.0;
            if (jetP4.Pt()*ptscale < 10.0 && (JECdown || syst==2)) unc = 0.01;

        }

    }
    else if (!isMc) {

        JetCorrector->setJetEta(correctedJet.eta());
	JetCorrector->setJetPt(pt);
        JetCorrector->setJetA(jet.jetArea());
	JetCorrector->setRho(rho);

        try{
    	    corrVec = JetCorrector->getSubCorrections();
        }
	catch(...){
	  std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	  std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	  std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
        }


        jetP4 *= corrVec[corrVec.size()-1];
        offJetP4 *= corrVec[0];
        pt = jetP4.Pt();

    }

    jetP4 *= unc*ptscale;
    offJetP4 *= unc*ptscale;
    if (jetP4.Pt()<=15.) {
        offJetP4 = jetP4;
    }

    return offJetP4-jetP4;
}


//BTAG HELPER METHOD
bool MultiLepEventSelector::isJetTagged(const pat::Jet & jet,
                                        TLorentzVector correctedJet_lv,
                                        edm::Event const & event,
                                        bool isMc,
                                        bool applySF,
                                        int shiftflag,
                                        bool subjetflag)
{
    bool _isTagged = false;

    if (jet.bDiscriminator("pfDeepCSVJetTags:probb")+jet.bDiscriminator("pfDeepCSVJetTags:probbb") > bTagCut) _isTagged = true;

    if (isMc && applySF){

      //TLorentzVector lvjet = correctJet(jet, event);
      TLorentzVector lvjet = correctedJet_lv;

      int _jetFlavor = abs(jet.hadronFlavour());
      double _heavySf = 1.0;
      double _heavyEff = 1.0;
      double _lightSf = 1.0;
      double _lightEff = 1.0;

      if(!subjetflag){

		_heavySf = reader.eval_auto_bounds("central",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
		if (shiftflag == 1 ||  BTagUncertUp ) _heavySf = reader.eval_auto_bounds("up",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
		else if (shiftflag == 2 ||  BTagUncertDown ) _heavySf = reader.eval_auto_bounds("down",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
		_heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+btagOP);

		if(_jetFlavor == 4){
		  _heavySf = reader.eval_auto_bounds("central",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
		  if (shiftflag == 1 ||  BTagUncertUp ) _heavySf = reader.eval_auto_bounds("up",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
		  else if (shiftflag == 2 ||  BTagUncertDown ) _heavySf = reader.eval_auto_bounds("down",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
		  _heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+btagOP);
		}

		_lightSf = reader.eval_auto_bounds("central",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
		if (shiftflag == 3 || MistagUncertUp ) _lightSf = reader.eval_auto_bounds("up",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
		else if (shiftflag == 4 ||  MistagUncertDown ) _lightSf = reader.eval_auto_bounds("down",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
		_lightEff = mBtagCond.GetMistagRate(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+btagOP);

      }
      else{

      	_heavySf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	if (shiftflag == 1 ||  BTagUncertUp ) _heavySf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	else if (shiftflag == 2 ||  BTagUncertDown ) _heavySf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	_heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+btagOP);

      	if(_jetFlavor == 4){
      	  _heavySf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  if (shiftflag == 1 ||  BTagUncertUp ) _heavySf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  else if (shiftflag == 2 ||  BTagUncertDown ) _heavySf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  _heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+btagOP);
      	}

      	_lightSf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	if (shiftflag == 3 || MistagUncertUp ) _lightSf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	else if (shiftflag == 4 ||  MistagUncertDown ) _lightSf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	_lightEff = mBtagCond.GetMistagRate(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+btagOP);
      }

      mBtagSfUtil.SetSeed(abs(static_cast<int>(sin(jet.phi())*1e5)));

      // sanity check
      bool _orig_tag = _isTagged;
      mBtagSfUtil.modifyBTagsWithSF(_isTagged, _jetFlavor, _heavySf, _heavyEff, _lightSf, _lightEff);
      if (_isTagged != _orig_tag) ++mNBtagSfCorrJets;

    } // end of btag scale factor corrections

    return _isTagged;
}


// ---------------------------------------------------------------
// ---------------------------------------------------------------
// NOTE: EVERYTHING ABOVE NEEDS TO BE EITHER REORGANIZED/REWRITTEN - end
// ---------------------------------------------------------------
// ---------------------------------------------------------------
