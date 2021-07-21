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
    opt.AddInput("c", "chi2").AddInput("A", "Acp").AddInput("o", "opt").AddInput("b","bias")
    opt.AddFlag("t","test").AddFlag("x","mixed")
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "Acp", "opt", "mixed", "bias" )
    opt.AddOutputName( "chi2", "Acp", "opt", "mixed", "bias" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs14" ]
    binlst=[ 
            "O_{{3}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{6}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{12}}^{{ {0} }}".format( opt.LeptonType() ), 
            "O_{{14}}^{{ {0} }}".format( opt.LeptonType() ) 
            ]
    alst = [0, 0, 0, 0]
    elst = [0, 0, 0, 0]
    acplst = [0, 0, 0, 0]
    errlst = [0, 0, 0, 0]

    c = pltmgr.NewCanvas()
    hist = ROOT.TH1D( "hist", "", 2, 0, 2 )
    err1 = ROOT.TH1D( "err1", "", 2, 0, 2 )
    err2 = ROOT.TH1D( "err2", "", 2, 0, 2 )
    hist.SetStats( 0 )

    samplelst = [ opt.SampleInfo() ]
    if not opt.SampleInfo():
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
        samplelst = [ x for x in input.samplelst if not any( y in x for y in ["QCD", "ttbar", "Data"] ) ]

    # Loop objlst
    inputfile = opt.GetInputName( samplelst[0] ).format( Y=opt.Year() ).replace("_mixed", "")
    if opt.LeptonType() == "co":
        histmgr.SetObjlst( inputfile.format( L="el" ), objlst, samplelst[0] + "_el" )
        histmgr.SetObjlst( inputfile.format( L="mu" ), objlst, samplelst[0] + "_mu" )
    else:
        histmgr.SetObjlst( inputfile, objlst, samplelst[0] )
   
    for idx, obj in enumerate( objlst ):
        h = histmgr.GetObj( samplelst[0] )
        nm  = h.GetBinContent( 1 )
        np  = h.GetBinContent( 2 )
        nm_err = h.GetBinError( 1 )
        np_err = h.GetBinError( 2 )
        acp = 100 * ( np - nm ) / ( np + nm ) 
        err_sq = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
        err    = 100 * math.sqrt( err_sq )
    
        alst[ idx ] += acp 
        elst[ idx ] += err


    toys = 1000 if opt.GetOption( "mixed" ) else 1
    for i in [ x for x in range(1, toys + 1) ]:

        for sample in samplelst:
            inputfile = opt.GetInputName( sample ).format( Y=opt.Year() )
            
            if opt.GetOption( "mixed" ):
                idx = inputfile.index("mixed") + 5
                inputfile = inputfile[:idx] + "_" + str(i) + inputfile[idx:]
            
            if opt.LeptonType() == "co":
                histmgr.SetObjlst( inputfile.format( L="el" ), objlst, sample + "_el" )
                histmgr.SetObjlst( inputfile.format( L="mu" ), objlst, sample + "_mu" )
            else:
                histmgr.SetObjlst( inputfile, objlst, sample )

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
    print( alst )
    print( elst )
    print( acplst )
    print( errlst )

    with open( opt.GetOutputName( opt.SampleInfo(), "AcpResult", "txt" ), "w" ) as outputfile:
        for idx, obj in enumerate( objlst ):
            outputfile.write("{} Acp: {} {}\n".format( obj, acplst[idx], errlst[idx] ) )
    
    for idx, obj in enumerate( objlst ):
        hist.SetBinContent( 1, alst[ idx ] )
        hist.SetBinError  ( 1, elst[ idx ] )
        err1.SetBinContent( 1, alst[ idx ] )
        err1.SetBinError  ( 1, elst[ idx ] )
        err2.SetBinContent( 1, alst[ idx ] )
        err2.SetBinError  ( 1, elst[ idx ] * 2)
        
        hist.SetBinContent( 2, acplst[ idx ] )
        hist.SetBinError  ( 2, errlst[ idx ] )
        err1.SetBinContent( 2, acplst[ idx ] )
        err1.SetBinError  ( 2, errlst[ idx ] )
        err2.SetBinContent( 2, acplst[ idx ] )
        err2.SetBinError  ( 2, errlst[ idx ] * 2)

        hist.GetXaxis().SetBinLabel( 1, "Biased {}".format( binlst[ idx ] ) )
        hist.GetXaxis().SetBinLabel( 2, "Biased {} after mixed".format( binlst[ idx ] )  )
        
        hist.Draw("EP")
        err2.Draw("E2 same")
        err1.Draw("E2 same")

        err2.SetFillColor( ROOT.kYellow )
        err1.SetFillColor( ROOT.kGreen )

        line = ROOT.TLine( 0, 0, 2, 0 )
        line.Draw( "same" )
        hist.Draw("same axis")
        hist.Draw("EP same")
        line.SetLineColor( ROOT.kRed )
        line.SetLineWidth( 2 )
        line.SetLineStyle( 3 )

        leg = pltmgr.NewLegend( 0.65, 0.6, 0.8, 0.8 )
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
        c.SaveAs( opt.GetOutputName( "{}_{}".format( tag, obj), "SimAcp" ) )


if __name__ == '__main__':
    main()
