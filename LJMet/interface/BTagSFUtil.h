/*************************************************************
 
 Class Usage:
 
 This class should only be used for upgrading and downgrading
 if a single operating point is used in an analysis.
 
 bool isBTagged = b-tag flag for jet
 int pdgIdPart = parton id
 float Btag_SF = MC/data scale factor for b/c-tagging efficiency
 float Btag_eff = b/c-tagging efficiency in data
 float Bmistag_SF = MC/data scale factor for mistag efficiency
 float Bmistag_eff = mistag efficiency in data
 
 Author: Michael Segala
 Contact: michael.segala@gmail.com
 Updated: Ulrich Heintz 12/23/2011
 Updated: Gena Kukartsev 10/30/2012
 Updated: Rizki Syarif 03/19/2019 : Made this class handle everything to do with Btagging.
 
 v 2.0
 
 *************************************************************/


#ifndef BTagSFUtil_lite_h
#define BTagSFUtil_lite_h

#include <Riostream.h>
#include "TRandom3.h"
#include "TMath.h"

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "FWLJMET/LJMet/interface/BtagHardcodedConditions.h"
#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"


class BTagSFUtil{
    
public:
    
    BTagSFUtil();
    BTagSFUtil(int seed);
    ~BTagSFUtil();

    void Initialize(const edm::ParameterSet& iConfig);
    
    void modifyBTagsWithSF( bool& isBTagged,
                           int pdgIdPart,
                           float Btag_SF = 0.98,
                           float Btag_eff = 1.0,
                           float Bmistag_SF = 1.0,
                           float Bmistag_eff = 1.0);
    
    void SetSeed( int seed );
    

    bool isJetTagged(const pat::Jet &jet,
                     TLorentzVector correctedJet_lv,
                     edm::Event const & event,
                     bool isMc,
                     int shiftflag = 0,
                     bool subjetflag = false);


    
private:
    
    bool debug;
    std::string mLegend = "\t[BTagSFUtil]: ";
    
    bool applySF(bool& isBTagged, float Btag_SF = 0.98, float Btag_eff = 1.0);
    
    TRandom3 rand_;

    double      bdisc_min;
    std::string DeepCSVfile;
    std::string DeepCSVSubjetfile;
    std::string btagOP;
    double      applyBtagSF;
    bool        BTagUncertUp;
    bool        BTagUncertDown;
    bool        MistagUncertUp;
    bool        MistagUncertDown;
    BtagHardcodedConditions mBtagCond;
    BTagCalibration       calib;
    BTagCalibration       calibsj;
    BTagCalibrationReader reader;
    BTagCalibrationReader readerSJ;

    
};


#endif
