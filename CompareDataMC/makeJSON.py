#!/bin/env python

import os
import sys
import json
import subprocess


def main(args):

    work_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/"

    cmd = ["ls", work_path + "results"]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    samplelst = [ x.replace("precut_muon_", "").replace(".root\n", "") for x in proc.stdout.readlines() ]
    sample = {"samplelst" : samplelst}

    for s in samplelst :
        sample[s] = { "path" : [work_path + "results/" + "precut_muon_" + s + ".root"] }

    with open('settings/Selection.json', 'w') as fp:
        json.dump(sample, fp, indent=4, sort_keys=True)

if __name__ == '__main__':
    main(sys.argv)
