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

    pltmgr.SetSinglePadWithPalette( c )
    pltmgr.Set2DAxis( hist )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryLeft( opt.Entry() )
    pltmgr.DrawLuminosity( opt.Lumi() )
    c.SaveAs( opt.GetOutputName( "2D_chi2_tmass", "bbSep" ) )

def PlotIntegral( correct, misid, mistag ):
    
    cor_eff = ROOT.TH1D( "cor_eff", "", 200, 0, 200 )
    tag_eff = ROOT.TH1D( "tag_eff", "", 200, 0, 200 )
    ide_eff = ROOT.TH1D( "ide_eff", "", 200, 0, 200 )
    eve_eff = ROOT.TH1D( "eve_eff", "", 200, 0, 200 )

    total = correct.Integral( 1, 201 ) + misid.Integral( 1, 201 ) + mistag.Integral( 1, 201 )

    for i in range( 202 ):
        cor = correct.Integral( 1, i )
        ide = misid.Integral( 1, i )
        tag = mistag.Integral( 1, i )
        sum = cor + ide + tag

        cor_eff.SetBinContent( i, float( cor / sum ) )
        tag_eff.SetBinContent( i, float( tag / sum ) )
        ide_eff.SetBinContent( i, float( ide / sum ) )

        eve_eff.SetBinContent( i, float( sum / total ) )
   
    
    info = """
    Eff    : {:0.3f}
    Correct: {:0.3f}
    Mistag : {:0.3f}
    Misid  : {:0.3f}
    """.format( 
            eve_eff.GetBinContent( cor_eff.FindBin( 20 ) - 1 ), \
            cor_eff.GetBinContent( cor_eff.FindBin( 20 ) - 1 ), \
            tag_eff.GetBinContent( cor_eff.FindBin( 20 ) - 1 ), \
            ide_eff.GetBinContent( cor_eff.FindBin( 20 ) - 1 )
            ) 
    
    print info

    c   = pltmgr.NewCanvas()
    leg = pltmgr.NewLegend( 0.7, 0.3, 0.9, 0.55 )
    leg.SetTextSize( 14 )
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
    tag_eff.SetLineColor( pltmgr.Orange)
    ide_eff.SetLineColor( pltmgr.Magenta)
    eve_eff.SetLineColor( pltmgr.Azure)

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

    c.SetGrid()
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( cor_eff )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( opt.Entry() )
    c.SaveAs( opt.GetOutputName( "Chi2_uppercut", "bbSep" ) )

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
    mistag.SetLineColor( pltmgr.Orange ) 
    misid .SetLineColor( pltmgr.Magenta )
    cor   .SetLineWidth( 2 )
    misid .SetLineWidth( 2 )
    mistag.SetLineWidth( 2 )

    leg.AddEntry( cor,    "Correct",   "l" )
    leg.AddEntry( misid,  "Mistidentified", "l" )
    leg.AddEntry( mistag, "Mistag", "l" )
    
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( cor )
    cor.SetMaximum( pltmgr.GetHistYmax( cor ) * 1.1 ) 
    
    pltmgr.DrawEntryLeft( opt.Entry() )
    pltmgr.DrawLuminosity( opt.Lumi() )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    c.SaveAs( opt.GetOutputName( "Optimisation", "bbSep" ) )

    mistag.Add( misid )
    eff_cor = EffHist( cor, mistag )

    eff_cor.Draw( "hist e" )
    eff_cor.SetLineColor( pltmgr.Azure )
    eff_cor.SetLineWidth( 2 )
    eff_cor.GetYaxis().SetTitle( "Correct b-tagged rate" )
    eff_cor.SetMaximum( 1 )
    eff_cor.SetMinimum( 0 )

    leg2 = pltmgr.NewLegend( 0.68, 0.57, 0.8, 0.87 )
    leg2.SetTextSize( 16 )
    leg2.SetFillColorAlpha( 0, 1 )
    leg2.AddEntry( eff_cor, "Correct type", "l" )
    leg2.Draw()
    
    line = ROOT.TLine( 150, 0, 150, 1 )
    line.Draw()
    line.SetLineColor( ROOT.kRed )
    line.SetLineStyle( 7 ) 
    line.SetLineWidth( 2 )
    
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( eff_cor )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    c.SaveAs( opt.GetOutputName( "Correct_eff", "bbSep" ) )

def PlotOptIntegral( correct, misid, mistag ):
    cor_eff = ROOT.TH1D( "cor_eff", "", 60, 0, 300 )
    tag_eff = ROOT.TH1D( "tag_eff", "", 60, 0, 300 )
    ide_eff = ROOT.TH1D( "ide_eff", "", 60, 0, 300 )
    eve_eff = ROOT.TH1D( "eve_eff", "", 60, 0, 300 )
    rej_eff = ROOT.TH1D( "ref_eff", "", 60, 0, 300 )

    total_cor = correct.Integral( 1, 61 )
    total = correct.Integral( 1, 61 ) + misid.Integral( 1, 61 ) + mistag.Integral( 1, 61 )
  
    for i in range( 62 ):
        cor = correct.Integral( 1, i )
        ide = misid.Integral( 1, i )
        tag = mistag.Integral( 1, i )
        sum = cor + ide + tag
      
        if sum == 0:
            continue

        cor_eff.SetBinContent( i, float( cor / sum ) )
        tag_eff.SetBinContent( i, float( tag / sum ) )
        ide_eff.SetBinContent( i, float( ide / sum ) )

        eve_eff.SetBinContent( i, float( sum / total ) )
        rej_eff.SetBinContent( i, float( cor / total_cor ) )

    c   = pltmgr.NewCanvas()
    leg = pltmgr.NewLegend( 0.7, 0.35, 0.9, 0.6 )
    leg.SetTextSize( 14 )
    leg.SetFillColorAlpha( 0, 1 )
    cor_eff.Draw( "hist" )
    tag_eff.Draw( "hist same" )
    ide_eff.Draw( "hist same" )
    eve_eff.Draw( "hist same" )
    rej_eff.Draw( "hist same" )
    leg.Draw()

    cor_eff.SetStats( 0 )
    cor_eff.GetXaxis().SetTitle( "M_{lb} [GeV]" )
    cor_eff.GetYaxis().SetTitle( "Rate" )

    cor_eff.SetLineColor( pltmgr.Green)
    tag_eff.SetLineColor( pltmgr.Orange)
    ide_eff.SetLineColor( pltmgr.Magenta)
    eve_eff.SetLineColor( pltmgr.Azure)
    rej_eff.SetLineColor( pltmgr.Red)
    cor_eff.SetLineWidth( 2 )
    tag_eff.SetLineWidth( 2 )
    ide_eff.SetLineWidth( 2 )
    eve_eff.SetLineWidth( 2 )
    rej_eff.SetLineWidth( 2 )

    leg.AddEntry( cor_eff, "Correct",          "l" )
    leg.AddEntry( tag_eff, "Mistag",           "l" )
    leg.AddEntry( ide_eff, "b-Misidentified",  "l" )
    leg.AddEntry( eve_eff, "Event efficiency", "l" )
    
    line = ROOT.TLine( 150, 0, 150, 1 )
    line.Draw("same")
    line.SetLineColor( ROOT.kRed )
    line.SetLineWidth( 2 )
    line.SetLineStyle( 7 )

    cor_eff.SetMaximum( 1. )
    cor_eff.SetMinimum( 0. )

    c.SetGrid()
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( cor_eff )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( opt.Entry() )
    c.SaveAs( opt.GetOutputName( "OptCut", "bbSep" ) )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2")
    opt.AddFlag("B", "bbSep")
    opt.Parsing() 
    opt.AddInputName( "chi2", "bbSep" )
    opt.AddOutputName( "chi2", "bbSep" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "chi2_tmass", "chi2_Correct", "chi2_Misid", "chi2_Mistag", "Cor_leptmass", "Misid_leptmass", "Mistag_leptmass" ]
    
    sample = "ttbar" 
    
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ] 
    for year in yearlst:
        histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ), objlst, "{}_{}".format( sample, year ) )
    
    # Plot 2D_chi2_tmass
    Plot2D( histmgr.GetMergedObj( sample ) )

    # Plot objlst
    correct = histmgr.GetMergedObj( sample )
    misid   = histmgr.GetMergedObj( sample )
    mistag  = histmgr.GetMergedObj( sample )
    
    Cor_leptmas     = histmgr.GetMergedObj( sample )
    Misid_leptmass  = histmgr.GetMergedObj( sample )
    Mistag_leptmass = histmgr.GetMergedObj( sample )

    if opt.GetOption( "chi2" ):
        PlotOptIntegral( Cor_leptmas, Misid_leptmass, Mistag_leptmass )
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
    c.SaveAs( opt.GetOutputName( "Rate_PDF", "bbSep" ) )

if __name__ == '__main__':
    main()
