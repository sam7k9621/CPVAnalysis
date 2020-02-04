#!/bin/env python
import sys
import os
import argparse
import subprocess
from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
from ROOT import TFile, TChain, TH2D, TEfficiency, TCanvas
from root_numpy import root2array, tree2array

def MergeObj( lst ):
    obj = lst.pop()
    for l in lst:
        obj.Add( l )
    return obj 

def main(args):
    parser = argparse.ArgumentParser("")
    parser.add_argument(
            '-s', '--sample',
            help='testing command',
            type=str,
            required=True
            )
    parser.add_argument(
            '-c', '--csv',
            help='testing command',
            type=str,
            required=True
            )
    parser.add_argument(
            '-y', '--year',
            help='testing command',
            type=str,
            required=True
            )
    parser.add_argument(
            '-t', '--test',
            help='testing command',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

      
    dir =  os.environ['CMSSW_BASE'] + "/src/CPVAnalysis/BaseLineSelector/results/BtagEff*{}*{}*{}*root"
    s = subprocess.Popen( 'ls ' + dir.format( opt.year, opt.sample, opt.csv ), shell=True, stdout=subprocess.PIPE )
    outputlst, err = s.communicate()
    outputlst = filter(None, outputlst.split('\n') )
   
    objdict = { "eff_b" : [], "eff_c" : [], "eff_l" : [], "eff_b_test" : [], "eff_c_test" : [], "eff_l_test" : [] }
    for output in outputlst:
        file = TFile.Open( output, 'read' )
        print ">> Processing file: \033[0;31m{}\033[0m".format( output )
        
        for key, value in objdict.items():
            obj = file.Get( key ) 
            obj.SetDirectory( 0 )
            value.append( obj )
        file.Close()

    # newfile = TFile( "data/BtagEffPlot_{}_{}.root".format( opt.year, opt.sample ), 'RECREATE' )

    c = pltmgr.NewCanvas()
    for key, value in objdict.items():
        
        if opt.year == "16":
            title = key.split("test")[0] + "tightJetID" if "test" in key else key + "_looseJetID"
        else:
            title = key.split("test")[0] + "looseJetID" if "test" in key else key + "_tightJetID"


        obj  = MergeObj( value )
        plot = obj.CreateHistogram()

        plot.Draw( "COLZ" )
        plot.SetTitle( "" )

        pltmgr.SetAxis( plot )
        pltmgr.SetSinglePadWithPalette( c )
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        pltmgr.DrawEntryLeft( title )
        pltmgr.DrawEntryRight( "{}_{}".format( opt.year, opt.sample ) )
        c.SaveAs( "results/BtagEffPlot_{}_{}_{}.pdf".format( opt.year, opt.sample, title ) )

        plot.SetOption( "COLZ" )
        # plot.SetName( key )
        # plot.Write() 

    # newfile.Close()

if __name__ == '__main__':
    main(sys.argv)
