#!/bin/env python
import sys
import os
import argparse
import subprocess
import ROOT
import ManagerUtils.PlotUtils.PlotUtils as pltmgr
from collections import OrderedDict 

def Plot1DEff( opt, objdict, tag, leg ):

    for key, value in objdict.items():
        if tag not in key : continue 

        key   = key.split('_', 1)
        entry = key[1]
        if opt.year == "16":
            entry = entry.split("_test")[0] + ( "_tightJetID" if "test" in entry else "_looseJetID" )
        else:
            entry = entry.split("_test")[0] + ( "_looseJetID" if "test" in entry else "_tightJetID" )
       
        plot = value.CreateGraph()
        color = pltmgr.GetColor()
        plot.SetLineColor( color )
        plot.SetMarkerColor( color )
        plot.SetMarkerStyle( pltmgr.GetMarker() )
        plot.SetLineWidth( 2 )
        plot.SetMarkerSize( 0.8 )
        plot.Draw( "EP same" )
  
        leg.AddEntry( plot, entry, "LP" )
    
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( "{}_{}_{}".format( opt.year, opt.sample, opt.workingpt ) )

def Plot2DEff( opt, objdict, tag ):
    for key, value in objdict.items():
        if tag != key : continue
        
        key   = key.split('_', 1)
        entry = key[1]
        if opt.year == "16":
            entry = entry.split("_test")[0] + ( "_tightJetID" if "test" in entry else "_looseJetID" )
        else:
            entry = entry.split("_test")[0] + ( "_looseJetID" if "test" in entry else "_tightJetID" )

        plot = value.CreateHistogram()
        plot.Draw( "COLZ" )
        plot.SetTitle( "" )

        pltmgr.SetAxis( plot )
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        pltmgr.DrawEntryLeft( entry )
        pltmgr.DrawEntryRight( "{}_{}_{}".format( opt.year, opt.sample, opt.workingpt ) )

def main(args):
    parser = argparse.ArgumentParser("")
    parser.add_argument(
            '-s', '--sample',
            help='testing command',
            type=str,
            required=True
            )
    parser.add_argument(
            '-w', '--workingpt',
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
            '-o', '--output',
            help='testing command',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

      
    pos =  os.environ['CMSSW_BASE'] + "/src/CPVAnalysis/BaseLineSelector/results/"
    dir =  "BtagEff*{}*{}*{}*root"
    s = subprocess.Popen( 'ls ' + pos + dir.format( opt.year, opt.sample, opt.csv ), shell=True, stdout=subprocess.PIPE )
    outputlst, err = s.communicate()
    outputlst = filter(None, outputlst.split('\n') )
    os.system( "hadd -f Btag_temp.root {}".format( " ".join( outputlst ) ) )

    objlst1 = [ "effPt_b", "effPt_c", "effPt_l", "effEta_b", "effEta_c", "effEta_l", "eff2D_b", "eff2D_c", "eff2D_l" ]
    objlst2 = [ x + "_test" for x in objlst1 ]
    objlst  = ( objlst1 + objlst2 )
    objlst.sort()
    
    objdict = OrderedDict()
    file = ROOT.TFile.Open( "Btag_temp.root", 'read' )
    for key in objlst:
        obj = file.Get( key ) 
        obj.SetDirectory( 0 )
        objdict[ key ] = obj
    file.Close()
    os.system( "rm Btag_temp.root" )



    # Plot 2D Eff
    c = pltmgr.NewCanvas()
    pltmgr.SetSinglePadWithPalette( c )
    Plot2DEff( opt, objdict, "eff2D_b" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_b", opt.workingpt ) )
    Plot2DEff( opt, objdict, "eff2D_c" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_c", opt.workingpt ) )
    Plot2DEff( opt, objdict, "eff2D_l" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_l", opt.workingpt ) )
    
    Plot2DEff( opt, objdict, "eff2D_b_test" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_b_test", opt.workingpt ) )
    Plot2DEff( opt, objdict, "eff2D_c_test" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_c_test", opt.workingpt ) )
    Plot2DEff( opt, objdict, "eff2D_l_test" )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "eff2D_l_test", opt.workingpt ) )
   

    # Plot Pt Eff
    pltmgr.SetSinglePad( c )
    frame   = c.DrawFrame( 0, 0, 1000, 1.4 )
    pltmgr.SetAxis( frame )
    frame.GetXaxis().SetTitle("P_{T}")
    frame.GetYaxis().SetTitle("Efficiency")
    
    leg = pltmgr.NewLegend( 0.45, 0.65, 0.9, 0.85 )
    leg.SetNColumns(2)
    leg.Draw()
    
    Plot1DEff( opt, objdict, "Pt", leg )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "Pt", opt.workingpt ) )

    # Plot Eta Eff
    frame   = c.DrawFrame( -3, 0, 3, 1.4 )
    pltmgr.SetAxis( frame )
    frame.GetXaxis().SetTitle("Eta")
    frame.GetYaxis().SetTitle("Efficiency")
    
    leg = pltmgr.NewLegend( 0.45, 0.65, 0.9, 0.85 )
    leg.SetNColumns(2)
    leg.Draw()
   
    Plot1DEff( opt, objdict, "Eta", leg )
    c.SaveAs( "results/BtagEffPlot_{}_{}_{}_{}.pdf".format( opt.year, opt.sample, "Eta", opt.workingpt ) )

    if opt.output:
        newfile = ROOT.TFile( "BtagEffPlot_{}_{}.root".format( opt.year, opt.sample ), 'RECREATE' )
        for key, value in objdict.items():
            if key not in ["eff2D_b", "eff2D_c", "eff2D_l"] : continue

            plot = value.CreateHistogram()
            plot.SetOption( "COLZ" )
            plot.SetName( key )
            plot.Write() 

        newfile.Close()

if __name__ == '__main__':
    main(sys.argv)
