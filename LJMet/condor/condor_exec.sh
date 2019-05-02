#!/bin/bash
#
# Template of the bash script for submitting a CONDOR job
#
# Arguments = INPUTTAR OUTPUT_DIR PREFIX $(process)
TARDIR=$1
INPUTTAR=$2
OUTPUT_DIR=$3
PREFIX=$4
JOBID=$5

uname -n
source /cvmfs/cms.cern.ch/cmsset_default.sh

# xrdcp ljmet.tar . 2>&1
# XRDEXIT=$?
# if [[ $XRDEXIT -ne 0 ]]; then
#     echo "exit code $XRDEXIT, failure in xrdcp of tarball"
#     exit $XRDEXIT
# fi

#setup scram architecture
export SCRAM_ARCH=slc6_amd64_gcc630

echo "make new scram area 'cmsrel'"
scramv1 project CMSSW CMSSW_9_4_13

#move tar to newly created scram area
mv -v ljmet.tar CMSSW_9_4_13/src/

#cd to scram area
cd CMSSW_9_4_13/src

#Untar file and clean. 
tar -xvf ljmet.tar
rm -rv ljmet.tar

echo "compiling 'scram b'"
scram b


echo "executing 'cmsenv'"
eval `scramv1 runtime -sh`

cd -

SECONDS=0
echo '----------------'
echo 'Start timing'
echo '----------------'
echo


echo "Running producer"
cmsRun runFWLJMet_singleLep.py isMC=True isTTbar=False maxEvents=-1

let "hrs=$SECONDS/60/60"
let "min=$SECONDS/60 - $hrs*60"
let "sec=$SECONDS - $hrs*60*60 - $min*60"
echo
echo '--------------CMSRUN DONE---------------------'
echo 'time elapsed : '$hrs' hrs '$min' min '$sec' sec'
echo '----------------------------------------------'


# copy output to eos
echo "xrdcp .root output for condor"
for FILE in *.root
do
  echo "xrdcp -f ${FILE} root://cmseos.fnal.gov//store/user/lpcljm/FWLJMET_condor_test/${FILE}"
  xrdcp -f ${FILE} root://cmseos.fnal.gov//store/user/lpcljm/FWLJMET_condor_test/${FILE} 2>&1
  XRDEXIT=$?
  if [[ $XRDEXIT -ne 0 ]]; then
    rm *.root
    echo "exit code $XRDEXIT, failure in xrdcp"
    exit $XRDEXIT
  fi
  rm ${FILE}
done
