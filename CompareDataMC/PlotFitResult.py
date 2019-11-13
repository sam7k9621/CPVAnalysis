import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.MakeHist as input
import math
from ROOT import RooRealVar, RooArgList, RooAddPdf, RooDataHist, RooFit, RooHistPdf, RooArgSet, THStack, kRed, kGreen, TLine, RooRandom, RooGaussian, RooDataSet, RooPlot

def GaussFit( pull ):
    p   = RooRealVar( "p", "Pull", -4., 7. )
    mu  = RooRealVar( "mu", "mean value", 0., -3., 3. )
    sig = RooRealVar( "sig", "sig", 1., -3., 3. )

    gaus = RooGaussian( "gaus", "gauss", p, mu, sig )
    pullset = RooDataSet( "pullset", "pullset", RooArgSet( p ) )

    for pu in pull:
        p.setVal( pu )
        pullset.add( RooArgSet( p ) )

    gaus.fitTo( pullset )

    frame = p.frame()
    pullset.plotOn( frame )
    gaus.plotOn( frame )

    c = pltmgr.NewCanvas( "c" )
    frame.Draw()
   
    txt = pltmgr.NewTextBox( .6, .70, .92, .85 )
    txt.SetTextAlign(12)
    txt.AddText( "{:<7}: {} ".format( "Entries", opt.GetOption( "pull" ) ) )
    txt.AddText( "{:<8}: {:0.2f} #pm {:0.2f}".format( "Mu", mu.getVal(), mu.getError() ) )
    txt.AddText( "{:<6}: {:0.2f} #pm {:0.2f}".format( "Sigma", sig.getVal(), sig.getError() ) )
    txt.Draw()

    frame.SetTitle( "" )
    pltmgr.SetAxis( frame )
    pltmgr.SetSinglePad( c )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    c.SaveAs( opt.GetResultName( "Pull", "FitResult" ) )

def main() :
    # Initialize parsing manager
    global opt
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput( "t", "template" ).AddInput( "u", "uncertainty" ).AddInput("p", "pull" )
    opt.SetName( "chi2" )
    opt.Parsing() 
    
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "lep_tmass" ]
    
    # Declare variable
    x = RooRealVar( "x", "x", 0, 500 )
    l = RooArgList(x)

    # Add Constraint template
    print "-"*90
    print ">> Adding constraint template"
    histmgr.SetObjlst( opt.GetFileName( "Data" ), objlst, "CT" )
    h1 = histmgr.GetObj( "CT" )
    data = RooDataHist( "d1", "d1", l, h1 )

    # Add signal template
    print "-"*90
    print ">> Adding signal template"
    histmgr.SetObjlst( opt.GetFileName( "ttbar" ), objlst, "ST" )
    h2 = histmgr.GetObj( "ST" )
    d2 = RooDataHist( "d2", "d2", l, h2 )
    sg = RooHistPdf( "sg", "sg", RooArgSet(x), d2 ) 

    # Add background template 
    print "-"*90
    print ">> Adding background template"
    if opt.GetOption( "template" ):
        filename = opt.GetFileName( "Data" ).replace( ".root", opt.GetOption( "template", True ) + ".root" ).replace( "template", "region" )
        histmgr.SetObjlst( filename, objlst, "BT" )
    else:
        for sample in input.samplelst:
            if any( x in sample for x in [ "ttbar", "Data", "QCD" ] ):
                continue 
            histmgr.SetObjlst( opt.GetFileName( sample ), objlst, "BT_" + sample )
     
    h3 = histmgr.GetMergedObj( "BT" )
    d3 = RooDataHist( "d3", "d3", l, h3 )
    bg = RooHistPdf( "bg", "bg", RooArgSet(x), d3 ) 

    # Construct composite pdf
    nsg   = RooRealVar( "nsg", "nsg", 300000, 100000, 1000000 )
    nbg   = RooRealVar( "nbg", "nbg", 40000, 0, 100000 )
    model = RooAddPdf( "model", "model", RooArgList( sg, bg ), RooArgList( nsg, nbg ) )  

    # Fitting
    model.fitTo( data, RooFit.Extended() )
    result = model.fitTo( data, RooFit.Save(), RooFit.Extended() )

    # Hist preparing 
    h2_nsg = h2.Clone()
    h2_err = h2.Clone()
    h3_err = h3.Clone()
    
    pltmgr.SetNormToUnity( h2 )
    pltmgr.SetNormToUnity( h3 )
    pltmgr.SetNormToUnity( h2_err )
    pltmgr.SetNormToUnity( h3_err )
    
    h2.Scale( nsg.getVal() )
    h3.Scale( nbg.getVal() )
    h2_err.Scale( nsg.getVal() + nsg.getError() )
    h3_err.Scale( nbg.getVal() + nbg.getError() )
    
    mc_sum = h2.Clone()
    mc_sum.Add( h3 )

    # Plot setting
    c = pltmgr.NewCanvas( "" )
    mc = THStack()
    h2.SetLineColor( kRed - 7 )
    h3.SetLineColor( kGreen - 6 )
    h2.SetFillColor( kRed - 7 )
    h3.SetFillColor( kGreen - 6 )
    mc.Add( h3 )
    mc.Add( h2 )

    leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
    leg.AddEntry( h1, "Data", "LE" )
    leg.AddEntry( h2, "Fitted t#bar{t}",   "F" )
    leg.AddEntry( h3, "Fitted background", "F" )
    
    top = pltmgr.NewTopPad()
    top.Draw()
    top.cd()

    mc.Draw( "HIST" )
    h1.Draw( "EP same" )
    leg.Draw()

    h1.SetLineColor( 1 )
    h1.SetLineWidth( 1 )
    h1.SetMarkerSize( 0.5 )
    h1.SetMarkerStyle( 20 )
    
    mc.SetMaximum( pltmgr.GetHistYmax( h1 ) * 1.5 )
    mc.GetYaxis().SetTitle( "Events" );
    mc.GetXaxis().SetTitle( "" );
    
    pltmgr.SetTopPlotAxis( mc )

    c.cd()
    
    bot = pltmgr.NewBottomPad()
    bot.Draw()
    bot.cd()

    xmin = h1.GetXaxis().GetXmin()
    xmax = h1.GetXaxis().GetXmax()
    
    line  = TLine( xmin, 1.0, xmax, 1.0 )
    upper = TLine( xmin, 1.5, xmax, 1.5 )
    lower = TLine( xmin, 0.5, xmax, 0.5 )
    rel   = pltmgr.DivideHist( h1, mc_sum, 1 )
    
    rel.Draw( "EP" )
    upper.Draw( "same" )
    lower.Draw( "same" )
    line.Draw ( "same" )
    
    line.SetLineColor( kRed )
    upper.SetLineStyle( 3 )
    lower.SetLineStyle( 3 )
    
    rel.SetMaximum( 1.6 )
    rel.SetMinimum( 0.4 )
    rel.GetYaxis().SetTitle( "Data/MC" )
    rel.GetXaxis().SetTitle( h1.GetXaxis().GetTitle() )
    pltmgr.SetBottomPlotAxis( rel )
    
    c.cd()
    
    pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
    pltmgr.DrawEntryLeft( opt.Entry() )
    pltmgr.DrawLuminosity( opt.Lumi() )

    tag = "lep_tmass" + opt.GetOption( "template", True ) + opt.GetOption( "uncertainty", True )
    c.SaveAs( opt.GetResultName( tag, "FitResult" ) )

    #-------------------------------------------------------------------------------------------------- 
    dat_val = h1.Integral( 1, h1.FindBin( 150 ) - 1 ) 
    nsg_val = h2.Integral( 1, h2.FindBin( 150 ) - 1 )
    nbg_val = h3.Integral( 1, h3.FindBin( 150 ) - 1 )
    nsg_err = h2_err.Integral( 1, h2_err.FindBin( 150 ) - 1 ) - nsg_val
    nbg_err = h3_err.Integral( 1, h3_err.FindBin( 150 ) - 1 ) - nbg_val
    cov_err = result.correlation( "nsg", "nbg" ) * nsg_err * nbg_err
    sum_err = math.sqrt( nsg_err**2 + nbg_err**2 + 2 * cov_err )
    fra_err = math.sqrt( nsg_val**2 * nbg_err**2 + nbg_val**2 * nsg_err**2 - 2 * nsg_val * nbg_val * cov_err ) / ( nsg_val + nbg_val )**2

    info = """
    Data events: {:0.1f}
    Fitted sg events after cut: {:0.1f} {:0.1f}
    Fitted bg events after cut: {:0.1f} {:0.1f}
    Fitted MC events after cut: {:0.1f} {:0.1f}
    Signal fraction after cut : {:0.1f}% {:0.1f}
    """.format( 
            dat_val, \
            nsg_val, nsg_err, \
            nbg_val, nbg_err, \
            nsg_val + nbg_val, sum_err, \
            nsg_val / ( nsg_val + nbg_val ) * 100, fra_err * 100
            )
    with open( opt.GetResultName( tag, "FitResult", "txt" ), "w" ) as outputfile:
        outputfile.write( info )


  #-------------------------------------------------------------------------------------------------- 
    if not opt.GetOption( "pull" ):
        return 
    Nevt = int( opt.GetOption( "pull" ) )
    pull = []
    ns   = h2_nsg.Integral()  # MC prediction
    nb   = h3.Integral()      # Fitted bkg events
    
    # Method of Expected yield
    for i in range( Nevt ):
        nsevt = RooRandom.randomGenerator().Poisson( ns ) 
        nbevt = RooRandom.randomGenerator().Poisson( nb )
        sgdata = sg.generate( RooArgSet(x), nsevt )
        bgdata = bg.generate( RooArgSet(x), nbevt )
        
        bgdata.append( sgdata )
        model.fitTo( bgdata, RooFit.Extended(), RooFit.NumCPU( 6 ) )
        pull.append( ( nsg.getVal() - ns ) / nsg.getError() ) 

    GaussFit( pull )

if __name__== '__main__':
    main()
