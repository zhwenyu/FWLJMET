#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

using namespace std;

class TestEventSelector : public BaseEventSelector {

public:


    TestEventSelector();
    ~TestEventSelector();


    // executes before loop over events
    //virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );

    // executes after loop over events
    virtual void EndJob();

protected:

    // containers for config parameter values
    //std::map<std::string,std::string>    msPar;

    std::string legend;
    bool bFirstEntry;
    bool debug;

    edm::EDGetTokenT<edm::TriggerResults>       triggersToken;
    edm::EDGetTokenT<pat::MuonCollection>       muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>   electronsToken;
    int       minLeptons; 
    float     min_muPt; 
    float     max_muEta; 
    float     min_elPt; 
    float     max_elEta; 
    std::vector<std::string>  vTargetTrigs;
    
    bool TriggerSelection(edm::Event const & event);
    bool LeptonsSelection(edm::Event const & event);


private:


};


static int reg = LjmetFactory::GetInstance()->Register(new TestEventSelector(), "TestSelector");


TestEventSelector::TestEventSelector()
{
}

TestEventSelector::~TestEventSelector()
{
}

void TestEventSelector::BeginJob( std::map<std::string, edm::ParameterSet const> par, edm::ConsumesCollector && iC)
{
    std::cout << mLegend << "initializing Test selection" << std::endl;

    debug = true;

    BaseEventSelector::BeginJob(par);

    triggersToken		= iC.consumes<edm::TriggerResults>(edm::InputTag("TriggerResults::HLT"));
    //vTargetTrigs			= iConfig.getParameter<std::vector<std::string>>("HLTpaths");
    vTargetTrigs.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v");
    
    muonsToken 			= iC.consumes<pat::MuonCollection>(edm::InputTag("slimmedMuons"));;
    electronsToken 		= iC.consumes<pat::ElectronCollection>(edm::InputTag("slimmedElectrons"));
    
    minLeptons = 3;
    min_muPt   = 20.;
    max_muEta  = 2.4;
    max_elEta  = 20.;
    max_elEta  = 2.4;
	
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

	//Muons
	edm::Handle< pat::MuonCollection > muonsHandle;
	event.getByToken(muonsToken, muonsHandle);   
	unsigned int iMu = 0; // index in input dataset
	unsigned int nSelMu = 0; //num of selected muons
    mvSelMuons.clear();
	for (std::vector<pat::Muon>::const_iterator _imu = muonsHandle->begin(); _imu != muonsHandle->end(); _imu++){
		iMu = _imu - muonsHandle->begin();
		
		if(debug) std::cout << std::endl;   		
		if(debug) std::cout << "\t" << "iMu = "<< iMu << " pt = " << _imu->pt() << " eta = " << _imu->eta();   

		if(_imu->pt() < min_muPt) continue;
		if(fabs(_imu->eta()) > max_muEta) continue;
		
		mvSelMuons.push_back( edm::Ptr<pat::Muon>( muonsHandle, iMu) );
		nSelMu++;
		
		if(debug) std::cout << " ---> " << "Pass";   
	}
		 
	//Electrons
	edm::Handle< pat::ElectronCollection > electronsHandle;
	event.getByToken(electronsToken, electronsHandle);
	unsigned int iEl = 0; // index in input dataset
	unsigned int nSelEl = 0; //num of selected muons
    mvSelElectrons.clear();
	for (std::vector<pat::Electron>::const_iterator _iel = electronsHandle->begin(); _iel != electronsHandle->end(); _iel++){
		iEl = _iel - electronsHandle->begin();
		
		if(debug) std::cout << std::endl;   		
		if(debug) std::cout << "\t" << "iEl = "<< iEl << " pt = " << _iel->pt() << " eta = " << _iel->eta();   
		if(_iel->pt() < min_elPt) continue;
		if(fabs(_iel->eta()) > max_elEta) continue;
		
		mvSelElectrons.push_back( edm::Ptr<pat::Electron>( electronsHandle, iEl) );
		nSelEl++;
	
		if(debug) std::cout << " ---> " << "Pass";   
	}
	
	//Check for min selected lepton requirement
	if(debug) std::cout << std::endl;   		
	if(debug)std::cout << "\t" << "nSelMu = "<< nSelMu << " nSelEl = "<< nSelEl << std::endl;   
	if ( (nSelMu + nSelEl) >= (unsigned int)minLeptons){ 
		pass = true;   		
		if(debug)std::cout << "\t" << "Has 3 leptons that passes selection ! "<< std::endl;
	}
    
    return pass;    
}
