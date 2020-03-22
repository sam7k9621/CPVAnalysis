#!/usr/bin/env bash
echo ${@:2}
cd $1/src/ && eval `scramv1 runtime -sh`
${@:2}
