#include "FWLJMET/LJMet/interface/MultiLepEventSelector.h"

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

    //nEvents info
    if(isMc) genToken            = iC.consumes<GenEventInfoProduct>(edm::InputTag("generator"));

    //Trigger
    triggersToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    trigger_cut         = selectorConfig.getParameter<bool>("trigger_cut");
    dump_trigger        = selectorConfig.getParameter<bool>("dump_trigger");
    mctrigger_path_el   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_el");
    mctrigger_path_mu   = selectorConfig.getParameter<std::vector<std::string>>("mctrigger_path_mu");
    trigger_path_el     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_el");;
    trigger_path_mu     = selectorConfig.getParameter<std::vector<std::string>>("trigger_path_mu");;

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
    jetsToken                = iC.consumes<pat::JetCollection>(selectorConfig.getParameter<edm::InputTag>("jet_collection"));
    AK8jetsToken             = iC.consumes<pat::JetCollection>(selectorConfig.getParameter<edm::InputTag>("AK8jet_collection"));
    JECup                    = selectorConfig.getParameter<bool>("JECup");
    JECdown                  = selectorConfig.getParameter<bool>("JECdown");
    JERup                    = selectorConfig.getParameter<bool>("JERup");
    JERdown                  = selectorConfig.getParameter<bool>("JERdown");
    doNewJEC                 = selectorConfig.getParameter<bool>("doNewJEC");
    doAllJetSyst             = selectorConfig.getParameter<bool>("doAllJetSyst");
    doLepJetCleaning         = selectorConfig.getParameter<bool>("doLepJetCleaning");
    CleanLooseLeptons        = selectorConfig.getParameter<bool>("CleanLooseLeptons");
    LepJetDR                 = selectorConfig.getParameter<double>("LepJetDR");
    LepJetDRAK8              = selectorConfig.getParameter<double>("LepJetDRAK8");
    jet_cuts                 = selectorConfig.getParameter<bool>("jet_cuts");
    jet_minpt                = selectorConfig.getParameter<double>("jet_minpt");
    jet_maxeta               = selectorConfig.getParameter<double>("jet_maxeta");
    jet_minpt_AK8            = selectorConfig.getParameter<double>("jet_minpt_AK8");
    jet_maxeta_AK8           = selectorConfig.getParameter<double>("jet_maxeta_AK8");
    min_jet                  = selectorConfig.getParameter<int>("min_jet");
    max_jet                  = selectorConfig.getParameter<int>("max_jet");
    leading_jet_pt           = selectorConfig.getParameter<double>("leading_jet_pt");
    //JET CORRECTION  initialization
    JetMETCorr.Initialize(selectorConfig); // REMINDER: THIS NEEDS --if(!isMc)JetMETCorr.SetFacJetCorr(event)-- somewhere before correcting jets for data, since data JEC is era dependent. !!

    //BTAG
    btag_cuts          = selectorConfig.getParameter<bool>("btag_cuts");  // this is currently not used anywhere but could be useful in the future. -- Mar 19, 2019.
    //BTAG parameter initialization
    btagSfUtil.Initialize(selectorConfig);


    //MET
    METtoken           = iC.consumes<std::vector<pat::MET> >(selectorConfig.getParameter<edm::InputTag>("met_collection"));
    met_cuts           = selectorConfig.getParameter<bool>("met_cuts");
    min_met            = selectorConfig.getParameter<double>("min_met");
    max_met            = selectorConfig.getParameter<double>("max_met");

    //Misc
    PFCandToken          = iC.consumes<pat::PackedCandidateCollection>(selectorConfig.getParameter<edm::InputTag>("PFparticlesCollection"));
    rhoJetsNC_Token      = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsNCInputTag"));
    rhoJetsToken         = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsInputTag"));


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

    //Record cut flow information - will be saved under folder named after the selector name.
    SetHistogram( "Trigger", 2, 0,2);
    SetHistogram("Primary Vertex", 2, 0,2);
    SetHistogram("MET filters", 2, 0,2);
    SetHistogram("Lepton Selection", 2, 0,2); // keeping it simple for now
    if(jet_cuts){
		SetHistogram("Jet Selection", 2, 0,2); // keeping it simple for now
    }
    SetHistogram("MET", 2, 0,2);
    SetHistogram("All cuts", 2, 0,2);




} // end of BeginJob()

bool MultiLepEventSelector::operator()( edm::Event const & event, pat::strbitset & ret) // EVENT SELECTOR!
{
  if(debug)std::cout << "Processing Event in MultiLepEventSelector::operator()" << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;
  if(debug)std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;

  //Save events before selections and MC negative weights. Histo is intialized in BaseEventSelector.h . This could also be written in BaseEventSelector.cc.
  int theWeight = 1;
  if(isMc){
      edm::Handle<GenEventInfoProduct> genEvtInfo;
      event.getByToken(genToken, genEvtInfo );
      theWeight = genEvtInfo->weight()/fabs(genEvtInfo->weight());
   }
  FillHist("nEvents", theWeight);

  while(1){ // standard infinite while loop trick to avoid nested ifs

    passCut(ret, "No selection");

    if( ! TriggerSelection(event) ) break;
    passCut(ret, "Trigger");
    FillHist("Trigger", 1);

    if( ! PVSelection(event) ) break;
    passCut(ret, "Primary Vertex");
    FillHist("Primary Vertex", 1);

    if( ! METfilter(event) ) break;
    passCut(ret, "MET filters");
    FillHist("MET filters", 1);

    //Collect selected leptons
    MuonSelection(event);
    ElectronSelection(event);

    if( ! LeptonsSelection(event, ret) ) break;
    FillHist("Lepton Selection", 1); // keeping it simple for now

    //Collect jets
    if( ! JetSelection(event, ret) ) break;
    FillHist("Jet Selection", 1); // keeping it simple for now

    //Collect AK8 jets
    AK8JetSelection(event);

    if( ! METSelection(event) ) break;
    passCut(ret, "MET");
    FillHist("MET", 1);


    passCut(ret, "All cuts");
    FillHist("All cuts", 1);
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
			mvSelMCTriggersEl[mctrigger_path_el.at(ipath)] = 0;
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if (trigName.find(mctrigger_path_el.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))) {
					passTrigEl = 1;
					mvSelMCTriggersEl[mctrigger_path_el.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
			}
		}
		if (passTrigEl>0) passTrigElMC = true;

		int passTrigMu = 0;
		if (debug) std::cout<< "\t" <<"	In MC Mu trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < mctrigger_path_mu.size() && mctrigger_path_mu.at(0)!="" ; ipath++){
			mvSelMCTriggersMu[mctrigger_path_mu.at(ipath)] = 0;
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if ( trigName.find(mctrigger_path_mu.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
					passTrigMu = 1;
					mvSelMCTriggersMu[mctrigger_path_mu.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
			}
		}
		if (passTrigMu>0) passTrigMuMC = true;

		//Loop over each data channel separately
		passTrigEl = 0;
		if (debug) std::cout<< "\t" <<"	In Data El trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < trigger_path_el.size() && trigger_path_el.at(0)!="" ; ipath++){
			mvSelTriggersEl[trigger_path_el.at(ipath)] = 0;
			for(unsigned int i=0; i<_tSize; i++){
				std::string trigName = trigNames.triggerName(i);
				if ( trigName.find(trigger_path_el.at(ipath)) == std::string::npos) continue;
				if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
					passTrigEl = 1;
					mvSelTriggersEl[trigger_path_el.at(ipath)] = 1;
					if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
				}
			}
		}
		if (passTrigEl>0) passTrigElData = true;

		passTrigMu = 0;
		if (debug) std::cout<< "\t" <<"	In Data Mu trig list: "<<std::endl;
		for (unsigned int ipath = 0; ipath < trigger_path_mu.size() && trigger_path_mu.at(0)!="" ; ipath++){
			mvSelTriggersMu[trigger_path_mu.at(ipath)] = 0;
			for(unsigned int i=0; i<_tSize; i++){
			std::string trigName = trigNames.triggerName(i);
			if ( trigName.find(trigger_path_mu.at(ipath)) == std::string::npos) continue;
			if (triggersHandle->accept(trigNames.triggerIndex(trigName))){
				passTrigMu = 1;
				mvSelTriggersMu[trigger_path_mu.at(ipath)] = 1;
				if (debug) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
			}
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
	    //	    if (patTrigNames.triggerName(i) == "Flag_BadChargedCandidateFilter") badchargedcandpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
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
	      //badchargedcandpass &&  // now not recommended due to high pT jet inefficiency
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
		  else{
		  	_n_muons++;
		  	continue;
		  } //skip to next muon if fail loose

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
		  else{ 
		  	_n_electrons++;
		  	continue;
		  } //skip to next el if fail loose


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
		else{
			return false;
		}
	}

	if(considerCut("Max Loose Leptons")){
		if ( nLooseLeps <= (unsigned int)maxLooseLeptons){
			pass_maxLooseLeptons = true;
			passCut(ret, "Max Loose Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_maxLooseLeptons"<<std::endl;
		}
		else{
			return false;
		}
	}


	if(considerCut("Min Leptons")){
		if ( nLeps >= (unsigned int)minLeptons){
			pass_minLeptons = true;
			passCut(ret, "Min Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_minLeptons"<<std::endl;
		}
		else{
			return false;
		}
	}

	if(considerCut("Max Leptons")){
		if ( nLeps <= (unsigned int)maxLeptons){
			pass_maxLeptons = true;
			passCut(ret, "Max Leptons");
			if(debug)std::cout << "\t\t\t" << "pass_maxLeptons"<<std::endl;
		}
		else{
			return false;
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


  if(!isMc) JetMETCorr.SetFacJetCorr(event);


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
				jetP4 = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJetReturnPatJet METHOD, CAN CALCULATE P4 FROM OUTPUT OF correctJetReturnPatJet RATHER THAN DOING EXACTLY EVERYTHING correctJetReturnPatJet IS DOING. FIX!!!! -- MAR 15,2019
/*				if (mbPar["doAllJetSyst"]) {
				  jetP4_jesup = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,1);
				  jetP4_jesdn = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,2);
				  jetP4_jerup = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,3);
				  jetP4_jerdn = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,4);
				}*/
				corrJet = JetMETCorr.correctJetReturnPatJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJet METHOD ! FIX !!!
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
				jetP4 = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJetReturnPatJet METHOD, CAN CALCULATE P4 FROM OUTPUT OF correctJetReturnPatJet RATHER THAN DOING EXACTLY EVERYTHING correctJetReturnPatJet IS DOING. FIX!!!! -- MAR 15,2019
/*				if (mbPar["doAllJetSyst"]) {
				  jetP4_jesup = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,1);
				  jetP4_jesdn = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,2);
				  jetP4_jerup = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,3);
				  jetP4_jerdn = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet,4);
				}*/
				corrJet = JetMETCorr.correctJetReturnPatJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet, syst); //THIS SEEMS REDUNDANT WITH correctJet METHOD ! FIX!!!! -- MAR 15,2019
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
      jetP4 = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet, syst);
/*      if (mbPar["doAllJetSyst"]) {
		jetP4_jesup = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet,1);
		jetP4_jesdn = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet,2);
		jetP4_jerup = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet,3);
		jetP4_jerdn = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet,4);
      }*/
      corrJet = JetMETCorr.correctJetReturnPatJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet, syst);
    }

    _isTagged = btagSfUtil.isJetTagged(*_ijet, jetP4, event, isMc);

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
				jetP4 = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet);
				corrJet = JetMETCorr.correctJetReturnPatJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet);
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
				jetP4 = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet);
				corrJet = JetMETCorr.correctJetReturnPatJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet);
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
      jetP4 = JetMETCorr.correctJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet);
      corrJet = JetMETCorr.correctJetReturnPatJet(*_ijet, event, rhoJetsToken, isAK8, reCorrectJet);
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

	//for jet correction
	bool reCorrectJet = doNewJEC;
	unsigned int syst;
	if (JECup){syst=1;}
	else if (JECdown){syst=2;}
	else if (JERup){syst=3;}
	else if (JERdown){syst=4;}
	else syst = 0; //nominal


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
	    TLorentzVector corrMET = JetMETCorr.correctMet(met,event,rhoJetsToken,vAllJets,reCorrectJet,syst);

	    //save to EventSelector object variable.
	    correctedMET_p4 = corrMET;

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
