from array import array
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import ROOT
import math
def GetGraph( histmgr ) :
    
    ACP = [-20, -15, -10, -5, 0, 5, 10, 15, 20]
    x, y   = array( 'f' ), array( 'f' )
    ex, ey = array( 'f' ), array( 'f' )
    for a in ACP :
        hist = histmgr.GetObj( "Acp_{}".format( str(a) ) )
        
        nm  = hist.GetBinContent( 1 )
        np  = hist.GetBinContent( 2 )
        nm_err = hist.GetBinError( 1 )
        np_err = hist.GetBinError( 2 )
        
        acp = 100 * ( np - nm ) / ( np + nm )
        err_sq = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
        err    = 100 * math.sqrt( err_sq )
        
        x.append( a )
        y.append( acp )
        ex.append( 0 )
        ey.append( err )
   
    gr = ROOT.TGraphErrors( len(ACP), x, y, ex, ey )
    return gr

def main() :
    
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt")
    opt.AddFlag("a", "Acp" )
    opt.Parsing()
    opt.AddInputName ( "chi2", "Acp", "opt" )
    opt.AddOutputName( "chi2", "Acp", "opt" )
  
    # Initiailze plot manager
    histmgr = pltmgr.Plotmgr()
    filelst = [ opt.GetInputName( "ttbar" ).replace("Acp_", "Acp_{}_".format( x ) ) for x in [-20, -15, -10, -5, 0, 5, 10, 15, 20] ]
    objlst = [ "weighted_GenObs3", "weighted_Obs3", "weighted_GenObs6", "weighted_Obs6", "weighted_GenObs12", "weighted_Obs12", "weighted_GenObs13", "weighted_Obs13" ]
    
    for file in filelst:
        histmgr.SetObjlst( file, objlst, file )
   
    # Loop object list
    for obj in [ "Observable3", "Observable6", "Observable12", "Observable13" ] :
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

        c.SaveAs( opt.GetOutputName( obj, "Sim" ) )

if __name__ == '__main__':
    main()
