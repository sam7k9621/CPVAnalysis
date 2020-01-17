from __future__ import division
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import CPVAnalysis.CompareDataMC.PlotInfo as info
import math

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("A", "Acp")
    opt.AddFlag("t","test").AddFlag("x", "mixed")
    
    opt.Parsing() 
    opt.AddInputName ( "Acp", "mixed" )
    opt.AddOutputName( "Acp", "mixed" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "Obs3", "Obs6", "Obs12", "Obs13" ]
    binlst=[ "O_{{3}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{6}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{12}}^{{ {0} }}".format( opt.LeptonType() ), "O_{{13}}^{{ {0} }}".format( opt.LeptonType() ) ]
    
    hstlst = [ ROOT.TH1D( "hst_{}".format(str(i)), "", 50, -0.5, 0.5 ) for i in range(4) ]

    for i in [x for x in range(1, 1001) ]:
        idx = opt.GetInputName( opt.SampleInfo(), dir="results/ReweightSample" ).index(".")
        inputfile = opt.GetInputName( opt.SampleInfo(), dir="results/ReweightSample" )[:idx] + "_" + str(i) + opt.GetInputName( opt.SampleInfo(), dir="results/ReweightSample" )[idx:]
        histmgr.SetObjlst( inputfile, objlst, "Acp" )

        # Loop objlst
        for idx, obj in enumerate( objlst ):
        
            # Initiailze hist
            h   = histmgr.GetObj( "Acp" )
            nm = h.Integral( 0, 1 )
            np = h.Integral( 2, 3 )
    
            nm_err = h.GetBinError( 1 )
            np_err = h.GetBinError( 2 )
       
            acp = ( np - nm ) / ( np + nm ) 
            err_sq = 4 * ( ( nm * np_err )**2 + ( np * nm_err )**2 ) / ( ( np + nm  )**4 )
            err    = math.sqrt( err_sq )

            hstlst[ idx ].Fill( acp * 100 )

    c = pltmgr.NewCanvas()
    for idx, hist in enumerate( hstlst ):
        hist.SetStats( 0 )
        hist.Draw()

        ymax = pltmgr.GetHistYmax( hist ) * 1.3
        line = ROOT.TLine( 0, 0, 0, ymax )
        line.Draw( "same" )
        line.SetLineColor( ROOT.kRed )
        line.SetLineWidth( 2 )

        leg = pltmgr.NewLegend( 0.65, 0.7, 0.8, 0.87)
        leg.Draw()
        leg.AddEntry( hist, objlst[idx], "LEP" )

        hist.SetMaximum(  ymax )
        hist.SetMinimum(  0 )
        hist.GetYaxis().SetTitle( "Events" )
        hist.GetXaxis().SetTitle( "A'_{CP}[%]" )

        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( hist )
        pltmgr.DrawCMSLabelOuter( pltmgr.SIMULATION )
        c.SaveAs( opt.GetOutputName( opt.SampleInfo() + "_" + objlst[idx], "SimAcpPull" ) )


if __name__ == '__main__':
    main()
