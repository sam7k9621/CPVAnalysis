import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import importlib
from decimal import Decimal

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "opt").AddInput("r", "region")
    opt.AddFlag( "d", "driven" ).AddFlag( "b", "0bjet" ).AddFlag( "i", "ISO" ).AddFlag( "p", "wopileup" ).AddFlag( "w", "wobtag" )
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "uncertainty", "opt", "region", "0bjet", "ISO", "wopileup", "wobtag")
    opt.AddOutputName( "chi2", "uncertainty", "opt", "region", "0bjet", "ISO", "wopileup", "wobtag" )
    
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ 
            # "JetFlavor", "bweight", "bsf",
            "DeepCSV", 
            "LJetPt0", "LJetPt1", "LJetPt2", "LJetPt3", "LJetPt4", "LJetPt5", "LJetPt6",
            "LJetEta0", "LJetEta1", "LJetEta2", "LJetEta3", "LJetEta4", "LJetEta5", "LJetEta6",
            "had_tmass", "lep_tmass", "LBJetPt", "HBJetPt", "Njets", 
            ]
    # objlst=[ "lep_tmass" ]
    # objlst=[ "had_tmass", "lep_tmass", "nVtx" ]
    # objlst=[ "had_tmass", "lep_tmass", "chi2", "LBJetPt", "HBJetPt", "LJetPt", "LJetEta", "LepPt", "LepEta", "LepIso", "nVtx", "Rho", "Njets" ]
    mclst = [ "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttbar" ]
    region = opt.GetOption( "region" )
    if not region:
        mclst.remove( "QCD" )
    else:
        mclst.remove( region )
        mclst.append( region ) # Dominant sample should be added last
    
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
    
    for sample in input.samplelst:
        histmgr.SetObjlst( opt.GetInputName( sample ), objlst, sample )

    if opt.GetOption( "driven" ):
        qcd_histlst = [ histmgr.GetMergedObj( "QCD" ) for o in objlst ]
        histmgr.RemoveObj( "QCD" )
        histmgr.SetObjlst( opt.GetInputName( "Data" ).replace("_WJets", "_QCD_0bjet").replace( "_wobtag", ""), objlst, "QCD" )
        opt.AddInputName( "driven" )

    c = pltmgr.NewCanvas( )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
        bg = ROOT.THStack()
        # Initiailze hist
        data = histmgr.GetObj( "Data" )
        histlst = []
        for i, mc in enumerate( mclst ):
            histlst.append( histmgr.GetMergedObj( mc ) )
            histlst[i].SetLineColor( pltmgr.colorlst[i] )
            histlst[i].SetFillColor( pltmgr.colorlst[i] )
            bg.Add( histlst[i] )

            leg.AddEntry( histlst[i], mc, "F" )
            if mc == "QCD" and opt.GetOption( "driven" ):
                pltmgr.SetNormToUnity( histlst[ i ] )
                histlst[ i ].Scale( qcd_histlst[ idx ].Integral() )

        print"*" * 90
        total = sum( [ x.Integral() for x in histlst ] )
        print "{:<10}: {:10.2f}".format( "Data", data.Integral() )
        print "{:<10}: {:10.2f}".format( "MC",   total )
        for i, mc in enumerate( mclst ):
            print "{:<10}: {:10.2f} {:5.2f}%".format( mc, histlst[i].Integral(), 100 * histlst[i].Integral() / total ) 

        leg.AddEntry( data, "Data", "le" )

        bg_sum = pltmgr.SumHist( histlst )
        top = pltmgr.NewTopPad()
        top.Draw()
        top.cd()
 
        data.Draw("EP")
        bg.Draw("HIST same")
        data.Draw( "sameaxis" )
        data.Draw( "EP same" )
        leg.Draw()
    
        data.GetYaxis().SetTitle( "Events" ) 
        data.SetMaximum( max( pltmgr.GetHistYmax( bg_sum ), pltmgr.GetHistYmax( data ) ) * 1.5 );
        
        pltmgr.SetTopPlotAxis( data )
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
        
        # ROOT.gPad.RedrawAxis()
        c.SaveAs( opt.GetOutputName( obj ) )
       
        
        # top.SetLogy( True )
        # c.SaveAs( opt.GetResultName( obj + "_logy" ) )


if __name__ == '__main__':
    main()
