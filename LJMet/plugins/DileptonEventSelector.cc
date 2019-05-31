#include "FWLJMET/LJMet/interface/DileptonEventSelector.h"


DileptonEventSelector::DileptonEventSelector(){
}


DileptonEventSelector::~DileptonEventSelector(){
}


void DileptonEventSelector::BeginJob( const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC){


    std::cout << mLegend << "initializing DileptonEventSelector selection" << std::endl;

    std::string thisSelectorName = GetName();
    const edm::ParameterSet& selectorConfig = iConfig.getParameterSet( thisSelectorName ) ;

    // PFCandToken = iC.consumes<pat::PackedCandidateCollection>(selectorConfig.getParameter<edm::InputTag>("PFparticlesCollection"));
    // rhoJetsNC_Token = iC.consumes<double>(selectorConfig.getParameter<edm::InputTag>("rhoJetsNCInputTag"));

    // //PFJetID
    // const edm::ParameterSet& PFJetIFconfig = selectorConfig.getParameterSet("pfJetIDSelector") ;
    // jetSel_ = boost::shared_ptr<PFJetIDSelectionFunctor>( new PFJetIDSelectionFunctor(PFJetIFconfig) );

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


    // hbhe_cut                 = selectorConfig.getParameter<bool>         ("hbhe_cut");
    // metfilters                   = selectorConfig.getParameter<bool>         ("metfilters");
    // hbhe_cut_value           = selectorConfig.getParameter<std::string>  ("hbhe_cut_value");
    // hbheiso_cut              = selectorConfig.getParameter<bool>         ("hbheiso_cut");
    // cscHalo_cut              = selectorConfig.getParameter<bool>         ("cscHalo_cut");
    // eesc_cut                 = selectorConfig.getParameter<bool>         ("eesc_cut");
    // ecalTP_cut               = selectorConfig.getParameter<bool>         ("ecalTP_cut");
    // goodVtx_cut              = selectorConfig.getParameter<bool>         ("goodVtx_cut");
    // badMuon_cut              = selectorConfig.getParameter<bool>         ("badMuon_cut");
    // badChargedHadron_cut     = selectorConfig.getParameter<bool>         ("badChargedHadron_cut");


    // METfilterToken = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("flag_tag"));
    // jet_cuts                 = selectorConfig.getParameter<bool>         ("jet_cuts");
    // jet_minpt                = selectorConfig.getParameter<double>       ("jet_minpt");
    // jet_maxeta               = selectorConfig.getParameter<double>       ("jet_maxeta");
    // min_jet                  = selectorConfig.getParameter<int>          ("min_jet");
    // max_jet                  = selectorConfig.getParameter<int>          ("max_jet");

    // electron_cuts            = selectorConfig.getParameter<bool>         ("electron_cuts");
    // min_electron             = selectorConfig.getParameter<int>          ("min_electron");
    // max_electron             = selectorConfig.getParameter<int>          ("max_electron");
    // electron_minpt           = selectorConfig.getParameter<double>       ("electron_minpt");
    // electron_maxeta          = selectorConfig.getParameter<double>       ("electron_maxeta");

    // min_lepton               = selectorConfig.getParameter<int>          ("min_lepton");

    // met_cuts                 = selectorConfig.getParameter<bool>         ("met_cuts");

    // trigger_collection       = selectorConfig.getParameter<edm::InputTag>("trigger_collection");
    // pv_collection            = selectorConfig.getParameter<edm::InputTag>("pv_collection");
    // jet_collection           = selectorConfig.getParameter<edm::InputTag>("jet_collection");
    // muon_collection          = selectorConfig.getParameter<edm::InputTag>("muon_collection");
    // electron_collection      = selectorConfig.getParameter<edm::InputTag>("electron_collection");
    // met_collection           = selectorConfig.getParameter<edm::InputTag>("met_collection");
    // doLepJetCleaning         = selectorConfig.getParameter<bool>         ("doLepJetCleaning");
    // doNewJEC                 = selectorConfig.getParameter<bool>         ("doNewJEC");
    // isMc                     = selectorConfig.getParameter<bool>         ("isMc");

    // //mva value
    // UseElMVA                 = selectorConfig.getParameter<bool>         ("UseElMVA");


    push_back("No selection");
    push_back("Trigger");
    push_back("Primary Vertex");
    push_back("MET filters");
    push_back("Min muon");
    push_back("Max muon");
    // push_back("Min electron");
    // push_back("Max electron");
    // push_back("Min lepton");
    // push_back("One jet or more");
    // push_back("Two jets or more");
    // push_back("Three jets or more");
    // push_back("Min jet multiplicity");
    // push_back("Max jet multiplicity");
    // push_back("Min MET");
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

    // if (electron_cuts){
    //     set("Min electron", min_electron"]);
    //     set("Max electron", max_electron"]);
    // }
    // else{
    //     set("Min electron", false);
    //     set("Max electron", false);
    // }

    // set("Min lepton", min_lepton);

    // if (jet_cuts){
    //     set("One jet or more", true);
    //     set("Two jets or more", true);
    //     set("Three jets or more", true);
    //     set("Min jet multiplicity", min_jet);
    //     set("Max jet multiplicity", max_jet);
    // }
    // else{
    //     set("One jet or more", false);
    //     set("Two jets or more", false);
    //     set("Three jets or more", false);
    //     set("Min jet multiplicity", false);
    //     set("Max jet multiplicity", false);
    // }

    // if (met_cuts) set("Min MET", min_met);

    set("All cuts", true);



    //Record cut flow information - will be saved under folder named after the selector name.
    SetHistogram( "Trigger", 2, 0,2);
    SetHistogram("Primary Vertex", 2, 0,2);
    SetHistogram("MET filters", 2, 0,2);
    SetHistogram("Min muon", 2, 0,2);
    SetHistogram("Max muon", 2, 0,2);
    // SetHistogram("Lepton Selection", 2, 0,2); // keeping it simple for now
    // if(jet_cuts){
    //     SetHistogram("Jet Selection", 2, 0,2); // keeping it simple for now
    // }
    // SetHistogram("MET", 2, 0,2);
    // SetHistogram("All cuts", 2, 0,2);


} // initialize()




bool DileptonEventSelector::operator()( edm::Event const & event, pat::strbitset & ret)
{


  if(debug)std::cout << "Processing Event in DileptonEventSelector::operator()" << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;
  if(debug)std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;

  //if(!isMc) JetMETCorr.SetFacJetCorr(event);

  // pat::strbitset retJet       = jetSel_->getBitTemplate();

  // //packed pf candidates and rho source needed miniIso
  // edm::Handle<pat::PackedCandidateCollection> packedPFCands;
  // event.getByToken(PFCandToken, packedPFCands);

  // //rho isolation from susy recommendation
  // edm::Handle<double> rhoJetsNC;
  // event.getByToken(rhoJetsNC_Token , rhoJetsNC);
  // double myRhoJetsNC = *rhoJetsNC;


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



/*
    //
    //_____ Electron cuts __________________________________
    //
    // loop over electrons

    int _n_electrons  = 0;
    int nSelElectrons = 0;
    //collection of electrons for cleaning
    std::vector<pat::Electron> electronsForCleaning;
    //read in pvs which we will need for dZ calculation:
    std::vector<reco::Vertex> goodPVs;
    edm::Handle<std::vector<reco::Vertex> > pvHandle;
    event.getByLabel(pv_collection, pvHandle);
    goodPVs = *(pvHandle.product());



    if ( electron_cuts ) {

      //get rho src
      edm::InputTag rhoSrc_it("fixedGridRhoFastjetAll","");
      edm::Handle<double> rhoHandle;
      event.getByLabel(rhoSrc_it, rhoHandle);
      double rhoIso = std::max(*(rhoHandle.product()), 0.0);


      //get electrons
      event.getByLabel( electron_collection, mhElectrons );

      mvSelElectrons.clear();



      for ( std::vector<pat::Electron>::const_iterator _iel = mhElectrons->begin(); _iel != mhElectrons->end(); _iel++){

        bool pass = false;
        bool passLoose=false;
        while(1){
          if (not _iel->gsfTrack().isNonnull() or not _iel->gsfTrack().isAvailable()) break;
          //skip if in barrel-endcap gap; doing it here means I never have to worry about it downstream since both electrons for analysis and those for cleaning are made here
          if( fabs(_iel->ecalDrivenMomentum().eta())>1.442 && fabs(_iel->ecalDrivenMomentum().eta())<1.556) break;
          //if (_iel->isEBEEGap()) break;

          //mva loose for cleaning
          pat::Electron* elptr = new pat::Electron(*_iel);
          float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);


          if(_iel->pt() > 10) {
            passLoose = _iel->electronID("mvaEleID-Fall17-noIso-V2-wpLoose");
            if (miniIso > 0.4) passLoose = false;
          }

          delete elptr;

          // electron Et cut
          if (_iel->pt()>electron_minpt){ }
          else break;

          // electron eta cut
          if ( fabs(_iel->eta())<electron_maxeta ){ }
          else break;

          pass = true;
          break;
        }
        //store loose electron for lepton-jet cleaning
        if(passLoose){
          electronsForCleaning.push_back(*_iel);
        }

        if ( pass ){
          ++nSelElectrons;
          // save every good electron
          mvSelElectrons.push_back( edm::Ptr<pat::Electron>( mhElectrons, _n_electrons) );

        }
        _n_electrons++;
      } // end of the electron loop

      if( nSelElectrons >= cut("Min electron", int()) || ignoreCut("Min electron") ) passCut(ret, "Min electron");
      else break;

      if( nSelElectrons <= cut("Max electron", int()) || ignoreCut("Max electron") ) passCut(ret, "Max electron");
      else break;

    } // end of electron cuts

    int nSelLeptons = nSelElectrons + nSelMuons;

    if( nSelLeptons >= cut("Min lepton", int()) || ignoreCut("Min lepton") ) passCut(ret, "Min lepton");
    else break;

    //     if (nSelLeptons < 2) std::cout<<"Too few leptons!"<<std::endl;

*/


/*

    //======================================================
    //
    // jet loop
    //
    //

    int _n_good_jets = 0;
    int _n_good_uncleaned_jets=0;
    int _n_jets = 0;
    //int njetsPF = 0;

    mvSelJets.clear();
    mvSelJetsCleaned.clear();
    mvAllJets.clear();

    event.getByLabel( jet_collection, mhJets );
    for (std::vector<pat::Jet>::const_iterator _ijet = mhJets->begin();_ijet != mhJets->end(); ++_ijet){

      retJet.set(false);
      // jet cuts for uncleaned jets
      bool pfID_barrel = (fabs(_ijet->correctedJet(0).eta()) < 2.7 && (*jetSel_)( *_ijet, retJet ) );
      bool pfID_endcap = (fabs(_ijet->correctedJet(0).eta()) >= 2.7 && fabs(_ijet->correctedJet(0).eta()) < 3.0 &&
                          (_ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 || _ijet->correctedJet(0).neutralMultiplicity() > 2));
      bool pfID_forward = (fabs(_ijet->correctedJet(0).eta()) >= 3.0 && (_ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 || _ijet->correctedJet(0).neutralMultiplicity() > 10));
      bool passPFID = pfID_barrel || pfID_endcap || pfID_forward;
      if ( passPFID){
        mvAllJets.push_back(edm::Ptr<pat::Jet>(mhJets, _n_jets));
        //cut on corrected jet quantities
        TLorentzVector corJetP4 = correctJet(*_ijet,event,false,false,0);
        if (( corJetP4.Pt()>jet_minpt ) && ( fabs(corJetP4.Eta())<jet_maxeta )){
          ++_n_good_uncleaned_jets;
          mvSelJets.push_back(edm::Ptr<pat::Jet>(mhJets, _n_jets));
        }
      }

      //lepton jet cleaning
      bool _cleaned = false;
      pat::Jet cleanedJet = *_ijet;
      TLorentzVector jetP4;
      pat::Jet tmpJet = _ijet->correctedJet(0);

      if ( doLepJetCleaning ){

        if (debug) std::cout << "Checking Overlap" << std::endl;
        //clean of muons
        for(unsigned int ilep=0; ilep < mvSelMuons.size(); ilep++){
          bool looseMuon; //bool to loop over only loose muons can easily do here since loose muon id so easy to implement
          //get muon isolation
          //new definition of iso based on muon pog page
          const reco::MuonPFIsolation pfIsolationR04 = mvSelMuons[ilep]->pfIsolationR04();
          double chIso  = pfIsolationR04.sumChargedHadronPt;
          double nhIso  = pfIsolationR04.sumNeutralHadronEt;
          double gIso   = pfIsolationR04.sumPhotonEt;
          double puIso  = pfIsolationR04.sumPUPt;
          double relIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso)) / mvSelMuons[ilep]->pt();
          //get miniIso
          pat::Muon* muptr = new pat::Muon(mvSelMuons[ilep]);
          float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (muptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

          if(miniIso > 0.4) looseMuon=false;
          else if(!(mvSelMuons[ilep]->isPFMuon())) looseMuon=false;
          else if(!( mvSelMuons[ilep]->isGlobalMuon() || mvSelMuons[ilep]->isTrackerMuon())) looseMuon=false;
          else looseMuon=true;
          if(!looseMuon) continue;
          if ( deltaR(mvSelMuons[ilep]->p4(),_ijet->p4()) < 0.4 ){
            if (debug) {
              std::cout << "Jet Overlaps with the Muon... Cleaning jet..." << std::endl;
              std::cout << "Lepton : pT = " << mvSelMuons[ilep]->pt() << " eta = " << mvSelMuons[ilep]->eta() << " phi = " << mvSelMuons[ilep]->phi() << std::endl;
              std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
            }
            //get the daughters of the muons
            std::vector<reco::CandidatePtr> muDaughters;
            for ( unsigned int isrc = 0; isrc < mvSelMuons[ilep]->numberOfSourceCandidatePtrs(); ++isrc ){
              if (mvSelMuons[ilep]->sourceCandidatePtr(isrc).isAvailable()) muDaughters.push_back( mvSelMuons[ilep]->sourceCandidatePtr(isrc) );
            }
            const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
            for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
              for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
                if ( (*_i_const).key() == muDaughters[muI].key() ) {


                  tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );

                  if (debug) std::cout << "Corrected Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
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
              std::cout << "Jet Overlaps with the Electron... Cleaning jet..." << std::endl;
              std::cout << "Lepton : pT = " << electronsForCleaning.at(ilep).pt() << " eta = " << electronsForCleaning.at(ilep).eta() << " phi = " << electronsForCleaning.at(ilep).phi() << std::endl;
              std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
            }
            const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
            for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
              for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
                if ( (*_i_const).key() == elDaughters[elI].key() ) {
                  tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );

                  if (debug) std::cout << "Corrected Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
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
          jetP4 = correctJet(cleanedJet, event,false,false,0);
          //annoying thing to convert our tlorentzstd::vector to root::math::lorentzstd::vector
          ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double > > rlv;
          rlv.SetXYZT(jetP4.X(),jetP4.Y(),jetP4.Z(),jetP4.T());
          cleanedJet.setP4(rlv);
        }
        else{
          //get the correct 4std::vector
          jetP4 = correctJet(tmpJet, event);
          //annoying thing to convert our tlorentzstd::vector to root::math::lorentzstd::vector
          ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double > > rlv;
          rlv.SetXYZT(jetP4.X(),jetP4.Y(),jetP4.Z(),jetP4.T());
          cleanedJet.setP4(rlv);

        }


        //now do kinematic cuts and save them, cleaning and JEC could only mess up pfID so use original jet for it (though probably do nothing)
        if (passPFID ){

          if (( cleanedJet.pt()>jet_minpt ) && ( fabs(cleanedJet.eta())<jet_maxeta )){
            ++_n_good_jets;
            mvSelJetsCleaned.push_back(cleanedJet);
          }
        }

      }//end lepton jet cleaning
      ++_n_jets;
    } // end of loop over jets

    if ( jet_cuts ) {

      if ( ignoreCut("One jet or more") || _n_good_jets >= 1 ) passCut(ret, "One jet or more");
      else break;

      if ( ignoreCut("Two jets or more") || _n_good_jets >= 2 ) passCut(ret, "Two jets or more");
      else break;

      if ( ignoreCut("Three jets or more") || _n_good_jets >= 3 ) passCut(ret, "Three jets or more");
      else break;

      if ( ignoreCut("Min jet multiplicity") || _n_good_jets >= cut("Min jet multiplicity",int()) ) passCut(ret, "Min jet multiplicity");
      else break;

      if ( ignoreCut("Max jet multiplicity") || _n_good_jets <= cut("Max jet multiplicity",int()) ) passCut(ret, "Max jet multiplicity");
      else break;

    } // end of jet cuts

*/


/*
    //
    //_____ MET cuts __________________________________
    //
    event.getByLabel( met_collection, mhMet );
    mpMet = edm::Ptr<pat::MET>( mhMet, 0);

    if ( met_cuts ) {


      // pfMet
            if ( mpMet.isNonnull() && mpMet.isAvailable() ) {

              pat::MET const & met = mhMet->at(0);
              if ( ignoreCut("Min MET") || met.et()>cut("Min MET", double()) ) passCut(ret, "Min MET");
            }
    } // end of MET cuts

*/


/*
    //
    //_____ Btagging selection _____________________
    //
    mvSelBtagJets.clear();
    bool _isTagged;

    for (std::vector<edm::Ptr<pat::Jet> >::const_iterator _ijet = mvSelJets.begin();
         _ijet != mvSelJets.end(); ++_ijet){

      _isTagged = isJetTagged(**_ijet, event);

      if (_isTagged) mvSelBtagJets.push_back(*_ijet);
    }

    */

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

void DileptonEventSelector::AnalyzeEvent( edm::EventBase const & event,
                                         LjmetEventContent & ec ){
    //
    // Compute analysis-specific quantities in the event,
    // return via event content
    //

    //   ec.SetValue("pi", 3.14);


    return;
}

