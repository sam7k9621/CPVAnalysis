import re
import ROOT
import importlib
import operator
import math
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
from collections import OrderedDict

label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x)[0] )
yield_up_tex = []
yield_dn_tex = []
acp_up_tex = []
acp_dn_tex = []


def GetEstimatedUnc( filename ):
    with open( filename, "r" ) as inputfile:
        yld = []
        for line in filter( lambda x: "Fitted" in x, inputfile.readlines() ):
            yld.append( float( line.split(" ")[-2] ) )
    return tuple(yld)

def GetObsUnc( sghist, bghist, numlst ):
    sgneg, sgpos = sghist.Integral(0,1), sghist.Integral(2,3)
    pltmgr.SetNormToUnity( bghist )
    bgneg_frac, bgpos_frac = bghist.Integral(0,1), bghist.Integral(2,3)
    new_numlst = []
    for num in numlst:
        neg, pos = sgneg - bgneg_frac*num[1], sgpos - bgpos_frac*num[1]
        acp = 100 * ( pos - neg ) / ( pos + neg )
        new_numlst.append( acp )
    return new_numlst

def GetUncDist( unc, bin, dn, up, unclst ):
    hist = ROOT.TH1F( unc, '', bin, dn, up )  
    hist.SetStats( False )
    for unc in unclst:
        hist.Fill( unc )
    return hist

def PlotYield( temp, unc_numlst, nom_num, idx ):
    c = pltmgr.NewCanvas( temp )

    dn = 100. / nom_num[idx] * ( min( unc_numlst[160][idx], unc_numlst[839][idx] ) - nom_num[idx] )
    up = 100. / nom_num[idx] * ( max( unc_numlst[160][idx], unc_numlst[839][idx] ) - nom_num[idx] )
    
    print temp, unc_numlst[160][2], 100. / nom_num[idx] * ( unc_numlst[160][idx] - nom_num[idx] )
    print temp, unc_numlst[839][2], 100. / nom_num[idx] * ( unc_numlst[839][idx] - nom_num[idx] )
    if "_up" in unc_numlst[160][2]:
        yield_up_tex.append( "${0:+.1f}$".format( 100. / nom_num[idx] * ( unc_numlst[160][idx] - nom_num[idx] ) ) ) 
        yield_dn_tex.append( "${0:+.1f}$".format( 100. / nom_num[idx] * ( unc_numlst[839][idx] - nom_num[idx] ) ) )
    else:
        yield_dn_tex.append( "${0:+.1f}$".format( 100. / nom_num[idx] * ( unc_numlst[160][idx] - nom_num[idx] ) ) )
        yield_up_tex.append( "${0:+.1f}$".format( 100. / nom_num[idx] * ( unc_numlst[839][idx] - nom_num[idx] ) ) )

    yldlst = [ 100 * ( x[idx] - nom_num[idx] )  / nom_num[idx] for x in unc_numlst ]
    xmin = math.floor( 1.3 * min( yldlst[0], yldlst[-1] ) )
    xmax = math.ceil ( 1.3 * max( yldlst[0], yldlst[-1] ) )
    hist = GetUncDist( temp, 50, xmin, xmax, yldlst )
    hist.Draw( "HIST" )
    
    hist.SetLineColor( 1 )
    hist.SetLineWidth( 2 )
    hist.GetYaxis().SetTitle( "Events" )
    hist.GetXaxis().SetTitle( "Estimated {} yields uncertainty [%]".format( temp ) )
    ymax = 1.2 * pltmgr.GetHistYmax( hist )
    hist.SetMaximum( ymax )
    
    box    = ROOT.TBox( dn, 0, up, ymax )
    box.Draw( "F" )
    box.SetLineColor( ROOT.kAzure - 9 ) 
    box.SetFillColor( ROOT.kAzure - 9 ) 
    box.SetFillStyle( 3744 ) 
    
    line_c = ROOT.TLine( 0, 0, 0, ymax )
    line_c.Draw()
    line_c.SetLineColor( pltmgr.Red )
    line_c.SetLineStyle( 3 )
    
    leg = pltmgr.NewLegend( 0.7, 0.6, 0.85, 0.8 )
    leg.AddEntry( hist, opt.GetOption( "uncertainty" ), "L" )
    leg.AddEntry( box, "1#sigma band", "F" )
    leg.Draw()
   
    hist.Draw( "HIST same" )
    hist.Draw( "sameaxis" )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( "1000 pseudo-datasets" )
    pltmgr.DrawEntryLeft( opt.Entry() )
    c.SaveAs( opt.GetOutputName( "{}_yield".format( temp ), "Syst" ) )

def PlotObs( obs, sghist, bghist, unc_numlst, nom_num ):
    c = pltmgr.NewCanvas( obs ) 
    nom    = GetObsUnc( sghist, bghist, [ nom_num ] )[0]
    unclst = GetObsUnc( sghist, bghist, unc_numlst )
    up = ( unclst[160] - nom )
    dn = ( unclst[839] - nom )
    acp_up_tex.append( "${0:+.1f}$".format( up ) ) 
    acp_dn_tex.append( "${0:+.1f}$".format( dn ) )

    unclst = [ ( x - nom ) for x in unclst ]
    xmin = 1.3 * math.floor( 1000 * min( unclst[0], unclst[-1] ) ) / 1000. 
    xmax = 1.5 * math.ceil ( 1000 * max( unclst[0], unclst[-1] ) ) / 1000.
    print up, dn 
    print xmin,  1.3 * min( unclst[0], unclst[-1] )
    print xmax,  1.5 * max( unclst[0], unclst[-1] ) 
    hist = GetUncDist( obs, 50, xmin, xmax, unclst )
    hist.Draw( "HIST" )
    
    hist.SetLineColor( 1 )
    hist.SetLineWidth( 2 )
    hist.GetYaxis().SetTitle( "Events" )
    hist.GetXaxis().SetTitle( "{} Syst. [%]".format( label( obs ) ) )
    
    ymax = 1.2 * pltmgr.GetHistYmax( hist )
    hist.SetMaximum( ymax )
    
    box    = ROOT.TBox( dn, 0, up, ymax )
    box.Draw( "F" )
    box.SetLineColor( ROOT.kAzure - 9 ) 
    box.SetFillColor( ROOT.kAzure - 9 ) 
    box.SetFillStyle( 3744 ) 
    
    line_c = ROOT.TLine( 0, 0, 0, ymax )
    line_c.Draw()
    line_c.SetLineColor( pltmgr.Red )
    line_c.SetLineStyle( 3 )
    
    leg = pltmgr.NewLegend( 0.7, 0.6, 0.85, 0.8 )
    leg.AddEntry( hist, opt.GetOption( "uncertainty" ), "L" )
    leg.AddEntry( box, "1#sigma band", "L" )
    leg.Draw()
    
    hist.Draw( "HIST same" )
    hist.Draw( "sameaxis" )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist )
    ROOT.TGaxis.SetMaxDigits( 8 )
    
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( "1000 pseudo-datasets" )
    pltmgr.DrawEntryLeft( opt.Entry() )
    c.SaveAs( opt.GetOutputName( obs, "Syst" ) )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("u", "uncertainty")
    opt.AddFlag("t","test").AddFlag("x","mixed")
    opt.Parsing() 
    
    opt.AddInputName ( "chi2", "opt" )
    opt.AddOutputName( "chi2", "opt", "mixed", "uncertainty" )
    
    histmgr = pltmgr.Plotmgr()
    objlst  = ["Obs3", "Obs6", "Obs12", "Obs14"]
    yearlst = [ "16" ]
    input  = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( 16 ))
    unclst = [ opt.GetOption( "uncertainty" ) ] 
    if opt.GetOption( "mixed" ):
        unclst = [ "{}_{}".format( unclst[0], x ) for x in [ "up", "dn" ] ]
    
    for year in yearlst:
        # Add SR data as signal template
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( year ), objlst, "SR" )
        sghist_obs3  = histmgr.GetObj( "SR" ) 
        sghist_obs6  = histmgr.GetObj( "SR" ) 
        sghist_obs12 = histmgr.GetObj( "SR" ) 
        sghist_obs14 = histmgr.GetObj( "SR" ) 
        
        # Add CR data as background template
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( year ).replace( ".", "_region_WJets_0b_wobtag." ), objlst, "CR" )
        bghist_obs3  = histmgr.GetObj( "CR" ) 
        bghist_obs6  = histmgr.GetObj( "CR" ) 
        bghist_obs12 = histmgr.GetObj( "CR" ) 
        bghist_obs14 = histmgr.GetObj( "CR" ) 
        
        # Get nominal value
        inputfile = opt.GetInputName( "lep_tmass", "FitResult", "txt" ).format( year )
        # return (sig, bkg )
        nom_num = GetEstimatedUnc( inputfile )
            
        # Get uncertainty value
        opt.AddInputName( "mixed", "uncertainty" )
        unc_numlst = []
        for uncobj in unclst:
            for i in range( 1, 501 ):     
                inputfile = opt.GetInputName( "lep_tmass", "FitResult", "txt" ).replace( "mixed", "mixed_{}" ).replace( opt.GetOption( "uncertainty" ), uncobj ).format( year, i )
                unc_numlst.append( tuple( list( GetEstimatedUnc( inputfile ) ) + [ uncobj ] ) )
         
        # Get sorted sg, bg yeild
        unc_numlst.sort( key = lambda number : number[1] )
        
        sig_up = 100 * ( unc_numlst[160][0] - nom_num[0] ) / nom_num[0]
        sig_dn = 100 * ( unc_numlst[839][0] - nom_num[0] ) / nom_num[0]
        bkg_dn = 100 * ( unc_numlst[160][1] - nom_num[1] ) / nom_num[1]
        bkg_up = 100 * ( unc_numlst[839][1] - nom_num[1] ) / nom_num[1]
        opt.RemoveInputName( "mixed", "uncertainty" ) 
        #-------------------------------------------------------------------------------------------------------------
        PlotYield( "signal",     unc_numlst, nom_num, 0 ) 
        PlotYield( "background", unc_numlst, nom_num, 1 )
        PlotObs( "Obs3", sghist_obs3, bghist_obs3, unc_numlst, nom_num )
        PlotObs( "Obs6", sghist_obs6, bghist_obs6, unc_numlst, nom_num )
        PlotObs( "Obs12", sghist_obs12, bghist_obs12, unc_numlst, nom_num )
        PlotObs( "Obs14", sghist_obs14, bghist_obs14, unc_numlst, nom_num )
    print "& $+1 \sigma$ & {}\\\\".format( " & ".join( yield_up_tex ) )
    print "& $-1 \sigma$ & {}\\\\".format( " & ".join( yield_dn_tex ) )
    print "& $+1 \sigma$ & {}\\\\".format( " & ".join( acp_up_tex ) )
    print "& $-1 \sigma$ & {}\\\\".format( " & ".join( acp_dn_tex ) )

if __name__ == '__main__':
    main()
