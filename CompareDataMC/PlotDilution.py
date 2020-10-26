import re
import os
import ROOT
import math
import importlib
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddFlag("B", "bbSep").AddInput("o","opt").AddInput("e", "uncertainty").AddInput("d", "dtg")
    opt.Parsing() 
    opt.AddInputName( "chi2", "bbSep", "opt", "uncertainty" )
    opt.AddOutputName( "chi2", "opt", "dtg" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "Ratio_Obs3", "Ratio_Obs6", "Ratio_Obs12", "Ratio_Obs14" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]

    sample = "ttbar_dtg{}".format( opt.GetOption("dtg") ) if opt.GetOption( "dtg" ) else "ttbar"

    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ))
        syslst = input.uncertainty
        if opt.LeptonType() == "co":
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ), objlst, "nom_el_{}".format( year ) )
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ), objlst, "nom_mu_{}".format( year ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}_{}".format( unc, "el", year ) )
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}_{}".format( unc, "mu", year ) )
        else:
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ), objlst, "nom_{}".format( year ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}".format( unc, year ) )
   
    # loop objlst 
    info = ""
    for idx, obj in enumerate( objlst ):
        hist = histmgr.GetMergedObj( "nom" )
        nw, nw_err = hist.GetBinContent( 1 ), hist.GetBinError( 1 )
        nc, nc_err = hist.GetBinContent( 2 ), hist.GetBinError( 2 )
        wrong_nom    = 100 * nw / ( nc + nw ) 
        
        dilution_nom = ( nc - nw ) / ( nc + nw )
        err_sq   = 4 * ( ( nw * nc_err )**2 + ( nc * nw_err )**2 ) / ( ( nc + nw )**4 )
        err      = math.sqrt( err_sq )

        wrong_systlst, dilution_systlst = [], []
        for unc in syslst:
            hist = histmgr.GetMergedObj( unc )
            nw, nw_err = hist.GetBinContent( 1 ), hist.GetBinError( 1 )
            nc, nc_err = hist.GetBinContent( 2 ), hist.GetBinError( 2 )
            wrong_unc    = 100 * nw / ( nc + nw ) 
            dilution_unc = ( nc - nw ) / ( nc + nw )
            # print unc,   wrong_nom, wrong_unc
            wrong_systlst.append( wrong_unc - wrong_nom )
            dilution_systlst.append( dilution_unc - dilution_nom )
        
        wrong_systp    = math.sqrt( sum( ( map(lambda n: n**2 if n > 0 else 0, wrong_systlst ) ) ) )
        wrong_systn    = math.sqrt( sum( ( map(lambda n: n**2 if n < 0 else 0, wrong_systlst ) ) ) )
        dilution_systp = math.sqrt( sum( ( map(lambda n: n**2 if n > 0 else 0, dilution_systlst ) ) ) )
        dilution_systn = math.sqrt( sum( ( map(lambda n: n**2 if n < 0 else 0, dilution_systlst ) ) ) )

        info += """
        {} Wrong fraction : {:3.4f} pm {:2.4f} p {:2.4f} m {:2.4f}
        {} Dilution factor: {:3.4f} pm {:2.4f} p {:2.4f} m {:2.4f}
        """.format( 
                obj, wrong_nom, 100 * err / 2, wrong_systp, wrong_systn,
                obj, dilution_nom, err, dilution_systp, dilution_systn )

        label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x )[0] )
        # print "${}$ & ${:3.2f}$ $Stat.^{{+{:2.2f}}}_{{-{:2.2f}}}$ $Syst.^{{+{:2.2f}}}_{{-{:2.2f}}}$ & ${:3.2f}$ $Stat.^{{+{:2.2f}}}_{{-{:2.2f}}}$ $Syst.^{{+{:2.2f}}}_{{-{:2.2f}}}$\\\\".format( label( obj ), 
                # wrong_nom, 100 * err / 2, 100 * err / 2, wrong_systp, wrong_systn,
                # dilution_nom, err, err, dilution_systp, dilution_systn
                # )
        print obj, dilution_nom, err, dilution_systp, dilution_systn
    with open( opt.GetOutputName( "DF", "Sim", "txt" ), "w" ) as outputfile:
        outputfile.write( info )

if __name__ == '__main__':
    main()
