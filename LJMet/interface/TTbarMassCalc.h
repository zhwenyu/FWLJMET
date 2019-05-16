#ifndef FWLJMET_LJMet_interface_TTbarMassCalc_h
#define FWLJMET_LJMet_interface_TTbarMassCalc_h

/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include <algorithm>
#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"
#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "DataFormats/JetReco/interface/GenJet.h"

using namespace std;


class TTbarMassCalc : public BaseCalc{
  
public:
  
    TTbarMassCalc();
    virtual ~TTbarMassCalc(){}
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;}

  
private:
  

    edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;
    edm::EDGetTokenT<int>                              genTtbarIdToken;


 
};



#endif