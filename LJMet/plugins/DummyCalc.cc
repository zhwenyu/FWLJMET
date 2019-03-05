#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

class LjmetFactory;

class DummyCalc : public BaseCalc {
public:
    DummyCalc();
    virtual ~DummyCalc();
    virtual int BeginJob();
    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};
    
private:
    edm::InputTag             elec_it;

};

static int reg = LjmetFactory::GetInstance()->Register(new DummyCalc(), "DummyCalc");


DummyCalc::DummyCalc()
{
}

DummyCalc::~DummyCalc()
{
}

int DummyCalc::BeginJob()
{
    return 0;
}

int DummyCalc::AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector)
{     
	std::cout << "Processing Event in DummyCalc::AnalyzeEvent" << std::endl;

	// ----- Get objects from the selector -----
    //std::vector<edm::Ptr<pat::Electron> >       const & vSelectedElectrons = selector->GetSelectedElectrons();

    // ----- Event kinematics -----
    //int _nSelElectrons   = (int)vSelElectrons.size();

    // Electron
    //std::vector <double> elPt;

    //
    //_____Electrons______
    //
    //for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
    //    elPt     . push_back((*iel)->pt()); //Must check: why ecalDrivenMomentum?
    //}

    //SetValue("elPt"     , elPt);
    
    return 0;
}

