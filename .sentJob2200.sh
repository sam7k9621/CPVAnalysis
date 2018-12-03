
#!/usr/bin/env sh
#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE
cd /wk_cms2/sam7k9621/CMSSW_8_0_19/src && eval `scramv1 runtime -sh`
MakeHist -l mu -u 20 -e b-tag_dn -o 150 -s WJetsToLNu_HT-400To600
