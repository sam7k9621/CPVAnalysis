#./SubmitJob.py -C MakeHist -Y $1 -T "-l $2 -u 20 -o 150 --wopileup" -S samplelst -I "-s "
#./SubmitJob.py -C MakeHist -Y $1 -T "-l $2 -u 20" -S samplelst -I "-s "
./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -o 150"               -S samplelst  
./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets"             -S samplelst  
./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r WJets --wobtag"    -S samplelst  
./SubmitJob.py -C "MakeHist -y $1 -l $2 -u 20 -r QCD --0bjet"       -S samplelst 
#./SubmitQJob.py -C MakeHist -Y $1 -T "-l $2 -s ttbar -u 20" -S uncertainty -I "-e "
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -c --TLV 
#MakeHist -y $1 -l $2 -u 20 -o 150 -s Data  -c --TLV
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -20
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -15
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -10
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a -5
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 0
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 5
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 10
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 15
#MakeHist -y $1 -l $2 -u 20 -o 150 -s ttbar -a 20
#MakeHist -y $1 -l $2 -s ttbar -c -B  
#MakeHist -y $1 -l $2 -s ttbar -c -B -u 20 
#./SubmitQJob.py -C MakeHist -Y $1 -T "-l $2 -s ttbar -a 0" -S shuffle -I "-m "
#./SubmitQJob.py -C MakeHist -Y $1 -T "-l $2 -s Data -a 0" -S shuffle -I "-m "
#./SubmitQJob.py -C MakeHist -Y $1 -T "-l $2 -s ttbar -a 10" -S shuffle -I "-m "
#MakeHist -y $1 -l $2 -s ttbar -a 10
