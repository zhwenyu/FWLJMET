#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

class LjmetFactory;

class TestCalc : public BaseCalc {
public:
    TestCalc();
    virtual ~TestCalc();
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};
    
private:
	bool debug;

    //edm::EDGetTokenT<pat::ElectronCollection>   electronsToken;

};

static int reg = LjmetFactory::GetInstance()->Register(new TestCalc(), "TestCalc");


TestCalc::TestCalc()
{
}

TestCalc::~TestCalc()
{
}

int TestCalc::BeginJob(edm::ConsumesCollector && iC)
{
	//do consumes here if need to access input file directly
	//electronsToken 		= iC.consumes<pat::ElectronCollection>(mPset.getParameter<edm::InputTag>("electronsCollection"));

	debug = mPset.getParameter<bool>("debug");

    return 0;
}

int TestCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{     
	if(debug)std::cout << "Processing Event in TestCalc::AnalyzeEvent" << std::endl;

	// ----- Get objects from the selector -----
    std::vector<edm::Ptr<pat::Muon> >       const & vSelMuons = selector->GetSelMuons();
    std::vector<edm::Ptr<pat::Electron> >   const & vSelElectrons = selector->GetSelElectrons();
    int _nSelMuons       = (int)vSelMuons.size();
    int _nSelElectrons   = (int)vSelElectrons.size();

    //
    //_____Muons______
    //
    std::vector <double> muPt;
    std::vector <double> muEta;
    for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++){
       muPt     . push_back((*imu)->pt()); 
       muEta     . push_back((*imu)->eta()); 
    }
    SetValue("Nmu"     , _nSelMuons);
    SetValue("muPt"     , muPt);
    SetValue("muEta"     , muEta);


    //
    //_____Electrons______
    //
    std::vector <double> elPt;
    std::vector <double> elEta;
    for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
       elPt     . push_back((*iel)->pt()); 
       elEta     . push_back((*iel)->eta()); 
    }
    SetValue("Nel"     , _nSelElectrons);
    SetValue("elPt"     , elPt);
    SetValue("elEta"     , elEta);
    
    return 0;
}

