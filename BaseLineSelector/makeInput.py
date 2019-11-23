#!/bin/env python

import subprocess
import os
import re
import sys
import json
import argparse
from collections import OrderedDict
import CPVAnalysis.BaseLineSelector.Samplelst16 as s16
import CPVAnalysis.BaseLineSelector.Samplelst17 as s17
import CPVAnalysis.BaseLineSelector.Samplelst18 as s18

context = '''
el = [
    {}
    ]

mu = [
    {}
    ]

mc = [
    {}
    ]
'''

text1 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ) ) )\n'

text2 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ), title = cms.string( "{}" ) ) )\n'

text3 = '''
sample{}.Info = cms.PSet(
        mu_HLT = cms.vint32( {} ),
        el_HLT = cms.vint32( {} ),
        lumimask = cms.string   ( "{}" ),
        puweight = cms.string   ( "{}" ),
        puweight_up = cms.string( "{}" ),
        puweight_dn = cms.string( "{}" )
        )
'''

def SplitFile( num ):
    if num == 2:
        return [ "[0-5]", "[6-9]"]
    elif num == 3:
        return [ "[0-3]", "[4-6]", "[7-9]"]
    elif num == 5:
        return [ "[0-1]", "[2-3]", "[4-5]", "[6-7]", "[8-9]" ]

def MakeContent( dir, mclst, datalst ):
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
            if temp[0] in output:
                samplelst.append( ( output + "/" + '_'.join( temp[1:] ), data ) )
                break

    content = OrderedDict()
    for sample, tag in samplelst:
        if tag[0] == 1:
            content[ tag[1] + "_0" ] = dir + sample + "/*.root"
        elif tag[0] < 10:
            for i in range( tag[0] ):
                content[ tag[1] + "_{}".format( str( i ) ) ] = dir + sample + "/*{}.root".format( SplitFile( tag[0] )[i] ) 
        else:
            if tag[0] == 10:
                filelst = [x+y for x in SplitFile(2) for y in SplitFile(5) ] 
            else:
                filelst = [x+y for x in SplitFile(3) for y in SplitFile(5) ] 
            
            for i in range( tag[0] ):
                content[ tag[1] + "_{}".format( str( i ) ) ] = dir + sample + "/*{}.root".format( filelst[i] ) 
            # include one-digit file
            content[ tag[1] + "_{}".format( str( tag[0] ) ) ] = dir + sample + "/*_[0-9].root"

    return content

def MakeSampleInfo( outputfile, year, content ):

    ###########################################################################################################################
    el, mu, mc = [], [], []
    for c in content:
        if "Run" in c:
            if "SingleMuon" in c:
                mu.append( c )
            else:
                el.append( c )
        else:
            mc.append( c )

    el = ["'" + x + "'" for x in el ]
    mu = ["'" + x + "'" for x in mu ]
    mc   = ["'" + x + "'" for x in mc ]
    with open('python/PreCut{}.py'.format( year ) , 'w+' ) as fp:
        fp.write( context.format( ', '.join( el ), ', '.join( mu ), ', '.join( mc ) ) )

    el = [  x.replace("EGamma_", "").replace("SingleElectron_", "") for x in el ]
    mu = [  x.replace("SingleMuon_", "") for x in mu ]
    with open('python/FullCut{}.py'.format( year ), 'w+' ) as  fp:
        fp.write( context.format( ', '.join( el ), ', '.join( mu ), ', '.join( mc ) ) )

    ###########################################################################################################################
    outputfile.write( "sample{} = cms.PSet()\n".format( year) )
    for key, value  in content.iteritems():
        outputfile.write( text1.format( year, key, value ))

    ###########################################################################################################################
    CMSSW_BASE =  os.environ['CMSSW_BASE']
    if year == "16":
        outputfile.write( text1.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_2016LegacySF_V1.csv" ) )
        outputfile.write( text2.format( year, "SR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root", "eff_b" ) )
        outputfile.write( text2.format( year, "SR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root", "eff_c" ) )
        outputfile.write( text2.format( year, "SR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root", "eff_l" ) )
        outputfile.write( text2.format( year, "CR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root", "eff_b" ) )
        outputfile.write( text2.format( year, "CR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root", "eff_c" ) )
        outputfile.write( text2.format( year, "CR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root", "eff_l" ) )
        outputfile.write( text3.format( year, 
            "1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 2412, 2413, 2414, 2415",
            "917, 918, 919, 920, 921, 922, 923",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/lumimask_16.txt",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_69200.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_72400.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_66000.csv"
            ) )

    elif year == "17":
        outputfile.write( text1.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_94XSF_V4_B_F.csv" ) )
        outputfile.write( text2.format( year, "SR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root", "eff_b"  ) )
        outputfile.write( text2.format( year, "SR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root", "eff_c"  ) )
        outputfile.write( text2.format( year, "SR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root", "eff_l"  ) )
        outputfile.write( text2.format( year, "CR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root", "eff_b"  ) )
        outputfile.write( text2.format( year, "CR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root", "eff_c"  ) )
        outputfile.write( text2.format( year, "CR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root", "eff_l"  ) )
        outputfile.write( text3.format( year,
            "1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344", 
            "938, 939, 940, 941, 942, 943, 944",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/lumimask_17.txt",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_69200.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_72400.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_66000.csv"
            ) )

    elif year == "18":
        outputfile.write( text1.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_102XSF_V1.csv" ) )
        outputfile.write( text2.format( year, "SR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root", "eff_b"  ) )
        outputfile.write( text2.format( year, "SR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root", "eff_c"  ) )
        outputfile.write( text2.format( year, "SR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root", "eff_l"  ) )
        outputfile.write( text2.format( year, "CR_eff_b",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root", "eff_b"  ) )
        outputfile.write( text2.format( year, "CR_eff_c",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root", "eff_c"  ) )
        outputfile.write( text2.format( year, "CR_eff_l",   CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root", "eff_l"  ) )
        outputfile.write( text3.format( year,
            "200, 201, 202",
            "116, 117, 118",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/lumimask_18.txt",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_69200.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_72400.csv",
            CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_66000.csv"
            ) )

def main(args):
    sample16 = MakeContent( 
            "/wk_cms2/youying/public/bprimekit_2016_Legacy/",
            s16.mclst, s16.datalst 
            )
    
    sample17 = MakeContent( 
            "/wk_cms2/youying/public/bprimekit_2017_ReMiniAODv2/",
            s17.mclst, s17.datalst 
            )
    
    sample18 = MakeContent( 
            "/wk_cms2/youying/public/bprimekit_2018_ReReco/",
            s18.mclst, s18.datalst 
            )
    
    outputfile = open( "python/SampleInfo.py", "w+")
    outputfile.write( "import FWCore.ParameterSet.Config as cms\n" )
    MakeSampleInfo( outputfile, "16", sample16 )
    MakeSampleInfo( outputfile, "17", sample17 )
    MakeSampleInfo( outputfile, "18", sample18 )
    outputfile.close()

if __name__ == '__main__':
    main(sys.argv)
