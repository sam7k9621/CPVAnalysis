import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import re
import json
import math
import ROOT
import importlib
import collections
import decimal 

def GetAcpSyst( obj, nomp, nomn, uncp, uncn ):
    GetAcp = lambda x,y : 100.*(x-y)/(x+y)
  
    acplst = [GetAcp( up, un ) - GetAcp( np, nn ) for np, nn, up, un in zip( nomp, nomn, uncp, uncn ) ]

    acplst = [ x for x in acplst if abs(x) < 1 ]
    
    if "mtop" in opt.GetOption("uncertainty"):
        acplst = [x/3. for x in acplst]
    
    xborder = max( abs( max( acplst ) ), abs( min( acplst ) ) )
   
    hist = ROOT.TH1D( obj, "", 100, -1*xborder, xborder ) 
    hist.SetStats( False )
    hist.GetXaxis().SetTitle( "{} Syst. [%]".format( obj ) )
    hist.GetYaxis().SetTitle( "Events" )
    for acp in acplst:
        hist.Fill( acp )
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
    
    return gaus.GetParameter(1), gaus.GetParameter(2)

def RetrieveJson( year, lepton ):
    objlst = [ "lep_tmass_obs3", "lep_tmass_obs6", "lep_tmass_obs12", "lep_tmass_obs14" ]
    nom_dic = { obj:None for obj in objlst }
    unc_dic = { obj:None for obj in objlst }
    
    for i in range( int( opt.GetOption("seed") ) ):
        nom_file = opt.GetInputName( "Yield", "Pseudo", "json" ).format( L=lepton, Y=year ).replace( ".", "_seed_{}.".format( i+1 ) )
        unc_file = nom_file.replace( ".", "_uncertainty_{}.".format( opt.GetOption( "uncertainty" ) ) )
        with open( nom_file, 'r' ) as nom_reader:
            nom_jf = json.loads( nom_reader.read() )
        with open( unc_file, 'r' ) as unc_reader:
            unc_jf = json.loads( unc_reader.read() )
    
        for obj in objlst:
            if nom_dic[obj]:
                nom_dic[obj] += nom_jf[obj]
                unc_dic[obj] += unc_jf[obj]
            else:
                nom_dic[obj] = list(nom_jf[obj])
                unc_dic[obj] = list(unc_jf[obj])

    return nom_dic, unc_dic

def FillHisto():
    objlst = [ "lep_tmass_obs3", "lep_tmass_obs6", "lep_tmass_obs12", "lep_tmass_obs14" ]
    opt.AddInputName( "opt" ) 

    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]
    leplst  = ["el", "mu"] if opt.LeptonType() == "co" else [ opt.LeptonType() ]
    GetSubLst = lambda lst, idx: [x[idx] for x in lst]


    nom_dic_p = { obj:None for obj in objlst }
    nom_dic_n = { obj:None for obj in objlst }
    unc_dic_p = { obj:None for obj in objlst }
    unc_dic_n = { obj:None for obj in objlst }
    pidx, nidx = 3, 4
    for year in yearlst:
        for lep in leplst:
            for obj in objlst:
                nom_jf, unc_jf = RetrieveJson( year, lep )
                if nom_dic_p[obj]:
                    nom_dic_p[obj] = [x+y for x, y in zip(nom_dic_p[obj], GetSubLst( nom_jf[obj], pidx ))]
                    nom_dic_n[obj] = [x+y for x, y in zip(nom_dic_n[obj], GetSubLst( nom_jf[obj], nidx ))]
                    unc_dic_p[obj] = [x+y for x, y in zip(unc_dic_p[obj], GetSubLst( unc_jf[obj], pidx ))]
                    unc_dic_n[obj] = [x+y for x, y in zip(unc_dic_n[obj], GetSubLst( unc_jf[obj], nidx ))]
                else:
                    nom_dic_p[obj] = GetSubLst( nom_jf[obj], pidx )
                    nom_dic_n[obj] = GetSubLst( nom_jf[obj], nidx )
                    unc_dic_p[obj] = GetSubLst( unc_jf[obj], pidx )
                    unc_dic_n[obj] = GetSubLst( unc_jf[obj], nidx )

    acp_syst = []
    for obj in objlst:
        acp_syst.append( GetAcpSyst( obj, nom_dic_p[obj], nom_dic_n[obj], unc_dic_p[obj], unc_dic_n[obj] ) )
    
    print "& {}".format(
            " & ".join( "${:+0.4f}$".format(x[0]) for x in acp_syst )
            )
    
    with open( opt.GetOutputName( "AcpUnc", type="txt"), "w" ) as outputfile:
        for idx, obj in enumerate(objlst): 
            outputfile.write( "{} {:+0.4f}\n".format( obj.split("_")[2], acp_syst[idx][0] ) )

def GetExpectedYield( filename ):
    with open( filename, "r" ) as inputfile:
        sg, bg = 0, 0
        for line in inputfile.readlines():
            if "ttbar" in line:
                sg += float( line.split( " " )[1].lstrip("\n") )
            if "MC" in line:                          
                bg = float( line.split( " " )[1].lstrip("\n") )
    return sg, bg - sg

def MakePDF( label, name ):
    hist = histmgr.GetMergedObj( label )
    
    temp      = ROOT.RooDataHist( name.format( "data" ), "", ROOT.RooArgList(m), hist )
    data      = temp.reduce("m<150")
    data_tail = temp.reduce("m>=150")
    pdf  = ROOT.RooHistPdf( name.format( "pdf" ), "", ROOT.RooArgSet(m), data )
    pdf_tail  = ROOT.RooHistPdf( name.format( "pdf_tail" ), "", ROOT.RooArgSet(m), data_tail )
    return data, data_tail, pdf, pdf_tail

def main() :
    # Initialize parsing manager
    global opt
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput( "u", "uncertainty" ).AddInput( "d", "seed" )
    opt.AddFlag("p","pull").AddFlag("t","test")
    opt.Parsing() 
    opt.AddInputName ( "chi2" )
    opt.AddOutputName( "chi2", "opt", "seed", "uncertainty" )
   
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
    input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( opt.Year() ))
    samplelst = [ x for x in input.samplelst if not any( y in x for y in [ "QCD", "Data" ] ) ]
    global m
    m = ROOT.RooRealVar( "m", "m", 0, 500 )
    o = ROOT.RooRealVar( "o", "o", -1, 1 )
    m.setBins( 250 )
    o.setBins( 50 )
    
    # Build reference template
    objlst_pseudo = [ "Obs3_leptmass", "Obs6_leptmass", "Obs12_leptmass", "Obs14_leptmass" ]
    e_nsg, e_nbg = GetExpectedYield( opt.GetInputName( "lep_tmass", "ExpectedYield", "txt" ).format( Y=opt.Year() ) )

    print opt.GetInputName( "lep_tmass", "ExpectedYield", "txt" ).format( Y=opt.Year() )
    return

    histmgr.SetObjlst( opt.GetInputName( "ttbar_semi" ).format( Y=opt.Year() ),  objlst_pseudo, "CT_sg_semi" )
    histmgr.SetObjlst( opt.GetInputName( "ttbar_dilep" ).format( Y=opt.Year() ), objlst_pseudo, "CT_sg_dilep" )
    histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=opt.Year() ).replace( ".", "_region_WJets_0b_wobtag." ), objlst_pseudo, "CT_bg" )
    
    gEvt = 200
    random = ROOT.TRandom3()
    random.SetSeed( int(opt.GetOption( "seed" )) )

    data_dict = { k: [] for k in objlst_pseudo }
    for obj in objlst_pseudo:
        hist_CT_sg = histmgr.GetMergedObj( "CT_sg" )
        data_CT_sg = ROOT.RooDataHist( "data_CT_sg", "data_CT_sg", ROOT.RooArgList(m,o), hist_CT_sg )
        pdf_CT_sg  = ROOT.RooHistPdf( "pdf_CT_sg", "pdf_CT_sg", ROOT.RooArgSet(m,o), data_CT_sg )
        
        hist_CT_bg = histmgr.GetMergedObj( "CT_bg" )
        data_CT_bg = ROOT.RooDataHist( "data_CT_bg", "data_CT_bg", ROOT.RooArgList(m,o), hist_CT_bg )
        pdf_CT_bg  = ROOT.RooHistPdf( "pdf_CT_bg", "pdf_CT_bg", ROOT.RooArgSet(m,o), data_CT_bg )

        # Generate pseudo data
        for i in range( gEvt ):
            p_nsg, p_nbg = random.Poisson( e_nsg ), random.Poisson( e_nbg )
            pseudo_sg_data = pdf_CT_sg.generateBinned(ROOT.RooArgSet(m,o), p_nsg )
            pseudo_bg_data = pdf_CT_bg.generateBinned(ROOT.RooArgSet(m,o), p_nbg )
            data_dict[ obj ].append( (pseudo_sg_data, pseudo_bg_data) ) 

    #-----------------------------------------------------------------------------------------------------------------------------
    # Add fitting template
    objlst_pseudo = [ "lep_tmass_obs3_p", "lep_tmass_obs3_n", "lep_tmass_obs6_p", "lep_tmass_obs6_n", "lep_tmass_obs12_p", "lep_tmass_obs12_n", "lep_tmass_obs14_p", "lep_tmass_obs14_n" ]
    # objlst_pseudo = [ "lep_tmass_obs3_p", "lep_tmass_obs3_n" ]
    if opt.GetOption( "uncertainty" ) == "background":
        for sample in samplelst:
            if "ttbar" not in sample:
                histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=opt.Year() ), objlst_pseudo, "BT_{}".format( sample ) )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_semi" ).format( Y=opt.Year() ),   objlst_pseudo, "ST_semi" )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_dilep" ).format( Y=opt.Year() ),  objlst_pseudo, "ST_dilep" )
    
    elif "WHF" in opt.GetOption( "uncertainty" ):
        for sample in samplelst:
            if "ttbar" not in sample:
                if "WJets" in sample:
                    histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=opt.Year() ).replace(".", "_{}.".format(opt.GetOption("uncertainty"))), objlst_pseudo, "BT_{}".format( sample ) )
                else:
                    histmgr.SetObjlst( opt.GetInputName( sample ).format( Y=opt.Year() ), objlst_pseudo, "BT_{}".format( sample ) )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_semi" ).format( Y=opt.Year() ),       objlst_pseudo, "ST_semi" )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_dilep" ).format( Y=opt.Year() ),      objlst_pseudo, "ST_dilep" )
    
    else:
        opt.AddInputName ( "uncertainty" )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_semi" ).format( Y=opt.Year() ),  objlst_pseudo, "ST_semi" )
        histmgr.SetObjlst( opt.GetInputName( "ttbar_dilep" ).format( Y=opt.Year() ), objlst_pseudo, "ST_dilep" )
        opt.RemoveInputName( "uncertainty" )
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=opt.Year() ).replace( ".", "_region_WJets_0b_wobtag." ), objlst_pseudo, "BT" )
       
    
    tmass = int( opt.GetOption( "opt" ) )
    objlst = ["Obs3", "Obs6", "Obs12", "Obs14"]
    yield_dict = { k[:-2]: [] for k in objlst_pseudo[::2] }
    
    for obj in objlst:
        obj_p, obj_n = "lep_tmass_{}_p".format( obj.lower() ), "lep_tmass_{}_n".format( obj.lower() )
        
        # Build signal template
        p_data_ST, p_data_ST_tail, p_pdf_ST, p_pdf_ST_tail = MakePDF( "ST", "p_{}_ST" ) 
        n_data_ST, n_data_ST_tail, n_pdf_ST, n_pdf_ST_tail = MakePDF( "ST", "n_{}_ST" ) 

        # Build background template
        p_data_BT, p_data_BT_tail, p_pdf_BT, p_pdf_BT_tail = MakePDF( "BT", "p_{}_BT" ) 
        n_data_BT, n_data_BT_tail, n_pdf_BT, n_pdf_BT_tail = MakePDF( "BT", "n_{}_BT" ) 
   
        # Define category to distinguish positive and negative samples events
        category = ROOT.RooCategory( "category", "category" )
        category.defineType("pos")
        category.defineType("neg")
        category.defineType("pos_tail")
        category.defineType("neg_tail")
   
        # Define constant parameter
        sg_pyd_ratio = p_data_ST_tail.sumEntries() / p_data_ST.sumEntries()
        sg_nyd_ratio = n_data_ST_tail.sumEntries() / n_data_ST.sumEntries()
        bg_pyd_ratio = p_data_BT_tail.sumEntries() / p_data_BT.sumEntries()
        bg_nyd_ratio = n_data_BT_tail.sumEntries() / n_data_BT.sumEntries()

        for data in data_dict[ obj + "_leptmass" ]:
            # Construct combined dataset in (x,sample)
            total_data = data[0].Clone()
            total_data.add( data[1] )
           
            # bug
            pseudo_p_data = total_data.reduce("o>=0").reduce("m<150")
            pseudo_n_data = total_data.reduce("o<0" ).reduce("m<150")
            
            pseudo_p_data_tail = total_data.reduce("o>=0").reduce("m>=150")
            pseudo_n_data_tail = total_data.reduce("o<0" ).reduce("m>=150")

            pseudo_data = ROOT.RooDataHist( 
                    "pseudo_data", "pseudo_data", ROOT.RooArgList(m), ROOT.RooFit.Index(category), 
                     ROOT.RooFit.Import("pos", pseudo_p_data), 
                     ROOT.RooFit.Import("neg", pseudo_n_data),
                     ROOT.RooFit.Import("pos_tail", pseudo_p_data_tail),
                     ROOT.RooFit.Import("neg_tail", pseudo_n_data_tail)
                    )
            
            # Build model pdf
            acp = ROOT.RooRealVar( "acp", "", 0, -1, 1)
            nbg = ROOT.RooRealVar( "nbg", "", 5000, 0, 100000 )
            
            bg_pn_ratio = pseudo_p_data.sumEntries() / ( pseudo_p_data.sumEntries() + pseudo_n_data.sumEntries() )
            bg_pn_frac = ROOT.RooRealVar( "bg_pn_frac", "", bg_pn_ratio)
            sg_pyd_frac = ROOT.RooRealVar( "sg_pyd_frac", "", sg_pyd_ratio ) 
            sg_nyd_frac = ROOT.RooRealVar( "sg_nyd_frac", "", sg_nyd_ratio )
            bg_pyd_frac = ROOT.RooRealVar( "bg_pyd_frac", "", bg_pyd_ratio )
            bg_nyd_frac = ROOT.RooRealVar( "bg_nyd_frac", "", bg_nyd_ratio )

            p_nbg   = ROOT.RooFormulaVar( "p_nbg", "@0*@1", ROOT.RooArgList( bg_pn_frac, nbg ) )
            p_nsg   = ROOT.RooRealVar(    "p_nsg",   "", 70000, 50000, 500000 )
            p_model = ROOT.RooAddPdf(     "p_model", "", ROOT.RooArgList( p_pdf_ST, p_pdf_BT ), ROOT.RooArgList( p_nsg, p_nbg ) )  
    
            p_nbg_tail   = ROOT.RooFormulaVar( "p_nbg_tail",   "@0*@1", ROOT.RooArgList( bg_pyd_frac, p_nbg ) )
            p_nsg_tail   = ROOT.RooFormulaVar( "p_nsg_tail",   "@0*@1", ROOT.RooArgList( sg_pyd_frac, p_nsg ) )
            p_model_tail = ROOT.RooAddPdf(     "p_model_tail", "", ROOT.RooArgList( p_pdf_ST_tail, p_pdf_BT_tail ), ROOT.RooArgList( p_nsg_tail, p_nbg_tail ) )

            n_nbg   = ROOT.RooFormulaVar( "n_nbg", "(1-@0)*@1", ROOT.RooArgList( bg_pn_frac, nbg ) )
            n_nsg   = ROOT.RooFormulaVar( "n_nsg", "@0*(1-@1)/(1+@1)", ROOT.RooArgList( p_nsg, acp ) )
            n_model = ROOT.RooAddPdf(     "n_model", "", ROOT.RooArgList( n_pdf_ST, n_pdf_BT ), ROOT.RooArgList( n_nsg, n_nbg ) )  
           
            n_nbg_tail   = ROOT.RooFormulaVar( "n_nbg_tail",   "@0*@1", ROOT.RooArgList( bg_nyd_frac, n_nbg ) )
            n_nsg_tail   = ROOT.RooFormulaVar( "n_nsg_tail",   "@0*@1", ROOT.RooArgList( sg_nyd_frac, n_nsg ) )
            n_model_tail = ROOT.RooAddPdf(     "n_model_tail", "", ROOT.RooArgList( n_pdf_ST_tail, n_pdf_BT_tail ), ROOT.RooArgList( n_nsg_tail, n_nbg_tail ) )
    
            simPdf = ROOT.RooSimultaneous("simPdf", "simultaneous pdf", category)
            simPdf.addPdf( p_model, "pos" )
            simPdf.addPdf( n_model, "neg" )
            simPdf.addPdf( p_model_tail, "pos_tail" )
            simPdf.addPdf( n_model_tail, "neg_tail" )
           

            result = simPdf.fitTo( pseudo_data, ROOT.RooFit.Extended(), ROOT.RooFit.Save() )
            # simPdf.fitTo( pseudo_data, ROOT.RooFit.Extended() )
           
            # print obj, 100*acp.getVal(), 100*acp.getError()
            # print obj
            # print p_nsg.getVal(), pseudo_p_data.sumEntries()-p_nbg.getVal()
            # print n_nsg.getVal(), pseudo_n_data.sumEntries()-n_nbg.getVal()
            # print bg_pn_frac.getVal() 
            # print pseudo_p_data.sumEntries(), p_nbg.getVal(), pseudo_n_data.sumEntries(), n_nbg.getVal() 
            # print pseudo_p_data.sumEntries(), p_nbg.getVal(), pseudo_p_data.sumEntries() - p_nbg.getVal(), p_nsg.getVal() 
            # print pseudo_n_data.sumEntries(), n_nbg.getVal(), pseudo_n_data.sumEntries() - n_nbg.getVal(), n_nsg.getVal() 
            
            yield_dict[ obj_p[:-2] ].append( ( 
                bg_pn_ratio, 
                pseudo_p_data.sumEntries()-p_nbg.getVal(), 
                pseudo_n_data.sumEntries()-n_nbg.getVal(), 
                p_nsg.getVal(),
                n_nsg.getVal(),
                100*acp.getVal() 
                ) )
           
            # if p_nsg.getVal() < 50000:
           
                # print result.status()

               #  sampleSet = ROOT.RooArgSet(category)
                # frame = m.frame()
                # pseudo_data.plotOn( frame, ROOT.RooFit.Cut("category==category::pos") )
                # simPdf.plotOn( frame, ROOT.RooFit.ProjWData(sampleSet,pseudo_data), ROOT.RooFit.Slice(category, "pos"), ROOT.RooFit.Components("p_model") )
                # simPdf.plotOn( frame, ROOT.RooFit.ProjWData(sampleSet,pseudo_data), ROOT.RooFit.Slice(category, "pos"), ROOT.RooFit.Components("p_pdf_ST") )
                # c = ROOT.TCanvas()
                # frame.Draw()
                # c.SaveAs("{}.pdf".format( obj ) )
                # break
            

            # yield_dict[ obj_p[:-2] ].append( ( pseudo_data_full, p_nsg_full, n_nsg_full, 100*(p_nsg_full-n_nsg_full)/(p_nsg_full+n_nsg_full)) )
            # yield_dict[ obj_p[:-2] ].append( ( pseudo_data.sumEntries(), p_nsg.getVal(), n_nsg.getVal(), 100*(p_nsg.getVal()-n_nsg.getVal()) / (p_nsg.getVal()+n_nsg.getVal()) ) )
            # print obj, pseudo_data_full, p_nsg_full+n_nsg_full, p_nsg_full, n_nsg_full, 100*(p_nsg_full-n_nsg_full)/(p_nsg_full+n_nsg_full)
            # print obj, p_nsg_full, pseudo_p_data.reduce("m<150").sumEntries() - p_nbg_full

    # print yield_dict
    with open( opt.GetOutputName( "Yield", "Pseudo", "json" ), 'w' ) as outfile:
        json.dump( yield_dict, outfile, indent=4 )

if __name__== '__main__':
    main()
