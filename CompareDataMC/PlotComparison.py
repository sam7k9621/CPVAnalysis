import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import importlib
import numpy as np
import math 
from decimal import *
from itertools import cycle 

def GetBinError( mgr, unclst, nom, sample ):
    err_lst = []
    # systematically
    Nbins = nom.GetNbinsX()
    for u in unclst:
        unc = mgr.GetMergedObj( sample, u )
        temp = [ 0. if nom.GetBinContent( i+1 ) < 1. else unc.GetBinContent( i+1 ) - nom.GetBinContent( i+1 ) for i in range( Nbins ) ]

        if "mtop" in u:
            temp = [ x / 3. for x in temp ]

        err_lst.append( temp )
    return err_lst

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("e", "uncertainty").AddInput("o", "opt").AddInput("r", "region").AddInput("n", "unc" ).AddInput("f", "WHF").AddInput( "g", "dtg" )
    opt.AddFlag( "d", "driven" ).AddFlag( "b", "0bjet" ).AddFlag( "p", "wopileup" ).AddFlag( "w", "wobtag" ).AddFlag("u", "wosyst" ).AddFlag( "t", "txt" )
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "opt", "region", "wopileup", "wobtag" )
    opt.AddOutputName( "chi2", "unc", "opt", "region", "wopileup", "wobtag", "driven", "WHF" )
  
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    region = opt.GetOption( "region" ).split("_")[0]
    
    # Initialize sample list
    # objlst=[ "lep_tmass", "had_tmass" ]
    # objlst=[ "lep_tmass", "had_tmass", "HBJetPt", "LBJetPt", "LepPt" ]
    # objlst=[ "lep_tmass_obs3_p", "lep_tmass_obs3_n", "lep_tmass_obs6_p", "lep_tmass_obs6_n", "lep_tmass_obs12_p", "lep_tmass_obs12_n", "lep_tmass_obs14_p", "lep_tmass_obs14_n" ]
    # objlst=[ "chi2" ]
    objlst=[ "Obs3_dist", "Obs6_dist", "Obs12_dist", "Obs14_dist" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]
    unclst = [] if opt.GetOption( "wosyst" ) else [ "ttbar_semi", "ttbar_dilep" ]
    qcd_histdict = {}
    # Initialize MC sample list
    mclst  = [ "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttbar_dilep", "ttbar_had", "ttbar_semi" ]
    if not region:
        mclst.remove( "QCD" )
        mclst.remove( "ttbar_had" )
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
                    histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ).replace( ".", "_uncertainty_{}.".format( unc ) ), objlst, "{}_{}_{}".format( sample, year, unc ) ) 

        if opt.GetOption( "driven" ):
            qcd_histdict[ year ] = [ histmgr.GetMergedObj( "QCD", "nom", year ) for o in objlst ]
            histmgr.RemoveObj( "QCD", year )
            histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ).replace( opt.GetOption( "region", True ) + opt.GetOption( "wobtag", True ), "_region_QCD_0b_wobtag"), objlst, "QCD_{}_nom".format( year ) )

    if opt.GetOption( "dtg" ):
        dtg_value = opt.GetOption("dtg")
        cedm_objlst=[ "obs3", "obs6", "obs12", "obs14" ]
        histmgr.SetObjlst( opt.GetInputName("ttbar_dtg{dtg}").format( Y="17", dtg=dtg_value ), objlst, "dtg_{}".format(dtg_value) )
        
        # histmgr.SetObjlst( "{}_semi_CEDM1_0j.root".format(opt.LeptonType()), cedm_objlst, "CEDM_1" )
        histmgr.SetObjlst( "results/{}_CEDM_dtg3.root".format(opt.LeptonType()), cedm_objlst, "CEDM_3" )
        histmgr.SetObjlst( "results/{}_CEDM_dtg-3.root".format(opt.LeptonType()), cedm_objlst, "CEDM_1" )
        histmgr.SetObjlst( "results/{}_CEDM_dtg0.root".format(opt.LeptonType()), cedm_objlst, "CEDM_0" )
        # histmgr.SetObjlst( "{}_semi_SM_0j.root"   .format(opt.LeptonType()), cedm_objlst, "CEDM_0" )

    c = pltmgr.NewNCanvas() if opt.GetOption( "dtg" ) else pltmgr.NewCanvas( )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        err_lst = []
        datalst = []
        histdict = {}
        
        if opt.GetOption("dtg"):
            leg = pltmgr.NewLegend( 0.35, 0.5, 0.9, 0.78)
        else:
            leg = pltmgr.NewLegend( 0.72, 0.16, 0.85, 0.81)
        bg = ROOT.THStack()
        
        # Retrieve data plots
        for year in yearlst:
            histdict[year] = []
            histdict[year] = []
        for year in yearlst:
            histdict[year] = []
            datalst.append( histmgr.GetObj( "Data_{}_nom".format( year ) ) )

        # Retrieve MC plots
        colorcycle =  cycle( pltmgr.colorlst )
        sigcolorcycle =  cycle( pltmgr.sigcolorlst )
        sig_num, sighistlst = 0, []
        for i, mc in enumerate( mclst ):
            color = next( sigcolorcycle ) if "ttbar" in mc else next( colorcycle )
            
            for year in yearlst:
                histdict[year].append( histmgr.GetMergedObj( mc, "nom", year ) )
                histdict[year][-1].SetLineColor( color )
                histdict[year][-1].SetFillColor( color )
                bg.Add( histdict[year][-1] )

                if "ttbar" in mc:
                    sig_num += histdict[year][-1].Integral()
                    sighistlst.append( histdict[year][-1] )

                if year == yearlst[0]:
                    if mc == "WJets":
                        leg.AddEntry( histdict[year][-1], "W+jets", "F" )
                    elif "ttbar_semi" in mc:
                        leg.AddEntry( histdict[year][-1], "t#bar{t} semileptonic", "F" )
                    elif "ttbar_dilep" in mc:
                        leg.AddEntry( histdict[year][-1], "t#bar{t} dileptonic", "F" )
                    elif "DYJets" in mc:
                        leg.AddEntry( histdict[year][-1], "DY+jets", "F" )
                    elif "SingleTop" in mc:
                        leg.AddEntry( histdict[year][-1], "Single t", "F" )
                    else:
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
        mc_sum   = pltmgr.SumHist( histlst )
        sig_sum  = pltmgr.SumHist( sighistlst )
        data     = pltmgr.SumHist( datalst )

        nom      = [ mc_sum.GetBinContent( i+1 ) for i in range( mc_sum.GetNbinsX() ) ]
        nom_err  = [ mc_sum.GetBinError( i+1 ) for i in range( mc_sum.GetNbinsX() ) ]
        if not pos or not neg:
            pos, neg = [ 0 ] * len( nom ), [ 0 ] * len( nom )
       
        pos_stat = map( lambda x, y: math.sqrt( x + y**2 ), pos, nom_err )
        neg_stat = map( lambda x, y: math.sqrt( x + y**2 ), neg, nom_err )
        pos_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, pos, nom )
        neg_syst = map( lambda x, y: math.sqrt( x ) / y if y > 0 else 0, neg, nom )

        bg_total = mc_sum.Integral()
        da_total = data.Integral()
      
        if opt.GetOption( "txt" ):
            with open( opt.GetOutputName( obj, "ExpectedYield", "txt" ), "w" ) as outputfile:
                outputfile.write( "{}: {:.2f}\n".format( "Data", da_total ) )
                outputfile.write( "{}: {:.2f}\n".format( "MC",   bg_total ) )
                for i, mc in enumerate( mclst ):
                    mc_yield = sum( [ histdict[year][i].Integral() for year in yearlst ] )
                    outputfile.write( "{}: {:.2f} {:.2f}%\n".format( mc, mc_yield, 100 * mc_yield / bg_total ) )

        stat = pltmgr.GetErrRatioPlot( mc_sum, pos_stat, neg_stat, True )
        stat.SetLineColorAlpha( ROOT.kGray + 3, 0.7 ) 
        stat.SetFillColorAlpha( ROOT.kGray + 3, 0.7 )
        stat.SetFillStyle( 3002 )
        leg.AddEntry( data, "Data", "lpe" )
        leg.AddEntry( stat, "Stat." if opt.GetOption( "wosyst" ) else "Stat. #oplus Syst.", "F" )

        top = pltmgr.NewNTopPad() if opt.GetOption( "dtg" ) else pltmgr.NewTopPad()
        top.Draw()
        top.cd()

        data.Draw("EP")
        bg.Draw("HIST same")
        stat.Draw("E2 same")

        if opt.GetOption( "dtg" ):
            dtg = histmgr.GetObj( "dtg_{}".format(dtg_value) )
            pltmgr.SetNormToUnity( dtg )
            dtg.Scale( sig_num ) 
            dtg.SetLineStyle( 3 )
            dtg.SetLineWidth( 2 )
            dtg.SetLineColor( ROOT.kBlue + 2 )
            dtg.Draw("HIST same")

        data.Draw( "sameaxis" )
        data.Draw( "EP same" )
        leg.Draw()

        data.SetMaximum( max( pltmgr.GetHistYmax( mc_sum ), pltmgr.GetHistYmax( data ) ) * 1.5 )

        data.SetLineColor( 1 )
        data.SetLineWidth( 1 )
        data.SetMarkerSize( 0.5 )
        data.SetMarkerStyle( 20 )
    
        top.Update()
        if opt.GetOption("dtg"):
            pltmgr.SetTopPlotAxis( data, digit=False )
        else:
            pltmgr.SetTopPlotAxis( data )

        histmgr.CustomTitle( data )

        if opt.GetOption("dtg"):
            top.SetLogy()
            data.SetMaximum( max( pltmgr.GetHistYmax( mc_sum ), pltmgr.GetHistYmax( data ) ) * 1000 )
            leg.SetNColumns(3)
        c.cd()
   
        bot = pltmgr.NewNMiddlePad() if opt.GetOption( "dtg" ) else pltmgr.NewBottomPad()
        bot.Draw()
        bot.cd()
    
        xmin = data.GetXaxis().GetXmin()
        xmax = data.GetXaxis().GetXmax()
    
        line  = ROOT.TLine( xmin, 1.0, xmax, 1.0 )
        upper = ROOT.TLine( xmin, 1.5, xmax, 1.5 )
        lower = ROOT.TLine( xmin, 0.5, xmax, 0.5 )
        rel   = pltmgr.DivideHist( data, mc_sum, 1 )

        rel.Draw( "E0 P" )
        if not opt.GetOption( "wosyst" ): 
            syst = pltmgr.GetErrRatioPlot( rel, pos_syst, neg_syst )
            syst.Draw( "E2 same" )
            syst.SetFillColorAlpha( ROOT.kAzure - 9, 0.8 )
            syst.SetLineColorAlpha( ROOT.kAzure - 9, 0.8 )
            leg.AddEntry( syst, "Syst.", "F" )
       
        if opt.GetOption( "dtg" ):
            leg.AddEntry( dtg, "d_{{tG}}={}".format(dtg_value), "L" )

        upper.Draw( "same" )
        lower.Draw( "same" )
        line.Draw ( "same" )
        rel.Draw( "E0 P same" )
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
        if opt.GetOption( "dtg" ):
            pltmgr.SetMiddlePlotAxis( rel )
        else:
            pltmgr.SetBottomPlotAxis( rel )
    
        c.cd()
        
        if opt.GetOption( "dtg" ):
            mid = pltmgr.NewNBottomPad()
            mid.Draw()
            mid.cd()
          
            cedm3 = histmgr.GetObj("CEDM_3")
            cedm1 = histmgr.GetObj("CEDM_1")
            cedm0  = histmgr.GetObj("CEDM_0")

            cedm3.SetStats(0)
            cedm1.SetStats(0)
            cedm0.SetStats(0)

            pltmgr.SetNormToUnity( cedm3 )
            pltmgr.SetNormToUnity( cedm1 )
            pltmgr.SetNormToUnity( cedm0 )

            rel3 = pltmgr.DivideHist( cedm3, cedm0, 1 )
            
            rel1 = rel3.Clone()
            point_lst = [ rel3.GetBinContent(i)  for i in range( rel1.GetNcells()) ]
            point_lst.reverse()
            for i, p in enumerate( point_lst ):
                rel1.SetBinContent( i, p )
            
            # rel1 = pltmgr.DivideHist( cedm1, cedm0, 1 )
            rel3.Draw("HIST") 
            rel1.Draw("HIST same") 
         
            legg = pltmgr.NewLegend( 0.65, 0.43, 0.9, 0.68)
            legg.AddEntry( rel1, "(d_{tG} = #font[122]{\55}3) / (d_{tG}= 0)", "L" ) 
            legg.AddEntry( rel3, "(d_{tG} = +3) / (d_{tG} = 0)", "L" ) 
            legg.SetTextSize(12)
            legg.Draw("same")
            # upper.Draw( "same" )
            # lower.Draw( "same" )
            line.Draw("same")
            rel3.Draw("hist same")
            rel3.Draw("sameaxis")
            
            rel3.SetLineStyle(1)
            rel3.SetLineWidth(1)
            rel3.SetLineColor(pltmgr.Green)
            rel1.SetLineStyle(1)
            rel1.SetLineWidth(1)
            rel1.SetLineColor(pltmgr.Orange)
          
            if "3" in obj:
                ymax = 1.6 
                ymin = 0.4
            else:
                ymax = 1.3
                ymin = 0.7

            # ymax = 1.3 if "3" in obj or "6" in obj else 1.15
            # ymin = 0.7 if "3" in obj or "6" in obj else 0.85

            rel3.SetMaximum( ymax )
            rel3.SetMinimum( ymin )
            rel3.GetYaxis().SetTitle( "BSM/SM" )
            rel3.GetXaxis().SetTitle( data.GetXaxis().GetTitle() + " / m^{3}_{t} [GeV]" )
       
            mid.Update()
            pltmgr.SetBottomPlotAxis( rel3 )
        
        c.cd()
        # pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
        pltmgr.DrawCMSLabel()
        pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        
        c.SaveAs( opt.GetOutputName( obj ) )
       
if __name__ == '__main__':
    main()
