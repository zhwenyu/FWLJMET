// COMMENT
#ifndef BtagHardcodedConditions_h
#define BtagHardcodedConditions_h

#include <iostream>
#include <vector>
#include <algorithm>

enum shift:char; //{ central, up, down, uncert };
enum method:char; //{ mujets, comb };
 
class BtagHardcodedConditions{
    
public:
    
    BtagHardcodedConditions();
    ~BtagHardcodedConditions();
    
    /**
     *   Returns the discriminant for a particular algo/OP
     */
    float getDiscriminant(const std::string & op);
    /**
     *   Returns the name of the b-tag algo used in PAT
     */
    std::string getAlgoName(const std::string & op);
    
    /**
     *   Returns the algorithm tag for a particular algo/OP, e.g. CSVM -> CSV
     */
    inline std::string getAlgoTag(const std::string & op){
        return op.substr(0,op.length()-1);
    }
    /**
     *   Returns the letter of the algo/OP, e.g. CSVM -> M
     */
    inline char getOPTag(const std::string & op) {
        return op[op.length()-1];
    }
    
    double GetBtagEfficiency(double pt, double eta, std::string tagger="CSVM", int year = 2018);
    double GetBtagScaleFactor(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetBtagSFUncertUp(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetBtagSFUncertDown(double pt, double eta, std::string tagger="CSVM", int year = 2016);

    double GetCtagEfficiency(double pt, double eta, std::string tagger="CSVM", int year = 2018);
    double GetCtagScaleFactor(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetCtagSFUncertUp(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetCtagSFUncertDown(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    
    double GetMistagRate(double pt, double eta, std::string tagger="CSVM", int year = 2018);
    double GetMistagScaleFactor(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetMistagSFUncertUp(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    double GetMistagSFUncertDown(double pt, double eta, std::string tagger="CSVM", int year = 2016);
    
private:
    double GetBtagScaleFactor2011(double pt, double eta, std::string tagger="CSVM");
    double GetBtagScaleFactor2012(double pt, double eta, std::string tagger="CSVM");
    double GetBtagScaleFactor2015(double pt, double eta, std::string tagger="CSVM");
    double GetBtagSFUncertainty2011(double pt, double eta, std::string tagger="CSVM");
    double GetBtagSFUncertainty2012(double pt, double eta, std::string tagger="CSVM");
    double GetBtagSFUncertainty2015(double pt, double eta, std::string tagger="CSVM");
    double GetMistagSF2011(double pt, double eta, std::string tagger, std::string meanminmax);
    double GetMistagSF2012(double pt, double eta, std::string tagger, std::string meanminmax);
    double GetMistagSF2015(double pt, double eta, std::string tagger, std::string meanminmax);

    /////////////Main interface functions 2016///////////
    double GetBtagSF2016(std::string tagger, method Method, shift Shift, double pt, double eta);
    double GetBtagSF2016_comb(std::string tagger, shift Shift, double pt, double eta);
    double GetCtagSF2016_comb(std::string tagger, shift Shift, double pt, double eta);
    double GetLFSF2016(  std::string tagger, shift Shift, double pt, double eta);
    ///////////// More Specific Getters 2016///////////////////////
    double GetBtagSF2016Loose(method Method, shift Shift, double pt, double eta);
    double GetBtagSF2016Loose_mujets(        shift Shift, double pt, double eta);
    double GetBtagSF2016Loose_comb(          shift Shift, double pt, double eta);
    double GetBtagSF2016Loose_subjet(        shift Shift, double pt, double eta);
    double GetBtagSF2016Medium(method Method,shift Shift, double pt, double eta);
    double GetBtagSF2016Medium_mujets(       shift Shift, double pt, double eta);
    double GetBtagSF2016Medium_comb(         shift Shift, double pt, double eta);
    double GetBtagSF2016Medium_subjet(       shift Shift, double pt, double eta);
    double GetBtagSF2016Tight(method Method, shift Shift, double pt, double eta);
    double GetBtagSF2016Tight_mujets(        shift Shift, double pt, double eta);
    double GetBtagSF2016Tight_comb(          shift Shift, double pt, double eta);
    double GetCtagSF2016Loose_comb(          shift Shift, double pt, double eta);
    double GetCtagSF2016Loose_subjet(        shift Shift, double pt, double eta);
    double GetCtagSF2016Medium_comb(         shift Shift, double pt, double eta);
    double GetCtagSF2016Medium_subjet(       shift Shift, double pt, double eta);
    double GetLFSF2016Tight(  shift Shift, double pt, double eta);
    double GetLFSF2016Loose(  shift Shift, double pt, double eta);
    double GetLFSF2016Medium( shift Shift, double pt, double eta);
    double GetLFSF2016Loose_subjet(  shift Shift, double pt, double eta);
    double GetLFSF2016Medium_subjet( shift Shift, double pt, double eta);

    inline void fillArray(float* a, float* b, int n) {
        for (int i=0;i<n;++i) a[i] = b[i];
    }
    
    float SFb_TCHPT_error11[14], SFb_CSVL_error11[14], SFb_CSVM_error11[14], SFb_CSVT_error11[14], SFb_JPL_error11[14], SFb_JPM_error11[14], SFb_JPT_error11[14];
    float SFb_TCHPT_error12[16], SFb_CSVL_error12[16], SFb_CSVM_error12[16], SFb_CSVT_error12[16], SFb_JPL_error12[16], SFb_JPM_error12[16], SFb_JPT_error12[16];
    float SFb_CSVL_error15[7], SFb_CSVM_error15[7], SFb_CSVT_error15[7];
    float SFb_CSVL_error16[7], SFb_CSVM_error16[7], SFb_CSVT_error16[7];
    float ptmin, ptmax;
    typedef std::vector< float > FVec;
    typedef std::vector< float >::iterator FVecI;
    FVec ptRange11, ptRange12, ptRange15, ptRange16;
    inline int findBin(float pt, FVec ptRange){
        return (std::upper_bound(ptRange.begin(), ptRange.end(), pt)-ptRange.begin())-1;
    }
    
};


#endif
