#include "FWLJMET/LJMet/interface/DileptonEventSelector.h"


DileptonEventSelector::DileptonEventSelector(){
}


DileptonEventSelector::~DileptonEventSelector(){
}


void DileptonEventSelector::BeginJob( const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC){


    std::cout << mLegend << "initializing DileptonEventSelector selection" << std::endl;

    std::string thisSelectorName = GetName();
    const edm::ParameterSet& selectorConfig = iConfig.getParameterSet( thisSelectorName ) ;

    debug               = selectorConfig.getParameter<bool>         ("debug");
    isMc                = selectorConfig.getParameter<bool>("isMc");
    bFirstEntry         = true; //in case anything needs a first entry bool.

    //Triggers
    triggersToken            = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    trigger_cut              = selectorConfig.getParameter<bool>         ("trigger_cut");
    dump_trigger             = selectorConfig.getParameter<bool>         ("dump_trigger");
    trigger_path_ee          = selectorConfig.getParameter<std::vector<std::string> >  ("trigger_path_ee");
    trigger_path_em          = selectorConfig.getParameter<std::vector<std::string> >  ("trigger_path_em");
    trigger_path_mm          = selectorConfig.getParameter<std::vector<std::string> >  ("trigger_path_mm");


    //PV
    const edm::ParameterSet& PVconfig = selectorConfig.getParameterSet("pvSelector") ;
    PVToken                  = iC.consumes<reco::VertexCollection>(PVconfig.getParameter<edm::InputTag>("pvSrc"));
    pv_cut                   = selectorConfig.getParameter<bool>         ("pv_cut");
    pvSel_                   = boost::shared_ptr<PVSelector>( new PVSelector(PVconfig) );

    //MET filter
    METfilterToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("flag_tag"));
    METfilterToken_extra = iC.consumes<bool>(selectorConfig.getParameter<edm::InputTag>("METfilter_extra"));
    metfilters           = selectorConfig.getParameter<bool>("metfilters");

    //Muon
    muonsToken               = iC.consumes<pat::MuonCollection>(selectorConfig.getParameter<edm::InputTag>("muonsCollection"));
    muon_cuts                = selectorConfig.getParameter<bool>         ("muon_cuts");
    min_muon                 = selectorConfig.getParameter<int>          ("min_muon");
    max_muon                 = selectorConfig.getParameter<int>          ("max_muon");
    muon_minpt               = selectorConfig.getParameter<double>       ("muon_minpt");
    muon_maxeta              = selectorConfig.getParameter<double>       ("muon_maxeta");

    //Electron
    electronsToken           = iC.consumes<pat::ElectronCollection>(selectorConfig.getParameter<edm::InputTag>("electronsCollection"));
    electron_cuts            = selectorConfig.getParameter<bool>         ("electron_cuts");
    min_electron             = selectorConfig.getParameter<int>          ("min_electron");
    max_electron             = selectorConfig.getParameter<int>          ("max_electron");
    electron_minpt           = selectorConfig.getParameter<double>       ("electron_minpt");
    electron_maxeta          = selectorConfig.getParameter<double>       ("electron_maxeta");
    UseElMVA                 = selectorConfig.getParameter<bool>         ("UseElMVA");
    
    //nLepton
    min_lepton               = selectorConfig.getParameter<int>          ("min_lepton");

    //Jets
    jetsToken                = iC.consumes<pat::JetCollection>(selectorConfig.getParameter<edm::InputTag>("jet_collection"));
    const edm::ParameterSet& PFJetIFconfig = selectorConfig.getParameterSet("pfJetIDSelector") ;
    jetSel_                                = boost::shared_ptr<PFJetIDSelectionFunctor>( new PFJetIDSelectionFunctor(PFJetIFconfig) );
    jet_cuts                 = selectorConfig.getParameter<bool>("jet_cuts");
    jet_minpt                = selectorConfig.getParameter<double>("jet_minpt");
    jet_maxeta               = selectorConfig.getParameter<double>("jet_maxeta");
    min_jet                  = selectorConfig.getParameter<int>("min_jet");
    max_jet                  = selectorConfig.getParameter<int>("max_jet");
    JECup                    = selectorConfig.getParameter<bool>("JECup");
    JECdown                  = selectorConfig.getParameter<bool>("JECdown");
    JERup                    = selectorConfig.getParameter<bool>("JERup");
    JERdown                  = selectorConfig.getParameter<bool>("JERdown");
    doNewJEC                 = selectorConfig.getParameter<bool>("doNewJEC");
    doLepJetCleaning         = selectorConfig.getParameter<bool>("doLepJetCleaning");
    JetMETCorr.Initialize(selectorConfig); // REMINDER: THIS NEEDS --if(!isMc)JetMETCorr.SetFacJetCorr(event)-- somewhere before correcting jets for data, since data JEC is era dependent. !!
    
    // Misc
    PFCandToken = iC.consumes<pat::PackedCandidateCollection>(selectorConfig.getParameter<edm::InputTag>("PFparticlesCollection"));
    rhoJetsNC_Token = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsNCInputTag"));
    rhoJetsToken = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsInputTag"));
    
    //MET
    METtoken           = iC.consumes<std::vector<pat::MET> >(selectorConfig.getParameter<edm::InputTag>("met_collection"));
    met_cuts           = selectorConfig.getParameter<bool>("met_cuts");
    min_met            = selectorConfig.getParameter<double>("min_met");
    
    //BTAG parameter initialization
    btagSfUtil.Initialize(selectorConfig);

    //-----------------------
    // Define and Set cuts
    //-----------------------
    
    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");
    push_back("Trigger");
    push_back("Primary Vertex");
    push_back("MET filters");
    push_back("Min muon");
    push_back("Max muon");
    push_back("Min electron");
    push_back("Max electron");
    push_back("Min lepton");
    push_back("Min jet multiplicity");
    push_back("Max jet multiplicity");
    push_back("Min MET");
    push_back("All cuts");          // sanity check


    set("No selection");
    set("Trigger", trigger_cut);
    set("Primary Vertex", pv_cut);
    set("MET filters", metfilters);
    if (muon_cuts){
        set("Min muon", min_muon);
        set("Max muon", max_muon);
    }
    else{
        set("Min muon", false);
        set("Max muon", false);
    }
    if (electron_cuts){
        set("Min electron", min_electron);
        set("Max electron", max_electron);
    }
    else{
        set("Min electron", false);
        set("Max electron", false);
    }
    set("Min lepton", min_lepton);
    if (jet_cuts){
        set("Min jet multiplicity", min_jet);
        set("Max jet multiplicity", max_jet);
    }
    else{
        set("Min jet multiplicity", false);
        set("Max jet multiplicity", false);
    }
    if(met_cuts) set("Min MET", min_met);
    else set("Min MET", false);
    set("All cuts", true);


    //Record cut flow information - will be saved under folder named after the selector name.
    SetHistogram( "Trigger", 2, 0,2);
    SetHistogram("Primary Vertex", 2, 0,2);
    SetHistogram("MET filters", 2, 0,2);
    SetHistogram("Min muon", 2, 0,2);
    SetHistogram("Max muon", 2, 0,2);
    SetHistogram("Min electron", 2, 0,2);
    SetHistogram("Max electron", 2, 0,2);
    SetHistogram("Min lepton", 2, 0,2); 
    SetHistogram("Min jet multiplicity", 2, 0,2); 
    SetHistogram("Max jet multiplicity", 2, 0,2); 
    SetHistogram("Min MET", 2, 0,2);
    SetHistogram("All cuts", 2, 0,2);


} 




bool DileptonEventSelector::operator()( edm::Event const & event, pat::strbitset & ret)
{


  if(debug)std::cout << "Processing Event in DileptonEventSelector::operator()" << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;
  if(debug)std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;


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

	if (! MuonSelection(event, ret)) break; //FillHist and passCut for this are in the method.

	if (! ElectronSelection(event, ret)) break; //FillHist and passCut for this are in the method.

	if (! LeptonsSelection(event, ret)) break; //FillHist and passCut for this are in the method.
	
	if (! JetSelection(event, ret)) break; //FillHist and passCut for this are in the method.

    if( ! METSelection(event) ) break;
    passCut(ret, "Min MET");
    FillHist("Min MET", 1);


    passCut(ret, "All cuts");

    break;

  } // end of while loop

  bFirstEntry = false;

  return (bool)ret;

  setIgnored(ret);

  return false;
}// end of operator()


bool DileptonEventSelector::TriggerSelection(edm::Event const & event)
{

    bool passTrig = false;

    int passEE = 0;
    int passEM = 0;
    int passMM = 0;

    //
    //_____ Trigger cuts __________________________________
    //
    if ( considerCut("Trigger") ) {

      if(debug)std::cout << "\t" <<"TriggerSelection:"<< std::endl;

      event.getByToken( triggersToken, TriggerHandle );
      const edm::TriggerNames trigNames = event.triggerNames(*TriggerHandle);

      unsigned int _tSize = TriggerHandle->size();

      // dump trigger names
      if (bFirstEntry && dump_trigger){
        for (unsigned int i=0; i<_tSize; i++){
          std::string trigName = trigNames.triggerName(i);
          std::cout << i << "   " << trigName;
          bool fired = TriggerHandle->accept(trigNames.triggerIndex(trigName));
          std::cout <<", FIRED = "<<fired<<std::endl;
        }
      }

      if (debug) std::cout << "\n Fired the following registered LJMet trigger(s):              " << std::endl;


      mvSelTriggersEl.clear();
      mvSelTriggersMu.clear();

      //Loop over each channel separately
      //loop over triggers we registered in LJMet (config)
      for (unsigned int ipath = 0; ipath < trigger_path_ee.size(); ipath++){
        //loop over triggers in sample
        for (unsigned int i=0; i<_tSize;i++){
          //compare strings
          std::string trigName_sample = trigNames.triggerName(i);
          std::string trigName_LJMet = trigger_path_ee.at(ipath);
          if ( trigName_sample.find(trigName_LJMet) == std::string::npos) continue;
          unsigned int _tIndex = trigNames.triggerIndex(trigName_sample);
          if (TriggerHandle->accept(_tIndex)){
            if (debug) std::cout << "           trigger_path_ee:" << trigNames.triggerName(i)  << std::endl;
            passEE++;
            mvSelTriggersEl[trigger_path_ee.at(ipath)] = 1;
            break;
          }
          else{
            mvSelTriggersEl[trigger_path_ee.at(ipath)] = 0;
          }

        }
      }

      //loop over triggers we registered in LJMet (config)
      for (unsigned int ipath = 0; ipath < trigger_path_em.size(); ipath++){
        //loop over triggers in sample
        for (unsigned int i=0; i<_tSize;i++){
          //compare strings
          std::string trigName_sample = trigNames.triggerName(i);
          std::string trigName_LJMet = trigger_path_em.at(ipath);
          if ( trigName_sample.find(trigName_LJMet) == std::string::npos) continue;
          unsigned int _tIndex = trigNames.triggerIndex(trigName_sample);
          if (TriggerHandle->accept(_tIndex)){
            if (debug) std::cout << "           trigger_path_em:" << trigNames.triggerName(i)  << std::endl;
            mvSelTriggersEl[trigger_path_em.at(ipath)] = 1;
            mvSelTriggersMu[trigger_path_em.at(ipath)] = 1;
            passEM++;
            break;
          }
          else{
            mvSelTriggersEl[trigger_path_em.at(ipath)] = 0;
            mvSelTriggersMu[trigger_path_em.at(ipath)] = 0;
          }

        }
      }

      //loop over triggers we registered in LJMet (config)
      for (unsigned int ipath = 0; ipath < trigger_path_mm.size(); ipath++){
        //loop over triggers in sample
        for (unsigned int i=0; i<_tSize;i++){
          //compare strings
          std::string trigName_sample = trigNames.triggerName(i);
          std::string trigName_LJMet = trigger_path_mm.at(ipath);
          if ( trigName_sample.find(trigName_LJMet) == std::string::npos) continue;
          unsigned int _tIndex = trigNames.triggerIndex(trigName_sample);
          if (TriggerHandle->accept(_tIndex)){
            if (debug) std::cout << "           trigger_path_mm:" << trigNames.triggerName(i)  << std::endl;
            passMM++;
            mvSelTriggersMu[trigger_path_mm.at(ipath)] = 1;
            break;
          }
          else{
            mvSelTriggersMu[trigger_path_mm.at(ipath)] = 0;
          }

        }
      }

      //debug
      if (debug){
        std::cout <<"\nCheck saved trig infos:"<< std::endl;

        for(const auto& x: mvSelTriggersEl){
          std::cout << "        "<< x.first << " = " <<x.second << std::endl;
        }
        for(const auto& x: mvSelTriggersMu){
          std::cout << "        "<< x.first << " = "<< x.second << std::endl;
        }
      }

      vSelTriggers.clear();
      vSelTriggers.push_back(passEE);
      vSelTriggers.push_back(passEM);
      vSelTriggers.push_back(passMM);
    }
    else{

      if(debug)std::cout << "\t" <<"IGNORING TriggerSelection."<< std::endl;

      passTrig = true;
    } // end of trigger cuts


    if ( ignoreCut("Trigger") || passEE + passEM + passMM > 0 ) {

      passTrig = true;

    }

    return passTrig;
}

bool DileptonEventSelector::PVSelection(edm::Event const & event)
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

bool DileptonEventSelector::METfilter(edm::Event const & event)
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

bool DileptonEventSelector::MuonSelection(edm::Event const & event, pat::strbitset & ret)
{

    //
    //_____ Muon cuts ________________________________
    //
    // loop over muons

    int _n_muons  = 0;
    int nSelMuons = 0;

    //get muons
    event.getByToken(muonsToken, muonsHandle);

    vSelMuons.clear();

    if ( muon_cuts ) { 
    
      if(debug)std::cout << "\t" <<"Applying MuonSelection"<< std::endl;

      for (std::vector<pat::Muon>::const_iterator _imu = muonsHandle->begin(); _imu != muonsHandle->end(); _imu++){

        bool pass = false;

		if (debug) std::cout<< "\t\t" << "pt    = " << _imu->pt() << std::endl; //DEBUG - rizki
		if (debug) std::cout<< "\t\t" << "|eta| = " << fabs(_imu->eta()) << std::endl; //DEBUG - rizki
		if (debug) std::cout<< "\t\t" << "phi = " << _imu->phi() << std::endl; //DEBUG - rizki

        //muon cuts
        while(1){
          if (_imu->pt()>muon_minpt){ if(debug)std::cout<< "\t\t\t" << "pass_muon_minpt"<< std::endl;}
          else break;

          if ( fabs(_imu->eta())<muon_maxeta ){ if(debug)std::cout<< "\t\t\t" << "pass_muon_maxeta"<< std::endl;}
          else break;

          if ( (*_imu).passed(reco::Muon::CutBasedIdLoose) ){ if(debug)std::cout<< "\t\t\t" << "pass_muon_CutBasedIdLoose"<< std::endl;}
          else break; // fail

          pass = true; // success
          break;
        }

        if ( pass ){
          ++nSelMuons;

          // save every good muon
          vSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
        }


        _n_muons++;
      } // end of the muon loop

      if( nSelMuons >= cut("Min muon", int()) || ignoreCut("Min muon") ){ 
          passCut(ret, "Min muon");
          FillHist("Min muon", 1);
      }
      else return false;

      if( nSelMuons <= cut("Max muon", int()) || ignoreCut("Max muon") ){
          passCut(ret, "Max muon");
          FillHist("Max muon", 1);
      }
      else return false;
      

    } // end of muon cuts
    else{ 
        if(debug)std::cout << "\t" <<"NOT applying MuonSelection"<< std::endl; 
      
      	for (std::vector<pat::Muon>::const_iterator _imu = muonsHandle->begin(); _imu != muonsHandle->end(); _imu++){

          // save ALL muons if muon_cuts is false
          vSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, _n_muons) );
          ++nSelMuons;
          _n_muons++;
      	}
                
    }

    if (debug) std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
    if (debug) std::cout<< "\t\t"<< "nSelMuons               = " << nSelMuons << " out of "<< muonsHandle->size() << std::endl; // DEBUG - rizki
    if (debug) std::cout<< "\t\t"<< "+++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
    
    return true;


}

bool DileptonEventSelector::ElectronSelection(edm::Event const & event, pat::strbitset & ret)
{

	//packed pf candidates and rho source needed miniIso
	edm::Handle<pat::PackedCandidateCollection> packedPFCandsHandle;
	event.getByToken(PFCandToken, packedPFCandsHandle);

	//rho isolation from susy recommendation
	edm::Handle<double> rhoJetsNC_Handle;
	event.getByToken(rhoJetsNC_Token, rhoJetsNC_Handle);
	double myRhoJetsNC = *rhoJetsNC_Handle;


    //
    //_____ Electron cuts __________________________________
    //
    // loop over electrons

    int _n_electrons  = 0;
    int nSelElectrons = 0;
    int nSelElectronsForJetLepClean = 0; //--> collection of electrons for lepton-jet cleaning.  

    //get electrons
    event.getByToken(electronsToken, electronsHandle );

    vSelElectrons.clear();
    vSelElectronsForJetLepClean.clear();

    if ( electron_cuts ) {

      if(debug)std::cout << "\t" <<"Applying ElectronSelection"<< std::endl;

      for ( std::vector<pat::Electron>::const_iterator _iel = electronsHandle->begin(); _iel != electronsHandle->end(); _iel++){

        bool pass = false;
        bool passForLepJetCleaning=false;

		if (debug) std::cout << "\t\t" << "pt                                          = " << _iel->pt() << std::endl; //DEBUG - rizki
		if (debug) std::cout << "\t\t" << "|eta| ( ->superCluster()->eta(), ->eta() )  = " << fabs(_iel->superCluster()->eta()) << ", " << fabs(_iel->eta()) << std::endl; //DEBUG - rizki
		if (debug) std::cout << "\t\t" << "phi ( ->superCluster()->phi(), ->phi() )    = " << _iel->superCluster()->phi() << ", " << _iel->phi() << std::endl; //DEBUG - rizki

        while(1){
          if (not _iel->gsfTrack().isNonnull() or not _iel->gsfTrack().isAvailable()) break;
          //skip if in barrel-endcap gap; doing it here means I never have to worry about it downstream since both electrons for analysis and those for cleaning are made here --> this is probably a comment made by Clint Richardson.
          if( fabs(_iel->ecalDrivenMomentum().eta())>1.442 && fabs(_iel->ecalDrivenMomentum().eta())<1.556) break;
          //if (_iel->isEBEEGap()) break;

          //mva loose for cleaning
          pat::Electron* elptr = new pat::Electron(*_iel);
          float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCandsHandle, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);
          
          //at some point all these hardcoded things needs to be configurable. this is all very messy. -- May 31, 2019.
          if(_iel->pt() > 10) { 
            passForLepJetCleaning = _iel->electronID("mvaEleID-Fall17-noIso-V2-wpLoose");
            if (miniIso > 0.4) passForLepJetCleaning = false;
          }

          delete elptr;

          // electron Et cut
          if (_iel->pt()>electron_minpt){ if(debug)std::cout<< "\t\t\t" << "pass_electron_minpt"<< std::endl;}
          else break;

          // electron eta cut
          if ( fabs(_iel->eta())<electron_maxeta ){ if(debug)std::cout<< "\t\t\t" << "pass_electron_maxeta"<< std::endl;}
          else break;

          pass = true;
          break;
        }
        //store electron for lepton-jet cleaning
        if(passForLepJetCleaning){
          if(debug)std::cout<< "\t\t\t" << "pass_electron_mvaLoose_miniIso_forLepJetCleaning"<< std::endl;
          vSelElectronsForJetLepClean.push_back( *_iel );
          ++nSelElectronsForJetLepClean;
        }

        if ( pass ){
          // save every good electron
          vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
          ++nSelElectrons;
        }
        _n_electrons++;
      } // end of the electron loop

      if( nSelElectrons >= cut("Min electron", int()) || ignoreCut("Min electron") ){
          passCut(ret, "Min electron");
          FillHist("Min electron", 1);
      }
      else return false;

      if( nSelElectrons <= cut("Max electron", int()) || ignoreCut("Max electron") ){
          passCut(ret, "Max electron");
          FillHist("Max electron", 1);
      }
      else return false;

    } // end of electron cuts
    else{ 
        if(debug)std::cout << "\t" <<"NOT applying ElectronSelection"<< std::endl; 
        
        for ( std::vector<pat::Electron>::const_iterator _iel = electronsHandle->begin(); _iel != electronsHandle->end(); _iel++){

          // save ALL electrons if muon_cuts is false
          vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, _n_electrons) );
          ++nSelElectrons;
          vSelElectronsForJetLepClean.push_back( *_iel );
          ++nSelElectronsForJetLepClean;
          _n_electrons++;
      	}

    }

	if (debug)std::cout<< "\t\t"<< "++++++++++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "nSelElectrons                         = " << nSelElectrons << " out of " << electronsHandle->size() <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "Electrons (for lepton-jet cleaning)   = " << nSelElectronsForJetLepClean << " out of " << electronsHandle->size() <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "++++++++++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki

    return true;
}

bool DileptonEventSelector::LeptonsSelection(edm::Event const & event, pat::strbitset & ret)
{

    if(debug)std::cout << "\t" <<"Applying LeptonsSelection"<< std::endl;

	// Count the leptons collected in MuonSelection and ElectronSelection
    int nSelLeptons = vSelElectrons.size() + vSelMuons.size(); 

    if( nSelLeptons >= cut("Min lepton", int()) || ignoreCut("Min lepton") ){
        passCut(ret, "Min lepton");
        FillHist("Min lepton", 1);

    }
    else{
    	if (nSelLeptons < 2){
    	    if(debug)std::cout<< "\t\t"<<"Too few leptons!"<<std::endl;  
    	}

        return false;
    }    

	if (debug)std::cout<< "\t\t"<< "++++++++++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "nSelLeptons                         = " << nSelLeptons <<std::endl; // DEBUG - rizki
	if (debug)std::cout<< "\t\t"<< "++++++++++++++++++++++++++++++++++++++++++++++++ " <<std::endl; // DEBUG - rizki
    
    return true;

}

bool DileptonEventSelector::JetSelection(edm::Event const & event, pat::strbitset & ret)
{

    if(debug)std::cout << "\t" <<"Jet Selection:"<< std::endl;

    if(!isMc) JetMETCorr.SetFacJetCorr(event);

    pat::strbitset retJet       = jetSel_->getBitTemplate();

	//packed pf candidates and rho source needed miniIso
	edm::Handle<pat::PackedCandidateCollection> packedPFCandsHandle;
	event.getByToken(PFCandToken, packedPFCandsHandle);

	//rho isolation from susy recommendation
	edm::Handle<double> rhoJetsNC_Handle;
	event.getByToken(rhoJetsNC_Token, rhoJetsNC_Handle);
	double myRhoJetsNC = *rhoJetsNC_Handle;


    //======================================================
    //
    // jet loop
    //
    //

    int _n_good_jets = 0;
    int _n_good_uncleaned_jets=0;
    int _n_jets = 0;

    vSelJets.clear();
    vSelCleanedJets.clear();
    vAllJets.clear();
    
    std::vector<pat::Electron> electronsForCleaning = vSelElectronsForJetLepClean;
    
    //for jet correction
    bool isAK8 = false;
    bool reCorrectJet = doNewJEC;
    unsigned int syst;
    if (JECup){syst=1;}
    else if (JECdown){syst=2;}
    else if (JERup){syst=3;}
    else if (JERdown){syst=4;}
    else syst = 0; //nominal
    
    //for btagging
    vSelBtagJets.clear();
    bool _isTagged;
    

    event.getByToken( jetsToken, jetsHandle );
    for (std::vector<pat::Jet>::const_iterator _ijet = jetsHandle->begin();_ijet != jetsHandle->end(); ++_ijet){

      retJet.set(false);
      // jet cuts for uncleaned jets
      bool pfID_barrel = (fabs(_ijet->correctedJet(0).eta()) < 2.7 && (*jetSel_)( *_ijet, retJet ) );
      bool pfID_endcap = (fabs(_ijet->correctedJet(0).eta()) >= 2.7 && fabs(_ijet->correctedJet(0).eta()) < 3.0 &&
                          (_ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 || _ijet->correctedJet(0).neutralMultiplicity() > 2));
      bool pfID_forward = (fabs(_ijet->correctedJet(0).eta()) >= 3.0 && (_ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 || _ijet->correctedJet(0).neutralMultiplicity() > 10));
      bool passPFID = pfID_barrel || pfID_endcap || pfID_forward;
      if ( passPFID){
        vAllJets.push_back(edm::Ptr<pat::Jet>(jetsHandle, _n_jets));
        //cut on corrected jet quantities
        TLorentzVector corJetP4 = JetMETCorr.correctJet(*_ijet,event, rhoJetsToken, isAK8, reCorrectJet, syst);
        if (( corJetP4.Pt()>jet_minpt ) && ( fabs(corJetP4.Eta())<jet_maxeta )){
          ++_n_good_uncleaned_jets;
          vSelJets.push_back(edm::Ptr<pat::Jet>(jetsHandle, _n_jets));
        }
      }

      //lepton jet cleaning
      bool _cleaned = false;
      pat::Jet cleanedJet = *_ijet;
      TLorentzVector jetP4;
      pat::Jet tmpJet = _ijet->correctedJet(0);

      if ( doLepJetCleaning ){

        //clean of muons
        for(unsigned int ilep=0; ilep < vSelMuons.size(); ilep++){
          bool looseMuon; //bool to loop over only loose muons can easily do here since loose muon id so easy to implement
          //get miniIso
          pat::Muon* muptr = new pat::Muon(vSelMuons[ilep]);
          float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCandsHandle, dynamic_cast<const reco::Candidate* > (muptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

          if(miniIso > 0.4) looseMuon=false;
          else if(!(vSelMuons[ilep]->isPFMuon())) looseMuon=false;
          else if(!( vSelMuons[ilep]->isGlobalMuon() || vSelMuons[ilep]->isTrackerMuon())) looseMuon=false;
          else looseMuon=true;
          if(!looseMuon) continue;
          if ( deltaR(vSelMuons[ilep]->p4(),_ijet->p4()) < 0.4 ){
            if (debug) {
              std::cout << "\t\t" <<"Jet Overlaps with the Muon... Cleaning jet..." << std::endl;
              std::cout << "\t\t" <<"Lepton : pT = " << vSelMuons[ilep]->pt() << " eta = " << vSelMuons[ilep]->eta() << " phi = " << vSelMuons[ilep]->phi() << std::endl;
              std::cout << "\t\t" <<"      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
            }
            //get the daughters of the muons
            std::vector<reco::CandidatePtr> muDaughters;
            for ( unsigned int isrc = 0; isrc < vSelMuons[ilep]->numberOfSourceCandidatePtrs(); ++isrc ){
              if (vSelMuons[ilep]->sourceCandidatePtr(isrc).isAvailable()) muDaughters.push_back( vSelMuons[ilep]->sourceCandidatePtr(isrc) );
            }
            const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
            for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
              for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
                if ( (*_i_const).key() == muDaughters[muI].key() ) {


                  tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );

                  if (debug) std::cout << "\t\t" <<"Corrected Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
                  _cleaned = true;
                  muDaughters.erase( muDaughters.begin()+muI );
                  break; //breaks out of mu daughters loop
                }// end of if muon ref == jet ref
              }//end loop over mu daughters

            }//end loop over jet constituents
          }//end check of muons being inside jet

          delete muptr;
        }// end loop over muons for cleaning


        //clean for electrons
        for (unsigned int ilep=0; ilep <electronsForCleaning.size();ilep++){
          if ( deltaR(electronsForCleaning.at(ilep).p4(),_ijet->p4()) < 0.4 ){
            //get the daughters of the electron
            std::vector<reco::CandidatePtr> elDaughters;
            for ( unsigned int isrc = 0; isrc < electronsForCleaning.at(ilep).numberOfSourceCandidatePtrs(); ++isrc ){
              if (electronsForCleaning.at(ilep).sourceCandidatePtr(isrc).isAvailable()) elDaughters.push_back( electronsForCleaning.at(ilep).sourceCandidatePtr(isrc) );
            }
            if (debug) {
              std::cout << "\t\t" <<"Jet Overlaps with the Electron... Cleaning jet..." << std::endl;
              std::cout << "\t\t" <<"Lepton : pT = " << electronsForCleaning.at(ilep).pt() << " eta = " << electronsForCleaning.at(ilep).eta() << " phi = " << electronsForCleaning.at(ilep).phi() << std::endl;
              std::cout << "\t\t" <<"      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
            }
            const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
            for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
              for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
                if ( (*_i_const).key() == elDaughters[elI].key() ) {
                  tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );

                  if (debug) std::cout << "\t\t" <<"Corrected Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
                  _cleaned = true;
                  elDaughters.erase( elDaughters.begin()+elI );
                  break;
                }
              }
            }
          }
        }//end electron cleaning



        //if not cleaned just use first jet (remember if no cleaning then cleanedJet==*_ijet) to get corrected four std::vector and set the cleaned jet to have it
        if (!_cleaned) {
          jetP4 = JetMETCorr.correctJet(cleanedJet, event, rhoJetsToken, isAK8, reCorrectJet, syst);
          //annoying thing to convert our tlorentzstd::vector to root::math::lorentzstd::vector
          ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double > > rlv;
          rlv.SetXYZT(jetP4.X(),jetP4.Y(),jetP4.Z(),jetP4.T());
          cleanedJet.setP4(rlv);
        }
        else{
          //get the correct 4std::vector
          jetP4 = JetMETCorr.correctJet(tmpJet, event, rhoJetsToken, isAK8, reCorrectJet, syst);
          //annoying thing to convert our tlorentzstd::vector to root::math::lorentzstd::vector
          ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double > > rlv;
          rlv.SetXYZT(jetP4.X(),jetP4.Y(),jetP4.Z(),jetP4.T());
          cleanedJet.setP4(rlv);

        }


        //now do kinematic cuts and save them, cleaning and JEC could only mess up pfID so use original jet for it (though probably do nothing)
        if (passPFID ){

          if (( cleanedJet.pt()>jet_minpt ) && ( fabs(cleanedJet.eta())<jet_maxeta )){
            ++_n_good_jets;
            vSelCleanedJets.push_back(cleanedJet);
          }
        }

      }//end lepton jet cleaning


      //
      //_____ Btagging  _____________________
      //
      _isTagged = btagSfUtil.isJetTagged(*_ijet, jetP4, event, isMc);
      if (_isTagged) vSelBtagJets.push_back(edm::Ptr<pat::Jet>( jetsHandle, _n_jets));


      ++_n_jets;
      
    } // end of loop over jets



	//---------
	//nJets cut
	//---------
    if ( jet_cuts ) {

      if ( ignoreCut("Min jet multiplicity") || _n_good_jets >= cut("Min jet multiplicity",int()) ){
          passCut(ret, "Min jet multiplicity");
          FillHist("Min jet multiplicity", 1);
      }
      else return false;

      if ( ignoreCut("Max jet multiplicity") || _n_good_jets <= cut("Max jet multiplicity",int()) ){
          passCut(ret, "Max jet multiplicity");
          FillHist("Max jet multiplicity", 1);
      }
      else return false;

    } // end of jet cuts


  return true;
}

bool DileptonEventSelector::METSelection(edm::Event const & event)
{

	bool pass = false;
	
	// WHY ISNT JET CORRECTION APPLIED HERE LIKE IN MultiLepEventSelector??? Is this fine??! --June 10, 2019

    //
    //_____ MET cuts __________________________________
    //
    event.getByToken( METtoken, mhMet );
    pMet = edm::Ptr<pat::MET>( mhMet, 0);

    if ( met_cuts ) {

      if (debug) std::cout<<"\t" <<"MET Selection:"<< std::endl;
      
      // pfMet
            if ( pMet.isNonnull() && pMet.isAvailable() ) {

              pat::MET const & met = mhMet->at(0);
              if ( ignoreCut("Min MET") || met.et()>cut("Min MET", double()) ) pass = true; 
            }
    } 
    else{
        if(debug)std::cout << "\t" <<"IGNORING MET selection"<< std::endl;
        pass = true;
    
    }
    // end of MET cuts
    
    return pass;

}

void DileptonEventSelector::AnalyzeEvent( edm::EventBase const & event,
                                         LjmetEventContent & ec ){
    //
    // Compute analysis-specific quantities in the event,
    // return via event content
    //

    //   ec.SetValue("pi", 3.14);


    return;
}

