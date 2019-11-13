#!/usr/bin/env bash
cd $1/src/ && eval `scramv1 runtime -sh`
$2 $@
