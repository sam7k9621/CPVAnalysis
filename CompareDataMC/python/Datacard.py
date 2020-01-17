import os

# sample, cross_section
mclst =[
     [ "QCD_HT100to200",               "QCD_HT100to200",             27990000         ],
     [ "QCD_HT200to300",               "QCD_HT200to300",             1712000          ],
     [ "QCD_HT300to500",               "QCD_HT300to500",             347700           ],
     [ "QCD_HT500to700",               "QCD_HT500to700",             32100            ],
     [ "QCD_HT700to1000",              "QCD_HT700to1000",            6831             ],
     [ "QCD_HT1000to1500",             "QCD_HT1000to1500",           1207             ],
     [ "QCD_HT1500to2000",             "QCD_HT1500to2000",           119.9            ],
     [ "QCD_HT2000toInf",              "QCD_HT2000toInf",            25.24            ],
     [ "DYJets_HT-70to100",            "DYJets_HT-70to100",          1.23 * 169.9     ],
     [ "DYJets_HT-100to200",           "DYJets_HT-100to200",         1.23 * 147.4     ],
     [ "DYJets_HT-200to400",           "DYJets_HT-200to400",         1.23 * 40.99     ],
     [ "DYJets_HT-400to600",           "DYJets_HT-400to600",         1.23 * 5.678     ],
     [ "DYJets_HT-600to800",           "DYJets_HT-600to800",         1.23 * 1.367     ],
     [ "DYJets_HT-800to1200",          "DYJets_HT-800to1200",        1.23 * 0.6304    ],
     [ "DYJets_HT-1200to2500",         "DYJets_HT-1200to2500",       1.23 * 0.1514    ],
     [ "DYJets_HT-2500toInf",          "DYJets_HT-2500toInf",        1.23 * 0.003565  ],
     [ "SingleTop_s-channel",          "ST_s-channel",               3.36             ],
     [ "SingleTop_t-channel_top",      "ST_t-channel_top",           136.02           ],
     [ "SingleTop_t-channel_antitop",  "ST_t-channel_antitop",       80.95            ],
     [ "SingleTop_tW_top",             "ST_tW_top",                  38.85            ],
     [ "SingleTop_tW_antitop",         "ST_tW_antitop",              38.85            ],
     [ "ttbar",                        "TTToSemiLeptonic",           365.34           ],
     [ "VV_WW",                        "WW",                         118.7            ],
     [ "VV_WZ",                        "WZ",                         47.13            ],
     [ "VV_ZZ",                        "ZZ",                         16.52            ],
     [ "WJets_HT-70To100",             "WJets_HT-70To100",           1.21 * 1292      ],
     [ "WJets_HT-100To200",            "WJets_HT-100To200",          1.21 * 1345.7    ],
     [ "WJets_HT-200To400",            "WJets_HT-200To400",          1.21 * 359.7     ],
     [ "WJets_HT-400To600",            "WJets_HT-400To600",          1.21 * 48.91     ],
     [ "WJets_HT-600To800",            "WJets_HT-600To800",          1.21 * 12.05     ],
     [ "WJets_HT-800To1200",           "WJets_HT-800To1200",         1.21 * 5.501     ],
     [ "WJets_HT-1200To2500",          "WJets_HT-1200To2500",        1.21 * 1.329     ],
     [ "WJets_HT-2500ToInf",           "WJets_HT-2500ToInf",         1.21 * 0.03216   ]
]

# CEDM_ntuple
CEDM_dict = {
    "0_0": 19.86648916,
    "0_5": 20.13314039,
    "1_0": 20.9547713 ,
    "1_5": 22.33671701,
    "2_0": 24.1848818 ,
    "2_5": 26.7946347 ,
    "3_0": 30.0738536 ,
    "3_5": 33.6303685 ,
    "4_0": 38.4841983 ,
    "4_5": 43.9452341 ,
    "5_0": 50.1732107 ,
}

def GetGennumber( year ) :
    with open( os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/BaseLineSelector/data/{}_Datacard.txt".format( year ) ) as file:  
        content = file.readlines()
    
    content = filter( None, [x.strip() for x in content] )
    content = {content[i]: float(content[i+1].split(":")[1]) for i in range(0, len(content), 2)}
    return content

