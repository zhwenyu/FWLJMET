#include "FWLJMET/LJMet/interface/CommonCalc.h"


int CommonCalc::BeginJob(edm::ConsumesCollector && iC){


    L1prefiringToken         = iC.consumes<double>(edm::InputTag("prefiringweight","NonPrefiringProb")); //Hardcoding.
    L1prefiringToken_up      = iC.consumes<double>(edm::InputTag("prefiringweight","NonPrefiringProbUp")); //Hardcoding.
    L1prefiringToken_down    = iC.consumes<double>(edm::InputTag("prefiringweight","NonPrefiringProbDown")); //Hardcoding.


  return 0;
}


int CommonCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector){
  //
  // compute event variables here
  //


  //
  //_____ Basic event Information _____________________
  //
  int iRun   = event.id().run();
  int iLumi  = (unsigned int)event.id().luminosityBlock();
  long long iEvent = (Long64_t)event.id().event();
  SetValue("event", iEvent);
  SetValue("lumi",  iLumi);
  SetValue("run",   iRun);
  

  //
  // _____ L1 Prefiring weights _____ --> This is probably better in a different calc. Its own calc maybe so it can be easily turned on/off. March 20, 2019.
  //
  edm::Handle<double> nonPrefiringProb;
  edm::Handle<double> nonPrefiringProbUp;
  edm::Handle<double> nonPrefiringProbDown;
  double L1NonPrefiringProb = 1.0;
  double L1NonPrefiringProbUp = 1.0;
  double L1NonPrefiringProbDown = 1.0;
  if(event.getByToken(L1prefiringToken , nonPrefiringProb)){
    L1NonPrefiringProb = *nonPrefiringProb;
  }
  if(event.getByToken(L1prefiringToken_up, nonPrefiringProbUp)){
    L1NonPrefiringProbUp = *nonPrefiringProbUp;
  }
  if(event.getByToken(L1prefiringToken_down, nonPrefiringProbDown)){
    L1NonPrefiringProbDown = *nonPrefiringProbDown;
  }

  SetValue("L1NonPrefiringProb",L1NonPrefiringProb);
  SetValue("L1NonPrefiringProbUp",L1NonPrefiringProbUp);
  SetValue("L1NonPrefiringProbDown",L1NonPrefiringProbDown);



  //
  // _____ Get objects from the selector _____________________
  //
  std::vector<edm::Ptr<pat::Jet> >      const & vAllJets = selector->GetAllJets();
  std::vector<edm::Ptr<pat::Jet> >      const & vSelJets = selector->GetSelJets();
  std::vector<edm::Ptr<pat::Jet> >      const & vSelBtagJets = selector->GetSelBtagJets();
  std::vector<edm::Ptr<pat::Muon> >     const & vSelMuons = selector->GetSelMuons();
  std::vector<edm::Ptr<pat::Muon> >     const & vLooseMuons = selector->GetSelLooseMuons();
  std::vector<edm::Ptr<pat::Electron> > const & vSelElectrons = selector->GetSelElectrons();
  std::vector<edm::Ptr<pat::Electron> >     const & vLooseElectrons = selector->GetSelLooseElectrons();
  edm::Ptr<pat::MET>                    const & pMet = selector->GetMet();
  (void)pMet; // silence warning
  
  
  //
  //_____Basic objects' info _________________________________
  //
  int _nAllJets      = (int)vAllJets.size();
  int _nSelJets      = (int)vSelJets.size();
  int _nSelBtagJets  = (int)vSelBtagJets.size();
  int _nSelMuons     = (int)vSelMuons.size();
  int _nLooseMuons   = (int)vLooseMuons.size();
  int _nSelElectrons = (int)vSelElectrons.size();
  int _nLooseElectrons   = (int)vLooseElectrons.size();
  SetValue("nAllJets",      _nAllJets);
  SetValue("nSelJets",      _nSelJets);
  SetValue("nSelBtagJets",  _nSelBtagJets);
  SetValue("nTightMuons",   _nSelMuons);
  SetValue("nLooseMuons",   _nLooseMuons);
  SetValue("nSelElectrons", _nSelElectrons);
  SetValue("nLooseElectrons", _nLooseElectrons);


  return 0;
}
