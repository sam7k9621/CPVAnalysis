import ROOT
import re
import math
from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

sum_prop = lambda x, y: math.sqrt( x**2 + y**2 )
div_prop = lambda x, ex, y, ey: (x/y) * math.sqrt( (ex/x)**2 + (ey/y)**2 )

def GetDF( filename ):
    lst = []
    with open( filename ) as inputfile:
        for line in filter( lambda x: "Dilution" in x, inputfile.readlines() ):
            temp = filter( None, line.split(" ") )
            df, stat, syst_up, syst_dn = temp[3], temp[5], temp[7], temp[9].rstrip("\n")
            df, stat, syst_up, syst_dn = float(df), float(stat), float(syst_up), float(syst_dn)
            lst.append( (df, sum_prop(stat, syst_up), sum_prop(stat, syst_dn) ) )
    return lst

def GetDFRatio( dic, idx ):
    nomlst, uplst, dnlst = [], [], []
    df_nom, df_nom_up, df_nom_dn = dic["0"][idx][0], dic["0"][idx][1], dic["0"][idx][2]
    for i in range(1, 6):
        df_unc, df_unc_up, df_unc_dn = dic[str(i)][idx][0], dic[str(i)][idx][1], dic[str(i)][idx][2]
        nomlst.append( df_unc / df_nom )
        uplst. append( div_prop( df_unc, df_unc_up, df_nom, df_nom_up ) )
        dnlst. append( div_prop( df_unc, df_unc_dn, df_nom, df_nom_dn ) )

    return nomlst, uplst, dnlst

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o","opt").AddInput("e", "uncertainty")
    opt.Parsing() 
    opt.AddInputName( "chi2", "opt", "uncertainty" )
    opt.AddOutputName( "chi2", "opt" )

    # Initialize plot manager
    objlst = ["Ratio_Obs3_Obs3", "Ratio_Obs3_Obs6", "Ratio_Obs3_Obs12", "Ratio_Obs3_Obs14"]
    objdic = {str(k):None for k in range(6) }
    for i in range(6):
        objdic[str(i)] = GetDF( opt.GetInputName( "DF", "Sim", "txt" ).format( L="co", Y=opt.Year() ).replace(".", "_dtg_{}.".format(i) ) )

    # Only consider Obs12
    nomlst, uplst, dnlst = GetDFRatio( objdic, 2 )

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
    ratio_total.GetXaxis().SetTitle( "d_{tG}" )

    xaxis = ratio_total.GetHistogram().GetXaxis()
    xaxis.SetLimits( 0.5, 5.5 )
    ratio_total.GetHistogram().SetMaximum( 1.03 )
    ratio_total.GetHistogram().SetMinimum( 0.97 )
   
    line = ROOT.TLine( 0.5, 1, 5.5, 1 )
    line.Draw( "same" ) 
    line.SetLineWidth( 2 )
    line.SetLineColor( ROOT.kRed )
    line.SetLineStyle( 8 )
  
    leg = pltmgr.NewLegend( 0.5, 0.74, 0.9, 0.87)
    leg.Draw()
    leg.AddEntry( ratio_total, "Obs_{12}", "EP" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( ratio_total.GetHistogram(), False )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryLeft( opt.Entry() )

    c.SaveAs( opt.GetOutputName( "DF", "Ratio" ) )
if __name__ == '__main__':
    main()
