#python PlotCompare.py       -y $1 -l $2 -c 20 
#python PlotCompare.py       -y $1 -l el -c 20 -o 150 --wobtag 
#python PlotCompare.py       -y $1 -l mu -c 20 -o 150 --wobtag 
#python PlotCompare.py       -y $1 -l el -c 20 -o 150  
#python PlotCompare.py       -y $1 -l mu -c 20 -o 150  
#python PlotCompare.py       -y $1 -l $2 -c 20 -o 150 --wopileup  
#python PlotCompare.py       -y $1 -l el -c 20 -r WJets -d --wobtag
#python PlotCompare.py       -y $1 -l mu -c 20 -r WJets -d --wobtag
#python PlotCompare.py       -y $1 -l el -c 20 -r WJets -d --wobtag --relax 
#python PlotCompare.py       -y $1 -l mu -c 20 -r WJets -d --wobtag --relax
#python PlotCompare.py       -y $1 -l el -c 20 -r QCD --0bjet  
#python PlotCompare.py       -y $1 -l mu -c 20 -r QCD --0bjet  
#python PlotCompare.py       -y $1 -l el -c 20 -r QCD --0bjet --wobtag 
#python PlotCompare.py       -y $1 -l mu -c 20 -r QCD --0bjet --wobtag 
python PlotSys.py           -y $1 -l el -c 20 -o 150
python PlotSys.py           -y $1 -l mu -c 20 -o 150
python PlotSys.py           -y $1 -l el -c 20 -r WJets_0b --wobtag --driven
python PlotSys.py           -y $1 -l mu -c 20 -r WJets_0b --wobtag --driven
python PlotSep.py           -y $1 -l el -B 
python PlotSep.py           -y $1 -l mu -B 
python PlotSep.py           -y $1 -l el -B -c 20
python PlotSep.py           -y $1 -l mu -B -c 20
python PlotClosureTest.py   -y $1 -l el -c 20 -t CR --wobtag --worelax
python PlotClosureTest.py   -y $1 -l mu -c 20 -t CR --wobtag --worelax
python PlotClosureTest.py   -y $1 -l el -c 20 -t CR --wobtag 
python PlotClosureTest.py   -y $1 -l mu -c 20 -t CR --wobtag
python PlotClosureTest.py   -y $1 -l el -c 20 -t SR --wobtag 
python PlotClosureTest.py   -y $1 -l mu -c 20 -t SR --wobtag 
#python PlotAsym.py          -y $1 -l $2 -c 20 -o 150 -a
#python PlotAcp.py           -y $1 -l $2 -s ttbar -A 10
#python PlotAcp.py           -y $1 -l $2 -s ttbar -A 10 -x
#python PlotAcp.py           -y $1 -l $2 -s ttbar -A 0 -x
#python PlotAcp.py           -y $1 -l $2 -s Data  -A 0 -x

return

python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -p 2000 
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u background
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u pileup_up
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u pileup_dn
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u btag_up
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u btag_dn
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u lepton_up
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u lepton_dn
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JER_up
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JER_dn
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JEC_up
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JEC_dn
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_10
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_11
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_12
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_13
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_14
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_15
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_16
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_17
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_18
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_19
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_20
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_21
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_22
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_23
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_24
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_25
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_26
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_27
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_28
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_29
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_30
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_31
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_32
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_33
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_34
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_35
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_36
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_37
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_38
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_39
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_40
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_41
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_42
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_43
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_44
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_45
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_46
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_47
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_48
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_49
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_50
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_51
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_52
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_53
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_54
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_55
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_56
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_57
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_58
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_59
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_60
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_61
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_62
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_63
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_64
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_65
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_66
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_67
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_68
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_69
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_70
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_71
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_72
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_73
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_74
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_75
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_76
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_77
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_78
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_79
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_80
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_81
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_82
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_83
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_84
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_85
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_86
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_87
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_88
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_89
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_90
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_91
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_92
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_93
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_94
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_95
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_96
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_97
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_98
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_99
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_100
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_101
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_102
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_103
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_104
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_105
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_106
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_107
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_108
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_109
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_110
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_111
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_1
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_2
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_3
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_4
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_6
python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_8

python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_up
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_dn
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u background
python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u background
