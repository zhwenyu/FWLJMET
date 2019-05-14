#ifndef FWLJMET_LJMet_interface_TpTpCalc_h
#define FWLJMET_LJMet_interface_TpTpCalc_h

/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include <algorithm>

using namespace std;

class LjmetFactory;


class TpTpCalc : public BaseCalc{

public:

    TpTpCalc(){};
    virtual ~TpTpCalc(){}
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;}


private:

    edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;

};





#endif