#!/bin/env python

import os
import sys

dataset = ["runB","runC","runD","runE","runF","runG","runH","mc"]
qsub ="""
#!/usr/bin/env sh

#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE

cd /wk_cms2/sam7k9621/CMSSW_8_0_19/src && eval `scramv1 runtime -sh`
precut -s {0} 
"""

for src in dataset:

    output = open( ".sentJob.sh", 'w' )
    output.write( qsub.format(src) )
    output.close()

    cmd = "qsub .sentJob.sh -N " + src
    os.system(cmd)
    os.system("rm .sentJob.sh")

