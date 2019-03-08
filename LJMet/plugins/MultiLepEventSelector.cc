#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

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

    edm::EDGetTokenT<edm::TriggerResults>       triggersToken;
    edm::EDGetTokenT<reco::VertexCollection>    PVToken;    
    edm::EDGetTokenT<pat::MuonCollection>       muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>   electronsToken;

    int       minLeptons;
    float     min_muPt;
    float     max_muEta;
    float     min_elPt;
    float     max_elEta;

    bool TriggerSelection(edm::Event const & event);
    bool PVSelection(edm::Event const & event);
    bool LeptonsSelection(edm::Event const & event);


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

    triggersToken       = iC.consumes<edm::TriggerResults>(selectorConfig.getParameter<edm::InputTag>("HLTcollection"));
    PVToken             = iC.consumes<reco::VertexCollection>(PVconfig.getParameter<edm::InputTag>("pvSrc"));
    muonsToken          = iC.consumes<pat::MuonCollection>(selectorConfig.getParameter<edm::InputTag>("muonsCollection"));;
    electronsToken      = iC.consumes<pat::ElectronCollection>(selectorConfig.getParameter<edm::InputTag>("electronsCollection"));

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
    push_back("Leptons");
    push_back("All cuts");

    //Reference: "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Trigger",trigger_cut);
    set("Primary Vertex",pv_cut);
    set("Leptons",true);
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

    if( PVSelection(event) ) passCut(ret, "Primary Vertex");
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
		if (debug) std::cout<< "\t" <<"The event FIRED the following registered trigger(s) in LJMet: "<<std::endl;

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

bool MultiLepEventSelector::LeptonsSelection(edm::Event const & event)
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
