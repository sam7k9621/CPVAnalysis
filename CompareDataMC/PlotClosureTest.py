import ROOT, importlib
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput( "t", "template" )
    opt.AddFlag("x","worelax").AddFlag("b","wobtag")

    opt.Parsing() 
    opt.AddInputName ( "chi2" )
    opt.AddOutputName( "chi2", "worelax", "wobtag" )
    
    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    objlst=[ "lep_tmass" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]
  
    template = opt.GetOption( "template" )
    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ) )
        # Add data-driven shape
        print "-" * 90
        print ">> Adding CR data"
        filename = opt.GetInputName( "Data" ).format( Y=year ).replace( ".", "_region_WJets_0b" + opt.GetOption( "worelax", True ) + opt.GetOption( "wobtag", True ) + "." )
        histmgr.SetObjlst( filename, objlst, year + "_Data" ) 
   
        if template == "SR":
            # Add SR background MC
            print "-" * 90
            print ">> Adding SR bkg. MC"
            for sample in input.samplelst:
                if any( x in sample for x in [ "ttbar", "Data", "QCD" ] ):
                    continue
                filename = opt.GetInputName( sample ).format( Y=year )
                histmgr.SetObjlst( filename, objlst, "{}_SR_{}".format( year, sample ) ) 
        elif template == "CR":
            # Add CR background MC
            print "-" * 90
            print ">> Adding CR bkg. MC"
            for sample in input.samplelst:
                if any( x in sample for x in [ "ttbar_semi", "ttbar_had", "ttbar_dilep", "Data" ] ):
                    continue 
                filename = opt.GetInputName( sample ).format( Y=year ).replace( ".", "_region_WJets_0b" + opt.GetOption( "worelax", True ) + opt.GetOption( "wobtag", True ) + "." )
                histmgr.SetObjlst( filename, objlst, "{}_CR_{}".format( year, sample ) ) 

            # Change into data-driven CR QCD sample
            print "-" * 90
            print ">> Adding data-driven CR QCD"
            qcd_histlst = [ histmgr.GetMergedObj( year + "_CR_QCD" ) for o in objlst ]
            histmgr.RemoveObj( "CR_QCD" )
            filename = opt.GetInputName( "Data" ).format( Y=year ).replace( ".", "_region_QCD_0b" + opt.GetOption( "wobtag", True ) + "." )
            histmgr.SetObjlst( filename, objlst, year + "_CR_QCD_" ) 
        else:
            print "Please specify the template"
            return

    # Loop objlst
    for idx, obj in enumerate( objlst ):
        # Retrieve hist
        data = histmgr.GetMergedObj( "Data" )
        # data_inv = histmgr.GetObj( "invData" )
        histlst = []
        mclst = [ "DYJets", "SingleTop", "VV", "WJets", "QCD" ]
        if template == "SR":
            mclst.remove( "QCD" )

        for i, mc in enumerate( mclst ):
            histlst.append( histmgr.GetMergedObj( template, mc ) )
            if mc == "QCD":
                pltmgr.SetNormToUnity( histlst[ i ] )
                histlst[ i ].Scale( qcd_histlst[ idx ].Integral() )
                continue
        bg_sum = pltmgr.SumHist( histlst )
 
        # Plot setting
        c = pltmgr.NewCanvas( obj )
        leg = pltmgr.NewLegend( 0.67, 0.51, 0.8, 0.81)
        
        bg_sum.Draw( "hist e" )
        data  .Draw( "ep same" )
        # data_inv.Draw( "ep same" )
        leg.Draw()
        
        bg_sum  .SetLineColor( 1 )
        data    .SetLineColor( ROOT.kAzure - 3 )
        # data_inv.SetLineColor( ROOT.kGreen - 6 )
        bg_sum  .SetLineWidth( 2 )
        data    .SetLineWidth( 2 )
        # data_inv.SetLineWidth( 2 )

        leg.AddEntry( data,     ( "CR data" ), "lep" )
        # leg.AddEntry( data_inv, ( "CS data ( #chi^{2} > 20 )" ), "lep" )
        leg.AddEntry( bg_sum,   ( template + " bkg. MC" ), "lep" )

        pltmgr.SetNormToUnity( bg_sum   )
        pltmgr.SetNormToUnity( data     )
        # pltmgr.SetNormToUnity( data_inv )
        
        bg_sum.GetYaxis().SetTitle( "PDF" ) 
        bg_sum.SetMaximum( pltmgr.GetHistYmax( bg_sum ) * 1.5 )
        
        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( bg_sum )
        pltmgr.DrawCMSLabelOuter()
        # pltmgr.DrawCMSLabelOuter( pltmgr.PRELIMINARY )
        pltmgr.DrawLuminosity( opt.Lumi() )
        c.SaveAs( opt.GetOutputName( template, "BGClosureTest" ) )

if __name__ == '__main__':
    main()
