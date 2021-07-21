from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import ROOT
import math
import re
def GetGraph( histmgr, D, D_err ) :
    
    input_acp = [-15, -10, -7, -5, -3, 0, 3, 5, 7, 10, 15 ]
    eff_acp_x, eff_acp_y, true_acp_x, true_acp_y     = array( 'f' ), array( 'f' ), array( 'f' ), array( 'f' )
    eff_acp_ex, eff_acp_ey, true_acp_ex, true_acp_ey = array( 'f' ), array( 'f' ), array( 'f' ), array( 'f' )
    for a in input_acp:
        hist = histmgr.GetMergedObj( "ttbar_semi", "Acp_{}".format( a ) )
        
        nm  = hist.GetBinContent( 1 )
        np  = hist.GetBinContent( 2 )
        nm_err = hist.GetBinError( 1 )
        np_err = hist.GetBinError( 2 )
       
        eff_acp     = 100 * ( np - nm ) / ( np + nm + mistag_evt[a] )
        err_sq      = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
        eff_acp_err = 100 * math.sqrt( err_sq )
      
        # A'cp part
        eff_acp_x. append( a )
        eff_acp_y. append( eff_acp )
        eff_acp_ex.append( 0 )
        eff_acp_ey.append( eff_acp_err )

        # Acp part ( Acp = D * A'cp )
        true_acp     = eff_acp / D 
        true_acp_err = abs(true_acp) * math.sqrt( ( eff_acp_err / eff_acp )**2 + ( D_err / D )**2 )
       
        print a, true_acp_err, math.sqrt( ( eff_acp_err / eff_acp )**2 + ( D_err / D )**2 )

        true_acp_x .append( a )
        true_acp_y .append( true_acp )
        true_acp_ex.append( 0 )
        true_acp_ey.append( 2 * true_acp_err )

    gr_eff_acp  = ROOT.TGraphErrors( len(input_acp), eff_acp_x, eff_acp_y, eff_acp_ex, eff_acp_ey )
    gr_true_acp = ROOT.TGraphErrors( len(input_acp), true_acp_x, true_acp_y, true_acp_ex, true_acp_ey )
    return gr_true_acp, gr_eff_acp

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt")
    opt.AddFlag("a", "Acp" ).AddFlag("b", "bbSep" )
    opt.Parsing()
    opt.AddInputName ( "chi2", "bbSep", "Acp", "opt" )
    opt.AddOutputName( "chi2", "bbSep", "Acp", "opt" )
  
    # Initiailze plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "weighted_Obs3", "weighted_Obs6", "weighted_Obs12", "weighted_Obs14" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]
    input_acp = [-15, -10, -7, -5, -3, 0, 3, 5, 7, 10, 15 ]
   
    filelst_el, filelst_mu = [], []
    for year in yearlst:
        filelst_el += [ opt.GetInputName( "ttbar_semi" ).format( Y=year, L="el" ).replace("Acp_", "Acp_{}_".format( x ) ) for x in input_acp ]
        filelst_mu += [ opt.GetInputName( "ttbar_semi" ).format( Y=year, L="mu" ).replace("Acp_", "Acp_{}_".format( x ) ) for x in input_acp ]

    for file in filelst_el + filelst_mu:
        histmgr.SetObjlst( file, objlst, file )
    
    filelst_el, filelst_mu = [], []
    for year in yearlst:
        filelst_el += [ opt.GetInputName( "ttbar_dilep" ).format( Y=year, L="el" ).replace("Acp_", "Acp_{}_".format( x ) ) for x in input_acp ]
        filelst_mu += [ opt.GetInputName( "ttbar_dilep" ).format( Y=year, L="mu" ).replace("Acp_", "Acp_{}_".format( x ) ) for x in input_acp ]

    for file in filelst_el + filelst_mu:
        histmgr.SetObjlst( file, ["Syst_weighted"], file )
 
    global mistag_evt
    mistag_evt = dict()
    for a in input_acp:
        hist = histmgr.GetMergedObj( "ttbar_dilep", "Acp_{}".format( a ) )
        mistag_evt[a] = hist.Integral()

    # Dilution factor
    year = "RunII" if opt.Year() == "RunII" else opt.Year()
    lept = "co" if opt.LeptonType() == "co" else opt.LeptonType()
    with open( opt.GetInputName( "DF", "Sim", "txt" ).format( Y=year, L=lept ).replace( "bbSep_Acp_", "" ), "r" ) as inputfile:
        DF_lst = [ x.lstrip().rstrip("\n") for x in inputfile.readlines() ]

    # Loop object list
    c = pltmgr.NewCanvas()
    for i, obj in enumerate( [ "Obs3", "Obs6", "Obs12", "Obs14" ] ):
        
        for df in DF_lst:
            if all( x in df for x in [obj, "Dilution"] ):
                DF     = float( df.split(" ")[3] )
                DF_stat = float( df.split(" ")[5] )
                DF_syst = max( float( df.split(" ")[7] ), float( df.split(" ")[9] ) )
                DF_err  = math.sqrt( DF_stat**2 + DF_syst**2 )
       
        gr1, gr2 = GetGraph( histmgr, DF, DF_err )
        
        gr1.Draw( 'AEP' )
        gr2.Draw( 'EP same' )
        
        gr2.SetMarkerColor( ROOT.kRed - 7 )
        gr2.SetMarkerStyle( 20 )
        gr2.SetMarkerSize( 1.2 )

        label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x )[0] )
        gr1.SetTitle( '' )
        gr1.GetXaxis().SetTitle( "Input asymmetry A_{{CP}} in {} [%]".format( label( obj ) ) )
        gr1.GetYaxis().SetTitle( "Output asymmetry A_{CP} [%]")
        gr1.SetMarkerColor( ROOT.kAzure - 3 )
        gr1.SetMarkerStyle( 33 )
        gr1.SetMarkerSize( 1.5 )
       
        fit_x1 = ROOT.TF1("fit_x1", "pol1", 0 )
        fit_x1.SetLineColor( ROOT.kAzure - 3 )
        fit_x1.SetLineWidth( 3 )
        gr1.Fit( "fit_x1")

        fit_x2 = ROOT.TF1("fit_x2", "pol1", 0 )
        fit_x2.SetLineColor( ROOT.kRed - 7 )
        fit_x2.SetLineStyle( 2 )
        fit_x2.SetLineWidth( 3 )
        gr2.Fit( "fit_x2" )
        
        line = ROOT.TLine( -20, -20, 20, 20 )
        line.SetLineStyle( 7 )
        line.Draw()

        leg = pltmgr.NewLegend( 0.65, 0.2, 0.85, 0.5 )
        leg.AddEntry( gr1, "A_{CP}", "P" )
        leg.AddEntry( gr2, "A'_{CP}", "P" )
        leg.AddEntry( fit_x1, "Fit to A_{CP}", "L" )
        leg.AddEntry( fit_x2, "Fit to A'_{CP}", "L" )
        leg.Draw()

        gr1.Draw( "EP same" )

        gr1.GetXaxis().SetLimits( -20, 20 )
        gr1.SetMaximum( 20 ) 
        gr1.SetMinimum( -20 ) 
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        pltmgr.DrawEntryLeft( opt.Entry() ) 
        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( gr1 )
        c.SaveAs( opt.GetOutputName( obj, "Sim" ) )

if __name__ == '__main__':
    main()
