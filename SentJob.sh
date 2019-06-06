#!/usr/bin/env bash
cd /afs/cern.ch/work/p/pusheng/CMSSW_9_4_13/src/ && eval `scramv1 runtime -sh`
$1 $@
