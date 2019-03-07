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
    // init sanity check histograms
    mpEc->SetHistogram(mName, "jes_correction", 100, 0.8, 1.2);
    mpEc->SetHistogram(mName, "met_correction", 100, 0.0, 2.0);
}

