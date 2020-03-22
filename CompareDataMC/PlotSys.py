import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import importlib
import numpy as np
import math 

def GetBinError( mgr, unclst, nom, sample ):
    err_lst, unc_pdf, unc_muFmuR = [], [], []
    # systematically
    Nbins = nom.GetNcells()
    for u in unclst:
        # if "PDF" in u:
            # unc_pdf.append( mgr.GetMergedObj( sample, u + "." ) )
            # print u, unc_pdf[-1].Integral()
        # elif "muFmuR" in u:
            # unc_muFmuR.append( mgr.GetMergedObj( sample, u + "." ) )
            # print u, unc_muFmuR[-1].Integral()
        # else:
       unc = mgr.GetMergedObj( sample, u )
       # print u
       # print [ unc.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( 8, 10 ) ]
       err_lst.append( [ unc.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( Nbins ) ] )

    # max_pdf    = max( unc_pdf, key = lambda x: x.Integral() )
    # min_pdf    = min( unc_pdf, key = lambda x: x.Integral() )
    # max_muFmuR = max( unc_muFmuR, key = lambda x: x.Integral() )
    # min_muFmuR = min( unc_muFmuR, key = lambda x: x.Integral() )
    # print "pdf_up", max_pdf.Integral()
    # print [ max_pdf.GetBinContent( i ) - nom.GetBinContent( i )    for i in range( 15, 18 ) ]
    # print "pdf_dn", min_pdf.Integral()
    # print [ min_pdf.GetBinContent( i ) - nom.GetBinContent( i )    for i in range( 15, 18 ) ]
    # print "mufmur_up"
    # print [ max_muFmuR.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( 15, 18 ) ]
    # print "mufmur_up"
    # print [ min_muFmuR.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( 15, 18 ) ]
    
    # err_lst.append( [ max_pdf.GetBinContent( i ) - nom.GetBinContent( i )    for i in range( Nbins ) ] )
    # err_lst.append( [ min_pdf.GetBinContent( i ) - nom.GetBinContent( i )    for i in range( Nbins ) ] )
    # err_lst.append( [ max_muFmuR.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( Nbins ) ] )
    # err_lst.append( [ min_muFmuR.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( Nbins ) ] )

    return err_lst

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "opt").AddInput("r", "region")
    opt.AddFlag( "d", "driven" ).AddFlag( "b", "0bjet" ).AddFlag( "p", "wopileup" ).AddFlag( "w", "wobtag" )
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "uncertainty", "opt", "region", "wopileup", "wobtag" )
    opt.AddOutputName( "chi2", "uncertainty", "opt", "region", "wopileup", "wobtag", "driven" )
   
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    region = opt.GetOption( "region" ).split("_")[0]
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
    objlst=[ "had_tmass" ]
    # objlst=[ "had_tmass", "lep_tmass", "chi2", "LBJetPt", "HBJetPt", "LJetPt", "LBJetEta", "HBJetEta", "LJetEta", "LepPt", "LepEta", "LepIso", "Njets" ]
    mclst  = [ "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttbar" ]
    unclst = [ "DYJets", "WJets", "ttbar" ] if region == "WJets" else []
    syslst = input.uncertainty 
    
    if opt.GetOption( "wobtag" ):
        syslst = [ x for x in syslst if "btag" not in x ]

    if not region:
        unclst.append( "ttbar" )
        mclst.remove( "QCD" )
    else:
        mclst.remove( region )
        mclst.append( region ) # Dominant sample should be added last
   
    for sample in input.samplelst:
        filename = opt.GetInputName( sample )
       
        histmgr.SetObjlst( filename, objlst, sample + "_nom" )
     
        if any( s in sample for s in unclst ):
            for unc in syslst: 
                if region:
                    histmgr.SetObjlst( filename.replace( "_wobtag.root", "_uncertainty_{}_wobtag.root".format( unc ) ), objlst, sample + "_" +  unc + "." )
                else:
                    histmgr.SetObjlst( opt.GetInputName( "ttbar" ).replace( ".root", "_uncertainty_{}.root".format( unc ) ), objlst, sample + "_" + unc + "." ) 

    if opt.GetOption( "driven" ):
        qcd_histlst = [ histmgr.GetMergedObj( "QCD", "nom" ) for o in objlst ]
        histmgr.RemoveObj( "QCD" )
        histmgr.SetObjlst( opt.GetInputName( "Data" ).replace( opt.GetOption( "region", True ) + opt.GetOption( "wobtag", True ), "_region_QCD_0b_wobtag"), objlst, "QCD_nom" )
        opt.AddInputName( "driven" )

    c = pltmgr.NewCanvas( )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        err_lst = []
        leg = pltmgr.NewLegend( 0.67, 0.41, 0.8, 0.81)
        bg = ROOT.THStack()
        # Initiailze hist
        data = histmgr.GetObj( "Data_nom" )
        histlst = []
        for i, mc in enumerate( mclst ):
            
            histlst.append( histmgr.GetMergedObj( mc, "nom" ) )
            histlst[i].SetLineColor( pltmgr.colorlst[ i ] )
            histlst[i].SetFillColor( pltmgr.colorlst[ i ] )
            bg.Add( histlst[i] )
            leg.AddEntry( histlst[i], mc, "F" )
         
            if mc == "QCD" and opt.GetOption( "driven" ):
                pltmgr.SetNormToUnity( histlst[ i ] )

            if any( s in mc for s in unclst ):
                err_lst.extend( GetBinError( histmgr, syslst, histlst[i], mc ) )

        # Get error square 
        pos, neg = [], []
        err_array = np.array( err_lst )
        for lst in err_array.T:
            pos.append( sum( map(lambda n: n**2 if n > 0 else 0, lst) ) )
            neg.append( sum( map(lambda n: n**2 if n < 0 else 0, lst) ) )

        print"*" * 90

        bg_sum   = pltmgr.SumHist( histlst )
        nom      = [ bg_sum.GetBinContent( i ) for i in range( bg_sum.GetNcells() ) ]
        pos = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, pos, nom )
        neg = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, neg, nom )

        bg_total = bg_sum.Integral()
        da_total = data.Integral()
        qcd_evt  = da_total - bg_total 
        if opt.GetOption( "driven" ):
            for h in histlst:
                if "Data" in h.GetName():
                    bg_total += qcd_evt
                    h.Scale( qcd_evt ) 
        
        if idx == 0:
            print "{:<10}: {:10.2f}".format( "Data", da_total )
            print "{:<10}: {:10.2f}".format( "MC",   bg_total )
            for i, mc in enumerate( mclst ):
                print "{:<10}: {:10.2f} {:5.2f}%".format( mc, histlst[i].Integral(), 100 * histlst[i].Integral() / bg_total ) 

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
        data.SetMaximum( max( pltmgr.GetHistYmax( bg_sum ), pltmgr.GetHistYmax( data ) ) * 1.5 )
        
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
  
        err = pltmgr.GetErrRatioPlot( rel, pos, neg )
        leg.AddEntry( err, "Syst. uncert.", "F" )
        rel.Draw( "EP" )
        err.Draw( "E2 same" )
        upper.Draw( "same" )
        lower.Draw( "same" )
        line.Draw ( "same" )
        rel.Draw( "sameaxis" )
        rel.Draw( "EP same" )
    
        line.SetLineColor( ROOT.kRed )
        line.SetLineStyle( 3  )
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
