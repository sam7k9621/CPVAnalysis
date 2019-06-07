import ROOT
import math
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

def EffHist( num, den ):
    numm = num.Clone()
    denn = den.Clone()
    denn.Add( numm )
    numm.Divide( denn )
    
    for i in range( num.GetNcells() ):
        N = denn.GetBinContent( i )
        p = numm.GetBinContent( i )

        if N == 0:
            numm.SetBinContent( i, 0 )
            continue
        
        numm.SetBinError( i, math.sqrt( p * ( 1-p ) / N ) )
    
    return numm

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

    total = correct.Integral( 1, 201 ) + misid.Integral( 1, 201 ) + mistag.Integral( 1, 201 )
    print total

    for i in range( 202 ):
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

def OptLeptmass( cor, misid, mistag ):
    
    c   = pltmgr.NewCanvas()
    leg = pltmgr.NewLegend( 0.68, 0.57, 0.8, 0.87 )
    leg.SetTextSize( 16 )
    leg.SetFillColorAlpha( 0, 1 )

    cor.Draw("hist")
    misid.Draw("hist same")
    mistag.Draw("hist same")
    leg.Draw()

    cor   .SetLineColor( pltmgr.Green )
    misid .SetLineColor( pltmgr.Magenta )
    mistag.SetLineColor( pltmgr.Cyan ) 
    cor   .SetLineWidth( 2 )
    misid .SetLineWidth( 2 )
    mistag.SetLineWidth( 2 )

    leg.AddEntry( cor,    "Correct",   "l" )
    leg.AddEntry( misid,  "Mistidentified", "l" )
    leg.AddEntry( mistag, "Mistag", "l" )
    
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( cor )
    cor.SetMaximum( pltmgr.GetHistYmax( cor ) * 1.1 ) 
    cor.GetYaxis().SetTitle( "Events x 10^{3}" )
    cor.GetXaxis().SetTitle( "M_{lb} [GeV]" )

    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( "Optimisation" ) )

    mistag.Add( misid )
    eff_cor = EffHist( cor, mistag )

    eff_cor.Draw( "hist e" )
    eff_cor.SetLineColor( pltmgr.Cyan )
    eff_cor.SetLineWidth( 2 )
    eff_cor.GetYaxis().SetTitle( "Correct b-tagged rate" )
    eff_cor.GetXaxis().SetTitle( "M_{lb} [GeV]" )
    eff_cor.SetMaximum( 1 )

    leg2 = pltmgr.NewLegend( 0.68, 0.57, 0.8, 0.87 )
    leg2.SetTextSize( 16 )
    leg2.SetFillColorAlpha( 0, 1 )
    leg2.AddEntry( eff_cor, "Correct type", "l" )
    leg2.Draw()
    
    line = ROOT.TLine( 150, 0, 150, 1 )
    line.Draw()
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )
    
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( eff_cor )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( "Correct_eff" ) )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddFlag("B", "bbSep").AddFlag("p", "pileup")
    opt.Parsing("MatchType") 

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "chi2_tmass", "chi2_Correct", "chi2_Misid", "chi2_Mistag", "Cor_leptmass", "Misid_leptmass", "Mistag_leptmass" ]
    histmgr.SetObjlst( opt.GetFileName( "ttbar" ), objlst )
    
    # Plot 2D_chi2_tmass
    Plot2D( histmgr.GetObj("ttbar") )

    # Plot objlst
    correct = histmgr.GetObj( "ttbar" )
    misid   = histmgr.GetObj( "ttbar" )
    mistag  = histmgr.GetObj( "ttbar" )
    
    Cor_leptmas     = histmgr.GetObj( "ttbar" )
    Misid_leptmass  = histmgr.GetObj( "ttbar" )
    Mistag_leptmass = histmgr.GetObj( "ttbar" )

    OptLeptmass( Cor_leptmas, Misid_leptmass, Mistag_leptmass )

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
