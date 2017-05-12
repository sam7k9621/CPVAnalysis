#!/bin/env python
import os, glob, sys
from subprocess import PIPE, Popen

path = "/wk_cms2/yichen/bpk_ntuple/"
era = [
       [path+"BPK_80X_SingleMuon_Run2016B-03Feb2017_ver2-v2/\*.root" ,"muon_precut_b.root"],
       [path+"BPK_80X_SingleMuon_Run2016C-03Feb2017-v1/\*.root"      ,"muon_precut_c.root"],
       [path+"BPK_80X_SingleMuon_Run2016D-03Feb2017-v1/\*.root"      ,"muon_precut_d.root"],
       [path+"BPK_80X_SingleMuon_Run2016E-03Feb2017-v1/\*.root"      ,"muon_precut_e.root"],
       [path+"BPK_80X_SingleMuon_Run2016F-03Feb2017-v1/\*.root"      ,"muon_precut_f.root"],
       [path+"BPK_80X_SingleMuon_Run2016G-03Feb2017-v1/\*.root"      ,"muon_precut_g.root"],
       [path+"BPK_80X_SingleMuon_Run2016H-03Feb2017_ver2-v1/\*.root" ,"muon_precut_h1.root"],
       [path+"BPK_80X_SingleMuon_Run2016H-03Feb2017_ver3-v1/\*.root" ,"muon_precut_h2.root"]
        ]


for index,(e,f) in enumerate(era):
    os.system('precut '+e+" "+f+'>>log'+str(index)+'.txt &')
    print 'finish running era '+e+" "+f

print ""
while(True):
    pipe = Popen("ps -U ${USER} | awk '{print $4}'  | grep -c precut", shell=True, stdout=PIPE).stdout
    output = pipe.read()

    if int(output) is 0 :
        break

    os.system('sleep 5')

    sys.stdout.write('\r'+"[Status] Remaining files : "+output.strip())
    sys.stdout.flush()

