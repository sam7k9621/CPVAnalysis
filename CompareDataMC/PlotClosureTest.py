import ROOT, importlib
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("r", "region").AddInput( "t", "template" )
    
    opt.SetName( "chi2", "region" )
    opt.Parsing() 
    
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "lep_tmass" ]
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
    
    # Add SR background MC
    template = opt.GetOption( "template" )
    if template == "SR":
        print "-" * 90
        print ">> Adding SR bkg. MC"
        for sample in input.samplelst:
            if any( x in sample for x in [ "ttbar", "Data", "QCD" ] ):
                continue 
            filename = opt.GetCustomFileName( opt.GetOption( "region", True ), "",sample )
            histmgr.SetObjlst( filename, objlst, "SR" + sample ) 
    elif template == "CR":
        # Add CR background MC
        print "-" * 90
        print ">> Adding CR bkg. MC"
        for sample in input.samplelst:
            if any( x in sample for x in [ "ttbar", "Data" ] ):
                continue 
            filename = opt.GetFileName( sample )
            histmgr.SetObjlst( filename, objlst, "CR" + sample ) 

        # Change into data-driven CR QCD sample
        print "-" * 90
        print ">> Adding data-driven CR QCD"
        qcd_histlst = [ histmgr.GetMergedObj( "CRQCD" ) for o in objlst ]
        histmgr.RemoveObj( "CRQCD" )
        filename = opt.GetCustomFileName( "region_WJets", "region_QCD_0bjet", "Data" )
        histmgr.SetObjlst( filename, objlst, "CRQCD" ) 
    else:
        print "Please specify the template"
        return
    
    # Add data-driven shape
    print "-" * 90
    print ">> Adding CR data"
    filename = opt.GetFileName( "Data" )
    histmgr.SetObjlst( filename, objlst, "Data" ) 
    
    # Add data-driven orthogonal shape
#     print "-" * 90
    # print ">> Adding orthogonal CS data"
    # filename = filename.replace( "chi2", "invChi2" )
    # histmgr.SetObjlst( filename, objlst, "invData" ) 
   
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        
        # Retrieve hist
        data = histmgr.GetObj( "Data" )
        # data_inv = histmgr.GetObj( "invData" )
        histlst = []
        mclst = [ "DYJets", "SingleTop", "VV", "WJets", "QCD" ]
        if template == "SR":
            mclst.remove( "QCD" )

        for i, mc in enumerate( mclst ):
            histlst.append( histmgr.GetMergedObj( template + mc ) )
            if mc == "QCD":
                pltmgr.SetNormToUnity( histlst[ i ] )
                histlst[ i ].Scale( qcd_histlst[ idx ].Integral() )
                continue
        bg_sum = pltmgr.SumHist( histlst )
 
        # Plot setting
        c = pltmgr.NewCanvas( obj )
        leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
        
        bg_sum.Draw( "hist e" )
        data    .Draw( "ep same" )
        # data_inv.Draw( "ep same" )
        leg.Draw()
        
        bg_sum  .SetLineColor( 1 )
        data    .SetLineColor( ROOT.kAzure - 3 )
        # data_inv.SetLineColor( ROOT.kGreen - 6 )
        bg_sum  .SetLineWidth( 2 )
        data    .SetLineWidth( 2 )
        # data_inv.SetLineWidth( 2 )

        leg.SetHeader( opt.LeptonType() + "-channel" ) 
        leg.AddEntry( data,     ( "CR data ( #chi^{2} < 20 )" ), "lep" )
        # leg.AddEntry( data_inv, ( "CS data ( #chi^{2} > 20 )" ), "lep" )
        leg.AddEntry( bg_sum,   ( template + " bkg. MC" ), "lep" )

        pltmgr.SetNormToUnity( bg_sum   )
        pltmgr.SetNormToUnity( data     )
        # pltmgr.SetNormToUnity( data_inv )
        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( bg_sum )
        pltmgr.DrawCMSLabelOuter( pltmgr.PRELIMINARY )
        pltmgr.DrawLuminosity( opt.Lumi() )
        
        bg_sum.GetYaxis().SetTitle( "PDF" ) 
        bg_sum.SetMaximum( pltmgr.GetHistYmax( bg_sum ) * 1.5 );
        c.SaveAs( opt.GetResultName( template, "BGClosureTest" ) )

if __name__ == '__main__':
    main()
