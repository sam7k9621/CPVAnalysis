import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import importlib
import numpy as np
import math 
from decimal import *

def GetBinError( mgr, unclst, nom, sample ):
    err_lst = []
    # systematically
    Nbins = nom.GetNcells()
    for u in unclst:
        unc = mgr.GetMergedObj( sample, u )
        err_lst.append( [ 0. if nom.GetBinContent( i ) < 1. else unc.GetBinContent( i ) - nom.GetBinContent( i ) for i in range( Nbins ) ] )
    return err_lst

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "opt").AddInput("r", "region").AddInput("n", "unc" ).AddInput("f", "WHF").AddInput( "g", "dtg" )
    opt.AddFlag( "d", "driven" ).AddFlag( "b", "0bjet" ).AddFlag( "p", "wopileup" ).AddFlag( "w", "wobtag" ).AddFlag("u", "wosyst" )
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "opt", "region", "wopileup", "wobtag" )
    opt.AddOutputName( "chi2", "unc", "opt", "region", "wopileup", "wobtag", "driven", "WHF" )
  
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    region = opt.GetOption( "region" ).split("_")[0]
    
    # Initialize sample list
    objlst=[ "lep_tmass", "had_tmass", "HBJetPt", "LBJetPt", "LepPt" ]
    # objlst=[ "Obs3_dist", "Obs6_dist", "Obs12_dist", "Obs14_dist" ]
    # objlst=[ "lep_tmass", "had_tmass" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]
    unclst = [] if opt.GetOption( "wosyst" ) else [ "ttbar" ]
    qcd_histdict = {}
    # Initialize MC sample list
    mclst  = [ "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttdilep", "ttbar" ]
    if not region:
        pass
        # mclst.remove( "QCD" )
    else:
        mclst.remove( region )
        mclst.append( region ) # Dominant sample should be added last
   
    # Read files
    for year in yearlst: 
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ) )
        syslst = opt.GetOption( "uncertainty" ).split()  if opt.GetOption( "uncertainty" ) else input.uncertainty
        for sample in input.samplelst:
            if "WJets" in sample and opt.GetOption( "WHF" ) :
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ).replace(".", "_WHF_{}.".format(opt.GetOption("WHF"))), objlst, "{}_{}_nom".format( sample, year ) )
            else:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ), objlst, "{}_{}_nom".format( sample, year ) )
         
            if any( s in sample for s in unclst ):
                for unc in syslst: 
                    histmgr.SetObjlst( opt.GetInputName( "ttbar" ).format( Y=year ).replace( ".", "_uncertainty_{}.".format( unc ) ), objlst, "{}_{}_{}".format( sample, year, unc ) ) 

        if opt.GetOption( "driven" ):
            qcd_histdict[ year ] = [ histmgr.GetMergedObj( "QCD", "nom", year ) for o in objlst ]
            histmgr.RemoveObj( "QCD", year )
            histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ).replace( opt.GetOption( "region", True ) + opt.GetOption( "wobtag", True ), "_region_QCD_0b_wobtag"), objlst, "QCD_{}_nom".format( year ) )

    if opt.GetOption( "dtg" ):
        histmgr.SetObjlst( opt.GetInputName("ttbar_dtg3").format( Y="17" ), objlst, "dtg3" )

    c = pltmgr.NewCanvas( )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        err_lst = []
        datalst = []
        histdict = {}
        leg = pltmgr.NewLegend( 0.72, 0.25, 0.85, 0.81)
        bg = ROOT.THStack()
        
        # Retrieve data plots
        for year in yearlst:
            histdict[year] = []
            datalst.append( histmgr.GetObj( "Data_{}_nom".format( year ) ) )

        # Retrieve MC plots
        for i, mc in enumerate( mclst ):
            for year in yearlst:
                histdict[year].append( histmgr.GetMergedObj( mc, "nom", year ) )
                histdict[year][-1].SetLineColor( pltmgr.colorlst[ i ] )
                histdict[year][-1].SetFillColor( pltmgr.colorlst[ i ] )
                bg.Add( histdict[year][-1] )
 
                if year == yearlst[0]:
                    leg.AddEntry( histdict[year][-1], mc, "F" )

                if mc == "QCD" and opt.GetOption( "driven" ):
                    pltmgr.SetNormToUnity( histdict[year][-1] )
                    histdict[year][-1].Scale( qcd_histdict[year][idx].Integral() )

                if any( s in mc for s in unclst ):
                    err_lst.extend( GetBinError( histmgr, [ "{}_{}".format( year, x ) for x in syslst ], histdict[year][-1], mc ) )

        # Get error square 
        pos, neg = [], []
        err_array = np.array( err_lst )
        for lst in err_array.T:
            pos.append( sum( map(lambda n: n**2 if n > 0 else 0, lst) ) )
            neg.append( sum( map(lambda n: n**2 if n < 0 else 0, lst) ) )
        print"*" * 90

        histlst = sum( [ v for k, v in histdict.iteritems() ], [] )
        bg_sum   = pltmgr.SumHist( histlst )
        data     = pltmgr.SumHist( datalst )

        nom      = [ bg_sum.GetBinContent( i ) for i in range( bg_sum.GetNcells() ) ]
        nom_err  = [ bg_sum.GetBinError( i ) for i in range( bg_sum.GetNcells() ) ]
        if not pos or not neg:
            pos, neg = [ 0 ] * len( nom ), [ 0 ] * len( nom )
       
        pos_stat = map( lambda x, y: math.sqrt( x + y**2 ), pos, nom_err )
        neg_stat = map( lambda x, y: math.sqrt( x + y**2 ), neg, nom_err )
        pos_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, pos, nom )
        neg_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, neg, nom )

        bg_total = bg_sum.Integral()
        da_total = data.Integral()
      
        if obj == "lep_tmass":
            with open( opt.GetOutputName( obj, "ExpectedYield", "txt" ), "w" ) as outputfile:
                outputfile.write( "{}: {:.2f}\n".format( "Data", da_total ) )
                outputfile.write( "{}: {:.2f}\n".format( "MC",   bg_total ) )
                for i, mc in enumerate( mclst ):
                    mc_yield = sum( [ histdict[year][i].Integral() for year in yearlst ] )
                    outputfile.write( "{}: {:.2f} {:.2f}%\n".format( mc, mc_yield, 100 * mc_yield / bg_total ) )

        print pos_stat

        stat = pltmgr.GetErrRatioPlot( bg_sum, pos_stat, neg_stat, True )
        stat.SetLineColor( ROOT.kGray + 3 ) 
        stat.SetFillColor( ROOT.kGray + 3 )
        stat.SetFillStyle( 3002 )
        leg.AddEntry( data, "Data", "le" )
        leg.AddEntry( stat, "Stat." if opt.GetOption( "wosyst" ) else "Stat. #oplus Syst.", "F" )

        top = pltmgr.NewTopPad()
        top.Draw()
        top.cd()

        data.Draw("EP")
        bg.Draw("HIST same")
        stat.Draw("E2 same")

        if opt.GetOption( "dtg" ):
            dtg = histmgr.GetObj( "dtg3" )
            dtg.SetLineStyle( 3 )
            dtg.SetLineWidth( 2 )
            dtg.SetLineColor( ROOT.kRed + 3 )
            dtg.Draw("HIST same")
        
        data.Draw( "sameaxis" )
        data.Draw( "EP same" )
        leg.Draw()

        data.SetMaximum( max( pltmgr.GetHistYmax( bg_sum ), pltmgr.GetHistYmax( data ) ) * 1.5 )
        
        data.SetLineColor( 1 )
        data.SetLineWidth( 1 )
        data.SetMarkerSize( 0.5 )
        data.SetMarkerStyle( 20 )
    
        top.Update()
        pltmgr.SetTopPlotAxis( data )

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

        rel.Draw( "E0 P" )
        if not opt.GetOption( "wosyst" ): 
            syst = pltmgr.GetErrRatioPlot( rel, pos_syst, neg_syst )
            syst.Draw( "E2 same" )
            syst.SetFillColor( ROOT.kAzure - 9 )
            syst.SetLineColor( ROOT.kAzure - 9 )
            leg.AddEntry( syst, "Syst.", "F" )
       
        if opt.GetOption( "dtg" ):
            leg.AddEntry( 0, "", "");
            leg.AddEntry( dtg, "d^{t}_{g}=3", "L" )

        upper.Draw( "same" )
        lower.Draw( "same" )
        line.Draw ( "same" )
        rel.Draw( "EP same" )
        rel.Draw( "sameaxis" )
    
        line.SetLineColor( ROOT.kRed )
        line.SetLineStyle( 3  )
        upper.SetLineStyle( 3 )
        lower.SetLineStyle( 3 )
    
        rel.SetMaximum( 1.6 )
        rel.SetMinimum( 0.4 )
        rel.GetYaxis().SetTitle( "Data/MC" )
        rel.GetXaxis().SetTitle( data.GetXaxis().GetTitle() )
        
        bot.Update()
        pltmgr.SetBottomPlotAxis( rel )
    
        c.cd()
    
        pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
        pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        
        c.SaveAs( opt.GetOutputName( obj ) )
       
if __name__ == '__main__':
    main()
