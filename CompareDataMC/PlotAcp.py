import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.PlotInfo as info
import math
from __future__ import division

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("A", "Acp").AddInput("o", "opt")
    opt.Parsing("Simulation") 

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs13" ]
    histmgr.SetObjlst( opt.GetFileName( "ttbar" ), objlst, "ttbar" )

    hist = ROOT.TH1D( "hist", "", 4, 0, 4 )
    err1 = ROOT.TH1D( "err1", "", 4, 0, 4 )
    err2 = ROOT.TH1D( "err2", "", 4, 0, 4 )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        
        # Initiailze hist
        h   = histmgr.GetObj( "ttbar" )
        nm  = h.Integral( 0, h.FindBin( 0 ) - 1 )
        np  = h.Integral( h.FindBin( 0 ), 201 )
        acp = ( np - nm ) / ( np + nm ) 
        
        err_sq = 4. * np * nm / ( ( np + nm )** 3 )
        err    = math.sqrt( err_sq )

        hist.SetBinContent( i + 1, Acp * 100 )
        hist.SetBinError  ( i + 1, err * 100 )
        err1.SetBinContent( i + 1, Acp * 100 )
        err1.SetBinContent( i + 1, Acp * 100 )
        err2.SetBinContent( i + 1, Acp * 100 )
        err2.SetBinContent( i + 1, Acp * 200 )

        hist.GetXaxis().SetBinLanbel( i + 1, obj ) 
        
    c = pltmgr.NewCanvas()
    hist.Draw()
    err2.Draw("E2 same")
    err1.Draw("E2 same")
    hist.Draw("EP same")

    err2.SetFillColor( ROOT.kYello )
    err1.SetFillColor( ROOT.kGreen )

    line = ROOT.TLine( 0, 0, 4, 0 )
    line.Draw( "same" )
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )

    leg = pltmgr.NewLegend( 0.65, 0.7, 0.8, 0.87)
    leg.Draw()
    leg.AddEntry( hist, "NOminal value", "LEP" )
    leg.AddEntry( err1, "1#sigma stat. error", "F" )
    leg.AddEntry( err2, "2#sigma stat. error", "F" )

    hist.SetMaximum( pltmgr.GetHistYmax( hist ) * 1.1 )
    hist.SetMinimum( -1 * pltmgr.GetHistYmax( data ) * 1.1 )
    hist.GetYaxis().SetTitle( "A'_{cp}[%]" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( "Acp" ) )


if __name__ == '__main__':
    main()
