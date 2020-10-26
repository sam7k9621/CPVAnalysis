#python tempsys.py -y RunII -l el -c 20 -o 150
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "pileup_up pileup_dn" -n pileup
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "bctag_up bctag_dn" -n bctag
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "lighttag_up lighttag_dn" -n lighttag
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "lepton_up lepton_dn" -n lepton
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "JER_up JER_dn" -n JER
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "JEC_up JEC_dn" -n JEC
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "PDF_up PDF_dn" -n PDF
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "muFmuR_up muFmuR_dn" -n muFmuR
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "ISR_up ISR_dn" -n ISR
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "FSR_up FSR_dn" -n FSR
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "CP5CR1 CP5CR2" -n CP5CR
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "hdampUP hdampDOWN" -n hdamp
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "CP5up CP5down" -n CP5
#python tempsys.py -y RunII -l el -c 20 -o 150 -e "mtop1695 mtop1755" -n mtop 

MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 16 -l el -u 20 -s ttbar --uncertainty resolution_-15
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 16 -l mu -u 20 -s ttbar --uncertainty resolution_-15
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 17 -l el -u 20 -s ttbar --uncertainty resolution_-15
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 17 -l mu -u 20 -s ttbar --uncertainty resolution_-15
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 18 -l el -u 20 -s ttbar --uncertainty resolution_-15
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_5
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_-5
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_10
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_-10
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_15
MakeHist -y 18 -l mu -u 20 -s ttbar --uncertainty resolution_-15

return

#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u pileup_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u pileup_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u bctag_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u bctag_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u lighttag_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u lighttag_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u lepton_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u lepton_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u JER_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u JER_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u JEC_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u JEC_dn

#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u PDF_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u PDF_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u muFmuR_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u muFmuR_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u ISR_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u ISR_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u FSR_up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u FSR_dn
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u CP5CR1
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u CP5CR2
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u hdampUP
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u hdampDOWN
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u CP5up
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u CP5down
#python aaa.py -y RunII -l mu -c 20 -o 150 --seed 20 -p -u background

#return





##python tempsys.py           -y $1 -l $2 -c 20 -r WJets_0b --wobtag --driven --wosyst
##python tempsys.py           -y $1 -l $2 -c 20 -r WJets_0b --driven --wosyst
##python tempsys.py           -y $1 -l $2 -c 20 -r QCD_0b --wobtag --wosyst
##python tempsys.py           -y $1 -l $2 -c 20 -o 150 
##python temp.py -y $1 -l $2 -c 20 -o 150
###python PlotSep.py           -y $1 -l $2 -B 
##python PlotSep.py           -y $1 -l $2 -B -c 20
python PlotClosureTest.py   -y $1 -l $2 -c 20 --wobtag -t CR
python PlotClosureTest.py   -y $1 -l $2 -c 20 --wobtag -t SR

##python PlotAcp.py           -y $1 -l $2 -c 20 -o 150
##python PlotAcp.py           -y $1 -l $2 -s ttbar -c 20 -o 150
##python PlotAcp.py           -y $1 -l $2 -s ttbar -A 10
##python PlotAcp.py           -y $1 -l $2 -s ttbar -A 10 -x
##python PlotAcp.py           -y $1 -l $2 -s ttbar -A 0 -x
##python PlotAcp.py           -y $1 -l $2 -s Data  -A 0 -x

#return 
##MakeHist -y $1 -l $2 -u 20 -s ttbar
##MakeHist -y $1 -l $2 -u 20 -s Data 
##MakeHist -y $1 -l $2 -u 20 -s Data -o 150
##MakeHist -y $1 -l $2 -u 20 -s Data  -r WJets_0b --wobtag
##MakeHist -y $1 -l $2 -u 20 -s Data -o 150 -r WJets_0b --wobtag
##python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -x -u pileup_up
##python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -x -u pileup_dn
##python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -x -u btag_up
##python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -x -u btag_dn
##python PlotUncDist.py -y RunII -l $2 -c 20 -o 150 -u pileup -x
##python PlotUncDist.py -y RunII -l $2 -c 20 -o 150 -u btag -x
#return 
#########################################3

##python PlotAsym.py          -y $1 -l $2 -c 20 -o 150 -a
##python PlotCompare.py       -y $1 -l $2 -c 20 -o 150  
##python PlotCompare.py       -y $1 -l $2 -c 20 -o 150 --wopileup  
##python PlotCompare.py       -y $1 -l $2 -c 20 -r WJets -d
##python PlotCompare.py       -y $1 -l $2 -c 20 -r QCD --0bjet 
##python PlotFitResult.py -y $1 -l $2 -c 20 -o 150  

#python PlotFitResult.py -y $1 -l $2 -c 20 -o 150 -p 2000 
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u background
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u pileup_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u pileup_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u btag_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u btag_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u lepton_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u lepton_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JER_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JER_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JEC_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u JEC_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u PDF_dn
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_up
#python PlotFitResult.py -l $2 -c 20 -y $1 -o 150 -u muFmuR_dn

#return 

#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u pileup_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u btag_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u lepton_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JER_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u JEC_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u PDF_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_up
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u muFmuR_dn
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u background
#python PlotUnc.py -y $1 -l $2 -c 20 -o 150 -u background
