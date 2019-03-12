#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "FWLJMET/LJMet/interface/MiniIsolation.h"


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

    // containers for config parameter values
    //std::map<std::string,std::string>    msPar;

    bool bFirstEntry;
    bool debug;

    bool isMc;

    bool trigger_cut;
    bool dump_trigger;
    std::vector<std::string> mctrigger_path_el;
    std::vector<std::string> mctrigger_path_mu;
    std::vector<std::string> trigger_path_el;
    std::vector<std::string> trigger_path_mu;

    bool pv_cut;
    boost::shared_ptr<PVSelector>              pvSel_;

    bool metfilters;

    bool met_cuts;
    double min_met;
    double max_met;
    
    bool   muon_cuts;
    int    min_muon;

    double muon_minpt;
    double muon_maxeta;
    bool   muon_useMiniIso;
    double muon_miniIso;
    double muon_dxy;
    double muon_dz;
    double muon_relIso;
    double loose_muon_miniIso;
    double loose_muon_minpt;
    double loose_muon_maxeta;
    double loose_muon_dxy;
    double loose_muon_dz;
    double loose_muon_relIso;

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



    edm::EDGetTokenT<edm::TriggerResults>            triggersToken;
    edm::EDGetTokenT<reco::VertexCollection>         PVToken;
    edm::EDGetTokenT<edm::TriggerResults>            METfilterToken;
    edm::EDGetTokenT<bool>                           METfilterToken_extra;
    edm::EDGetTokenT<std::vector<pat::MET> >         METtoken;
    edm::EDGetTokenT<pat::MuonCollection>            muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>        electronsToken;
    edm::EDGetTokenT<pat::PackedCandidateCollection> PFCandToken;
    edm::EDGetTokenT<double>                         rhoJetsNC_Token;

    int       minLeptons;
    float     min_muPt;
    float     max_muEta;
    float     min_elPt;
    float     max_elEta;

    bool TriggerSelection  (edm::Event const & event);
    bool PVSelection       (edm::Event const & event);
    bool METfilter         (edm::Event const & event);
    int  MuonSelection     (edm::Event const & event);
    int  ElectronSelection (edm::Event const & event);
    bool LeptonsSelection  (edm::Event const & event);
    bool METSelection      (edm::Event const & event);


private:


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
    const edm::ParameterSet& selectorConfig = iConfig.getParameterSet("MultiLepSelector") ;

    debug               = selectorConfig.getParameter<bool>("debug");

    isMc                = selectorConfig.getParameter<bool>("isMc");

    trigger_cut         = selectorConfig.getParameter<bool>("trigger_cut");
    dump_trigger        = selectorConfig.getParameter<bool>("dump_trigger");
    mctrigger_path_el   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_el");
    mctrigger_path_mu   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_mu");
    trigger_path_el     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_el");
    trigger_path_mu     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_mu");

    pv_cut         = selectorConfig.getParameter<bool>("pv_cut");
    const edm::ParameterSet& PVconfig = selectorConfig.getParameterSet("pvSelector") ;
    pvSel_ = boost::shared_ptr<PVSelector>( new PVSelector(PVconfig) );

    metfilters         = selectorConfig.getParameter<bool>("metfilters");

    muon_cuts                = selectorConfig.getParameter<bool>("muon_cuts");
    min_muon                 = selectorConfig.getParameter<int>("min_muon");
    muon_minpt               = selectorConfig.getParameter<double>("muon_minpt");
    muon_maxeta              = selectorConfig.getParameter<double>("muon_maxeta");
    muon_useMiniIso          = selectorConfig.getParameter<bool>("muon_useMiniIso");
    muon_miniIso             = selectorConfig.getParameter<double>("muon_miniIso");
    loose_muon_miniIso       = selectorConfig.getParameter<double>("loose_muon_miniIso");
    loose_muon_minpt         = selectorConfig.getParameter<double>("loose_muon_minpt");
    loose_muon_maxeta        = selectorConfig.getParameter<double>("loose_muon_maxeta");
    muon_dxy                 = selectorConfig.getParameter<double>("muon_dxy");
    muon_dz                  = selectorConfig.getParameter<double>("muon_dz");
    loose_muon_dxy           = selectorConfig.getParameter<double>("loose_muon_dxy");
    loose_muon_dz            = selectorConfig.getParameter<double>("loose_muon_dz");

    muon_relIso              = selectorConfig.getParameter<double>("muon_relIso");
    loose_muon_relIso        = selectorConfig.getParameter<double>("loose_muon_relIso");


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


    met_cuts           = selectorConfig.getParameter<bool>("met_cuts");
    min_met            = selectorConfig.getParameter<double>("min_met");
    max_met            = selectorConfig.getParameter<double>("max_met");


    triggersToken        = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    PVToken              = iC.consumes<reco::VertexCollection>(PVconfig.getParameter<edm::InputTag>("pvSrc"));
    METfilterToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("flag_tag"));
    METfilterToken_extra = iC.consumes<bool>(selectorConfig.getParameter<edm::InputTag>("METfilter_extra"));
    METtoken             = iC.consumes<std::vector<pat::MET> >(selectorConfig.getParameter<edm::InputTag>("met_collection"));
    muonsToken           = iC.consumes<pat::MuonCollection>(selectorConfig.getParameter<edm::InputTag>("muonsCollection"));;
    electronsToken       = iC.consumes<pat::ElectronCollection>(selectorConfig.getParameter<edm::InputTag>("electronsCollection"));
    PFCandToken          = iC.consumes<pat::PackedCandidateCollection>(selectorConfig.getParameter<edm::InputTag>("PFparticlesCollection"));
    rhoJetsNC_Token      = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoInputTag"));

    minLeptons = selectorConfig.getParameter<int>("minLeptons");
    min_muPt   = selectorConfig.getParameter<double>("min_muPt");
    max_muEta  = selectorConfig.getParameter<double>("max_muEta");;
    min_elPt  = selectorConfig.getParameter<double>("min_elPt");
    max_elEta  = selectorConfig.getParameter<double>("max_elEta");

    bFirstEntry = true; //in case anything needs a first entry bool.

    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");
    push_back("Trigger");
    push_back("Primary Vertex");
    push_back("MET filters");
    push_back("Leptons");
    push_back("MET");
    push_back("All cuts");

    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Trigger",trigger_cut);
    set("Primary Vertex",pv_cut);
    set("MET filters", metfilters);
    set("Leptons",true);
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

    int totalSelLooseMuon     = MuonSelection(event);
    int totalSelLooseElectron = ElectronSelection(event);
    if( LeptonsSelection(event) ) passCut(ret, "Leptons");
    else break;

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
// 			TLorentzVector corrMET = correctMet(met, event);
//
//         	if (debug) std::cout<<"\t\t" <<"MET = " << corrMET.Pt()<< std::endl;
//
// 			if ( corrMET.Pt() > min_met ) passMinMET=true;
// 			if ( corrMET.Pt() < max_met ) passMaxMET=false;

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


int MultiLepEventSelector::MuonSelection(edm::Event const & event)
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


	return nSelLooseMuons;


}

int MultiLepEventSelector::ElectronSelection(edm::Event const & event)
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

	return nSelLooseElectrons;

}



bool MultiLepEventSelector::LeptonsSelection(edm::Event const & event)
{
	bool pass = false;

	if(debug)std::cout << "\t" <<"LeptonsSelection:"<< std::endl;

	//Muons
	edm::Handle< pat::MuonCollection > muonsHandle;
	event.getByToken(muonsToken, muonsHandle);
	unsigned int iMu = 0; // index in input dataset
	unsigned int nSelMu = 0; //num of selected muons
	vSelMuons.clear();
	for (std::vector<pat::Muon>::const_iterator _imu = muonsHandle->begin(); _imu != muonsHandle->end(); _imu++){
		iMu = _imu - muonsHandle->begin();

		if(debug) std::cout << std::endl;
		if(debug) std::cout << "\t\t" << "iMu = "<< iMu << " pt = " << _imu->pt() << " eta = " << _imu->eta();

		if(_imu->pt() < min_muPt) continue;
		if(fabs(_imu->eta()) > max_muEta) continue;

		vSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, iMu) );
		nSelMu++;

		if(debug) std::cout << " ---> " << "Pass";
	}

	//Electrons
	edm::Handle< pat::ElectronCollection > electronsHandle;
	event.getByToken(electronsToken, electronsHandle);
	unsigned int iEl = 0; // index in input dataset
	unsigned int nSelEl = 0; //num of selected muons
	vSelElectrons.clear();
	for (std::vector<pat::Electron>::const_iterator _iel = electronsHandle->begin(); _iel != electronsHandle->end(); _iel++){
		iEl = _iel - electronsHandle->begin();

		if(debug) std::cout << std::endl;
		if(debug) std::cout << "\t\t" << "iEl = "<< iEl << " pt = " << _iel->pt() << " eta = " << _iel->eta();
		if(_iel->pt() < min_elPt) continue;
		if(fabs(_iel->eta()) > max_elEta) continue;

		vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, iEl) );
		nSelEl++;

		if(debug) std::cout << " ---> " << "Pass";
	}

	//Check for min selected lepton requirement
	if(debug) std::cout << std::endl;
	if(debug)std::cout << "\t" << "\t" << "nSelMu = "<< nSelMu << " nSelEl = "<< nSelEl << std::endl;
	if ( (nSelMu + nSelEl) >= (unsigned int)minLeptons){
		pass = true;
		if(debug)std::cout << "\t" << "\t" << "Has "<< minLeptons << " leptons that passes selection ! "<< std::endl;
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
