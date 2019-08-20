#!/usr/bin/env bash
cd /afs/cern.ch/work/p/pusheng/CMSSW_10_2_15/src/ && eval `scramv1 runtime -sh`
$1 $@
