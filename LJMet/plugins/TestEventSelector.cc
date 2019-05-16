#include "FWLJMET/LJMet/interface/TestEventSelector.h"


TestEventSelector::TestEventSelector()
{
}

TestEventSelector::~TestEventSelector()
{
}

void TestEventSelector::BeginJob( const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC)
{

    std::cout << mLegend << "initializing Test selection" << std::endl;

    BaseEventSelector::BeginJob(iConfig,(edm::ConsumesCollector &&)iC);
    const edm::ParameterSet& selectorConfig = iConfig.getParameterSet("test_selector") ;

    debug = selectorConfig.getParameter<bool>("debug");

    triggersToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    muonsToken          = iC.consumes<pat::MuonCollection>(selectorConfig.getParameter<edm::InputTag>("muonsCollection"));;
    electronsToken      = iC.consumes<pat::ElectronCollection>(selectorConfig.getParameter<edm::InputTag>("electronsCollection"));

    vTargetTrigs        = selectorConfig.getParameter<std::vector<std::string>>("HLTtargets");

    minLeptons = selectorConfig.getParameter<int>("minLeptons");
    min_muPt   = selectorConfig.getParameter<double>("min_muPt");
    max_muEta  = selectorConfig.getParameter<double>("max_muEta");;
    min_elPt   = selectorConfig.getParameter<double>("min_elPt");
    max_elEta  = selectorConfig.getParameter<double>("max_elEta");

    bFirstEntry = true; //in case anything needs a first entry bool.

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");
    push_back("Trigger");
    push_back("Leptons");
    push_back("All cuts");

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Trigger",true);
    set("Leptons",true);
    set("All cuts",true);


} // end of BeginJob()

bool TestEventSelector::operator()( edm::Event const & event, pat::strbitset & ret) // EVENT SELECTOR!
{
  if(debug)std::cout << "Processing Event in TestEventSelector::operator()" << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;
  if(debug)std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  if(debug)std::cout << "=====================================" <<std::endl;

  while(1){ // standard infinite while loop trick to avoid nested ifs

    passCut(ret, "No selection");

    if( TriggerSelection(event) ) passCut(ret, "Trigger");
    else break;

    if( LeptonsSelection(event) ) passCut(ret, "Leptons");
    else break;

    passCut(ret, "All cuts");
    break;

  } // end of while loop


  bFirstEntry = false;

  return (bool)ret;

  setIgnored(ret);

  return false;
}// end of operator()


void TestEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
  //
  // Compute analysis-specific quantities in the event,
  // return via event content
  //

  //   ec.SetValue("pi", 3.14);


  return;
}

void TestEventSelector::EndJob()
{
}

bool TestEventSelector::TriggerSelection(edm::Event const & event)
{
    bool passTrig = false;

    if(debug) std::cout << std::endl;
    if(debug)std::cout << "\t" <<"TriggerSelection:"<< std::endl;
    if(debug) std::cout << std::endl;

    edm::Handle< edm::TriggerResults > triggersHandle;
    event.getByToken(triggersToken,triggersHandle);
    //event.getByLabel("TriggerResults::HLT", triggersHandle);
    const edm::TriggerNames trigNames = event.triggerNames(*triggersHandle);

    unsigned int nTrig = triggersHandle->size();
    for(std::vector<std::string>::const_iterator targetTrig = vTargetTrigs.begin(); targetTrig!= vTargetTrigs.end(); targetTrig++){

      for(unsigned int i=0; i<nTrig;i++){
      	std::string trigName = trigNames.triggerName(i);
      	if(trigName.find((*targetTrig)) == std::string::npos) continue;
      	if(triggersHandle->accept(trigNames.triggerIndex(trigName))){
      		if(debug)std::cout << "\t"<<"FIRED : "<< (*targetTrig) << std::endl;
      		passTrig = true;
      	}
      }
    }

    return passTrig;
}

bool TestEventSelector::LeptonsSelection(edm::Event const & event)
{
	bool pass = false;

	if(debug) std::cout << std::endl;
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
		if(debug) std::cout << "\t" << "iMu = "<< iMu << " pt = " << _imu->pt() << " eta = " << _imu->eta();

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
		if(debug) std::cout << "\t" << "iEl = "<< iEl << " pt = " << _iel->pt() << " eta = " << _iel->eta();
		if(_iel->pt() < min_elPt) continue;
		if(fabs(_iel->eta()) > max_elEta) continue;

		vSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, iEl) );
		nSelEl++;

		if(debug) std::cout << " ---> " << "Pass";
	}

	//Check for min selected lepton requirement
	if(debug) std::cout << std::endl;
	if(debug)std::cout << "\t" << "nSelMu = "<< nSelMu << " nSelEl = "<< nSelEl << std::endl;
	if ( (nSelMu + nSelEl) >= (unsigned int)minLeptons){
		pass = true;
		if(debug)std::cout << "\t" << "Has "<< minLeptons << " leptons that passes selection ! "<< std::endl;
	}

	return pass;
}
