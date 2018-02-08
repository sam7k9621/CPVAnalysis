#!/bin/env python

import os
import sys
import json
import subprocess


def main(args):

    work_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/"

    cmd = ["ls", work_path]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    samplelst = [ x.replace("precut_muon_", "").replace(".root\n", "") for x in proc.stdout.readlines() ]

    content = {}
    datalst = []
    for sample in samplelst :

        path = work_path + "precut_muon_" + sample + ".root"
        if "run" in sample :
                    datalst.append(path)

        else:
            content[sample] = {
                    "path" : [path],
                    "cross_section" : None,
                    "gen_num" : None
                    }
    content["Data"] = {
            "path" : datalst
            }
    content["samplelst"] = samplelst
    content["lumi"] = 36814

    with open('settings/test.json', 'w') as fp:
        json.dump(content, fp, indent=4, sort_keys=True)

if __name__ == '__main__':
    main(sys.argv)
