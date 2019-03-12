#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"


class LjmetFactory;

class MultiLepCalc : public BaseCalc {
public:
    MultiLepCalc();
    virtual ~MultiLepCalc();
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};
    
private:
	bool debug;
	bool isMc;

    edm::EDGetTokenT<std::vector<PileupSummaryInfo>>   PupInfoToken;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>>   PupInfoToken2;

    edm::EDGetTokenT<edm::TriggerResults >             muflagtagToken;


};

static int reg = LjmetFactory::GetInstance()->Register(new MultiLepCalc(), "MultiLepCalc");


MultiLepCalc::MultiLepCalc()
{
}

MultiLepCalc::~MultiLepCalc()
{
}

int MultiLepCalc::BeginJob(edm::ConsumesCollector && iC)
{
	//do consumes here if need to access input file directly

	//PU info
	PupInfoToken 		= iC.consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo"));
	PupInfoToken2 		= iC.consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("TriggerResults::RECO"));
	
	//Bad, dup, mu flag
	muflagtagToken 		= iC.consumes<edm::TriggerResults >(edm::InputTag("slimmedAddPileupInfo"));
	

	debug = mPset.getParameter<bool>("debug");
	isMc  = mPset.getParameter<bool>("isMc");

    return 0;
}

int MultiLepCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{     
	if(debug)std::cout << "Processing Event in MultiLepCalc::AnalyzeEvent" << std::endl;

	// ----- Get objects from the selector -----
    std::map<std::string, unsigned int>         const & mSelMCTriggersEl = selector->GetSelectedMCTriggersEl();
    std::map<std::string, unsigned int>         const & mSelTriggersEl   = selector->GetSelectedTriggersEl();
    std::map<std::string, unsigned int>         const & mSelMCTriggersMu = selector->GetSelectedMCTriggersMu();
    std::map<std::string, unsigned int>         const & mSelTriggersMu   = selector->GetSelectedTriggersMu();
    
    std::vector<edm::Ptr<reco::Vertex>>         const & vSelPVs       = selector->GetSelPVs();
    std::vector<edm::Ptr<pat::Muon> >           const & vSelMuons     = selector->GetSelMuons();
    std::vector<edm::Ptr<pat::Electron> >       const & vSelElectrons = selector->GetSelElectrons();
    int _nSelMuons       = (int)vSelMuons.size();
    int _nSelElectrons   = (int)vSelElectrons.size();


    //
    //_____Triggers______
    //
    std::vector<std::string> vsSelMCTriggersEl, vsSelTriggersEl, vsSelMCTriggersMu, vsSelTriggersMu;
    std::vector<int> viSelMCTriggersEl, viSelTriggersEl, viSelMCTriggersMu, viSelTriggersMu;
    for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersEl.begin(); j != mSelMCTriggersEl.end();j++) {
        vsSelMCTriggersEl.push_back(j->first);
        viSelMCTriggersEl.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersEl.begin(); j != mSelTriggersEl.end();j++) {
        vsSelTriggersEl.push_back(j->first);
        viSelTriggersEl.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersMu.begin(); j != mSelMCTriggersMu.end();j++) {
        vsSelMCTriggersMu.push_back(j->first);
        viSelMCTriggersMu.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersMu.begin(); j != mSelTriggersMu.end();j++) {
        vsSelTriggersMu.push_back(j->first);
        viSelTriggersMu.push_back((int)(j->second));
    }
    SetValue("vsSelMCTriggersEl", vsSelMCTriggersEl);
    SetValue("vsSelTriggersEl", vsSelTriggersEl);
    SetValue("vsSelMCTriggersMu", vsSelMCTriggersMu);
    SetValue("vsSelTriggersMu", vsSelTriggersMu);
    SetValue("viSelMCTriggersEl", viSelMCTriggersEl);
    SetValue("viSelTriggersEl", viSelTriggersEl);
    SetValue("viSelMCTriggersMu", viSelMCTriggersMu);
    SetValue("viSelTriggersMu", viSelTriggersMu);


    //
    //_____Primary Vertex______
    //
    SetValue("nPV", (int)vSelPVs.size());

    //
    //_____PU______
    //
    int NumTrueInts = -1;
    int NumPUInts = -1;
    if(isMc){
      edm::Handle<std::vector<PileupSummaryInfo>> PupInfo;
      if(! event.getByToken(PupInfoToken, PupInfo) ){
      	event.getByToken(PupInfoToken2, PupInfo);
      }
      
      for(std::vector<PileupSummaryInfo>::const_iterator PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
		int BX = PVI->getBunchCrossing();
		if(BX == 0){
		  NumTrueInts = PVI->getTrueNumInteractions();
		  NumPUInts = PVI->getPU_NumInteractions();
		}
      }
    }
    SetValue("nTrueInteractions",NumTrueInts);
    SetValue("nPileupInteractions",NumPUInts);

    //
    //_____Bad & duplicate muon flags______
    //
    bool badmuonflag = false;
    bool dupmuonflag = false;	  

    if(!isMc){
      edm::Handle<edm::TriggerResults > PatTriggerResults;
      event.getByToken( muflagtagToken, PatTriggerResults );
      const edm::TriggerNames patTrigNames = event.triggerNames(*PatTriggerResults);

      for (unsigned int i=0; i<PatTriggerResults->size(); i++){
		if (patTrigNames.triggerName(i) == "Flag_badMuons") badmuonflag = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
		if (patTrigNames.triggerName(i) == "Flag_duplicateMuons") dupmuonflag = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
      }
    }
    SetValue("flagBadMu",badmuonflag);
    SetValue("flagDupMu",dupmuonflag);


    //
    //_____Muons______
    //

    std::vector <double> muPt;
    std::vector <double> muEta;
    for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++){
       muPt.push_back((*imu)->pt()); 
       muEta.push_back((*imu)->eta()); 
    }
    SetValue("Nmu",_nSelMuons);
    SetValue("muPt",muPt);
    SetValue("muEta",muEta);


    //
    //_____Electrons______
    //
    std::vector <double> elPt;
    std::vector <double> elEta;
    for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
       elPt.push_back((*iel)->pt()); 
       elEta.push_back((*iel)->eta()); 
    }
    SetValue("Nel",_nSelElectrons);
    SetValue("elPt", elPt);
    SetValue("elEta",elEta);
    
    return 0;
}

