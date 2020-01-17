import ROOT
import importlib
import operator
import math
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

tex = []

def GetEstimatedUnc( filename ):
    with open( filename, "r" ) as inputfile:
        yld = []
        for line in filter( lambda x: "Fitted" in x, inputfile.readlines() ):
            yld.append( float( line.split(" ")[-2] ) )
    return yld

def GetObsUnc( histmgr, nom, unc, obs ):
    sghist = histmgr.GetObj( "SR" )
    sgneg, sgpos = sghist.Integral(0,1), sghist.Integral(2,3)
   
    bghist = histmgr.GetObj( "CR" )
    pltmgr.SetNormToUnity( bghist )
    bghist.Scale( nom[1] )
    bgneg, bgpos = bghist.Integral(0,1), bghist.Integral(2,3)
    neg, pos = sgneg - bgneg, sgpos - bgpos 

    acp_nom = ( pos - neg ) / ( pos + neg )
    
    pltmgr.SetNormToUnity( bghist )
    bghist.Scale( unc[1] )
    bgneg, bgpos = bghist.Integral(0,1), bghist.Integral(2,3)
    neg, pos = sgneg - bgneg, sgpos - bgpos 

    acp_unc = ( pos - neg ) / ( pos + neg )

    print ">> {:5s}: {:0.4f}%".format( obs, (acp_nom - acp_unc) * 100 )

    tex.append("${:+0.4f}$".format((acp_nom - acp_unc) * 100))

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("u", "uncertainty")
    opt.AddFlag("t","test")
    opt.Parsing() 
    
    opt.AddInputName ( "chi2", "opt" )
    opt.AddOutputName( "chi2", "opt", "uncertainty" )
    nom_file = opt.GetInputName( "lep_tmass", "FitResult", "txt" )
    
    func = min if "dn" in opt.GetOption( "uncertainty" ) else max
    if "PDF" in opt.GetOption( "uncertainty" ):
        unclst = [ "PDF_" + str(x) for x in range(10, 110) ]
        if "dn" in opt.GetOption( "uncertainty" ):
            asunc = "PDF_110"
        else:
            asunc = "PDF_111"

    elif "muFmuR" in opt.GetOption( "uncertainty" ):
        unclst = [ "muFmuR_" + str(x) for x in range(1,9) if x not in [5, 7] ]
    else:
        unclst = [ opt.GetOption( "uncertainty" ) ]
  
    nom = GetEstimatedUnc( nom_file )
    unc_chosen = []
    if "PDF" in opt.GetOption( "uncertainty" ):
        unc_file = nom_file[:-4] + "_uncertainty_" + asunc + nom_file[-4:]
        unc = GetEstimatedUnc( unc_file )
        asunc = [ (i - j) / j for i, j in zip( unc, nom ) ] 
    
    for uncobj in unclst:
        unc_file = nom_file[:-4] + "_uncertainty_" + uncobj + nom_file[-4:]
        unc = GetEstimatedUnc( unc_file )

        res = [ (i - j) / j for i, j in zip( unc, nom ) ] 
        unc_chosen.append( res )
        
    unc_chosen = func( unc_chosen, key=lambda x: x[0])
    if "PDF" in opt.GetOption( "uncertainty" ):
        unc_chosen  = [ i / abs(i) * math.sqrt( i**2 + j**2 ) for i, j in zip( unc_chosen, asunc ) ]

    print ">> Signal     yield uncertainty: {:0.3f}%".format( 100 * unc_chosen[0] )
    print ">> Background yield uncertainty: {:0.3f}%".format( 100 * unc_chosen[1] )

    histmgr = pltmgr.Plotmgr()
    objlst  = ["Obs3", "Obs6", "Obs12", "Obs13"]
    
    # Add SR data as signal template
    histmgr.SetObjlst( "results/Hist_{}_{}_Data_chi2_20_opt_150.root".format( opt.Year(), opt.LeptonType() ), objlst, "SR" )
    
    # Add CR data as background template
    histmgr.SetObjlst( "results/Hist_{}_{}_Data_chi2_20_opt_150_region_WJets.root".format( opt.Year(), opt.LeptonType() ), objlst, "CR" )

    unc_chosen = [ i * ( 1 + j ) for i, j in zip( nom, unc_chosen ) ]
    GetObsUnc( histmgr, nom, unc_chosen, "Obs3" )
    GetObsUnc( histmgr, nom, unc_chosen, "Obs6" )
    GetObsUnc( histmgr, nom, unc_chosen, "Obs12" )
    GetObsUnc( histmgr, nom, unc_chosen, "Obs13" )

    print " & ".join( tex )

if __name__ == '__main__':
    main()
