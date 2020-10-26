from __future__ import division
import ROOT
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import importlib
import math
import numpy as np 

def main() :
    # Initialize parsing manager
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("r", "region").AddInput("e", "uncertainty")
    opt.AddFlag( "b", "0bjet" ).AddFlag( "p", "wopileup" ).AddFlag( "w", "wobtag" ).AddFlag("u", "wosyst" )
    
    opt.Parsing() 
    opt.AddInputName ( "chi2", "opt", "region", "wopileup", "wobtag" )
    opt.AddOutputName( "chi2", "opt", "region", "wopileup", "wobtag", "uncertainty" )

    # Initialize plot manager
    histmgr = pltmgr.Plotmgr()
    region = opt.GetOption( "region" ).split("_")[0]
    objlst=[ "had_tmass", "Obs14_dist" ]
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))

    # Initialize MC sample list
    mclst  = [ "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttbar" ]
    if not region:
        mclst.remove( "QCD" )
    else:
        mclst.remove( region )
        mclst.append( region ) # Dominant sample should be added last
   
    # Read files
    for sample in input.samplelst:
        filename = opt.GetInputName( sample )
        histmgr.SetObjlst( filename, objlst, sample + "_nom" )
    
        if sample == "ttbar":
            histmgr.SetObjlst( filename.replace(".", "_uncertainty_{}.".format( opt.GetOption( "uncertainty" ) ) ), objlst, sample + "_unc" ) 
        else:
            histmgr.SetObjlst( filename, objlst, sample + "_unc" )

    c = pltmgr.NewCanvas( )
    # Loop objlst
    for idx, obj in enumerate( objlst ):
        leg = pltmgr.NewLegend( 0.67, 0.61, 0.8, 0.81)
        mc_nom = ROOT.THStack()
        mc_unc = ROOT.THStack()
        # Initiailze hist
        histlst_nom = []
        histlst_unc = []
        for i, sample in enumerate( mclst ):
            
            if "ttbar" not in sample:
                continue
            
            histlst_nom.append( histmgr.GetMergedObj( sample, "nom" ) )
            # histlst_nom[i].SetLineColor( pltmgr.colorlst[i] )
            # histlst_nom[i].SetFillColor( pltmgr.colorlst[i] )
            histlst_nom[-1].SetLineColorAlpha( pltmgr.Red, 0.4 )
            histlst_nom[-1].SetFillColorAlpha( pltmgr.Red, 0.4 )
            
            histlst_unc.append( histmgr.GetMergedObj( sample, "unc" ) )
            histlst_unc[-1].SetLineColorAlpha( ROOT.kAzure - 9, 0.4 )
            histlst_unc[-1].SetFillColorAlpha( ROOT.kAzure - 9, 0.4 )
            
            mc_nom.Add( histlst_nom[-1] )
            mc_unc.Add( histlst_unc[-1] )
            leg.AddEntry( histlst_nom[-1], "{}_nom".format(sample), "F" )
            leg.AddEntry( histlst_unc[-1], "{}_{}".format(sample, opt.GetOption("uncertainty")), "F" )

        bg_sum = pltmgr.SumHist( histlst_nom )
      
        bg_sum.Draw("EP")
        mc_nom.Draw( "HIST same" )
        mc_unc.Draw( "HIST same" )
        bg_sum.Draw("sameaxis")
        leg.Draw()
        
        line = ROOT.TLine( 0, 0, 0, pltmgr.GetHistYmax( bg_sum ) * 1.5 )
        line.Draw( "same" )
        line.SetLineColor( ROOT.kRed )
        line.SetLineStyle( 3 )
        line.SetLineWidth( 2 )
         
        bg_sum.SetMaximum( pltmgr.GetHistYmax( bg_sum ) * 1.5 )
        pltmgr.SetSinglePad( c )
        pltmgr.SetAxis( bg_sum )
        pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
        pltmgr.DrawLuminosity( opt.Lumi() )
        pltmgr.DrawEntryLeft( opt.Entry() )
        c.SaveAs( opt.GetOutputName( obj, "Stack_MC" ) )


if __name__ == '__main__':
    main()
