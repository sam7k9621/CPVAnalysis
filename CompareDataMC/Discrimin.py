#!/bin/env python
import sys
import argparse
import numpy
from array import array
from ROOT import TFile, TH1D, TCanvas, TLegend, THStack
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr

def PassJetID( entry, idx ):
    return  (
            getattr( entry, "JetInfo.NHF" )[ idx ] < 0.9 and
            getattr( entry, "JetInfo.NEF" )[ idx ] < 0.9 and
            getattr( entry, "JetInfo.NConstituents" )[ idx ] > 1 and

            abs( getattr( entry, "JetInfo.Eta" )[ idx ] ) <= 2.4 and
            getattr( entry, "JetInfo.CHF" )[ idx ] > 0 and
            getattr( entry, "JetInfo.NCH" )[ idx ] > 0
            )

def PassJetKinematic( entry, idx ):
    return (
            getattr( entry, "JetInfo.Pt" )[ idx ] > 30 and
            abs( getattr( entry, "JetInfo.Eta" )[ idx ] ) < 2.4
            )

def FillEvent( filename, deepCSV, LdeepCSV ):
    file      = TFile.Open( filename, 'read' )
    tree      = file.Get("bprimeKit/root")

    for idx, entry in enumerate( tree ):
       
        jsize  = getattr( entry, "JetInfo.Size" )
        bb_csv = getattr( entry, "JetInfo.pfDeepCSVJetTags_probb" )
        bbb_csv= getattr( entry, "JetInfo.pfDeepCSVJetTags_probbb" )

        leading = True
        for i in range( jsize ):
            if PassJetID( entry, i ) and PassJetKinematic( entry, i ):
                deepCSV.Fill(  bb_csv[ i ] + bbb_csv[ i ] )
                if leading:
                    LdeepCSV.Fill(  bb_csv[ i ] + bbb_csv[ i ] )
                    leading = False

    file.Close()

def PlotHist( data, wjet, arg, tag ):
    bg     = THStack()
    leg = pltmgr.NewLegend( 0.67,0.51, 0.8,0.81 )
    leg.AddEntry( data, "Data", "le" )
    leg.AddEntry( wjet, "WJets", "F" )

    ###
    pltmgr.SetNormToUnity( wjet )
    wjet.Scale( data.Integral() )
    bg.Add( wjet )
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
    data.SetMaximum( max( pltmgr.GetHistYmax( wjet ), pltmgr.GetHistYmax( data ) ) * 1.5 )

    pltmgr.SetTopPlotAxis( data )
    data.SetTitle( "" )
    data.SetStats( False )
    data.SetLineColor( 1 )
    data.SetLineWidth( 1 )
    data.SetMarkerSize( 0.5 )
    data.SetMarkerStyle( 20 )
    wjet.SetLineColor( pltmgr.Azure )
    wjet.SetFillColor( pltmgr.Azure )

    c.cd()

    bot = pltmgr.NewBottomPad()
    bot.Draw()
    bot.cd()

    xmin = data.GetXaxis().GetXmin()
    xmax = data.GetXaxis().GetXmax()

    line  = ROOT.TLine( xmin, 1.0, xmax, 1.0 )
    upper = ROOT.TLine( xmin, 1.5, xmax, 1.5 )
    lower = ROOT.TLine( xmin, 0.5, xmax, 0.5 )
    rel   = pltmgr.DivideHist( data,  wjet, 1 )

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
    c.SaveAs( "{}_WJets_{}JetToolBox.pdf".format( tag, arg.module ) )

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-m','--module',type=str)
    
    arg = parser.parse_args()
    
    filename = "/afs/cern.ch/work/y/youying/public/bprimekit_test/bpk_ntuple_{}_{}JetToolBox.root"
    data   = TH1D( "data",  "data",  100, 0, 1 )
    wjet   = TH1D( "wjet",  "wjet",  100, 0, 1 )
    Ldata  = TH1D( "Ldata", "Ldata", 100, 0, 1 )
    Lwjet  = TH1D( "Lwjet", "Lwjet", 100, 0, 1 )

    FillEvent( filename.format( "data",  arg.module ),  data, Ldata )
    FillEvent( filename.format( "wjets", arg.module ),  wjet, Lwjet )

    PlotHist( data, wjet,  arg, "DeepCSV" )
    PlotHist( Ldata, Lwjet, arg, "LeadingDeepCSV" )

if __name__ == '__main__':
    main()
