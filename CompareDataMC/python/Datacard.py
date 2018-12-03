
sample_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/"
# sample, cross_section
mclst =[
        [ "QCD_HT100to200",             27990000         ],
        [ "QCD_HT200to300",             1712000          ],
        [ "QCD_HT300to500",             347700           ],
        [ "QCD_HT500to700",             32100            ],
        [ "QCD_HT700to1000",            6831             ],
        [ "QCD_HT1000to1500",           1207             ],
        [ "QCD_HT1500to2000",           119.9            ],
        [ "QCD_HT2000toInf",            25.24            ],
        [ "DYJet_HT-70to100",           1.23 * 169.9     ],
        [ "DYJet_HT-100to200",          1.23 * 147.4     ],
        [ "DYJet_HT-200to400",          1.23 * 40.99     ],
        [ "DYJet_HT-400to600",          1.23 * 5.678     ],
        [ "DYJet_HT-600to800",          1.23 * 1.367     ],
        [ "DYJet_HT-800to1200",         1.23 * 0.6304    ],
        [ "DYJet_HT-1200to2500",        1.23 * 0.1514    ],
        [ "DYJet_HT-2500toInf",         1.23 * 0.003565  ],
        [ "SingleTop_s-ch",             3.36             ],
        [ "SingleTop_t-ch",             136.02           ],
        [ "SingleTop_t-ch_anti",        80.95            ],
        [ "SingleTop_tW",               35.6             ],
        [ "SingleTop_tW_anti",          35.6             ],
        [ "TTbar",                      831.76           ],
        [ "VV_WW",                      118.7            ],
        [ "VV_WZ",                      47.13            ],
        [ "VV_ZZ",                      16.52            ],
        [ "WJetsToLNu_HT-100To200",     1.21 * 1345.7    ],
        [ "WJetsToLNu_HT-200To400",     1.21 * 359.7     ],
        [ "WJetsToLNu_HT-400To600",     1.21 * 48.91     ],
        [ "WJetsToLNu_HT-600To800",     1.21 * 12.05     ],
        [ "WJetsToLNu_HT-800To1200",    1.21 * 5.501     ],
        [ "WJetsToLNu_HT-1200To2500",   1.21 * 1.329     ],
        [ "WJetsToLNu_HT-2500ToInf",    1.21 * 0.03216   ]
]

def GetGennumber() :
    with open( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/Datacard.txt" ) as file:  
        content = file.readlines()
    
    content = filter( None, [x.strip() for x in content] )
    content = {content[i]: float(content[i+1].split(":")[1]) for i in range(0, len(content), 2)}
    return content
