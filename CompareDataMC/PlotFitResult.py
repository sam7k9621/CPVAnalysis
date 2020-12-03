import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import math, importlib
import numpy as np
import ROOT
def GetBinError( nom, unclst ):
    err_lst = []
    Nbins = nom.GetNcells()
    for u in unclst:
        err_lst.append( [ u.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( Nbins ) ] )

    return err_lst

def main() :
    # Initialize parsing manager
    global opt
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("p", "pull" ).AddInput( "u", "uncertainty" )
    opt.Parsing() 
    opt.AddInputName ( "chi2" )
    opt.AddOutputName( "chi2", "opt", "uncertainty" )
   
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "lep_tmass" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]

    # Declare variable
    # ROOT.gErrorIgnoreLevel = ROOT.kWarning
    # ROOT.RooMsgService.instance().setGlobalKillBelow( ROOT.RooFit.ERROR )
    # ROOT.RooMsgService.instance().setSilentMode( True );
    
    x = ROOT.RooRealVar( "x", "x", 0, 300 )
    x.setBins( 60 )
    l = ROOT.RooArgList(x)

    pos_stat_dict, neg_stat_dict = {}, {}
    pos_syst_dict, neg_syst_dict = {}, {}
    hist_dalst, hist_sglst, hist_bglst = [], [], []
    nsgerrlst, nbgerrlst, covlst = [], [], []

    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ))
        # Add Constraint template
        print "-"*90
        print ">> Adding constraint template"
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ), objlst, "CT_{}".format( year ) )
        hist_da = histmgr.GetObj( "CT_{}".format( year ) )
        data    = ROOT.RooDataHist( "data", "data", l, hist_da )
        
        # Add background template 
        print "-"*90
        print ">> Adding background template"
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ).replace( ".root", "_region_WJets_0b_wobtag.root" ), objlst, "BT_{}".format( year ) )
        hist_bg = histmgr.GetObj( "BT_{}".format( year ) )
        bg      = ROOT.RooDataHist( "bg", "bg", l, hist_bg )
        bg_pdf  = ROOT.RooHistPdf( "bg_pdf", "bg_pdf", ROOT.RooArgSet(x), bg )
        
        # Add signal template
        print "-"*90
        print ">> Adding signal template"
        histmgr.SetObjlst( opt.GetInputName( "ttbar" ).format( Y=year ), objlst, "ST_nom_{}".format( year ) ) 
        hist_sg = histmgr.GetObj( "ST_nom_{}".format( year ) )
        sg      = ROOT.RooDataHist( "sg", "sg", l, hist_sg )
        sg_pdf  = ROOT.RooHistPdf( "sg_pdf", "sg_pdf", ROOT.RooArgSet(x), sg )

        histlst_sg = []
        for unc in input.uncertainty:
            histmgr.SetObjlst( opt.GetInputName( "ttbar" ).format( Y=year ).replace(".", "_uncertainty_{}.".format( unc ) ), objlst, "ST_{}".format( unc ) ) 
            hist_sg_unc = histmgr.GetObj( "ST_{}".format( unc ) )
            sg_unc      = ROOT.RooDataHist( "sg_unc", "sg_unc", l, hist_sg_unc )
            sg_unc_pdf  = ROOT.RooHistPdf( "sg_unc_pdf", "sg_unc_pdf", ROOT.RooArgSet(x), sg_unc )

            nsg_unc = ROOT.RooRealVar( "nsg_unc", "nsg_unc", 300000, 100000, 1000000 )
            nbg_unc = ROOT.RooRealVar( "nbg_unc", "nbg_unc", 40000, 0, 100000 )
            model   = ROOT.RooAddPdf( "model", "model", ROOT.RooArgList( sg_unc_pdf, bg_pdf ), ROOT.RooArgList( nsg_unc, nbg_unc ) )  
            result_unc = model.fitTo( data, ROOT.RooFit.Save(), ROOT.RooFit.Extended() )

            histlst_sg.append( hist_sg_unc.Clone() )
            pltmgr.SetNormToUnity( histlst_sg[-1] )
            histlst_sg[-1].Scale( nsg_unc.getVal() )

        nsg_nom = ROOT.RooRealVar( "nsg_nom", "nsg_nom", 300000, 100000, 1000000 )
        nbg_nom = ROOT.RooRealVar( "nbg_nom", "nbg_nom", 40000, 0, 100000 )
        model   = ROOT.RooAddPdf( "model", "model", ROOT.RooArgList( sg_pdf, bg_pdf ), ROOT.RooArgList( nsg_nom, nbg_nom ) )  
        result_nom = model.fitTo( data, ROOT.RooFit.Save(), ROOT.RooFit.Extended() )
 
        # Get Chi/dof
        chi2plot = x.frame()
        data.plotOn( chi2plot, ROOT.RooFit.Name( "data" ) )
        model.plotOn( chi2plot, ROOT.RooFit.Name( "pdf" ) )
        freeparams = result_nom.floatParsFinal().getSize() 
        chi2overdof = chi2plot.chiSquare("pdf", "data", freeparams )
        print "Chi2/dof", chi2overdof 

        # Uncertainty measurement
        nsg_err  = nsg_nom.getError()
        nbg_err  = nbg_nom.getError()
        cov_err  = result_nom.correlation( "nsg_nom", "nbg_nom" ) * nsg_err * nbg_err
        stat_err = math.sqrt( nsg_err**2 + nbg_err**2 + 2 * cov_err ) / ( nsg_nom.getVal() + nbg_nom.getVal() )
       
        pltmgr.SetNormToUnity( hist_sg )
        pltmgr.SetNormToUnity( hist_bg )
        hist_sg_err = hist_sg.Clone()
        hist_bg_err = hist_bg.Clone()
        hist_sg.Scale( nsg_nom.getVal() )
        hist_bg.Scale( nbg_nom.getVal() )
        hist_sg_err.Scale( nsg_nom.getVal() + nsg_err )
        hist_bg_err.Scale( nbg_nom.getVal() + nbg_err )

        mass = int( opt.GetOption( "opt" ) )
        nsgerrlst.append( hist_sg_err.Integral( 1, hist_sg_err.FindBin( mass ) - 1 ) - hist_sg.Integral( 1, hist_sg.FindBin( mass ) - 1 ) )
        nbgerrlst.append( hist_bg_err.Integral( 1, hist_bg_err.FindBin( mass ) - 1 ) - hist_bg.Integral( 1, hist_bg.FindBin( mass ) - 1 ) )
        covlst.append( result_nom.correlation( "nsg_nom", "nbg_nom" ) * nsgerrlst[-1] * nbgerrlst[-1] )

        err_lst = []
        err_lst.extend( GetBinError( hist_sg, histlst_sg ) )
        pos, neg = [], []
        err_array = np.array( err_lst )
        for lst in err_array.T:
            pos.append( sum( map(lambda n: n**2 if n > 0 else 0, lst) ) )
            neg.append( sum( map(lambda n: n**2 if n < 0 else 0, lst) ) )

        mc_sum = hist_sg + hist_bg 
        nom    = [ mc_sum.GetBinContent( i ) for i in range( mc_sum.GetNcells() ) ]
        pos_stat_dict[ year ] = map( lambda x, y: x + (stat_err*y)**2, pos, nom ) 
        neg_stat_dict[ year ] = map( lambda x, y: x + (stat_err*y)**2, neg, nom ) 
        pos_syst_dict[ year ] = pos 
        neg_syst_dict[ year ] = neg
   
        hist_sglst.append( hist_sg )
        hist_bglst.append( hist_bg )
        hist_dalst.append( hist_da )

    hist_da = pltmgr.SumHist( hist_dalst )
    hist_sg = pltmgr.SumHist( hist_sglst )
    hist_bg = pltmgr.SumHist( hist_bglst )

    mc_sum = hist_sg + hist_bg
    nom    = [ mc_sum.GetBinContent( i ) for i in range( mc_sum.GetNcells() ) ]
    pos_stat = [ math.sqrt( sum(x) ) for x in zip( *pos_stat_dict.values() ) ] 
    neg_stat = [ math.sqrt( sum(x) ) for x in zip( *neg_stat_dict.values() ) ] 
    pos_syst = [ sum(x) for x in zip( *pos_syst_dict.values() ) ] 
    neg_syst = [ sum(x) for x in zip( *neg_syst_dict.values() ) ]
    pos_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, pos_syst, nom ) 
    neg_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, neg_syst, nom ) 

    # Hist preparing
    mass = int( opt.GetOption( "opt" ) )
    dat_val = hist_da.Integral( 1, hist_da.FindBin( mass ) - 1 )
    nsg_val = hist_sg.Integral( 1, hist_sg.FindBin( mass ) - 1 )
    nbg_val = hist_bg.Integral( 1, hist_sg.FindBin( mass ) - 1 )
    
    nsg_err = math.sqrt( sum( [ x**2 for x in nsgerrlst ] ) )
    nbg_err = math.sqrt( sum( [ x**2 for x in nbgerrlst ] ) )
    fra_err = math.sqrt( sum( [ nsg_val**2 * nbgerr**2 + nbg_val**2 * nsgerr**2 - 2 * nsg_val * nbg_val * cov for nsgerr, nbgerr, cov in zip( nsgerrlst, nbgerrlst, covlst ) ] ) ) /  ( nsg_val + nbg_val )**2
    info = """
    Data events: {:0.1f}
    Fitted sg events after cut: {:0.1f} {:0.1f}
    Fitted bg events after cut: {:0.1f} {:0.1f}
    Signal fraction after cut : {:0.1f}% {:0.1f}
    """.format( 
            dat_val, \
            nsg_val, nsg_err, \
            nbg_val, nbg_err, \
            nsg_val / ( nsg_val + nbg_val ) * 100, fra_err * 100 
            )
    tag = "lep_tmass"
    with open( opt.GetOutputName( tag, "FitResult", "txt" ).replace( "mixed", "mixed_{}".format( i  ) ), "w" ) as outputfile:
        outputfile.write( info )

    # Plot setting
    c = pltmgr.NewCanvas( "" )
    
    mc = ROOT.THStack()
    hist_sg.SetLineColor( 1 )
    hist_bg.SetLineColor( 1 )
    hist_sg.SetLineWidth( 1 )
    hist_bg.SetLineWidth( 1 )
    hist_sg.SetFillColor( pltmgr.Red )
    hist_bg.SetFillColor( ROOT.kGreen - 6 )
    mc.Add( hist_bg )
    mc.Add( hist_sg )
    
    stat = pltmgr.GetErrRatioPlot( mc_sum, pos_stat, neg_stat, True )
    stat.SetLineColor( ROOT.kGray + 3 ) 
    stat.SetFillColor( ROOT.kGray + 3 )
    stat.SetFillStyle( 3001 )

    top = pltmgr.NewTopPad()
    top.Draw()
    top.cd()
    
    hist_da.Draw( "EP" )
    mc.Draw( "HIST same" )
    stat.Draw( "E2 same" )
    hist_da.Draw( "sameaxis" )
    hist_da.Draw( "EP same" )
   
    leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
    leg.AddEntry( hist_da, "Data", "LE" )
    leg.AddEntry( hist_sg, "Fitted t#bar{t}",   "F" )
    leg.AddEntry( hist_bg, "Fitted background", "F" )
    leg.AddEntry( stat,    "Fit unc. #oplus Syst.", "F" )
    leg.Draw()
    
    hist_da.SetLineColor( 1 )
    hist_da.SetLineWidth( 1 )
    hist_da.SetMarkerSize( 0.6 )
    hist_da.SetMarkerStyle( 20 )
    hist_da.SetMaximum( pltmgr.GetHistYmax( hist_da ) * 1.5 )
    
    top.Update()
    pltmgr.SetTopPlotAxis( hist_da )
    
    c.cd()
    
    bot = pltmgr.NewBottomPad()
    bot.Draw()
    bot.cd()
    
    xmin = hist_da.GetXaxis().GetXmin()
    xmax = hist_da.GetXaxis().GetXmax()
    
    line  = ROOT.TLine( xmin, 1.0, xmax, 1.0 )
    upper = ROOT.TLine( xmin, 1.2, xmax, 1.2 )
    lower = ROOT.TLine( xmin, 0.8, xmax, 0.8 )
    rel   = pltmgr.DivideHist( hist_da, mc_sum, 1 )
    
    rel.Draw( "E0 P" )

    syst = pltmgr.GetErrRatioPlot( rel, pos_syst, neg_syst )
    syst.Draw( "E2 same" )
    upper.Draw( "same" )
    lower.Draw( "same" )
    line.Draw ( "same" )
    rel.Draw( "EP same" )
    rel.Draw( "sameaxis" )
   
    leg.AddEntry( syst, "Syst.", "F" )
    syst.SetFillColor( ROOT.kAzure - 9 )
    syst.SetLineColor( ROOT.kAzure - 9 )
    
    line.SetLineColor( ROOT.kRed )
    line.SetLineStyle( 3 )
    line.SetLineWidth( 2 )
    upper.SetLineStyle( 3 )
    lower.SetLineStyle( 3 )
    
    rel.SetMaximum( 1.25 )
    rel.SetMinimum( 0.75 )
    rel.GetYaxis().SetTitle( "Data/MC" )
    rel.GetXaxis().SetTitle( hist_da.GetXaxis().GetTitle() )
    
    bot.Update()
    pltmgr.SetBottomPlotAxis( rel )
    
    c.cd()
    
    pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
    pltmgr.DrawEntryLeft( opt.Entry() )
    pltmgr.DrawLuminosity( opt.Lumi() )
    
    c.SaveAs( opt.GetOutputName( "lep_tmass","FitResult") )
    


if __name__== '__main__':
    main()
