import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
def Plot2D( hist ):
    c = pltmgr.NewCanvas()
    
    hist.Draw( "COLZ" )

    pltmgr.SetAxis( hist )
    pltmgr.SetSinglePadWithPalette( c )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryLeft( opt.Entry() )
    c.SaveAs( opt.GetResultName( "2D_chi2_tmass" ) )

def PlotIntegral( correct, misid, mistag ):
    
    cor_eff = ROOT.TH1D( "cor_eff", "", 200, 0, 200 )
    tag_eff = ROOT.TH1D( "tag_eff", "", 200, 0, 200 )
    ide_eff = ROOT.TH1D( "ide_eff", "", 200, 0, 200 )
    eve_eff = ROOT.TH1D( "eve_eff", "", 200, 0, 200 )

    total = correct.Integral() + misid.Integral() + mistag.Integral()
    for i in range( 201 ):
        cor = correct.Integral( 1, i )
        ide = misid.Integral( 1, i )
        tag = mistag.Integral( 1, i )
        sum = cor + ide + tag

        cor_eff.SetBinContent( i, float( cor / sum ) )
        tag_eff.SetBinContent( i, float( tag / sum ) )
        ide_eff.SetBinContent( i, float( ide / sum ) )

        eve_eff.SetBinContent( i, float( sum / total ) )

    c   = pltmgr.NewCanvas()
    leg = pltmgr.NewLegend( 0.7, 0.6, 0.92, 0.87 )
    leg.SetTextSize( 16 )
    leg.SetFillColorAlpha( 0, 1 )
    cor_eff.Draw( "hist" )
    tag_eff.Draw( "hist same" )
    ide_eff.Draw( "hist same" )
    eve_eff.Draw( "hist same" )
    leg.Draw()

    cor_eff.SetStats( 0 )
    cor_eff.GetXaxis().SetTitle( "#chi^{2}_{min} upper cut" )
    cor_eff.GetYaxis().SetTitle( "Rate" )

    cor_eff.SetLineColor( pltmgr.Green)
    tag_eff.SetLineColor( pltmgr.Magenta)
    ide_eff.SetLineColor( pltmgr.Cyan)
    eve_eff.SetLineColor( pltmgr.Blue)

    cor_eff.SetLineWidth( 2 )
    tag_eff.SetLineWidth( 2 )
    ide_eff.SetLineWidth( 2 )
    eve_eff.SetLineWidth( 2 )

    leg.AddEntry( cor_eff, "Correct",          "l" )
    leg.AddEntry( tag_eff, "Mistag",           "l" )
    leg.AddEntry( ide_eff, "b-Misidentified",  "l" )
    leg.AddEntry( eve_eff, "Event efficiency", "l" )
    
    line = ROOT.TLine( 20, 0, 20, 1 )
    line.Draw("same")
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )
    line.SetLineStyle( 7 )

    cor_eff.SetMaximum( 1. )
    cor_eff.SetMinimum( 0. )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( cor_eff )

    c.SetGrid()
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( opt.Entry() )
    c.SaveAs( opt.GetResultName( "Chi2_uppercut" ) )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddFlag("B", "bbSep")
    opt.Parsing("MatchType") 

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "chi2_tmass", "chi2_Correct", "chi2_Misid", "chi2_Mistag" ]
    histmgr.SetObjlst( opt.GetFileName( "TTbar" ), objlst )
    
    # Plot 2D_chi2_tmass
    Plot2D( histmgr.GetObj("TTbar") )

    # Plot objlst
    correct = histmgr.GetObj( "TTbar" )
    misid   = histmgr.GetObj( "TTbar" )
    mistag  = histmgr.GetObj( "TTbar" )
    
    PlotIntegral( correct, misid, mistag )
    
    c   = pltmgr.NewCanvas()
    leg = pltmgr.NewLegend( 0.7, 0.65, 0.83, 0.87 )

    correct.Draw( "hist" )
    misid.Draw( "hist same" )
    mistag.Draw( "hist same" )
    leg.Draw()

    correct.SetLineColor( pltmgr.Azure )
    misid.SetLineColor  ( pltmgr.Green )
    mistag.SetLineColor ( pltmgr.Red )

    correct.SetLineWidth( 2 )
    misid.SetLineWidth( 2 )
    mistag.SetLineWidth( 2 )

    pltmgr.SetNormToUnity( correct )
    pltmgr.SetNormToUnity( misid )
    pltmgr.SetNormToUnity( mistag )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( correct )
    correct.SetMaximum( 1. )

    leg.AddEntry( correct, "Correct",         "l" )
    leg.AddEntry( misid,   "b-Misidentified", "l" )
    leg.AddEntry( mistag,  "Mistag",          "l" )

    c.SetLogy( True )
    pltmgr.DrawEntryRight( opt.Entry() )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( "Rate_PDF" ) )

if __name__ == '__main__':
    main()
