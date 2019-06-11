./SubmitCondorJob.py -C MakeHist -T "-l el --pileup" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l mu --pileup" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20" -S samplelst
./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 --pileup" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 --pileup" -S samplelst
./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 --pileup -o 150" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 --pileup -o 150" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l el -u 20 -r 0bjet --pileup" -S samplelst
#./SubmitCondorJob.py -C MakeHist -T "-l mu -u 20 -r 0bjet --pileup" -S samplelst
