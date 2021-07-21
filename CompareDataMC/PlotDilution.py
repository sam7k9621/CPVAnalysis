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

    # Mistag dilep sample
    sample = "ttbar_dtg{}".format( opt.GetOption("dtg") ) if opt.GetOption( "dtg" ) else "ttbar_dilep"
    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ))
        syslst = input.uncertainty
        if opt.GetOption("dtg"):
            syslst = [x for x in syslst if "CP5" not in x and "hdamp" not in x and "mtop" not in x and "SR" not in x]
        
        if opt.LeptonType() == "co":
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ), ["Syst_weighted"], "nom_el_{}_{}".format( year, sample ) )
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ), ["Syst_weighted"], "nom_mu_{}_{}".format( year, sample ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), ["Syst_weighted"], "{}_{}_{}_{}".format( unc, "el", year, sample ) )
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), ["Syst_weighted"], "{}_{}_{}_{}".format( unc, "mu", year, sample ) )
        else:
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ), ["Syst_weighted"], "nom_{}_{}".format( year, sample ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ).replace(".root", "_uncertainty_{}.root".format( unc ) ), ["Syst_weighted"], "{}_{}_{}".format( unc, year, sample ) )

    mistag_dict = dict()
    for unc in syslst+["nom"]:
        hist = histmgr.GetMergedObj( unc, sample )
        mistag_dict[unc] = hist.Integral()
  
    # Main semi sample
    sample = "ttbar_dtg{}".format( opt.GetOption("dtg") ) if opt.GetOption( "dtg" ) else "ttbar_semi"
    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ))
        syslst = input.uncertainty
        if opt.GetOption("dtg"):
            syslst = [x for x in syslst if "CP5" not in x and "hdamp" not in x and "mtop" not in x and "SR" not in x]
        
        if opt.LeptonType() == "co":
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ), objlst, "nom_el_{}_{}".format( year, sample ) )
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ), objlst, "nom_mu_{}_{}".format( year, sample ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="el" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}_{}_{}".format( unc, "el", year, sample ) )
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year, L="mu" ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}_{}_{}".format( unc, "mu", year, sample ) )
        else:
            histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ), objlst, "nom_{}_{}".format( year, sample ) )
            for unc in syslst:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=year ).replace(".root", "_uncertainty_{}.root".format( unc ) ), objlst, "{}_{}_{}".format( unc, year, sample ) )
    
    # loop objlst 
    info = ""
    for idx, obj in enumerate( objlst ):
        hist = histmgr.GetMergedObj( "nom", sample )
        nw, nw_err = hist.GetBinContent( 1 ), hist.GetBinError( 1 )
        nc, nc_err = hist.GetBinContent( 2 ), hist.GetBinError( 2 )
        
        if not opt.GetOption("dtg"):
            nc += mistag_dict["nom"] / 2.
            nw += mistag_dict["nom"] / 2.
        
        total = nc + nw
        wrong_nom    = nw / total 
        dilution_nom = ( nc - nw ) / total

        print total, nc/total, nw/total

        err_sq   = wrong_nom * (1-wrong_nom) / total 
        # err_sq   = 4 * ( ( nw * nc_err )**2 + ( nc * nw_err )**2 ) / ( ( nc + nw )**4 )
        err      = math.sqrt( err_sq )

        wrong_systlst, dilution_systlst = [], []
        for unc in syslst:
            hist = histmgr.GetMergedObj( unc, sample )
            nw, nw_err = hist.GetBinContent( 1 ), hist.GetBinError( 1 )
            nc, nc_err = hist.GetBinContent( 2 ), hist.GetBinError( 2 )
            total = nc + nw + mistag_dict["nom"]
            wrong_unc    = nw / total 
            dilution_unc = ( nc - nw ) / total

            if "mtop" in unc:
                wrong_systlst.append( (wrong_unc - wrong_nom) / 3 )            
                dilution_systlst.append( (dilution_unc - dilution_nom) / 3 )
            else:
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
                obj, 100 * wrong_nom, 100 * err, 100 * wrong_systp, 100 * wrong_systn,
                obj, dilution_nom, 2 * err, dilution_systp, dilution_systn )
        
        print "${}$ & ${:2.2f}$ $Stat.^{{+{:2.2f}}}_{{-{:2.2f}}}$ $Syst.^{{+{:2.2f}}}_{{-{:2.2f}}}$ & ${:2.4f}$ $Stat.^{{+{:2.4f}}}_{{-{:2.4f}}}$ $Syst.^{{+{:2.4f}}}_{{-{:2.4f}}}$\\\\".format(
        # print "{} {2.2f} {2.2f} {2.2f} {2.2f} {2.2f} {2.4f} {2.4f} {2.4f} {2.4f} {2.4f} ".format(
                obj, 100 * wrong_nom, 100.*err, 100.*err, 100 * wrong_systp, 100 * wrong_systn,
                dilution_nom, 2 * err, 2 * err, dilution_systp, dilution_systn 
                )
        print "\\\\[-1em]"

        label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x )[0] )
        # print obj, dilution_nom, err, dilution_systp, dilution_systn
    with open( opt.GetOutputName( "DF", "Sim", "txt" ), "w" ) as outputfile:
        outputfile.write( info )

if __name__ == '__main__':
    main()
