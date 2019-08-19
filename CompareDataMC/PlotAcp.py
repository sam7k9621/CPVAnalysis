from __future__ import division
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.PlotInfo as info
import math
import numpy as np 

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("A", "Acp").AddInput("o", "opt")
    opt.AddFlag("t","test").AddFlag("u","uncertainty").AddFlag("x","mixed")
    
    opt.SetName( "chi2", "Acp", "opt", "mixed" )
    opt.Parsing() 

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs13" ]
    binlst=[ "O_{{3}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{6}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{12}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{13}}^{{ {0} }}".format( opt.LeptonType() ) ]
    acplst = [0, 0, 0, 0]
    errlst = [0, 0, 0, 0]

    hist = ROOT.TH1D( "hist", "", 4, 0, 4 )
    err1 = ROOT.TH1D( "err1", "", 4, 0, 4 )
    err2 = ROOT.TH1D( "err2", "", 4, 0, 4 )
    hist.SetStats( 0 )
    
    # Loop objlst
    toys = 1000 if opt.GetOption( "mixed" ) else 1
    for i in [ x for x in range(1, toys + 1) ]:
        inputfile = opt.GetFileName( opt.SampleInfo(), dir="results/ReweightSample" )
        if opt.GetOption( "mixed" ):
            idx = inputfile.index(".")
            inputfile = inputfile[:idx] + "_" + str(i) + inputfile[idx:]
        
        histmgr.SetObjlst( inputfile, objlst, "Acp" )

        for idx, obj in enumerate( objlst ):
            # Initiailze hist
            h   = histmgr.GetObj( "Acp" )
            nm  = h.GetBinContent( 1 )
            np  = h.GetBinContent( 2 )

            nm_err = h.GetBinError( 1 )
            np_err = h.GetBinError( 2 )
           
            acp = ( np - nm ) / ( np + nm ) 
            err_sq = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
            err    = math.sqrt( err_sq )

            if opt.GetOption( "uncertainty" ):
                if opt.LeptonType() == "el":
                    err += 0.00094
                if opt.LeptonType() == "mu":
                    err += 0.00070
            
            acplst[ idx ] += acp 
            errlst[ idx ] += err

    acplst = [ x / toys for x in acplst ]
    errlst = [ x / toys for x in errlst ]
    print "Acp", acplst
    print "Err", errlst
    
    for idx, obj in enumerate( objlst ):
        hist.SetBinContent( idx + 1, acplst[ idx ] * 100 )
        hist.SetBinError  ( idx + 1, errlst[ idx ] * 100 )
        err1.SetBinContent( idx + 1, acplst[ idx ] * 100 )
        err1.SetBinError  ( idx + 1, errlst[ idx ] * 100 )
        err2.SetBinContent( idx + 1, acplst[ idx ] * 100 )
        err2.SetBinError  ( idx + 1, errlst[ idx ] * 200 )

        hist.GetXaxis().SetBinLabel( idx + 1, binlst[ idx ] ) 
        
    c = pltmgr.NewCanvas()
    hist.Draw()
    err2.Draw("E2 same")
    err1.Draw("E2 same")
    hist.Draw("EP same")

    err2.SetFillColor( ROOT.kYellow )
    err1.SetFillColor( ROOT.kGreen )

    line = ROOT.TLine( 0, 0, 4, 0 )
    line.Draw( "same" )
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )

    leg = pltmgr.NewLegend( 0.65, 0.4, 0.8, 0.23)
    leg.Draw()
    leg.AddEntry( hist, "Nominal value", "LEP" )
    leg.AddEntry( err1, "1#sigma stat. error", "F" )
    leg.AddEntry( err2, "2#sigma stat. error", "F" )

    ymax =  math.ceil( pltmgr.GetHistYmax( hist ) )
    hist.SetMaximum(  ymax )
    hist.SetMinimum( -1 * ymax )
    hist.GetYaxis().SetTitle( "A'_{cp}[%]" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( opt.SampleInfo(), "SimAcp" ) )


if __name__ == '__main__':
    main()
