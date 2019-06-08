
#!/usr/bin/env sh
#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE
cd /afs/cern.ch/work/p/pusheng/CMSSW_9_4_13/src && eval `scramv1 runtime -sh`
PreCut -s el_Run2017D_0
