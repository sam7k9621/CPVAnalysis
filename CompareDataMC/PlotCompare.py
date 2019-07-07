import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.PlotInfo as info
import CPVAnalysis.CompareDataMC.MakeHist as input
from decimal import Decimal

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "opt").AddInput("r", "region")
    opt.AddFlag("p","pileup").AddFlag( "d", "driven" ).AddFlag( "b", "0bjet" ).AddFlag( "i", "ISO" )
    
    opt.SetName( "chi2", "uncertainty", "opt", "region", "pileup", "0bjet", "ISO" )
    opt.Parsing() 
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs13", "had_tmass", "lep_tmass", "chi2", "LBJetPt", "HBJetPt", "LJetPt", "LJetEta", "LepPt", "LepEta", "LepIso", "nVtx", "Rho" ]
    for sample in input.samplelst:
        histmgr.SetObjlst( opt.GetFileName( sample ), objlst, sample )

    if opt.GetOption( "driven" ):
        qcd_histlst = [ histmgr.GetMergedObj( "QCD" ) for o in objlst ]
        histmgr.RemoveObj( "QCD" )
        histmgr.SetObjlst( opt.GetFileName( "Data" ).replace("_WJets", "_QCD"), objlst, "QCD" )
        opt.SetName( "driven" )

    # Loop objlst
    for obj in objlst:

        c = pltmgr.NewCanvas( obj )
        leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
        bg = ROOT.THStack()
        # Initiailze hist
        data = histmgr.GetObj( "Data" )
        histlst = []
        for i, mc in enumerate( info.mclst ):
            histlst.append( histmgr.GetMergedObj( mc ) )
            histlst[i].SetLineColor( pltmgr.colorlst[i] )
            histlst[i].SetFillColor( pltmgr.colorlst[i] )
            bg.Add( histlst[i] )
            leg.AddEntry( histlst[i], mc, "F" )
            if mc == "QCD" and opt.GetOption( "driven" ):
                pltmgr.SetNormToUnity( histlst[ i ] )
                histlst[ i ].Scale( qcd_histlst[i].Integral() )
        leg.AddEntry( data, "Data", "le" )


        bg_sum = pltmgr.SumHist( histlst )
        top = pltmgr.NewTopPad()
        top.Draw()
        top.cd()
  
        bg.Draw("HIST")
        data.Draw( "EP same" )
        leg.Draw()
    
        bg.GetYaxis().SetTitle( "Events" ) 
        bg.SetMaximum( pltmgr.GetHistYmax( data ) * 1.5 );
        pltmgr.SetTopPlotAxis( bg )
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
        pltmgr.DrawLuminosity( 41540 )
        pltmgr.DrawEntryLeft( opt.Entry() )
        c.SaveAs( opt.GetResultName( obj ) )
        
        top.SetLogy( True )
        c.SaveAs( opt.GetResultName( obj + "_logy" ) )


if __name__ == '__main__':
    main()
