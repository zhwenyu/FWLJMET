#include <math.h>
#include <boost/algorithm/string.hpp>
#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/Candidate/interface/Candidate.h"

using namespace std;

BaseEventSelector::BaseEventSelector():
mName(""),
mLegend("")
{
}


void BaseEventSelector::BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC)
{
}

void BaseEventSelector::EndJob()
{
}

void BaseEventSelector::BeginEvent(edm::EventBase const & event, LjmetEventContent & ec)
{
}


void BaseEventSelector::EndEvent(edm::EventBase const & event, LjmetEventContent & ec)
{
}


void BaseEventSelector::Init( void )
{

	// NOTE: These are used by Jets correction methods and BTag methods, but probably these can be re organized too.
    // init sanity check histograms --> ACTUALLY, CAN REMOVE THESE, MAR 15, 2019
//     mpEc->SetHistogram(mName, "jes_correction", 100, 0.8, 1.2);
//     mpEc->SetHistogram(mName, "met_correction", 100, 0.0, 2.0);
//     mpEc->SetHistogram(mName, "nBtagSfCorrections", 100, 0.0, 10.0);
    mpEc->SetHistogram(mName, "nEvents", 4, -2,2); // to record total events prior to any selection (and ideally negative weights for MC). 
}

