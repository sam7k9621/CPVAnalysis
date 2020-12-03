import ROOT
import re
import math
from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
def GetDFRatio( lst ):
    nom, nom_up, nom_dn = lst.pop(0)
    
    return [ unc/nom for unc, unc_up, unc_dn in lst],  [ math.sqrt( (nom_up/nom)**2 + (unc_up/unc)**2 ) for unc, unc_up, unc_dn in lst], [ math.sqrt( (nom_dn/nom)**2 + (unc_dn/unc)**2 ) for unc, unc_up, unc_dn in lst]
    # for unc, unc_up, unc_dn in lst:
        # # unc, unc_up, unc_dn = l
        # print unc/nom, (unc/nom) * math.sqrt( (nom_up/nom)**2 + (unc_up/unc)**2 ), (unc/nom) * math.sqrt( (nom_dn/nom)**2 + (unc_dn/unc)**2 )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o","opt").AddInput("e", "uncertainty")
    opt.Parsing() 
    opt.AddInputName( "chi2", "opt", "uncertainty" )
    opt.AddOutputName( "chi2", "opt" )

    # Initialize plot manager
    objlst = ["Obs3", "Obs6", "Obs12", "Obs14"]
    is_float = re.compile('\d+(\.\d+)?')
    filename = opt.GetOutputName( "DF", "Sim", "txt" )
    dbook = {k:[] for k in objlst}
    with open( filename, "r" ) as inputfile:
        lines= filter( lambda x : "Dilution" in x, inputfile.readlines() )
        for idx, obj in enumerate( objlst ):    
            nom, stat, pos, neg = tuple( float(l) for l in lines[idx].split() if is_float.match( l ) ) 
            dbook[ obj ].append( (nom, math.sqrt( stat**2 + pos**2 ), math.sqrt( stat**2 + neg**2 ) ) )

    for i in range( 1, 6 ):
        with open( filename.replace(".", "_dtg_{}.".format(i)), "r" ) as inputfile:
            lines= filter( lambda x : "Dilution" in x, inputfile.readlines() )
            for idx, obj in enumerate( objlst ):    
                nom, stat, pos, neg = tuple( float(l) for l in lines[idx].split() if is_float.match( l ) ) 
                dbook[ obj ].append( (nom, math.sqrt( stat**2 + pos**2 ), math.sqrt( stat**2 + neg**2 ) ) )

    obj = "Obs12"
    nomlst, uplst, dnlst = GetDFRatio( dbook[obj] )

    x, y     = array( 'd' ), array( 'd' )
    exl, exh = array( 'd' ), array( 'd' )
    eyl, eyh = array( 'd' ), array( 'd' )
    for idx, ( nom, up, dn ) in enumerate( zip( nomlst, uplst, dnlst ) ):
        x.append( idx + 1 )
        y.append( nom )
        exl.append( 0 )
        exh.append( 0 )
        eyh.append( up )
        eyl.append( dn )

    ratio_total = ROOT.TGraphAsymmErrors( len(nomlst), x, y, exl, exh, eyl, eyh ) 
    c = pltmgr.NewCanvas()
    ratio_total.Draw("AEP")
    
    ratio_total.SetMarkerStyle( 20 )
    ratio_total.SetMarkerSize( 1.3 )
    ratio_total.SetMarkerColor( ROOT.kGreen - 6 )
    
    ratio_total.SetLineColor( ROOT.kGreen - 6 )
    ratio_total.SetLineWidth( 2 )

    ratio_total.SetTitle( "" )
    ratio_total.GetYaxis().SetTitle( "D_{CEDM} / D_{SM}" )
    ratio_total.GetXaxis().SetTitle( "d_{t}^{g}" )

    xaxis = ratio_total.GetHistogram().GetXaxis()
    xaxis.SetLimits( 0.5, 5.5 )
    ymax = pltmgr.GetGraphlstYmax( [ ratio_total ] ) * 1.1
    ymin = pltmgr.GetGraphlstYmin( [ ratio_total ] ) * 0.9
    ratio_total.GetHistogram().SetMaximum( ymax )
    ratio_total.GetHistogram().SetMinimum( ymin )
   
    line = ROOT.TLine( 0.5, 1, 5.5, 1 )
    line.Draw( "same" ) 
    line.SetLineWidth( 2 )
    line.SetLineColor( ROOT.kRed )
    line.SetLineStyle( 8 )
  
    leg = pltmgr.NewLegend( 0.5, 0.74, 0.9, 0.87)
    leg.Draw()
    leg.AddEntry( ratio_total, obj, "EP" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( ratio_total.GetHistogram(), False )
    pltmgr.DrawCMSLabel( pltmgr.PRELIMINARY )
    pltmgr.DrawLuminosity( opt.Lumi() )
    pltmgr.DrawEntryLeft( opt.Entry() )

    c.SaveAs( opt.GetOutputName( "DF", "Ratio" ) )
if __name__ == '__main__':
    main()
