#!/bin/env python

import os
import sys
import argparse
from decimal import *

def GetEvt( lst ):
    bgevt = Decimal(0)
    sgevt = Decimal(0)
    for l in lst :
        if "data" not in l[0] :
            bgevt += Decimal(l[1])
        if "TTbar" in l[0] :
            sgevt += Decimal(l[1])
    bgevt -= sgevt

    return (sgevt, bgevt)

def diffVal( central, scale ):
    with open( central, 'r' ) as f1 : 
        cen = [ x.rstrip('\n').split() for x in f1.readlines() ]
    with open( scale,   'r' ) as f2 :
        sca = [ x.rstrip('\n').split() for x in f2.readlines() ]

    cen_sg, cen_bg = GetEvt( cen )
    sca_sg, sca_bg = GetEvt( sca )

    print "cen_sg", "cen_bg", cen_sg, cen_bg
    print "sca_sg", "sca_bg", sca_sg, sca_bg
    print "sg evt", ( sca_sg - cen_sg ) / cen_sg * Decimal(100)
    print "bg evt", ( sca_bg - cen_bg ) / cen_bg * Decimal(100)

def main(args):

    parser = argparse.ArgumentParser(
            "Submit job for FullCut"
            )

    parser.add_argument(
            '-l', '--lepton',
            help='lepton type',
            type=str,
            required=True
            )
    
    parser.add_argument(
            '-e', '--uncertainty',
            help='systematic uncertainty',
            type=str,
            default=""
            )

    parser.add_argument(
            '-c', '--chi2',
            help='chi2 cut',
            type=str
            )

    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    central = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CompareDataMC/results/Yield_" + opt.lepton

    if opt.chi2 :
        central  += "_chi2_" + opt.chi2

    scale_up = central
    scale_dn = central
    central += ".txt"

    if opt.uncertainty :
        scale_up  += "_uncertainty_" + opt.uncertainty + "_up"
        scale_dn  += "_uncertainty_" + opt.uncertainty + "_dn"

    scale_up += ".txt"
    scale_dn += ".txt"

    print opt.uncertainty + "_up"
    diffVal( central, scale_up )
    print opt.uncertainty + "_dn"
    diffVal( central, scale_dn )

if __name__ == '__main__':
    main(sys.argv)
