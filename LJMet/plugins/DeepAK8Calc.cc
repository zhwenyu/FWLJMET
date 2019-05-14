#include "FWLJMET/LJMet/interface/DeepAK8Calc.h"


using namespace std;


int DeepAK8Calc::BeginJob(edm::ConsumesCollector && iC){

  return 0;

}


int DeepAK8Calc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector){
  //DECLARE VARIABLES
  std::vector<double> dnn_B;
  std::vector<double> dnn_C;
  std::vector<double> dnn_J;
  std::vector<double> dnn_W;
  std::vector<double> dnn_Z;
  std::vector<double> dnn_H;
  std::vector<double> dnn_T;

  std::vector<int> dnn_largest;
  std::vector<int> decorr_largest;
  
  std::vector<double> decorr_B;
  std::vector<double> decorr_C;
  std::vector<double> decorr_J;
  std::vector<double> decorr_W;
  std::vector<double> decorr_Z;
  std::vector<double> decorr_H;
  std::vector<double> decorr_T;

  std::vector<pat::Jet> const & SelCorrAK8Jets = selector->GetSelCorrJetsAK8();

  int i = 0; //i is an iterator
  for (std::vector<pat::Jet>::const_iterator ijet = SelCorrAK8Jets.begin(); ijet != SelCorrAK8Jets.end(); ijet++){

    if (ijet->pt() < 170) continue;    
 
    float rawL = ijet->bDiscriminator("pfDeepBoostedJetTags:probQCDothers");
    float rawC = ijet->bDiscriminator("pfDeepBoostedJetTags:probQCDcc") + ijet->bDiscriminator("pfDeepBoostedJetTags:probQCDc");
    float rawB = ijet->bDiscriminator("pfDeepBoostedJetTags:probQCDbb") + ijet->bDiscriminator("pfDeepBoostedJetTags:probQCDb");
    float rawW = ijet->bDiscriminator("pfDeepBoostedJetTags:probWcq") + ijet->bDiscriminator("pfDeepBoostedJetTags:probWqq");
    float rawZ = ijet->bDiscriminator("pfDeepBoostedJetTags:probZbb") + ijet->bDiscriminator("pfDeepBoostedJetTags:probZcc") + ijet->bDiscriminator("pfDeepBoostedJetTags:probZqq");
    float rawH = ijet->bDiscriminator("pfDeepBoostedJetTags:probHbb") + ijet->bDiscriminator("pfDeepBoostedJetTags:probHcc") + ijet->bDiscriminator("pfDeepBoostedJetTags:probHqqqq");
    float rawT = ijet->bDiscriminator("pfDeepBoostedJetTags:probTbcq") + ijet->bDiscriminator("pfDeepBoostedJetTags:probTbqq");

    float rawmax = std::max({rawL,rawC,rawB,rawW,rawZ,rawH,rawT});    

    int dnn_Largest = 10;
    float epsilon = 1e-4;

    // J, T, H, Z, W, B, C = 0, 1, 2, 3, 4, 5, 6
    if (rawmax - rawL < epsilon) dnn_Largest = 0;
    else if (rawmax - rawT < epsilon) dnn_Largest = 1;
    else if (rawmax - rawH < epsilon) dnn_Largest = 2;
    else if (rawmax - rawZ < epsilon) dnn_Largest = 3;
    else if (rawmax - rawW < epsilon) dnn_Largest = 4;
    else if (rawmax - rawB < epsilon) dnn_Largest = 5;
    else if (rawmax - rawC < epsilon) dnn_Largest = 6;
    else dnn_Largest = 10;


    float dcrawL = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probQCDothers");
    float dcrawC = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probQCDcc") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probQCDc");
    float dcrawB = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probQCDbb") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probQCDb");
    float dcrawW = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probWcq") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probWqq");
    float dcrawZ = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probZbb") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probZcc") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probZqq");
    float dcrawH = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probHbb") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probHcc") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probHqqqq");
    float dcrawT = ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probTbcq") + ijet->bDiscriminator("pfMassDecorrelatedDeepBoostedJetTags:probTbqq");

    float dcrawmax = std::max({dcrawL,dcrawC,dcrawB,dcrawW,dcrawZ,dcrawH,dcrawT});    

    int decorr_Largest = 10;

    // J, T, H, Z, W, B, C = 0, 1, 2, 3, 4, 5, 6
    if (dcrawmax - dcrawL < epsilon) decorr_Largest = 0;
    else if (dcrawmax - dcrawT < epsilon) decorr_Largest = 1;
    else if (dcrawmax - dcrawH < epsilon) decorr_Largest = 2;
    else if (dcrawmax - dcrawZ < epsilon) decorr_Largest = 3;
    else if (dcrawmax - dcrawW < epsilon) decorr_Largest = 4;
    else if (dcrawmax - dcrawB < epsilon) decorr_Largest = 5;
    else if (dcrawmax - dcrawC < epsilon) decorr_Largest = 6;
    else decorr_Largest = 10;


    //PUSH_BACK(...)
    dnn_B.push_back(rawB);
    dnn_C.push_back(rawC);
    dnn_J.push_back(rawL);
    dnn_W.push_back(rawW);
    dnn_Z.push_back(rawZ);
    dnn_H.push_back(rawH);
    dnn_T.push_back(rawT);

    dnn_largest.push_back(dnn_Largest);
    decorr_largest.push_back(decorr_Largest);

    decorr_B.push_back(dcrawB);
    decorr_C.push_back(dcrawC);
    decorr_J.push_back(dcrawL);
    decorr_W.push_back(dcrawW);
    decorr_Z.push_back(dcrawZ);
    decorr_H.push_back(dcrawH);
    decorr_T.push_back(dcrawT);

    i++;
  }

  //SETVALUES...
  SetValue("dnn_B",dnn_B);
  SetValue("dnn_C",dnn_C);
  SetValue("dnn_J",dnn_J);
  SetValue("dnn_W",dnn_W);
  SetValue("dnn_Z",dnn_Z);
  SetValue("dnn_H",dnn_H);
  SetValue("dnn_T",dnn_T);

  SetValue("dnn_largest",dnn_largest);
  SetValue("decorr_largest",decorr_largest);

  SetValue("decorr_B",decorr_B);
  SetValue("decorr_C",decorr_C);
  SetValue("decorr_J",decorr_J);
  SetValue("decorr_W",decorr_W);
  SetValue("decorr_Z",decorr_Z);
  SetValue("decorr_H",decorr_H);
  SetValue("decorr_T",decorr_T);

  return 0;

}


int DeepAK8Calc::EndJob()
{
  return 0;
}
