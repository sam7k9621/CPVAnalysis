./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 --wopileup" -S samplelst -I "-s "
./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 --wopileup" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 -o 150" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 -o 150" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 -r WJets"       -S samplelst -I "-s " 
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 -r WJets"       -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 -r QCD --0bjet" -S samplelst -I "-s "
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 -r QCD --0bjet" -S samplelst -I "-s "

#MakeHist -l mu -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -l el -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -l mu -u 20 -o 150 -s Data  -c --TLV 
#MakeHist -l el -u 20 -o 150 -s Data  -c --TLV

#./SubmitCondorJob.py -C MakeHist -T "-l mu -s Data -a 0" -S shuffle -I "-m "
#./SubmitCondorJob.py -C MakeHist -T "-l el -s Data -a 0" -S shuffle -I "-m "
#./SubmitCondorJob.py -C MakeHist -T "-l mu -s ttbar -a 0" -S shuffle -I "-m "
#./SubmitCondorJob.py -C MakeHist -T "-l el -s ttbar -a 0" -S shuffle -I "-m "
#./SubmitCondorJob.py -C MakeHist -T "-l mu -s ttbar -a 10" -S shuffle -I "-m "
#./SubmitCondorJob.py -C MakeHist -T "-l el -s ttbar -a 10" -S shuffle -I "-m "
