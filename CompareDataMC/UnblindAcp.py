from __future__ import division
from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import re
import ROOT
import math
import importlib

quad = lambda m, n: math.sqrt( m**2 + n**2 )

def GetUncorrAcp( data, bg, norm, bias ):
    pltmgr.SetNormToUnity( bg )
    bg.Scale( norm )
    bgneg, bgpos = bg.Integral( 0, 1 ), bg.Integral( 2, 3 )

    nm = data.GetBinContent( 1 ) - bgneg 
    np = data.GetBinContent( 2 ) - bgpos 
    print np, nm
    nm_err = math.sqrt( nm ) 
    np_err = math.sqrt( np )

    effe_acp       = 100 * ( np - nm ) / ( np + nm ) 
    effe_acp_stat  = 100 * math.sqrt( 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 ) )
    effe_acp_syst  = float( bias )
    effe_acp_total = quad( effe_acp_stat, effe_acp_syst )

    return ( effe_acp, effe_acp_stat, effe_acp_syst, effe_acp_total )

def GetErrGraph( nomlst, stat, up, dn, bin ):
    x, y     = array( 'd' ), array( 'd' )
    ex,  ey  = array( 'd' ), array( 'd' )
    exl, exh = array( 'd' ), array( 'd' )
    eyl, eyh = array( 'd' ), array( 'd' )

    for idx, nom in enumerate( nomlst ):
        x.append( bin + idx )
        y.append( nom )
        ex.append( 0 )
        ey.append( stat[idx] )
        exl.append( 0 )
        exh.append( 0 )
        eyh.append( up[idx] )
        eyl.append( dn[idx] )

    ans_stat  = ROOT.TGraphErrors( len(nomlst), x, y, ex, ey )
    ans_total = ROOT.TGraphAsymmErrors( len(nomlst), x, y, exl, exh, eyl, eyh )
    return ans_stat, ans_total

def GetEstimatedUnc( filename ):
    with open( filename, "r" ) as inputfile:
        yld = []
        for line in filter( lambda x: "Fitted" in x, inputfile.readlines() ):
            yld.append( float( line.split(" ")[-2] ) )
    return yld

def GetDilutionFactor( filename ):
    with open( filename, 'r' ) as inputfile:
        DF_lst = [ x.lstrip().rstrip("\n") for x in inputfile.readlines() if "Dilution" in x ]
    return DF_lst

def GetDetectorBias( filename ):
    with open( filename, 'r' ) as inputfile:
        biaslst = [x.lstrip().rstrip("\n").split(" ")[3] for x in inputfile.readlines()]
    return  biaslst

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt")
    opt.AddFlag("t","test")
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "opt" )
    opt.AddOutputName( "chi2", "opt" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs14" ]

    # Read channel-dependent files
    inputfile = opt.GetInputName( "Data" )
    histmgr.SetObjlst( inputfile.format( L="el" ), objlst, "Data_el" )
    histmgr.SetObjlst( inputfile.format( L="el" ).replace(".", "_region_WJets_0b_wobtag." ), objlst, "BG_el" )
    histmgr.SetObjlst( inputfile.format( L="mu" ), objlst, "Data_mu" )
    histmgr.SetObjlst( inputfile.format( L="mu" ).replace(".", "_region_WJets_0b_wobtag." ), objlst, "BG_mu" )
    
    norm_file_el = opt.GetInputName( "lep_tmass", "FitResult", "txt" ).format( L="el" )
    norm_el = GetEstimatedUnc( norm_file_el )
    norm_file_mu = opt.GetInputName( "lep_tmass", "FitResult", "txt" ).format( L="mu" )
    norm_mu = GetEstimatedUnc( norm_file_mu )
 
    biaslst_el = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="el" ).replace("_chi2_20_opt_150", "") )
    biaslst_mu = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="mu" ).replace("_chi2_20_opt_150", "") )
    biaslst_co = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="co" ).replace("_chi2_20_opt_150", "") )
   
    # Prepare uncorrected Acp value
    effe_acplst_el, effe_acp_statlst_el, effe_acp_systlst_el, effe_acp_totallst_el = [], [], [], []
    effe_acplst_mu, effe_acp_statlst_mu, effe_acp_systlst_mu, effe_acp_totallst_mu = [], [], [], []
    effe_acplst_co, effe_acp_statlst_co, effe_acp_systlst_co, effe_acp_totallst_co = [], [], [], []
    print "*" * 90
    for idx, obj in enumerate( objlst ):
        data_el   = histmgr.GetObj( "Data_el" )
        data_mu   = histmgr.GetObj( "Data_mu" )
        data_co   = data_el.Clone()
        data_co.Add( data_mu )
        bghist_el = histmgr.GetObj( "BG_el" )
        bghist_mu = histmgr.GetObj( "BG_mu" )
        bghist_co = bghist_el.Clone()
        bghist_co.Add( bghist_mu )

        result_el = GetUncorrAcp( data_el, bghist_el, norm_el[1], biaslst_el[idx] )
        result_mu = GetUncorrAcp( data_mu, bghist_mu, norm_mu[1], biaslst_mu[idx] )
        result_co = GetUncorrAcp( data_co, bghist_co, norm_el[1] + norm_mu[1], biaslst_co[idx] )

        effe_acplst_el      .append( result_el[0] )
        effe_acplst_mu      .append( result_mu[0] )
        effe_acplst_co      .append( result_co[0] )
        effe_acp_statlst_el .append( result_el[1] )
        effe_acp_statlst_mu .append( result_mu[1] )
        effe_acp_statlst_co .append( result_co[1] )
        effe_acp_systlst_el .append( result_el[2] )
        effe_acp_systlst_mu .append( result_mu[2] )
        effe_acp_systlst_co .append( result_co[2] )
        effe_acp_totallst_el.append( result_el[3] )
        effe_acp_totallst_mu.append( result_mu[3] )
        effe_acp_totallst_co.append( result_co[3] )

        print "{:6} {:20}: {:2.3f} Stat. {:2.3f}  Syst. {:2.3f}  Syst+Stat. {:2.3f}".format( obj, "Effective el Acp[%]", result_el[0], result_el[1], result_el[2], result_el[3] )
        print "{:6} {:20}: {:2.3f} Stat. {:2.3f}  Syst. {:2.3f}  Syst+Stat. {:2.3f}".format( obj, "Effective mu Acp[%]", result_mu[0], result_mu[1], result_mu[2], result_mu[3] )
        print "{:6} {:20}: {:2.3f} Stat. {:2.3f}  Syst. {:2.3f}  Syst+Stat. {:2.3f}".format( obj, "Effective co Acp[%]", result_co[0], result_co[1], result_co[2], result_co[3] )
    
    # Prepare dilution factor 
    DF_file = opt.GetInputName( "DF", "Sim", "txt" ).format( L="co" )
    dflst   = GetDilutionFactor( DF_file )

    # Correct Acp
    true_acplst, true_acp_statlst, true_acp_total_uplst, true_acp_total_dnlst = [], [], [], []
    print "*" * 90
    for idx, obj in enumerate( objlst ):  
        df         = float( dflst[idx].split(" ")[3] )
        df_stat    = float( dflst[idx].split(" ")[5] )
        df_syst_up = float( dflst[idx].split(" ")[7] )
        df_syst_dn = float( dflst[idx].split(" ")[9] )
        
        true_acp          = effe_acplst_co[idx] / df 
        true_acp_stat     = abs( true_acp ) * quad( effe_acp_statlst_co[idx] / effe_acplst_co[idx], df_stat / df )
        true_acp_syst_up  = abs( true_acp ) * quad( effe_acp_systlst_co[idx] / effe_acplst_co[idx], df_syst_up / df )
        true_acp_syst_dn  = abs( true_acp ) * quad( effe_acp_systlst_co[idx] / effe_acplst_co[idx], df_syst_dn / df )
        true_acp_total_up = quad( true_acp_stat, true_acp_syst_up )
        true_acp_total_dn = quad( true_acp_stat, true_acp_syst_dn )

        true_acplst.append( true_acp )
        true_acp_statlst.append( true_acp_stat )
        true_acp_total_uplst.append( true_acp_total_up )
        true_acp_total_dnlst.append( true_acp_total_dn )

        print "{} {:20}: {:2.3f} Stat. {:2.3f}  Syst. +{:2.3f} -{:2.3f} Syst+Stat. +{:2.3f} -{:2.3f}".format( obj, "True Acp[%]", true_acp, true_acp_stat, true_acp_syst_up, true_acp_syst_dn, true_acp_total_up, true_acp_total_dn )

    effe_stat_el_gr, effe_total_el_gr = GetErrGraph( effe_acplst_el, effe_acp_statlst_el, effe_acp_totallst_el, effe_acp_totallst_el, 0.2 )
    effe_stat_mu_gr, effe_total_mu_gr = GetErrGraph( effe_acplst_mu, effe_acp_statlst_mu, effe_acp_totallst_mu, effe_acp_totallst_mu, 0.35 )
    effe_stat_co_gr, effe_total_co_gr = GetErrGraph( effe_acplst_co, effe_acp_statlst_co, effe_acp_totallst_co, effe_acp_totallst_co, 0.5 )
    true_stat_gr, true_total_gr = GetErrGraph( true_acplst, true_acp_statlst, true_acp_total_uplst, true_acp_total_dnlst, 0.65 )

    # 8TeV measurement
    alpha_acplst = [-0.13, -0.31, 0.06, 0.16]
    alpha_acp_statlst = [0.42, 0.42, 0.42, 0.42]
    alpha_acp_total_uplst = [0.44, 0.44, 0.44, 0.44]
    alpha_acp_total_dnlst = [0.44, 0.44, 0.44, 0.44]
    alpha_stat_gr, alpha_total_gr = GetErrGraph( alpha_acplst, alpha_acp_statlst, alpha_acp_total_uplst, alpha_acp_total_dnlst, 0.8 )

    c = pltmgr.NewCanvas()
    alpha_stat_gr.Draw( "AEP" ) 
    
    ymax = pltmgr.GetGraphlstYmax( [ effe_total_el_gr, effe_total_mu_gr, true_total_gr, alpha_total_gr ] ) * 1.8
    ymin = pltmgr.GetGraphlstYmin( [ effe_total_el_gr, effe_total_mu_gr, true_total_gr, alpha_total_gr ] ) * 1.3

    line0 = ROOT.TLine( 0, 0, 4, 0 )
    line1 = ROOT.TLine( 1, ymin*0.5, 1, ymax*0.5 ) 
    line2 = ROOT.TLine( 2, ymin*0.5, 2, ymax*0.5 ) 
    line3 = ROOT.TLine( 3, ymin*0.5, 3, ymax*0.5 ) 
    
    line0.Draw( "same" )
    line1.Draw( "same" ) 
    line2.Draw( "same" ) 
    line3.Draw( "same" ) 
    
    # true_stat_gr.Draw( "EP same" ) 
    effe_stat_el_gr .Draw( "EP same" )
    effe_stat_mu_gr .Draw( "EP same" )
    effe_stat_co_gr .Draw( "EP same" )
    alpha_stat_gr   .Draw( "EP same" )
    # true_total_gr   .Draw( "EP same" )
    effe_total_el_gr.Draw( "EP same" )
    effe_total_mu_gr.Draw( "EP same" )
    effe_total_co_gr.Draw( "EP same" )
    alpha_total_gr  .Draw( "EP same" )

    true_total_gr   .SetMarkerStyle( 20 )
    effe_total_el_gr.SetMarkerStyle( 22 )
    effe_total_mu_gr.SetMarkerStyle( 23 )
    effe_total_co_gr.SetMarkerStyle( 33 )
    alpha_total_gr  .SetMarkerStyle( 20 )

    true_total_gr   .SetMarkerSize( 1.3 )
    effe_total_el_gr.SetMarkerSize( 1.2 )
    effe_total_mu_gr.SetMarkerSize( 1.2 )
    effe_total_co_gr.SetMarkerSize( 1.8 )
    alpha_total_gr  .SetMarkerSize( 1.3 )

    effe_total_co_gr.SetMarkerColor( pltmgr.Orange )
    true_total_gr .SetMarkerColor( pltmgr.Red )
    alpha_total_gr.SetMarkerColor( ROOT.kGreen - 6  )
   
    effe_total_co_gr.SetLineColor( pltmgr.Orange )
    true_total_gr .SetLineColor( pltmgr.Red )
    alpha_total_gr.SetLineColor( ROOT.kGreen - 6 )
    effe_stat_co_gr.SetLineColor( pltmgr.Orange )
    true_stat_gr  .SetLineColor( pltmgr.Red )
    alpha_stat_gr .SetLineColor( ROOT.kGreen - 6 )
    
    true_total_gr   .SetLineWidth( 2 )
    effe_total_el_gr.SetLineWidth( 2 )
    effe_total_mu_gr.SetLineWidth( 2 )
    effe_total_co_gr.SetLineWidth( 2 )
    alpha_total_gr  .SetLineWidth( 2 )
    true_stat_gr    .SetLineWidth( 2 )
    effe_stat_el_gr .SetLineWidth( 2 )
    effe_stat_mu_gr .SetLineWidth( 2 )
    effe_stat_co_gr .SetLineWidth( 2 )
    alpha_stat_gr   .SetLineWidth( 2 )
   
    alpha_stat_gr.SetTitle( "" )
    alpha_stat_gr.GetYaxis().SetTitle( "Asymmetry [%]" )
    xaxis = alpha_stat_gr.GetHistogram().GetXaxis()
    xaxis.SetLimits( -0., 4. )
    x1 = xaxis.GetBinLowEdge(1)
    x2 = xaxis.GetBinUpEdge( xaxis.GetNbins() )
    
    xaxis.Set( 4, x1, x2 )
    alpha_stat_gr.GetHistogram().SetMaximum( ymax )
    alpha_stat_gr.GetHistogram().SetMinimum( ymin )
   
    label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x)[0] )
    for idx, obj in enumerate( objlst ):
        xaxis.SetBinLabel( idx + 1, label( obj ) ) 
    
    line0.SetLineColor( pltmgr.Gray )
    line1.SetLineColor( ROOT.kAzure - 9 )
    line2.SetLineColor( ROOT.kAzure - 9 )
    line3.SetLineColor( ROOT.kAzure - 9 )
    
    line0.SetLineWidth( 2 )
    line1.SetLineWidth( 2 ) 
    line2.SetLineWidth( 2 ) 
    line3.SetLineWidth( 2 ) 
    
    line0.SetLineStyle( 8 )
    line1.SetLineStyle( 6 ) 
    line2.SetLineStyle( 6 ) 
    line3.SetLineStyle( 6 ) 
   
    leg = pltmgr.NewLegend( 0.38, 0.74, 0.9, 0.87)
    leg.Draw()
    leg.SetTextSize( 14 )
    leg.SetNColumns(2)
    leg.AddEntry( effe_total_el_gr, "A'_{CP} in e+jets", "P" )
    # leg.AddEntry( true_total_gr, "A_{CP} (SM correction)", "P" )
    leg.AddEntry( effe_total_mu_gr, "A'_{CP} in #mu+jets", "P" )
    leg.AddEntry( alpha_total_gr, "A'_{CP} in lepton_jets (8 TeV)", "P" )
    leg.AddEntry( effe_total_co_gr, "A'_{CP} in lepton+jets", "P" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( alpha_stat_gr.GetHistogram(), False )
    pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
    pltmgr.DrawLuminosity( opt.Lumi() )
    pltmgr.DrawEntryLeft( opt.Entry() )

    c.SaveAs( opt.GetOutputName( "Acp", "Unblind" ) )

if __name__ == '__main__':
    main()
