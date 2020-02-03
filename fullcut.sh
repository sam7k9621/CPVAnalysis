./SubmitJob.py -C "FullCut -y $1 -l el -r WJets" -S el mc 
./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets" -S mu mc
./SubmitJob.py -C "FullCut -y $1 -l el -r QCD" -S el mc 
./SubmitJob.py -C "FullCut -y $1 -l mu -r QCD" -S mu mc
./SubmitJob.py -C "FullCut -y $1 -l el" -S el mc 
./SubmitJob.py -C "FullCut -y $1 -l mu" -S mu mc 

 #./SubmitCondorJob.py -C FullCut -Y 16 -T "-l el -e JER_up" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l el -e JER_dn" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l el -e JEC_up" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l el -e JEC_dn" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l mu -e JER_up" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l mu -e JER_dn" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l mu -e JEC_up" -S mc -I "-s "
#./SubmitCondorJob.py -C FullCut -Y 16 -T "-l mu -e JEC_dn" -S mc -I "-s "
