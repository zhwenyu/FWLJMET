#!/bin/bash

thisDir=$PWD

cd $PWD/../../../ #make sure this is ${CMSSW_BASE}/src/

tar --exclude "*/.git" --exclude "git*" --exclude "FWLJMET/LJMet/CRAB3" --exclude "FWLJMET/LJMet/condor" -zcvf $thisDir/ljmet.tar *

cd -