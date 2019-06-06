#./SubmitJob.py -C MakeHist -T "-l el --pileup" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l mu --pileup" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l el -u 20" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l mu -u 20" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l el -u 20 --pileup" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l mu -u 20 --pileup" -S samplelst
./SubmitJob.py -C MakeHist -T "-l el -u 20 --pileup -o 150" -S samplelst
./SubmitJob.py -C MakeHist -T "-l mu -u 20 --pileup -o 150" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l el -u 20 -r 0bjet --pileup" -S samplelst
#./SubmitJob.py -C MakeHist -T "-l mu -u 20 -r 0bjet --pileup" -S samplelst
