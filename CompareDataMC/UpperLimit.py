from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import ROOT
import math
import numpy 
from sympy import symbols, solve, Eq, diff

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

def GetErrGraph( nomlst, up, dn ):
    x, y     = array( 'd' ), array( 'd' )
    ex,  ey  = array( 'd' ), array( 'd' )
    exl, exh = array( 'd' ), array( 'd' )
    eyl, eyh = array( 'd' ), array( 'd' )

    for idx, nom in enumerate( nomlst ):
        x.append( idx / 2. )
        y.append( nom )
        exl.append( 0 )
        exh.append( 0 )
        eyh.append( up[idx] )
        eyl.append( dn[idx] )

    ans_total = ROOT.TGraphAsymmErrors( len(nomlst), x, y, exl, exh, eyl, eyh )
    return ans_total

def GetEstimatedUnc( filename ):
    yld = []
    with open( filename, "r" ) as inputfile:
        for line in filter( lambda x: "fitted" in x, inputfile.readlines() ):
            temp = filter( None,line.split(" ") ) 
            yld.append( (float(temp[-2]), float(temp[-1])) )
    return yld

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

def GetDilutionFactor( filename ):
    with open( filename, 'r' ) as inputfile:
        linelst = inputfile.readlines()
        linelst = [ x.lstrip().rstrip("\n").split(" ") for x in linelst if "Dilution" in x ]
    
    return [(float(x[3]), float(x[5]), float(x[7]), float(x[9])) for x in linelst ]

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
    # objlst=[ "val_obs12"]
    objlst=[ "val_obs3", "val_obs6", "val_obs12", "val_obs14" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]

    # Get genlevel Acp sample
    nsample = 11
    for i in range( nsample ):
        i = str(i/2.) if i%2 else str(i/2)
        histmgr.SetObjlst( "/wk_cms2/sam7k9621/CEDM_sample/co_CEDM_dtg{}.root".format( i ), objlst, "dtg_{}".format( i ) )
        # histmgr.SetObjlst( "co_CEDM_dtg{}_genlevel.root".format( i ), objlst, "dtg_{}".format( i ) )
   
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

    # Dilution factor
    raw_dflst = GetDilutionFactor( opt.GetInputName( "DF", "Sim", "txt" ).format( L="co", Y=opt.Year() ) )
    dflst = [(x[0], quad(x[1], x[2]), quad(x[1], x[3])) for x in raw_dflst]
    
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
    canvas = pltmgr.NewCanvas() 
    for idx, obj in enumerate( objlst ):
        result_el = GetUncorrAcp( norm_el[idx*2], norm_el[idx*2+1], p_systlst_el[idx], n_systlst_el[idx] )
        result_mu = GetUncorrAcp( norm_mu[idx*2], norm_mu[idx*2+1], p_systlst_mu[idx], n_systlst_mu[idx] )
        result_co = GetUncorrAcp( norm_co[idx*2], norm_co[idx*2+1], p_systlst_co[idx], n_systlst_co[idx] )

        true_acp = result_co[0] / dflst[idx][0]
        true_acp_up = true_acp * quad( (result_co[4]/result_co[0]), (dflst[idx][1]/dflst[idx][0]) )
        true_acp_dn = true_acp * quad( (result_co[5]/result_co[0]), (dflst[idx][2]/dflst[idx][0]) )

        true_acp_stat    = true_acp * quad( (result_co[1]/result_co[0]), (raw_dflst[idx][1]/raw_dflst[idx][0]) )
        true_acp_syst_up = true_acp * quad( (result_co[2]/result_co[0]), (raw_dflst[idx][2]/raw_dflst[idx][0]) )
        true_acp_syst_dn = true_acp * quad( (result_co[3]/result_co[0]), (raw_dflst[idx][3]/raw_dflst[idx][0]) )

        cedm_acplst, cedm_acperrlst = [], []
        true_acplst, true_acpuplst, true_acpdnlst = [], [], []
        for i in range( nsample ):
            i = str(i/2.) if i%2 else str(i/2)
            
            hist = histmgr.GetObj( "dtg_{}".format(i) )
            nm, np         = hist.GetBinContent(1), hist.GetBinContent(2)
            nm_err, np_err = hist.GetBinError(1),   hist.GetBinError(2)
            
            # print i, 100 * ( np - nm ) / ( np + nm ), 100 * math.sqrt( 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 ) )
            cedm_acplst.append( 100 * ( np - nm ) / ( np + nm ) )
            cedm_acperrlst.append( 100 * math.sqrt( 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 ) ) )

            true_acplst.append( true_acp )
            true_acpuplst.append( true_acp_up )
            true_acpdnlst.append( true_acp_dn )

        gr = GetErrGraph( cedm_acplst, cedm_acperrlst, cedm_acperrlst )
        gr_true_bd = GetErrGraph( true_acplst, true_acpuplst, true_acpdnlst )
        gr_true_pt = ROOT.TGraphAsymmErrors( 
                1, 
                array('d', [1.5]),
                array('d', [true_acplst[0]]),
                array('d', [1.5]),
                array('d', [1.5]),
                array('d', [0]),
                array('d', [0])
                ) 
        gr.Draw("AXP") 

        gr_true_bd.SetFillColorAlpha(ROOT.kAzure-9, 0.6)
        gr_true_bd.SetLineColorAlpha(ROOT.kAzure-9, 0.6)
        gr_true_pt.SetFillColorAlpha(ROOT.kAzure-9, 0.6)
        gr_true_pt.SetMarkerSize(0)
        gr_true_pt.SetLineColor(ROOT.kRed)
        gr_true_pt.SetLineWidth(1)
       
        gr.SetTitle("")
        gr.GetYaxis().SetTitle( "True Asymmetry A_{CP}[%]" )
        gr.GetXaxis().SetTitle( "d_{tG}")
        gr.SetMarkerSize( 0 )
        gr.SetMarkerColor( 0 )
        gr.GetXaxis().SetLimits( 0, 3 )
        gr.GetHistogram().SetMaximum(.7) 
        gr.GetHistogram().SetMinimum(-.06)
      
        # Eq(b*y*x**2+(c*y-1)*x+d*y-a)
        fa = ROOT.TF1("fa", "(x+[0])/([1]*x*x+[2]*x+[3])", 0, 5)
        fa.SetParameter(0, 1)
        fa.SetParameter(1, 1)
        fa.SetParameter(2, 1)
        fa.SetParameter(3, 1)
        fa.SetLineColor( 1 )
        result = gr.Fit(fa, "S" )
        
        fit_a, fit_b, fit_c, fit_d = fa.GetParameter(0), fa.GetParameter(1), fa.GetParameter(2), fa.GetParameter(3)
        ae, be, ce, de = fa.GetParError(0), fa.GetParError(1), fa.GetParError(2), fa.GetParError(3)
        
        ab_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(0, 1)
        ac_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(0, 2)
        ad_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(0, 3)
        bc_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(1, 2)
        bd_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(1, 3)
        cd_cov = ROOT.TVirtualFitter.GetFitter().GetCovarianceMatrixElement(2, 3)
    
        print true_acp

        x, y, a, b, c, d = symbols('x y a b c d', real=True)
        f = solve(Eq(b*y*x**2+(c*y-1)*x+d*y-a),x)[0]
        
        f_y = float( diff(f, y).subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}) )
        f_a = float( diff(f, a).subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}) )
        f_b = float( diff(f, b).subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}) )
        f_c = float( diff(f, c).subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}) )
        f_d = float( diff(f, d).subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}) )
        
        der_vec = numpy.array([[f_y, f_a, f_b, f_c, f_d]])
        der_vec_tran = numpy.transpose(der_vec)
        stat_matrix = numpy.array([
            [true_acp_stat**2, 0, 0, 0, 0],
            [0, ae**2, ab_cov, ac_cov, ad_cov],
            [0, ab_cov, be**2, bc_cov, bd_cov],
            [0, ac_cov, bc_cov, ce**2, cd_cov],
            [0, ad_cov, bd_cov, cd_cov, de**2]
            ])
        
        syst_up_matrix = numpy.array([
            [true_acp_syst_up**2, 0, 0, 0, 0],
            [0, ae**2, ab_cov, ac_cov, ad_cov],
            [0, ab_cov, be**2, bc_cov, bd_cov],
            [0, ac_cov, bc_cov, ce**2, cd_cov],
            [0, ad_cov, bd_cov, cd_cov, de**2]
            ])
        
        syst_dn_matrix = numpy.array([
            [true_acp_syst_dn**2, 0, 0, 0, 0],
            [0, ae**2, ab_cov, ac_cov, ad_cov],
            [0, ab_cov, be**2, bc_cov, bd_cov],
            [0, ac_cov, bc_cov, ce**2, cd_cov],
            [0, ad_cov, bd_cov, cd_cov, de**2]
            ])
        
        dtg_stat    = math.sqrt( numpy.dot( numpy.matmul( der_vec, stat_matrix ),    der_vec_tran ).item(0) )
        dtg_syst_up = math.sqrt( numpy.dot( numpy.matmul( der_vec, syst_up_matrix ), der_vec_tran ).item(0) )
        dtg_syst_dn = math.sqrt( numpy.dot( numpy.matmul( der_vec, syst_dn_matrix ), der_vec_tran ).item(0) )

        dtG = float(f.subs({ y:true_acp, a:fit_a, b:fit_b, c:fit_c, d:fit_d}))
        total_unc = math.sqrt(dtg_stat**2+ dtg_syst_up**2) 
        conversion = 246*math.sqrt(2) / (1000**2) *172.5
        print dtG, dtG+2*total_unc
        print (dtG+2*total_unc)*conversion

        band68 = ROOT.TGraphErrors( nsample ) 
        for i in range(nsample):
            band68.SetPoint(i, gr.GetX()[i], 0)
        ROOT.TVirtualFitter.GetFitter().GetConfidenceIntervals(band68, 0.68)
        
        band68.Draw("E3 same")
        band68.SetFillStyle(3002)
        band68.SetFillColor(1)
        
        gr_true_bd.Draw("E3 same")
        gr_true_pt.Draw("LPF same")
        gr.Draw("XP same")
        
        leg = pltmgr.NewLegend( 0.25, 0.65, 0.6, 0.85)
        leg.SetTextSize( 12 )
        leg.AddEntry(gr_true_pt, "Measured A_{CP} and its total uncertainty", "FL" )
        leg.AddEntry(band68, "Analytical fit to MC and its uncertainty", "LF" )
        leg.Draw()

        pltmgr.SetSinglePad(canvas)
        pltmgr.SetAxis( gr.GetHistogram(), False )
        pltmgr.DrawCMSLabel()
        pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        canvas.SaveAs( opt.GetOutputName( obj, "Upperlimit" ) )

if __name__ == '__main__':
    main()
