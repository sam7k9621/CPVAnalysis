#!/bin/env python
import sys
import argparse
import numpy
from array import array
from ROOT import TFile, TH1D, TCanvas, TLegend, THStack
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr

def PlotHist( data, dy, arg, tag ):
    bg     = THStack()
    leg = pltmgr.NewLegend( 0.67,0.51, 0.8,0.81 )
    leg.AddEntry( data, "Data", "le" )
    leg.AddEntry( dy, "DYJets", "F" )

    ###
    pltmgr.SetNormToUnity( dy )
    dy.Scale( data.Integral() )
    bg.Add( dy )
    ###

    c = pltmgr.NewCanvas()
    top = pltmgr.NewTopPad()
    top.Draw()
    top.cd()

    data.Draw( "EP" )
    bg.  Draw( "HIST same" )
    data.Draw( "sameaxis" )
    data.Draw( "EP same" )
    leg.Draw()

    data.GetYaxis().SetTitle( "Events" )
    data.SetMaximum( max( pltmgr.GetHistYmax( dy ), pltmgr.GetHistYmax( data ) ) * 1.5 )

    pltmgr.SetTopPlotAxis( data )
    data.SetTitle( "" )
    data.SetStats( False )
    data.SetLineColor( 1 )
    data.SetLineWidth( 1 )
    data.SetMarkerSize( 0.5 )
    data.SetMarkerStyle( 20 )
    dy.SetLineColor( pltmgr.Azure )
    dy.SetFillColor( pltmgr.Azure )

    c.cd()

    bot = pltmgr.NewBottomPad()
    bot.Draw()
    bot.cd()

    xmin = data.GetXaxis().GetXmin()
    xmax = data.GetXaxis().GetXmax()

    line  = ROOT.TLine( xmin, 1.0, xmax, 1.0 )
    upper = ROOT.TLine( xmin, 1.5, xmax, 1.5 )
    lower = ROOT.TLine( xmin, 0.5, xmax, 0.5 )
    rel   = pltmgr.DivideHist( data,  dy, 1 )

    rel.Draw( "EP" )
    upper.Draw( "same" )
    lower.Draw( "same" )
    line.Draw( "same" )

    line.SetLineColor( ROOT.kRed )
    upper.SetLineStyle( 3 )
    lower.SetLineStyle( 3 )

    rel.SetMaximum( 1.6 )
    rel.SetMinimum( 0.4 )

    rel.GetYaxis().SetTitle( "Data/MC" )
    rel.GetXaxis().SetTitle( tag )
    pltmgr.SetBottomPlotAxis( rel )

    c.cd()
    pltmgr.DrawEntryRight( "{}JetToolBox".format( arg.module ) )
    pltmgr.DrawCMSLabelOuter( pltmgr.PRELIMINARY )
    c.SaveAs( "{}_DYJets_{}JetToolBox.pdf".format( tag, arg.module ) )

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-m','--module',type=str)
    
    arg = parser.parse_args()
   
    filename = "/eos/cms/store/user/pusheng/public/FullCut/CheckBWeight_18_mu_{}_{}Jettoolbox.root"

    histmgr = pltmgr.Plotmgr() 
    objlst = [ "Zmass", "LeadingLepPt", "LeadingLepEta", "LeadingJetDeepCSV", "LeadingJetPt", "LeadingJetEta" ]
    histmgr.SetObjlst( filename.format( "Run", arg.module ), objlst, "Run" )
    histmgr.SetObjlst( filename.format( "mc",  arg.module ), objlst, "mc" )

    for obj in objlst:
        PlotHist( histmgr.GetObj( "Run" ), histmgr.GetObj( "mc" ), arg, obj )

if __name__ == '__main__':
    main()
