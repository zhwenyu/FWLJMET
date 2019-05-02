#!/bin/bash

thisDir=/uscms_data/d3/rsyarif/Brown2018/Validating_FWLJMET/CMSSW_9_4_13/src/FWLJMET/LJMet/condor

cd /uscms_data/d3/rsyarif/Brown2018/Validating_FWLJMET/CMSSW_9_4_13/src/

tar --exclude "*/.git" --exclude "git*" --exclude "FWLJMET/LJMet/CRAB3" --exclude "FWLJMET/LJMet/condor" -zcvf $thisDir/ljmet.tar *

cd -