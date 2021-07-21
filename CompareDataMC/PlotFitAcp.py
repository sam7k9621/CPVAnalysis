import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.ParseMgr as parmgr
import math, importlib
import numpy as np
import ROOT

def MakePDF( label, name ):
    hist = histmgr.GetMergedObj( label )
    
    temp      = ROOT.RooDataHist( name.format( "data" ), "", ROOT.RooArgList(x), hist )
    data      = temp.reduce("x<150")
    data_tail = temp.reduce("x>=150")
    pdf  = ROOT.RooHistPdf( name.format( "pdf" ), "", ROOT.RooArgSet(x), data )
    pdf_tail  = ROOT.RooHistPdf( name.format( "pdf_tail" ), "", ROOT.RooArgSet(x), data_tail )
    return data, data_tail, pdf, pdf_tail

def main() :
    # Initialize parsing manager
    global opt
    opt = parmgr.Parsemgr()
    opt.AddInput("c", "chi2").AddInput("o", "opt").AddInput("p", "pull" )
    opt.AddFlag("f", "fuzzy")
    opt.Parsing() 
    opt.AddInputName ( "chi2" )
    opt.AddOutputName( "chi2", "opt" )
   
    # Initialize plot manager
    global histmgr
    histmgr = pltmgr.Plotmgr()
    objlst = [ "lep_tmass_obs3_p", "lep_tmass_obs3_n", "lep_tmass_obs6_p", "lep_tmass_obs6_n", "lep_tmass_obs12_p", "lep_tmass_obs12_n", "lep_tmass_obs14_p", "lep_tmass_obs14_n" ]
    yearlst = [ "16", "17", "18" ] if opt.Year() == "RunII" else [ opt.Year() ]

    # ROOT.gErrorIgnoreLevel = ROOT.kWarning
    # ROOT.RooMsgService.instance().setGlobalKillBelow( ROOT.RooFit.ERROR )
    # ROOT.RooMsgService.instance().setSilentMode( True );
   
    global x
    x = ROOT.RooRealVar( "x", "x", 0, 500 )
    x.setBins( 250 )

    yield_dic = {k: None for k in yearlst}
    for year in yearlst:
        input = importlib.import_module( "CPVAnalysis.CompareDataMC.MakeHist{}".format( year ))
        # Add Constraint template
        print "-"*90
        print ">> Adding constraint template"
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ), objlst, "CT_{}".format( year ) )

        
        # Add background template 
        print "-"*90
        print ">> Adding background template"
        histmgr.SetObjlst( opt.GetInputName( "Data" ).format( Y=year ).replace( ".root", "_region_WJets_0b_wobtag.root" ), objlst, "BT_{}".format( year ) )
       
        # Add signal template
        print "-"*90
        print ">> Adding signal template"
        histmgr.SetObjlst( opt.GetInputName( "ttbar_semi" ) .format( Y=year ), objlst, "ST_nom_{}_semi" .format( year ) ) 
        histmgr.SetObjlst( opt.GetInputName( "ttbar_dilep" ).format( Y=year ), objlst, "ST_nom_{}_dilep".format( year ) ) 
    
        yield_dic[year] = {k:None for k in ["Obj3", "Obj6", "Obj12", "Obj14"]}
        for obj in ["Obj3", "Obj6", "Obj12", "Obj14"]:
            # Build constraint template
            p_data_CT, p_data_CT_tail, p_pdf_CT, p_pdf_CT_tail = MakePDF( "CT_{}".format(year), "p_{}_CT" ) 
            n_data_CT, n_data_CT_tail, n_pdf_CT, n_pdf_CT_tail = MakePDF( "CT_{}".format(year), "n_{}_CT" ) 
            
            # Build background template
            p_data_BT, p_data_BT_tail, p_pdf_BT, p_pdf_BT_tail = MakePDF( "BT_{}".format(year), "p_{}_BT" ) 
            n_data_BT, n_data_BT_tail, n_pdf_BT, n_pdf_BT_tail = MakePDF( "BT_{}".format(year), "n_{}_BT" ) 
           
            # Build signal template
            p_data_ST, p_data_ST_tail, p_pdf_ST, p_pdf_ST_tail = MakePDF( "ST_nom_{}".format(year), "p_{}_ST" ) 
            n_data_ST, n_data_ST_tail, n_pdf_ST, n_pdf_ST_tail = MakePDF( "ST_nom_{}".format(year), "n_{}_ST" ) 

            # Define constant parameter
            sg_pyd_ratio = p_data_ST_tail.sumEntries() / p_data_ST.sumEntries()
            sg_nyd_ratio = n_data_ST_tail.sumEntries() / n_data_ST.sumEntries()
            bg_pyd_ratio = p_data_BT_tail.sumEntries() / p_data_BT.sumEntries()
            bg_nyd_ratio = n_data_BT_tail.sumEntries() / n_data_BT.sumEntries()
            
            # Define category to distinguish positive and negative samples events
            category = ROOT.RooCategory( "category", "category" )
            category.defineType("pos")
            category.defineType("neg")
            category.defineType("pos_tail")
            category.defineType("neg_tail")
            
            total_data = ROOT.RooDataHist( 
                    "total_data", "total_data", ROOT.RooArgList(x), ROOT.RooFit.Index(category), 
                     ROOT.RooFit.Import("pos", p_data_CT ), 
                     ROOT.RooFit.Import("neg", n_data_CT ),
                     ROOT.RooFit.Import("pos_tail", p_data_CT_tail),
                     ROOT.RooFit.Import("neg_tail", n_data_CT_tail)
                    )
            
            # Build model pdf
            # acp = ROOT.RooRealVar( "acp", "", 0, -.01, .01 )
            nbg = ROOT.RooRealVar( "nbg", "", 5000, 0, 100000 )
            
            bg_pn_ratio = p_data_BT.sumEntries() / ( p_data_BT.sumEntries() + n_data_BT.sumEntries() )
            bg_pn_frac  = ROOT.RooRealVar( "bg_pn_frac", "", bg_pn_ratio)
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
            n_nsg   = ROOT.RooRealVar(    "n_nsg",   "", 70000, 50000, 500000 )
            # n_nsg   = ROOT.RooFormulaVar( "n_nsg", "@0*(1-@1)/(1+@1)", ROOT.RooArgList( p_nsg, acp ) )
            n_model = ROOT.RooAddPdf(     "n_model", "", ROOT.RooArgList( n_pdf_ST, n_pdf_BT ), ROOT.RooArgList( n_nsg, n_nbg ) )  
           
            n_nbg_tail   = ROOT.RooFormulaVar( "n_nbg_tail",   "@0*@1", ROOT.RooArgList( bg_nyd_frac, n_nbg ) )
            n_nsg_tail   = ROOT.RooFormulaVar( "n_nsg_tail",   "@0*@1", ROOT.RooArgList( sg_nyd_frac, n_nsg ) )
            n_model_tail = ROOT.RooAddPdf(     "n_model_tail", "", ROOT.RooArgList( n_pdf_ST_tail, n_pdf_BT_tail ), ROOT.RooArgList( n_nsg_tail, n_nbg_tail ) )
    
            simPdf = ROOT.RooSimultaneous("simPdf", "simultaneous pdf", category)
            simPdf.addPdf( p_model, "pos" )
            simPdf.addPdf( n_model, "neg" )
            simPdf.addPdf( p_model_tail, "pos_tail" )
            simPdf.addPdf( n_model_tail, "neg_tail" )
           

            result = simPdf.fitTo( total_data, ROOT.RooFit.Extended(), ROOT.RooFit.Save() )

            yield_dic[year][obj] = (p_nsg.getVal(), n_nsg.getVal(), p_nsg.getError(), n_nsg.getError())

    obj_yld = {k:list([0,0,0,0]) for k in ["Obj3", "Obj6", "Obj12", "Obj14"] }
    for obj in ["Obj3", "Obj6", "Obj12", "Obj14"]:
        for year in yearlst:
            obj_yld[obj][0] += yield_dic[year][obj][0] 
            obj_yld[obj][1] += yield_dic[year][obj][1] 
            obj_yld[obj][2] = math.sqrt( obj_yld[obj][2]**2 + yield_dic[year][obj][2]**2 )
            obj_yld[obj][3] = math.sqrt( obj_yld[obj][3]**2 + yield_dic[year][obj][3]**2 )

    info = """
    {} fitted positive sg events after cut: {:8.0f} {:8.0f}
    {} fitted negative sg events after cut: {:8.0f} {:8.0f}
    """
    with open( opt.GetOutputName( "Acp", "FitResult", "txt" ), "w" ) as outputfile:
        for obj in ["Obj3", "Obj6", "Obj12", "Obj14"]:
            outputfile.write( 
                    info.format( 
                        obj, obj_yld[obj][0], obj_yld[obj][2],
                        obj, obj_yld[obj][1], obj_yld[obj][3]
                        ))

if __name__== '__main__':
    main()
