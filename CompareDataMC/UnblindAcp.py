from __future__ import division
from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import re
import ROOT
import math
import importlib
uncertainty = [ 
    "pileup_up", "pileup_dn", "bctag_up", "bctag_dn", "lighttag_up", "lighttag_dn", "lepton_up", "lepton_dn", "JER_up", "JER_dn", "JEC_up", "JEC_dn",
    "PDF_up", "PDF_dn", "muFmuR_up", "muFmuR_dn", "ISR_up", "ISR_dn", "FSR_up", "FSR_dn", "CP5CR1", "CP5CR2", "CP5down", "CP5up", "hdampDOWN", "hdampUP", "CP5up", "CP5down", "FR_up", "FR_dn", "mtop1695", "mtop1755", "background", "resolution_10", "resolution_-10", "WHF_10", "woTopPT"
    ]

quad = lambda m, n: math.sqrt( m**2 + n**2 )

def GetUncorrAcp( pevt, nevt, syst_up, syst_dn ):
    np = pevt[0]
    nm = nevt[0]
    np_err = pevt[1]
    nm_err = nevt[1]

    effe_acp       = 100 * ( np - nm ) / ( np + nm ) 
    effe_acp_stat  = 100 * math.sqrt( 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 ) )
    effe_acp_syst_up  = float( syst_up )
    effe_acp_syst_dn  = float( syst_dn )
    effe_acp_total_up = quad( effe_acp_stat, effe_acp_syst_up )
    effe_acp_total_dn = quad( effe_acp_stat, effe_acp_syst_dn )

    return ( effe_acp, effe_acp_stat, effe_acp_syst_up, effe_acp_syst_dn, effe_acp_total_up, effe_acp_total_dn )

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
    yld = []
    with open( filename, "r" ) as inputfile:
        for line in filter( lambda x: "fitted" in x, inputfile.readlines() ):
            temp = filter( None,line.split(" ") ) 
            yld.append( (float(temp[-2]), float(temp[-1])) )
    return yld

def GetDilutionFactor( filename ):
    with open( filename, 'r' ) as inputfile:
        DF_lst = [ x.lstrip().rstrip("\n") for x in inputfile.readlines() if "Dilution" in x ]
    return DF_lst

def GetDetectorBias( filename ):
    with open( filename, 'r' ) as inputfile:
        linelst = inputfile.readlines()
        meanlst = [float(x.lstrip().rstrip("\n").split(" ")[2]) for x in linelst]
        errlst  = [float(x.lstrip().rstrip("\n").split(" ")[3]) for x in linelst]
        
        biaslst = [ max( abs(x), abs(y) ) for x, y in zip( meanlst, errlst ) ]
    return  biaslst

def GetSystSource( filename ):
    with open( filename, "r" ) as inputfile:
        systlst = [ float(x.lstrip().rstrip("\n").split(" ")[1]) for x in inputfile.readlines()]
    return systlst

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
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]

    # Nominal pos/neg event number
    norm_el, norm_mu = [(0,0)]*8, [(0,0)]*8
    for year in yearlst:
        norm_file_el = opt.GetInputName( "Acp", "FitResult", "txt" ).format( L="el", Y=year )
        norm_el = [ (x[0]+y[0], quad(x[1],y[1])) for x, y in zip( norm_el, GetEstimatedUnc( norm_file_el ) ) ]
        norm_file_mu = opt.GetInputName( "Acp", "FitResult", "txt" ).format( L="mu", Y=year )
        norm_mu = [ (x[0]+y[0], quad(x[1],y[1])) for x, y in zip( norm_mu, GetEstimatedUnc( norm_file_mu ) ) ]
    norm_co = [(x[0]+y[0], quad(x[1], y[1])) for x, y in zip(norm_el, norm_mu)]

    # Systematic uncertainty value
    systdic_el, systdic_mu, systdic_co = dict(), dict(), dict()
    el_txtfile = opt.GetInputName( "AcpUnc", "Stack", "txt" ).format( Y=opt.Year(), L="el" )
    mu_txtfile = opt.GetInputName( "AcpUnc", "Stack", "txt" ).format( Y=opt.Year(), L="mu" )
    co_txtfile = opt.GetInputName( "AcpUnc", "Stack", "txt" ).format( Y=opt.Year(), L="co" )
    
    for unc in uncertainty:
        systdic_el[unc] = GetSystSource( el_txtfile[:-4] + "_seed_20_uncertainty_{}".format( unc ) + el_txtfile[-4:] )
        systdic_mu[unc] = GetSystSource( mu_txtfile[:-4] + "_seed_20_uncertainty_{}".format( unc ) + mu_txtfile[-4:] )
        systdic_co[unc] = GetSystSource( co_txtfile[:-4] + "_seed_20_uncertainty_{}".format( unc ) + co_txtfile[-4:] )
  
    # Detector bias value
    biaslst_el = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="el", Y=opt.Year() ).replace("_chi2_20_opt_150", "") )
    biaslst_mu = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="mu", Y=opt.Year() ).replace("_chi2_20_opt_150", "") )
    biaslst_co = GetDetectorBias( opt.GetInputName( "Data_Acp_0_mixed", "AcpResult", "txt" ).format( L="co", Y=opt.Year() ).replace("_chi2_20_opt_150", "") )

    # Combined syst. and detector bias
    p_systlst_el, p_systlst_mu, p_systlst_co = [0]*4, [0]*4, [0]*4
    n_systlst_el, n_systlst_mu, n_systlst_co = [0]*4, [0]*4, [0]*4

    for k, value in systdic_el.iteritems():
        for idx, v in enumerate(value):
            if v > 0:
                p_systlst_el[idx] += (v**2)
            else:
                n_systlst_el[idx] += (v**2)
    
    for k, value in systdic_mu.iteritems():
        for idx, v in enumerate(value):
            if v > 0:
                p_systlst_mu[idx] += (v**2)
            else:
                n_systlst_mu[idx] += (v**2)

    for k, value in systdic_co.iteritems():
        for idx, v in enumerate(value):
            if v > 0:
                p_systlst_co[idx] += (v**2)
            else:
                n_systlst_co[idx] += (v**2)

    p_systlst_el = [ math.sqrt( x+ (y**2) ) for x, y in zip(p_systlst_el, biaslst_el) ]
    p_systlst_mu = [ math.sqrt( x+ (y**2) ) for x, y in zip(p_systlst_mu, biaslst_mu) ]
    p_systlst_co = [ math.sqrt( x+ (y**2) ) for x, y in zip(p_systlst_co, biaslst_co) ]
    n_systlst_el = [ math.sqrt( x+ (y**2) ) for x, y in zip(n_systlst_el, biaslst_el) ]
    n_systlst_mu = [ math.sqrt( x+ (y**2) ) for x, y in zip(n_systlst_mu, biaslst_mu) ]
    n_systlst_co = [ math.sqrt( x+ (y**2) ) for x, y in zip(n_systlst_co, biaslst_co) ]

    # Prepare uncorrected Acp value
    effe_acplst_el, effe_acp_statlst_el, effe_acp_totallst_el_up, effe_acp_totallst_el_dn = [], [], [], []
    effe_acplst_mu, effe_acp_statlst_mu, effe_acp_totallst_mu_up, effe_acp_totallst_mu_dn = [], [], [], []
    effe_acplst_co, effe_acp_statlst_co, effe_acp_totallst_co_up, effe_acp_totallst_co_dn = [], [], [], []
    
    print "*" * 90
    for idx, obj in enumerate( objlst ):
        result_el = GetUncorrAcp( norm_el[idx*2], norm_el[idx*2+1], p_systlst_el[idx], n_systlst_el[idx] )
        result_mu = GetUncorrAcp( norm_mu[idx*2], norm_mu[idx*2+1], p_systlst_mu[idx], n_systlst_mu[idx] )
        result_co = GetUncorrAcp( norm_co[idx*2], norm_co[idx*2+1], p_systlst_co[idx], n_systlst_co[idx] )

        effe_acplst_el      .append( result_el[0] )
        effe_acplst_mu      .append( result_mu[0] )
        effe_acplst_co      .append( result_co[0] )
        effe_acp_statlst_el .append( result_el[1] )
        effe_acp_statlst_mu .append( result_mu[1] )
        effe_acp_statlst_co .append( result_co[1] )
        # effe_acp_systlst_el .append( result_el[2] )
        # effe_acp_systlst_mu .append( result_mu[2] )
        # effe_acp_systlst_co .append( result_co[2] )
        effe_acp_totallst_el_up.append( result_el[4] )
        effe_acp_totallst_el_dn.append( result_el[5] )
        effe_acp_totallst_mu_up.append( result_mu[4] )
        effe_acp_totallst_mu_dn.append( result_mu[5] )
        effe_acp_totallst_co_up.append( result_co[4] )
        effe_acp_totallst_co_dn.append( result_co[5] )

        print "{:6} {:20}: & ${:+2.3f} \pm {:2.3f}(stat) ^{{+{:2.3f}}}_{{-{:2.3f}}}(syst)$".format( 
                obj, "Effective el Acp[%]", 
                result_el[0], result_el[1],
                result_el[2], result_el[3] 
                )

        print "{:6} {:20}: & ${:+2.3f} \pm {:2.3f}(stat) ^{{+{:2.3f}}}_{{-{:2.3f}}}(syst)$".format( 
                obj, "Effective mu Acp[%]", 
                result_mu[0], result_mu[1],
                result_mu[2], result_mu[3] 
                )

        print "{:6} {:20}: & ${:+2.3f} \pm {:2.3f}(stat) ^{{+{:2.3f}}}_{{-{:2.3f}}}(syst)$".format( 
                obj, "Effective co Acp[%]", 
                result_co[0], result_co[1],
                result_co[2], result_co[3] 
                )
        
        # print "{:6} {:20}: {:2.3f} Stat. {:2.3f}  Syst. {:2.3f}  Syst+Stat. {:2.3f}".format( obj, "Effective co Acp[%]", result_co[0], result_co[1], result_co[2], result_co[3] )

    # Prepare dilution factor 
    # DF_file = opt.GetInputName( "DF", "Sim", "txt" ).format( L="co", Y=opt.Year() )
    # dflst   = GetDilutionFactor( DF_file )

    # Correct Acp
    # true_acplst, true_acp_statlst, true_acp_total_uplst, true_acp_total_dnlst = [], [], [], []
    print "*" * 90
    # for idx, obj in enumerate( objlst ):  
        # df         = float( dflst[idx].split(" ")[3] )
        # df_stat    = float( dflst[idx].split(" ")[5] )
        # df_syst_up = float( dflst[idx].split(" ")[7] )
        # df_syst_dn = float( dflst[idx].split(" ")[9] )
        
        # true_acp          = effe_acplst_co[idx] / df 
        # true_acp_stat     = abs( true_acp ) * quad( effe_acp_statlst_co[idx] / effe_acplst_co[idx], df_stat / df )
        # true_acp_syst_up  = abs( true_acp ) * quad( effe_acp_systlst_co[idx] / effe_acplst_co[idx], df_syst_up / df )
        # true_acp_syst_dn  = abs( true_acp ) * quad( effe_acp_systlst_co[idx] / effe_acplst_co[idx], df_syst_dn / df )
        # true_acp_total_up = quad( true_acp_stat, true_acp_syst_up )
        # true_acp_total_dn = quad( true_acp_stat, true_acp_syst_dn )

        # true_acplst.append( true_acp )
        # true_acp_statlst.append( true_acp_stat )
        # true_acp_total_uplst.append( true_acp_total_up )
        # true_acp_total_dnlst.append( true_acp_total_dn )

        # print "{} {:20}: {:2.3f} Stat. {:2.3f}  Syst. +{:2.3f} -{:2.3f} Syst+Stat. +{:2.3f} -{:2.3f}".format( obj, "True Acp[%]", true_acp, true_acp_stat, true_acp_syst_up, true_acp_syst_dn, true_acp_total_up, true_acp_total_dn )

    effe_stat_el_gr, effe_total_el_gr = GetErrGraph( effe_acplst_el, effe_acp_statlst_el, effe_acp_totallst_el_up, effe_acp_totallst_el_dn, 0.2 )
    effe_stat_mu_gr, effe_total_mu_gr = GetErrGraph( effe_acplst_mu, effe_acp_statlst_mu, effe_acp_totallst_mu_up, effe_acp_totallst_mu_dn, 0.35 )
    effe_stat_co_gr, effe_total_co_gr = GetErrGraph( effe_acplst_co, effe_acp_statlst_co, effe_acp_totallst_co_up, effe_acp_totallst_co_dn, 0.5 )
    # true_stat_gr, true_total_gr = GetErrGraph( true_acplst, true_acp_statlst, true_acp_total_uplst, true_acp_total_dnlst, 0.65 )

    # 8TeV measurement
    alpha_acplst = [-0.13, -0.31, 0.06, 0.16]
    alpha_acp_statlst = [0.42, 0.42, 0.42, 0.42]
    alpha_acp_total_uplst = [0.44, 0.44, 0.44, 0.44]
    alpha_acp_total_dnlst = [0.44, 0.44, 0.44, 0.44]
    alpha_stat_gr, alpha_total_gr = GetErrGraph( alpha_acplst, alpha_acp_statlst, alpha_acp_total_uplst, alpha_acp_total_dnlst, 0.8 )

    c = pltmgr.NewCanvas()
    alpha_stat_gr.Draw( "AEP" ) 
    
    # ymax = pltmgr.GetGraphlstYmax( [ effe_total_el_gr, effe_total_mu_gr ] ) * 1.8
    # ymin = pltmgr.GetGraphlstYmin( [ effe_total_el_gr, effe_total_mu_gr ] ) * 1.3

    ymax = 1
    ymin = -1
    # ymax = pltmgr.GetGraphlstYmax( [ effe_total_el_gr, effe_total_mu_gr, true_total_gr, alpha_total_gr ] ) * 1.8
    # ymin = pltmgr.GetGraphlstYmin( [ effe_total_el_gr, effe_total_mu_gr, true_total_gr, alpha_total_gr ] ) * 1.3

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

    # true_total_gr   .SetMarkerStyle( 20 )
    effe_total_el_gr.SetMarkerStyle( 22 )
    effe_total_mu_gr.SetMarkerStyle( 23 )
    effe_total_co_gr.SetMarkerStyle( 33 )
    alpha_total_gr  .SetMarkerStyle( 20 )

    # true_total_gr   .SetMarkerSize( 1.3 )
    effe_total_el_gr.SetMarkerSize( 1.2 )
    effe_total_mu_gr.SetMarkerSize( 1.2 )
    effe_total_co_gr.SetMarkerSize( 1.8 )
    alpha_total_gr  .SetMarkerSize( 1.3 )

    effe_total_co_gr.SetMarkerColor( pltmgr.Orange )
    # true_total_gr .SetMarkerColor( pltmgr.Red )
    alpha_total_gr.SetMarkerColor( ROOT.kGreen - 6  )
   
    effe_total_co_gr.SetLineColor( pltmgr.Orange )
    # true_total_gr .SetLineColor( pltmgr.Red )
    alpha_total_gr.SetLineColor( ROOT.kGreen - 6 )
    effe_stat_co_gr.SetLineColor( pltmgr.Orange )
    # true_stat_gr  .SetLineColor( pltmgr.Red )
    alpha_stat_gr .SetLineColor( ROOT.kGreen - 6 )
    
    # true_total_gr   .SetLineWidth( 2 )
    effe_total_el_gr.SetLineWidth( 2 )
    effe_total_mu_gr.SetLineWidth( 2 )
    effe_total_co_gr.SetLineWidth( 2 )
    alpha_total_gr  .SetLineWidth( 2 )
    # true_stat_gr    .SetLineWidth( 2 )
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
    leg.AddEntry( alpha_total_gr, "A'_{CP} in lepton+jets (8 TeV)", "P" )
    leg.AddEntry( effe_total_co_gr, "A'_{CP} in lepton+jets", "P" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( alpha_stat_gr.GetHistogram(), False )
    pltmgr.DrawCMSLabel()
    pltmgr.DrawLuminosity( opt.Lumi() )
    pltmgr.DrawEntryLeft( opt.Entry() )

    c.SaveAs( opt.GetOutputName( "Acp", "Unblind" ) )

if __name__ == '__main__':
    main()
