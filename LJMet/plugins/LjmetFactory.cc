#include "FWLJMET/LJMet/interface/LjmetFactory.h"

// Ensure a single instance
LjmetFactory * LjmetFactory::instance = 0;

LjmetFactory::LjmetFactory(): theSelector(0)
{
    mLegend = "[LjmetFactory]: ";
}

LjmetFactory::~LjmetFactory()
{
}

int LjmetFactory::Register(BaseCalc * calc, std::string name)
{
    std::string _name = name;
    if (_name == "") {
        // no name given - need to generate one
        char buf[256];
        unsigned long _calc_number = mpCalculators.size()+1;
        sprintf(buf, "calc%lu", _calc_number);
        _name.append(buf);
    }
    
    calc->setName(_name);
    
    if (mpCalculators.find(_name) != mpCalculators.end()) {
        std::cout << mLegend << "calculator " << _name << " already registered, rename" << std::endl;
    } else {
        calc->init();
        mpCalculators[_name] = calc;
    }
    
    return 0;
}

int LjmetFactory::Register(BaseEventSelector * object, std::string name)
{
    std::string _name = name;
    if (_name == "") {
        // no name given - need to generate one
        char buf[256];
        unsigned long _object_number = mpSelectors.size() + 1;
        sprintf(buf, "selector%lu", _object_number);
        _name.append(buf);
    }
    
    object->setName(_name);
    
    if (mpSelectors.find(_name) != mpSelectors.end()) {
        std::cout << mLegend << "event selector " << _name << " already registered, rename" << std::endl;
    } else {
        object->init();
        mpSelectors[_name] = object;
    }
    
    return 0;
}

BaseEventSelector * LjmetFactory::GetEventSelector(std::string name)
{
    // Return pointer to registered event selector
    // Exit if not found
    if (mpSelectors.find(name) == mpSelectors.end()) {
        std::cout << mLegend << "event selector " << name << " not registered" << std::endl;
        std::exit(-1);
    }
    
    // cache the current event selector
    theSelector = mpSelectors[name];
    
    return theSelector;
}

void LjmetFactory::RunAllCalculators(edm::EventBase const & event, BaseEventSelector * selector, LjmetEventContent & ec)
{
    // Loop over all registered calculators and compute
    // implemented variables
    for (std::map<std::string, BaseCalc * >::const_iterator iCalc = mpCalculators.begin(); iCalc != mpCalculators.end(); ++iCalc) {
        iCalc->second->SetEventContent(&ec);
        iCalc->second->AnalyzeEvent(event, selector);
    }
}

void LjmetFactory::RunAllProducers(edm::EventBase const & event, BaseEventSelector * selector)
{
    // Loop over all registered calculators and
    // run all producer methods (comes before selection)
    for (std::map<std::string, BaseCalc * >::const_iterator iCalc = mpCalculators.begin(); iCalc != mpCalculators.end(); ++iCalc) {
        iCalc->second->ProduceEvent(event, selector);
    }
}

void LjmetFactory::SetAllCalcConfig( std::map<std::string, edm::ParameterSet const> mPar )
{
    // Set each calc's parameter set, if present
    for (std::map<std::string, BaseCalc * >::const_iterator iCalc = mpCalculators.begin(); iCalc != mpCalculators.end(); ++iCalc) {
        std::string _name = iCalc->second->GetName();
        if (mPar.find(_name) != mPar.end()) {
            iCalc->second->SetPSet(mPar[_name]);
        }
    }
}

void LjmetFactory::SetExcludedCalcs( std::vector<std::string> vExcl )
{
    mvExcludedCalcs = vExcl;
    std::vector<std::string>::const_iterator c;
    for ( c = mvExcludedCalcs.begin(); c != mvExcludedCalcs.end(); ++c) {
        std::cout << mLegend << "removing " << *c << std::endl;
        if (mpCalculators.find(*c)!=mpCalculators.end()) {
            delete mpCalculators[*c];
            mpCalculators.erase(*c);
        }
    }
}

void LjmetFactory::BeginJobAllCalc()
{
    // Run all BeginJob()'s
    for (std::map<std::string, BaseCalc * >::const_iterator iCalc = mpCalculators.begin(); iCalc != mpCalculators.end(); ++iCalc) {
        iCalc->second->BeginJob();
    }
}

void LjmetFactory::EndJobAllCalc()
{
    // Run all EndJob()'s
    for (std::map<std::string, BaseCalc * >::const_iterator iCalc = mpCalculators.begin(); iCalc != mpCalculators.end(); ++iCalc) {
        iCalc->second->EndJob();
    }
}

void LjmetFactory::RunBeginEvent(edm::EventBase const & event, LjmetEventContent & ec)
{
    theSelector->BeginEvent(event, ec);
}

void LjmetFactory::RunEndEvent(edm::EventBase const & event, LjmetEventContent & ec)
{
    theSelector->EndEvent(event, ec);
}
