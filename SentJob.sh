#!/usr/bin/env bash
cd /afs/cern.ch/work/p/pusheng/CMSSW_8_0_32/src/ && eval `scramv1 runtime -sh`
$1 $@
