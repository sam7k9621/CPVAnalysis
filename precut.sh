#./SubmitJob.py -C "PreCut -y $1 -l $2 -W" --maxRunJobs 10 -S $2
./SubmitJob.py -C "PreCut -y $1" --MaxRunJobs 5 -S mc -E "CEDM"
