import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
from itertools import cycle

def main() :
    # Initialize parsing manager
    global opt 
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o","opt")
    opt.Parsing() 
    opt.AddInputName( "chi2", "opt" )
    opt.AddOutputName( "chi2", "opt" )

    # Initialize plot manager
    objlst = ["lep_tmass", "had_tmass", "Obs3_dist"]
    
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
   
    for i in range( 6 ):
        histmgr.SetObjlst( opt.GetInputName( "ttbar_dtg{}".format( i ) ).format( Y=opt.Year() ), objlst, "dtg_{}".format( i ) )

    c = pltmgr.NewCanvas()
    for idx, obj in enumerate( objlst ):
        histlst = []
        colorcycle =  cycle( pltmgr.colorlst )
        leg = pltmgr.NewLegend( 0.72, 0.4, 0.85, 0.8)
        for i in range( 6 ):
            color = next( colorcycle )
            histlst.append( histmgr.GetObj( "dtg_{}".format( i ) ) ) 
            histlst[-1].SetLineColor( color )
            histlst[-1].SetLineWidth( 2 )
            
            leg.AddEntry( histlst[-1], "d^{{t}}_{{g}} = {}".format( i ), "L" )
            pltmgr.SetNormToUnity( histlst[-1] )

        hist = histlst.pop()
        hist.GetYaxis().SetTitle( "PDF" )
        hist.Draw("HIST")
        for h in histlst:
            h.Draw( "HIST same" )
        hist.Draw("sameaxis" )
        leg.Draw( "same" )
        
        hist.SetMaximum( pltmgr.GetHistYmax( hist ) * 1.3 )
        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( hist )
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        # pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        c.SaveAs( opt.GetOutputName( obj, "CEDM_dist" ) )


if __name__ == '__main__':
    main()
