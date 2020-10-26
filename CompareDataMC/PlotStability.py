import ROOT
import math
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

def Plot2D( title, hist16, hist17, hist18 ):
    c = pltmgr.NewCanvas()
    hist16 = hist16.ProjectionX()
    hist16.SetName( "2016" )
    hist17 = hist17.ProjectionX() 
    hist17.SetName( "2017" )
    hist18 = hist18.ProjectionX() 
    hist18.SetName( "2018" )
 
    leg = pltmgr.NewLegend( 0.68, 0.57, 0.8, 0.87 )
    leg.AddEntry( hist16, "2016", "L" )
    leg.AddEntry( hist17, "2017", "L" )
    leg.AddEntry( hist18, "2018", "L" )
    hist16.Draw( "hist" )
    hist17.Draw( "hist same" )
    hist18.Draw( "hist same" )
    leg.Draw( "same" )

    hist16.SetLineWidth( 2 )
    hist17.SetLineWidth( 2 )
    hist18.SetLineWidth( 2 )
    hist16.SetLineColor( pltmgr.Green )
    hist17.SetLineColor( pltmgr.Red )
    hist18.SetLineColor( pltmgr.Azure )

    pltmgr.SetNormToUnity( hist16 )
    pltmgr.SetNormToUnity( hist17 )
    pltmgr.SetNormToUnity( hist18 )

    hist16.SetStats( False )
    hist16.GetYaxis().SetTitle( "PDF" )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist16 )
    pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( opt.Entry() )
    c.SaveAs( opt.GetOutputName( title, "bbSep" ) )

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddFlag("B", "bbSep")
    opt.Parsing() 
    opt.AddInputName( "chi2", "bbSep" )
    opt.AddOutputName( "chi2", "bbSep" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst = [ "chi2_tmass", "chi2_jjmass" ]
    filename = opt.GetInputName( "ttbar" )
    histmgr.SetObjlst( filename, objlst, "16" )
    histmgr.SetObjlst( filename.replace( "16", "17" ), objlst, "17" )
    histmgr.SetObjlst( filename.replace( "16", "18" ), objlst, "18" )
    
    # Plot 2D_chi2_tmass
    Plot2D( "had_tmass", histmgr.GetObj("16"), histmgr.GetObj("17"), histmgr.GetObj("18") )
    Plot2D( "jj_tmass", histmgr.GetObj("16"), histmgr.GetObj("17"), histmgr.GetObj("18") )

if __name__ == '__main__':
    main()
