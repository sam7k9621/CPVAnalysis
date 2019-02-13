from array import array
from decimal import Decimal
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import ROOT
def GetGraph( histmgr ) :
    
    ACP = [-20, -15, -10, -5, 0, 5, 10, 15, 20]
    x, y   = array( 'f' ), array( 'f' )
    ex, ey = array( 'f' ), array( 'f' )
    for a in ACP :
        hist = histmgr.GetObj( "Acp_{}".format( str(a) ) )
        
        nm  = Decimal( hist.Integral( 0, hist.FindBin(0) - 1 ) )
        np  = Decimal( hist.Integral( hist.FindBin(0), 201 ) )
        acp = 100 * ( np - nm ) / ( np + nm )
        
        err_sq = Decimal(4.) * np * nm 
        err_sq = err_sq / ( (np + nm )**3 )
        err    = 100 * err_sq.sqrt()
        
        x.append( a )
        y.append( acp )
        ex.append( 0 )
        ey.append( err )
   
    gr = ROOT.TGraphErrors( len(ACP), x, y, ex, ey )
    return gr

def main() :
    
    # Initiailze plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "GenObs2", "Obs2", "GenObs3", "Obs3", "GenObs4", "Obs4", "GenObs7", "Obs7" ]
    filelst = [
            "results/Hist_co_TTbar_chi2_20_Acp_-20_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_-15_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_-10_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_-5_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_0_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_5_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_10_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_15_Opt_150.root",
            "results/Hist_co_TTbar_chi2_20_Acp_20_Opt_150.root"
            ]
    for file in filelst:
        histmgr.SetObjlst( file, objlst )
   
    # Loop object list
    for obj in [ "Observable2", "Observable3", "Observable4", "Observable7" ] :
        gr1 = GetGraph( histmgr )
        gr2 = GetGraph( histmgr )
        
        c = pltmgr.NewCanvas()
        pltmgr.SetSinglePad( c )
        
        pltmgr.SetAxis( gr1 )
        gr1.SetTitle( '' )
        gr1.GetXaxis().SetTitle( "Input asymmetry A_{CP} [%]")
        gr1.GetYaxis().SetTitle( "Output asymmetry A_{CP} [%]")
        gr1.SetMarkerColor( ROOT.kAzure - 3 )
        gr1.SetMarkerStyle( 20 )
        gr1.SetMaximum( pltmgr.GetGraphYmax( gr1 ) * 1.1 ) 
        gr1.SetMinimum( pltmgr.GetGraphYmin( gr1 ) * 1.1 ) 
        
        gr2.SetMarkerColor( ROOT.kRed - 7 )
        gr2.SetMarkerStyle( 22 )

        fit_x1 = ROOT.TF1("fit_x1", "pol1", 0 )
        fit_x1.SetLineColor( ROOT.kAzure - 3 )
        gr1.Fit( "fit_x1" )
        
        fit_x2 = ROOT.TF1("fit_x2", "pol1", 0 )
        fit_x2.SetLineColor( ROOT.kRed - 7 )
        fit_x2.SetLineStyle( 7 )
        gr2.Fit( "fit_x2" )
    
        gr1.Draw( 'AEP' )
        gr2.Draw( 'EP SAME' )
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        pltmgr.DrawEntryLeft( "1 lep, #geq 4 jets ( 2 b jets )" ) 

        leg = pltmgr.NewLegend( 0.65, 0.2, 0.85, 0.5 )
        leg.SetHeader( obj )
        leg.AddEntry( gr1, "A_{CP}", "P" )
        leg.AddEntry( gr2, "A'_{CP}", "P" )
        leg.AddEntry( fit_x1, "Fit to A_{CP}", "L" )
        leg.AddEntry( fit_x2, "Fit to A'_{CP}", "L" )
        leg.Draw()

        c.SaveAs( "results/Hist_{}.pdf".format( obj ) )

if __name__ == '__main__':
    main()
