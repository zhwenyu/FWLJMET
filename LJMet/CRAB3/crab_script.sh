

#see: https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3AdvancedTopic#Running_a_user_script_with_CRAB

#to access PDF weights for VLQ TT/BB samples
export LHAPDF_DATA_PATH="/cvmfs/cms.cern.ch/lhapdf/pdfsets/current/":${LHAPDF_DATA_PATH}

#Then run the normal cmsRun (CHECK: have you passed necessary config arguments? ie. 'isMC=True','isTTbar=False','maxEvents=-1')
cmsRun -j FrameworkJobReport.xml -p PSet.py
