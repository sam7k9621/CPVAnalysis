import ROOT
import importlib
import operator
import math
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
from collections import OrderedDict

def GetEstimatedUnc( filename ):
    with open( filename, "r" ) as inputfile:
        yld = []
        for line in filter( lambda x: "Fitted" in x, inputfile.readlines() ):
            yld.append( float( line.split(" ")[-2] ) )
    return yld

def GetObsUnc( sghist, bghist, nom, unc ):
    sgneg, sgpos = sghist.Integral(0,1), sghist.Integral(2,3)
   
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
    return 100 * ( acp_nom - acp_unc )

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("u", "uncertainty")
    opt.AddFlag("t","test")
    opt.Parsing() 
    
    opt.AddInputName ( "chi2", "opt" )
    opt.AddOutputName( "chi2", "opt", "uncertainty" )
    
    histmgr = pltmgr.Plotmgr()
    objlst  = ["Obs3", "Obs6", "Obs12", "Obs13"]
    yearlst = [ "16", "17", "18" ]
    input  = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( 16 ))
    unclst = [ opt.GetOption( "uncertainty" ) ] if opt.GetOption( "uncertainty" ) else input.uncertainty

    yield_uncdict = OrderedDict( [ ( unc, list() ) for unc in unclst ])
    acp_uncdict   = OrderedDict( [ ( unc, list() ) for unc in unclst ])
    for year in yearlst:
        # Add SR data as signal template
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( year ), objlst, "SR" )
        sghist_obs3  = histmgr.GetObj( "SR" ) 
        sghist_obs6  = histmgr.GetObj( "SR" ) 
        sghist_obs12 = histmgr.GetObj( "SR" ) 
        sghist_obs13 = histmgr.GetObj( "SR" ) 
        
        # Add CR data as background template
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( year ).replace( ".", "_region_WJets_0b_wobtag." ), objlst, "CR" )
        bghist_obs3  = histmgr.GetObj( "CR" ) 
        bghist_obs6  = histmgr.GetObj( "CR" ) 
        bghist_obs12 = histmgr.GetObj( "CR" ) 
        bghist_obs13 = histmgr.GetObj( "CR" ) 
        
        # Get nominal value
        nom_file = opt.GetInputName( "lep_tmass", "FitResult", "txt" ).format( year )
        nom = GetEstimatedUnc( nom_file )
   
        # Get up/dn value
        for uncobj in unclst:
            unc_file = nom_file[:-4] + "_uncertainty_" + uncobj + nom_file[-4:]
            unc = GetEstimatedUnc( unc_file ) 
            res = [ (i - j) / j for i, j in zip( unc, nom ) ] 
            
            # Signal yield uncertainty
            yield_uncdict[ uncobj ].append( "${0:+.3f}$".format( 100 * res[0] ) )

            # Background yield uncertainty
            yield_uncdict[ uncobj ].append( "${0:+.3f}$".format( 100 * res[1] ) )
   
            acp_uncdict[ uncobj ].append( "${0:+.1f}$".format( 1000 * GetObsUnc( sghist_obs3,  bghist_obs3,  nom, unc ) ) )
            acp_uncdict[ uncobj ].append( "${0:+.1f}$".format( 1000 * GetObsUnc( sghist_obs6,  bghist_obs6,  nom, unc ) ) )
            acp_uncdict[ uncobj ].append( "${0:+.1f}$".format( 1000 * GetObsUnc( sghist_obs12, bghist_obs12, nom, unc ) ) )
            acp_uncdict[ uncobj ].append( "${0:+.1f}$".format( 1000 * GetObsUnc( sghist_obs13, bghist_obs13, nom, unc ) ) )
    
        label = lambda x : "$+1 \sigma$" if "_up" in x else "$-1 \sigma$"
    for key, value in yield_uncdict.iteritems():
        print key
        print "& {} & {}\\\\".format( label( key ), " & ".join( value ) )
    
    print "*" * 50
    for key, value in acp_uncdict.iteritems():
        print key
        print "& {} & {}\\\\".format( label( key ), " & ".join( value ) )

        # print yield_uncdict 
        # print acp_uncdict

if __name__ == '__main__':
    main()
