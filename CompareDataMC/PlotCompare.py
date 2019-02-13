import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.PlotInfo as info
import CPVAnalysis.CompareDataMC.MakeHist as input
from decimal import Decimal

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "Opt").AddInput("r", "region")
    opt.Parsing("Stack") 

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "had_tmass", "lep_tmass", "chi2", "LJetPt", "LJetEta", "LepPt", "LepEta" ]
    for sample in input.samplelst:
        histmgr.SetObjlst( opt.GetFileName( sample ), objlst )

    # Loop objlst
    for obj in objlst:

        c = pltmgr.NewCanvas()
        leg = pltmgr.NewLegend( 0.7, 0.51, 0.83, 0.81)
        bg = ROOT.THStack()
       
        # Initiailze hist
        data = histmgr.GetObj( "Data" )
        histlst = []
        for i, mc in enumerate( info.mclst ):
            hist = histmgr.GetMergedObj( mc )
            hist.SetLineColor( pltmgr.colorlst[i] )
            hist.SetFillColor( pltmgr.colorlst[i] )
    
            bg.Add( hist )
            leg.AddEntry( hist, mc, "F" )
            histlst.append( hist )
        leg.AddEntry( data, "Data", "le" )
    
        bg_sum = pltmgr.SumHist( histlst )
    
        top = pltmgr.NewTopPad()
        top.Draw()
        top.cd()
    
        bg.Draw("HIST")
        data.Draw( "EP same" )
        leg.Draw()
    
        pltmgr.SetTopPlotAxis( bg )
        bg.SetMaximum( pltmgr.GetHistYmax( data ) * 1.5 );
        data.SetLineColor( 1 )
        data.SetLineWidth( 1 )
        data.SetMarkerSize( 0.5 )
        data.SetMarkerStyle( 20 )
    
        c.cd()
    
        bot = pltmgr.NewBottomPad()
        bot.Draw()
        bot.cd()
    
        xmin = data.GetXaxis().GetXmin()
        xmax = data.GetXaxis().GetXmax()
    
        line  = ROOT.TLine( xmin, 1.0, xmax, 1.0 )
        upper = ROOT.TLine( xmin, 1.5, xmax, 1.5 )
        lower = ROOT.TLine( xmin, 0.5, xmax, 0.5 )
        rel   = pltmgr.DivideHist( data, bg_sum, 1 )
    
        rel.Draw( "EP" )
        upper.Draw( "same" )
        lower.Draw( "same" )
        line.Draw ( "same" )
    
        line.SetLineColor( ROOT.kRed )
        upper.SetLineStyle( 3 )
        lower.SetLineStyle( 3 )
    
        rel.SetMaximum( 1.6 )
        rel.SetMinimum( 0.4 )
        rel.GetYaxis().SetTitle( "Data/MC" )
        rel.GetXaxis().SetTitle( data.GetXaxis().GetTitle() )
        pltmgr.SetBottomPlotAxis( rel )
    
        c.cd()
    
        pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
        pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        c.SaveAs( opt.GetResultName( obj ) )

if __name__ == '__main__':
    main()
