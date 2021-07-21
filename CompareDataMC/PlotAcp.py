from __future__ import division
import os
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import math
import importlib
import numpy as np 
def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("A", "Acp").AddInput("o", "opt").AddInput("e", "uncertainty")
    opt.AddFlag("t","test").AddFlag("x","mixed").AddFlag("B", "bbSep")
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "bbSep", "Acp", "opt", "uncertainty", "mixed" )
    opt.AddOutputName( "chi2", "Acp", "opt", "mixed", "uncertainty" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs14" ]
    binlst=[ 
            "O_{{3}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{6}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{12}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{14}}^{{ {0} }}".format( opt.LeptonType() ) 
            ]
    acplst = [0, 0, 0, 0]
    errlst = [0, 0, 0, 0]

    hist = ROOT.TH1D( "hist", "", 4, 0, 4 )
    err1 = ROOT.TH1D( "err1", "", 4, 0, 4 )
    err2 = ROOT.TH1D( "err2", "", 4, 0, 4 )
    hist.SetStats( 0 )

    samplelst = [ opt.SampleInfo() ]
    if not opt.SampleInfo():
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
        samplelst = [ x for x in input.samplelst if not any( y in x for y in ["QCD", "ttbar", "Data"] ) ]

    if opt.SampleInfo() == "ttbar":
        samplelst = ["ttbar_semi", "ttbar_dilep" ]

    # Loop objlst
    toys = 1000 if opt.GetOption( "mixed" ) else 1
    for i in [ x for x in range(1, toys + 1) ]:
        for sample in samplelst:
            inputfile = opt.GetInputName( sample )
            if opt.GetOption( "mixed" ):
                idx = inputfile.index(".")
                inputfile = inputfile[:idx] + "_" + str(i) + inputfile[idx:]
            
            if opt.LeptonType() == "co":
                histmgr.SetObjlst( inputfile.format( Y=opt.Year(), L="el" ), objlst, sample + "_el" )
                histmgr.SetObjlst( inputfile.format( Y=opt.Year(), L="mu" ), objlst, sample + "_mu" )
            else:
                histmgr.SetObjlst( inputfile.format( Y=opt.Year() ), objlst, sample )

        for idx, obj in enumerate( objlst ):
            histlst = []
            # Initiailze hist
            for sample in samplelst:
                histlst.append( histmgr.GetMergedObj( sample ) )
           
            h = pltmgr.SumHist( histlst )
            nm  = h.GetBinContent( 1 )
            np  = h.GetBinContent( 2 )
            nm_err = h.GetBinError( 1 )
            np_err = h.GetBinError( 2 )
            acp = 100 * ( np - nm ) / ( np + nm ) 
            err_sq = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
            err    = 100 * math.sqrt( err_sq )

            acplst[ idx ] += acp 
            errlst[ idx ] += err

    acplst = [ x / toys for x in acplst ]
    errlst = [ x / toys for x in errlst ]

    with open( opt.GetOutputName( opt.SampleInfo(), "AcpResult", "txt" ), "w" ) as outputfile:
        for idx, obj in enumerate( objlst ):
            outputfile.write("{} Acp: {} {}\n".format( obj, acplst[idx], errlst[idx] ) )
    
    for idx, obj in enumerate( objlst ):
        hist.SetBinContent( idx + 1, acplst[ idx ] )
        hist.SetBinError  ( idx + 1, errlst[ idx ] )
        err1.SetBinContent( idx + 1, acplst[ idx ] )
        err1.SetBinError  ( idx + 1, errlst[ idx ] )
        err2.SetBinContent( idx + 1, acplst[ idx ] )
        err2.SetBinError  ( idx + 1, errlst[ idx ] * 2)

        hist.GetXaxis().SetBinLabel( idx + 1, binlst[ idx ] ) 
        
    c = pltmgr.NewCanvas()
    hist.Draw("EP")
    err2.Draw("E2 same")
    err1.Draw("E2 same")

    err2.SetFillColor( ROOT.kYellow )
    err1.SetFillColor( ROOT.kGreen )

    line = ROOT.TLine( 0, 0, 4, 0 )
    line.Draw( "same" )
    hist.Draw("same axis")
    hist.Draw("EP same")
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )
    line.SetLineStyle( 3 )

    leg = pltmgr.NewLegend( 0.65, 0.4, 0.8, 0.23)
    leg.Draw()
    leg.AddEntry( hist, "Nominal value", "LEP" )
    leg.AddEntry( err1, "1#sigma stat. error", "F" )
    leg.AddEntry( err2, "2#sigma stat. error", "F" )

    ymax = pltmgr.GetHistYmax( hist ) 
    ymax = math.ceil( ymax ) if ymax < 1 else ymax * 1.5
    hist.SetMaximum(  ymax )
    hist.SetMinimum( -1 * ymax )
    hist.GetYaxis().SetTitle( "A'_{cp} [%]" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist, False )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    tag = opt.SampleInfo() if opt.SampleInfo() else "Background"
    c.SaveAs( opt.GetOutputName( tag, "SimAcp" ) )


if __name__ == '__main__':
    main()
