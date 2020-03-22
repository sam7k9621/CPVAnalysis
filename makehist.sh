#BigBird $3

#./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20"      -S samplelst  
#./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -o 150"      -S samplelst  
#./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets_0b --wobtag"    -S samplelst  
#./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets_0b_worelax --wobtag"    -S samplelst  
#./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r QCD_0b --wobtag"       -S samplelst 
##./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r QCD_1b --wobtag"       -S samplelst 
##./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets_0b"    -S samplelst  
##./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets_0b --wobtag --relax"    -S samplelst  

#return 

#BigBird $4
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s ttbar -u 20 -o 150" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s ttbar                -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s DYJets_HT-70to100    -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s DYJets_HT-100to200   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s DYJets_HT-200to400   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty

#BigBird $5

#./SubmitJob.py   -C "MakeHist -y $1 -l $2 -s DYJets_HT-400to600   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py   -C "MakeHist -y $1 -l $2 -s DYJets_HT-600to800   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py   -C "MakeHist -y $1 -l $2 -s DYJets_HT-800to1200  -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py   -C "MakeHist -y $1 -l $2 -s DYJets_HT-1200to2500 -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py   -C "MakeHist -y $1 -l $2 -s DYJets_HT-2500toInf  -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty

#BigBird $6

#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-70To100    -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-100To200   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-200To400   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-400To600   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty

#BigBird $7

#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-600To800   -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-800To1200  -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-1200To2500 -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty
#./SubmitJob.py  -C "MakeHist -y $1 -l $2 -s WJets_HT-2500ToInf  -u 20 -r WJets_0b --wobtag" -S uncertainty -I uncertainty

#MakeHist -y 16 -l el -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 16 -l el -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y 17 -l el -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 17 -l el -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y 18 -l el -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 18 -l el -u 20 -o 150 -s Data  -c --TLV

#MakeHist -y 16 -l mu -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 16 -l mu -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y 17 -l mu -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 17 -l mu -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y 18 -l mu -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y 18 -l mu -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -20
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -15
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -10
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -5
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 0
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 5
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 10
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 15
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 20
#MakeHist -y 16 -l el -s ttbar -c -B  
#MakeHist -y 16 -l el -s ttbar -c -B -u 20 
#MakeHist -y 17 -l el -s ttbar -c -B  
#MakeHist -y 17 -l el -s ttbar -c -B -u 20 
#MakeHist -y 18 -l el -s ttbar -c -B  
#MakeHist -y 18 -l el -s ttbar -c -B -u 20 

#MakeHist -y 16 -l mu -s ttbar -c -B  
#MakeHist -y 16 -l mu -s ttbar -c -B -u 20 
#MakeHist -y 17 -l mu -s ttbar -c -B  
#MakeHist -y 17 -l mu -s ttbar -c -B -u 20 
#MakeHist -y 18 -l mu -s ttbar -c -B  
#MakeHist -y 18 -l mu -s ttbar -c -B -u 20 
./SubmitJob.py -C "MakeHist -y $1 -l $2 -s ttbar -a 0"  -S shuffle -I mixed
./SubmitJob.py -C "MakeHist -y $1 -l $2 -s Data  -a 0"  -S shuffle -I mixed
./SubmitJob.py -C "MakeHist -y $1 -l $2 -s ttbar -a 10" -S shuffle -I mixed
#MakeHist -y $1 -l $2 -s ttbar -a 10
