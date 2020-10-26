import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import re
import json
import math
import ROOT
import importlib
import collections

GetAcp    = lambda x, y: 100. * float( x - y ) / ( x + y )
GetAcpErr = lambda x, y: 100. * math.sqrt( 4. * float(x) * y / ( (x + y )**3) ) 
nevent    = lambda bkg, frac, pos, neg: [ GetAcp( p - b * f, n - b * (1-f) ) for b, f, p, n in zip( bkg, frac, pos, neg ) ] 
neventerr = lambda bkg, frac, pos, neg: [ GetAcpErr( p - b * f, n - b * (1-f) ) for b, f, p, n in zip( bkg, frac, pos, neg ) ] 
label = lambda x: "O_{{{}}}".format( re.findall(r'\d+', x )[0] )

def GetExpectedYield( filename ):
    with open( filename, "r" ) as inputfile:
        sg, bg = 0, 0
        for line in inputfile.readlines():
            if "ttbar" in line:
                sg = float( line.split( " " )[1].lstrip("\n") )
            if "MC" in line:                          
                bg = float( line.split( " " )[1].lstrip("\n") )
    return sg, bg - sg

def GetYldSyst( obj, nom, unc ):
    vlst  = []
    digit = 100. if "sig" in obj else 10. 
    for n, u in zip( nom, unc ):
        vlst.append( 100. * (u - n) / n )
    xmin, xmax =  math.floor( digit*min( vlst ) ) / digit, math.ceil( digit*max( vlst ) ) / digit

    hist = ROOT.TH1D( obj, "", 100, xmin, xmax )
    hist.SetStats( False )
    hist.GetXaxis().SetTitle( "Fitted {}. yield [%]".format( obj.split("_")[1] ) )
    hist.GetYaxis().SetTitle( "Events" )
    for v in vlst:
        hist.Fill( v )
    return PlotSyst( obj, hist ) 

def GetAcpSyst( obj, nom, unc ):
    hist = ROOT.TH1D( obj, "", 120, -0.011, 0.013 ) 
    hist.SetStats( False )
    hist.GetXaxis().SetTitle( "{} Syst. [%]".format( label( obj ) ) )
    hist.GetYaxis().SetTitle( "Events" )
    for n, u in zip( nom, unc ):
        hist.Fill( u-n )
    return PlotSyst( obj, hist )

def PlotSyst( obj, hist ):
    c = pltmgr.NewCanvas()
    hist.Draw( "HIST" )

    ymax = 1.4 * pltmgr.GetHistYmax( hist )
    hist.SetMaximum( ymax )
    hist.SetLineColor( 1 )
    hist.SetLineWidth( 2 )
    
    hist.Fit( "gaus", "q" )
    gaus = hist.GetFunction( "gaus" )
    gaus.Draw( "same" )
    gaus.SetLineColorAlpha( ROOT.kGreen - 6, 0.5 )

    mean = gaus.GetParameter(1)
    dn   = mean - gaus.GetParameter(2)
    up   = mean + gaus.GetParameter(2)

    box = ROOT.TBox( dn, 0, up, ymax )
    box.Draw( "F" )
    box.SetLineColorAlpha( ROOT.kAzure - 9, 0.4 )
    box.SetFillColorAlpha( ROOT.kAzure - 9, 0.4 )
    box.SetFillStyle( 3744 )

    line = ROOT.TLine( mean, 0, mean, ymax )
    line.Draw()
    line.SetLineColor( pltmgr.Red )
    line.SetLineStyle( 3 )

    leg = pltmgr.NewLegend( 0.65, 0.6, 0.8, 0.8 )
    leg.AddEntry( hist, "{}: {}#sigma".format( opt.GetOption("uncertainty"), "+1" if "up" in obj else "#font[122]{\55}1" ), "L" )
    leg.AddEntry( gaus, "Gauss Fit", "L" )
    leg.AddEntry( line, "mean  : {:+.4f}".format( gaus.GetParameter(1) ), "L" )
    leg.AddEntry( box,  "sigma : {:+.4f}".format( gaus.GetParameter(2) ), "F" )
    leg.Draw()

    hist.Draw( "sameaxis" )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( hist )
    ROOT.TGaxis.SetMaxDigits( 8 )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryRight( "4000 pseudo-datasets" )
    pltmgr.DrawEntryLeft( opt.Entry() )
    c.SaveAs( opt.GetOutputName( obj, "Syst" ) )

    # Judge the sigma (for Acp syst) from the sign of mean value
    return gaus.GetParameter(1), math.copysign( gaus.GetParameter(2), gaus.GetParameter(1) )

def FillHisto():
    objlst = [ "Obs3_leptmass", "Obs6_leptmass", "Obs12_leptmass", "Obs14_leptmass" ]
    opt.AddInputName( "opt" ) 

    Sublst = lambda lst, idx: [x[idx] for x in lst]
    nom_df = collections.OrderedDict( (k,[ [], [], [], [] ]) for k in objlst )
    unc_df = collections.OrderedDict( (k,[ [], [], [], [] ]) for k in objlst )
    
    for i in range( int( opt.GetOption("seed") ) ):
        
        nom_file = opt.GetInputName( "Yield", "Pseudo", "json" ).replace( ".", "_seed_{}.".format( i+1 ) )
        unc_file = nom_file.replace( ".", "_uncertainty_{}.".format( opt.GetOption( "uncertainty" ) ) )
        with open( nom_file, 'r' ) as nom_reader:
            nom_jf = json.loads( nom_reader.read() )
        with open( unc_file, 'r' ) as unc_reader:
            unc_jf = json.loads( unc_reader.read() )
        
        for obj in objlst:
            nom_df[ obj ][0] += Sublst( nom_jf[ obj ], 0 )
            unc_df[ obj ][0] += Sublst( unc_jf[ obj ], 0 )
            nom_df[ obj ][1] += Sublst( nom_jf[ obj ], 1 )
            unc_df[ obj ][1] += Sublst( unc_jf[ obj ], 1 )
            nom_df[ obj ][2] += nevent( Sublst( nom_jf[obj], 1 ), Sublst( nom_jf[obj], 2 ), Sublst( nom_jf[obj], 3 ), Sublst( nom_jf[obj], 4 ) )
            unc_df[ obj ][2] += nevent( Sublst( unc_jf[obj], 1 ), Sublst( unc_jf[obj], 2 ), Sublst( unc_jf[obj], 3 ), Sublst( unc_jf[obj], 4 ) )
        
    sig_syst, bkg_syst, acp_syst = [], [], []
    for obj in objlst:
        obs = obj.split("_")[0]
        sig_syst.append( GetYldSyst( "{}_sig".format( obs ), nom_df[obj][0], unc_df[obj][0] )[0] )
        bkg_syst.append( GetYldSyst( "{}_bkg".format( obs ), nom_df[obj][1], unc_df[obj][1] )[0] )
        acp_syst.append( GetAcpSyst( "{}_acp".format( obs ), nom_df[obj][2], unc_df[obj][2] )[1] ) # use sigma as uncertainty

    print "& ${}1 \sigma$ & {} & {} & {} \\\\".format(
            "+" if "_up" in opt.GetOption( "uncertainty" ) else "-",
            " & ".join( "${:+0.2f}$".format(x) for x in sig_syst ),
            " & ".join( "${:+0.2f}$".format(x) for x in bkg_syst ),
            " & ".join( "${:+0.3f}$".format(x) for x in acp_syst )
            )

def CheckHist( hist, obj, temp ):
    hist.SetName( temp )
    c = pltmgr.NewCanvas()
    hist.Draw("LEGO")

    hist.SetStats( False )
    hist.SetTitle( "" )
    hist.GetXaxis().SetTitle( "M_{jjb} [GeV]" )
    hist.GetYaxis().SetTitle( label( obj ) )

    pltmgr.SetSinglePad( c )
    pltmgr.Set3DAxis( hist )
    c.SaveAs( opt.GetOutputName( "{}_{}".format( obj, temp ), "Check" ) )

def CompareHist( template, pseudo, obj, title ):
    c = pltmgr.NewCanvas()
    template.Draw( "hist" )
    pseudo.Draw( "hist same" )
    
    pltmgr.SetNormToUnity( template )
    pltmgr.SetNormToUnity( pseudo )
   
    ymax = 1.2 * pltmgr.GetHistYmax( template )
    
    template.SetStats( False )
    template.SetMaximum( ymax )
    template.GetYaxis().SetTitle( "PDF" )
    template.SetLineColor( ROOT.kAzure - 9 )
    template.SetLineWidth(2)
    
    pseudo  .GetYaxis().SetTitle( "PDF" )
    pseudo  .SetLineColor( pltmgr.Red )
    pseudo  .SetLineWidth(2)
    
    leg = pltmgr.NewLegend( 0.65, 0.6, 0.8, 0.8 )
    leg.AddEntry( template, "{} template".format( title ), "L" )
    leg.AddEntry( pseudo  , "Pseudo dataset",  "L" )
    leg.Draw()
  
    template.Draw( "hist same" )
    template.Draw( "sameaxis" )
    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( template )
    pltmgr.DrawCMSLabel( pltmgr.SIMULATION )
    pltmgr.DrawEntryLeft( opt.Entry() )
    c.SaveAs( opt.GetOutputName( "{}_{}".format( obj, title ), "Compare" ) )

def main() :
    # Initialize parsing manager
    global opt
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput( "u", "uncertainty" ).AddInput( "d", "seed" )
    opt.AddFlag("p","pull").AddFlag("t","test")
    opt.Parsing() 
    opt.AddInputName ( "chi2" )
    opt.AddOutputName( "chi2", "opt", "seed", "uncertainty" )
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
    samplelst = [ x for x in input.samplelst if not any( y in x for y in [ "QCD", "Data" ] ) ]
   
    # Initialize plot manager
    global histmgr
    histmgr = pltmgr.Plotmgr()
   
    ROOT.gErrorIgnoreLevel = ROOT.kWarning
    ROOT.RooMsgService.instance().setGlobalKillBelow( ROOT.RooFit.ERROR )
    ROOT.RooMsgService.instance().setSilentMode( True );

    if opt.GetOption( "pull" ):
        FillHisto()
        return  
    
    # Declare variable
    x = ROOT.RooRealVar( "x", "x", 0, 500 )
    y = ROOT.RooRealVar( "y", "y", -1, 1 )
    x.setBins( 50 )
    y.setBins( 50 )
    
    # Build reference template
    objlst_pseudo = [ "Obs3_leptmass", "Obs6_leptmass", "Obs12_leptmass", "Obs14_leptmass" ]
    # objlst_pseudo = [ "Obs3_leptmass" ]
    e_nsg, e_nbg = GetExpectedYield( opt.GetInputName( "lep_tmass", "ExpectedYield", "txt" ) )
    pseudo_nsg = ROOT.RooRealVar( "pseudo_nsg", "pseudo_nsg", e_nsg )
    pseudo_nbg = ROOT.RooRealVar( "pseudo_nbg", "pseudo_nbg", e_nbg )

    histmgr.SetObjlst( opt.GetInputName( "ttbar" ), objlst_pseudo, "CT_sg" )
    histmgr.SetObjlst( opt.GetInputName( "Data" ).replace( ".", "_region_WJets_0b_wobtag." ), objlst_pseudo, "CT_bg" )
    
    random = ROOT.TRandom3()
    random.SetSeed( int(opt.GetOption( "seed" )) )
    data_dict = collections.OrderedDict( (k,[]) for k in objlst_pseudo )
    gEvt = 200
    for obj in objlst_pseudo:
        hist_CT_sg = histmgr.GetObj( "CT_sg" )
        data_CT_sg = ROOT.RooDataHist( "data_CT_sg", "data_CT_sg", ROOT.RooArgList(x,y), hist_CT_sg )
        pdf_CT_sg  = ROOT.RooHistPdf( "pdf_CT_sg", "pdf_CT_sg", ROOT.RooArgSet(x,y), data_CT_sg ) 

        hist_CT_bg = histmgr.GetObj( "CT_bg" )
        data_CT_bg = ROOT.RooDataHist( "data_CT_bg", "data_CT_bg", ROOT.RooArgList(x,y), hist_CT_bg )
        pdf_CT_bg  = ROOT.RooHistPdf( "pdf_CT_bg", "pdf_CT_bg", ROOT.RooArgSet(x,y), data_CT_bg ) 
        
        # Generate and fit to pseudo data
        for i in range( gEvt ):
            p_nsg, p_nbg = random.Poisson( e_nsg ), random.Poisson( e_nbg )
            pseudo_sg_data = pdf_CT_sg.generateBinned(ROOT.RooArgSet(x,y), p_nsg )
            pseudo_bg_data = pdf_CT_bg.generateBinned(ROOT.RooArgSet(x,y), p_nbg )
            data_dict[ obj ].append( (pseudo_sg_data, pseudo_bg_data) ) 
     
            if opt.GetOption( "test" ):
                CompareHist( hist_CT_sg.ProjectionX(), pseudo_sg_data.createHistogram( "hist", x ), obj, "sg" )
                CompareHist( hist_CT_sg.ProjectionY(), pseudo_sg_data.createHistogram( "hist", y ), obj.split("_")[0], "sg" )
                CompareHist( hist_CT_bg.ProjectionX(), pseudo_bg_data.createHistogram( "hist", x ), obj, "bg" )
                CompareHist( hist_CT_bg.ProjectionY(), pseudo_bg_data.createHistogram( "hist", y ), obj.split("_")[0], "bg" )
                CheckHist( pseudo_sg_data.createHistogram( "x,y" ), obj, "sg_pseudo" )
                CheckHist( pseudo_bg_data.createHistogram( "x,y" ), obj, "bg_pseudo" )
                CheckHist( hist_CT_sg, obj, "sg_template" )
                CheckHist( hist_CT_bg, obj, "bg_template" )
                return 
      
    #-----------------------------------------------------------------------------------------------------------------------------
    
    # Add fitting template
    if opt.GetOption( "uncertainty" ) == "background":
        for sample in samplelst:
            if "ttbar" in sample:
                histmgr.SetObjlst( opt.GetInputName( "ttbar" ), ["leptmass"], "ST" )
            else:
                histmgr.SetObjlst( opt.GetInputName( sample ), ["leptmass"], "BT_{}".format( sample ) )
        histmgr.SetObjlst( opt.GetInputName( "Data" ).replace( ".", "_region_WJets_0b_wobtag." ), objlst_pseudo, "DT" )
    else:
        opt.AddInputName ( "uncertainty" )
        histmgr.SetObjlst( opt.GetInputName( "ttbar" ), ["leptmass"], "ST" )
        opt.RemoveInputName( "uncertainty" )
        histmgr.SetObjlst( opt.GetInputName( "Data" ).replace( ".", "_region_WJets_0b_wobtag." ), objlst_pseudo, "DT" )
        
    # Build signal template
    hist_ST = histmgr.GetObj( "ST" )
    data_ST = ROOT.RooDataHist( "data_ST", "data_ST", ROOT.RooArgList( x ), hist_ST )
    pdf_ST  = ROOT.RooHistPdf( "pdf_ST", "pdf_ST", ROOT.RooArgSet( x ), data_ST )

    # Build background template
    if opt.GetOption( "uncertainty" ) == "background":
        hist_BT = histmgr.GetMergedObj( "BT" )
        data_BT = ROOT.RooDataHist( "data_BT", "data_BT", ROOT.RooArgList(x), hist_BT ) 
        pdf_BT  = ROOT.RooHistPdf( "pdf_BT", "pdf_BT", ROOT.RooArgSet(x), data_BT )
    
    tmass = int( opt.GetOption( "opt" ) )
    yield_dict = collections.OrderedDict( (k,[]) for k in objlst_pseudo )
    for obj, datasets in data_dict.iteritems():
        # Build pseudo background template
        hist_DT_bg = histmgr.GetObj( "DT" )
        data_DT_bg = ROOT.RooDataHist( "data_DT_bg", "data_DT_bg", ROOT.RooArgList(x,y), hist_DT_bg )
        pdf_DT_bg  = ROOT.RooHistPdf( "pdf_DT_bg", "pdf_DT_bg", ROOT.RooArgSet(x,y), data_DT_bg ) 
       
        for i, data in enumerate( datasets ):
            pseudo_data = data[0].Clone()
            pseudo_data.add( data[1] )

            # Get pseudo background fraction
            p_nDT = random.Poisson( e_nbg )
            data_DT = pdf_DT_bg.generateBinned(ROOT.RooArgSet(x,y), p_nDT )
            pseudo_bg_full = data_DT.reduce("x<150")
            pseudo_bg_full_frac = pseudo_bg_full.reduce("y>0").sumEntries() / pseudo_bg_full.sumEntries()
           
            if opt.GetOption( "uncertainty" ) != "background":
                hist_BT = data_DT.createHistogram( "{}_{}".format( obj, i ), x )
                pdf_BT  = ROOT.RooHistPdf( "pdf_BT", "pdf_BT", ROOT.RooArgSet( x ), data_DT.reduce(ROOT.RooArgSet(x) ) )
            
            # Build model pdf
            nsg = ROOT.RooRealVar( "nsg", "nsg", 300000, 100000, 1000000 )
            nbg = ROOT.RooRealVar( "nbg", "nbg", 40000, 0, 100000 )
            model = ROOT.RooAddPdf( "model", "model", ROOT.RooArgList( pdf_ST, pdf_BT ), ROOT.RooArgList( nsg, nbg ) )  
            model.fitTo( pseudo_data, ROOT.RooFit.Extended() )
            model.fitTo( pseudo_data, ROOT.RooFit.Extended() )
        
            # Apply full selection cut
            pltmgr.SetNormToUnity( hist_ST ).Scale( nsg.getVal() )
            nsg_full = hist_ST.Integral( 0, hist_ST.FindBin( tmass ) -1 )
            pltmgr.SetNormToUnity( hist_BT ).Scale( nbg.getVal() )
            nbg_full = hist_BT.Integral( 0, hist_BT.FindBin( tmass ) -1 )


            pseudo_data_full = pseudo_data.reduce("x<150")
            pseudo_data_pos  = pseudo_data_full.reduce("y>0").sumEntries()
            pseudo_data_neg  = pseudo_data_full.sumEntries() - pseudo_data_pos 

            yield_dict[ obj ].append( ( nsg_full, nbg_full, pseudo_bg_full_frac, pseudo_data_pos, pseudo_data_neg ) )
    
    with open( opt.GetOutputName( "Yield", "Pseudo", "json" ), 'w' ) as outfile:
        json.dump( yield_dict, outfile, indent=4 )

if __name__== '__main__':
    main()
