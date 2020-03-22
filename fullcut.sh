#./SubmitJob.py -C "FullCut -y $1 -l el -r WJets_medium" -S el mc 
#./SubmitJob.py -C "FullCut -y $1 -l el -r WJets_1b" -S el mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets_0b" -S mu mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets_medium" -S mu mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets_1b" -S mu mc 

./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets_0b --worelax" -S mu mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r WJets_1b" -S mu mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r QCD_0b" -S mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu -r QCD_1b" -S mc 
#./SubmitJob.py -C "FullCut -y $1 -l mu" -S mu mc 

./SubmitJob.py -C "FullCut -y $1 -l el -r WJets_0b --worelax" -S el mc 
#./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b" -S $2 mc 
#./SubmitJob.py -C "FullCut -y $1 -l el -r WJets_1b" -S el mc 
#./SubmitJob.py -C "FullCut -y $1 -l $2 -r QCD_0b" -S $2 mc 
#./SubmitJob.py -C "FullCut -y $1 -l el -r QCD_1b" -S mc 
#./SubmitJob.py -C "FullCut -y $1 -l $2" -S $2 mc 

##return
#./SubmitJob.py -C "FullCut -y $1 -l $2 -e JER_up" -S mc -E TTT
#./SubmitJob.py -C "FullCut -y $1 -l $2 -e JER_dn" -S mc -E TTT
#./SubmitJob.py -C "FullCut -y $1 -l $2 -e JEC_up" -S mc -E TTT
#./SubmitJob.py -C "FullCut -y $1 -l $2 -e JEC_dn" -S mc -E TTT

#./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_up" -S mc -E WJets
#./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_dn" -S mc -E WJets

return

./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_dn" -S mc -E WJets
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_up" -S mc -E WJets

./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_up" -S mc -E TTT
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_dn" -S mc -E TTT
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_up" -S mc -E TTT
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_dn" -S mc -E TTT

./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_up" -S mc -E DY
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JER_dn" -S mc -E DY
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_up" -S mc -E DY
./SubmitJob.py -C "FullCut -y $1 -l $2 -r WJets_0b -e JEC_dn" -S mc -E DY

