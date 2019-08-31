import os 
import subprocess
import sys
import FWCore.ParameterSet.Config as cms 

def SplitFile( num ):
    if num == 2:
        return [ "[0-5]", "[6-9]"]
    elif num == 3:
        return [ "[0-3]", "[4-6]", "[7-9]"]
    elif num == 5:
        return [ "[0-1]", "[2-3]", "[4-5]", "[6-7]", "[8-9]" ]

def FillContent( year, dir, mclst, datalst ):
    s = subprocess.Popen( 'ls ' + dir, shell=True, stdout=subprocess.PIPE )
    outputlst, err = s.communicate()
    outputlst = filter(None, outputlst.split('\n') )
   
    samplelst = []
    for mc in mclst:
        for output in outputlst:
            if all( sub in output for sub in mc[1].split("_") ):
                samplelst.append( ( output, mc ) )
                outputlst.remove( output )
                break

    for data in datalst:
        for output in outputlst:
            temp = data[1].split("_")
            if temp[0] in output.lower():
                samplelst.append( ( output + "/" + '_'.join( temp[1:] ), data ) )
                break

    content = {}
    for sample, tag in samplelst:
        if tag[0] == 1:
            content[ year + "_" + tag[1] + "_0" ] = dir + sample + "/*.root"
        elif tag[0] < 10:
            for i in range( tag[0] ):
                content[ year + "_" + tag[1] + "_{}".format( str( i ) ) ] = dir + sample + "/*{}.root".format( SplitFile( tag[0] )[i] ) 
        else:
            if tag[0] == 10:
                filelst = [x+y for x in SplitFile(2) for y in SplitFile(5) ] 
            else:
                filelst = [x+y for x in SplitFile(3) for y in SplitFile(5) ] 
            
            for i in range( tag[0] ):
                content[ year + "_" + tag[1] + "_{}".format( str( i ) ) ] = dir + sample + "/*{}.root".format( filelst[i] ) 
            # include one-digit file
            content[ year + "_" + tag[1] + "_{}".format( str( tag[0] ) ) ] = dir + sample + "/*_[0-9].root"
    
    CMSSW = os.environ[ "CMSSW_BASE" ] + "/src/CPVAnalysis/BaseLineSelector/data/"
    ###########################################################################################################################
    content[ "16_BtagWeight" ] = CMSSW + "DeepCSV_94XSF_V4_B_F.csv"
    content[ "16_SR_eff_b" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "16_SR_eff_c" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "16_SR_eff_l" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "16_CR_eff_b" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "16_CR_eff_c" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "16_CR_eff_l" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    
    #########################################################################################################################
    content[ "17_BtagWeight" ] = CMSSW + "DeepCSV_94XSF_V4_B_F.csv"
    content[ "17_SR_eff_b" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "17_SR_eff_c" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "17_SR_eff_l" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "17_CR_eff_b" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "17_CR_eff_c" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "17_CR_eff_l" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    
    #########################################################################################################################
    content[ "18_BtagWeight" ] = CMSSW + "DeepCSV_94XSF_V4_B_F.csv"
    content[ "18_SR_eff_b" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "18_SR_eff_c" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "18_SR_eff_l" ]   = CMSSW + "beffPlot_TTToSemiLeptonic_0pt4941.root"
    content[ "18_CR_eff_b" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "18_CR_eff_c" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"
    content[ "18_CR_eff_l" ]   = CMSSW + "beffPlot_Wjets_0pt1522.root"

    return content

def MakeInput( process, year, dir, mclst, datalst ):
    content = FillContent( year, dir, mclst, datalst ) 

    for key, value  in content.iteritems():
        taglst = key.split("_")

        if year in taglst[0]:
            # Btag SF weight
            if "BtagWeight" in taglst:
                setattr( process, "_".join( taglst[1:] ), cms.PSet( path = cms.string( value ) ) ) 
            
            # Btag Efficiency plot
            elif "eff" in taglst:
                setattr( process, "_".join( taglst[1:] ), cms.PSet( path = cms.string( value ), title = cms.string( "_".join( taglst[2:] ) ) ) ) 
            
            # Samplelst
            else:
                setattr( process, "_".join( taglst[1:] ), cms.PSet( path = cms.string( value ) ) )


