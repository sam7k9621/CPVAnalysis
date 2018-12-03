#nohup ./submitJob.py -C FullCut -T "-l el -e JER_up" -S sample -R 95 > fullcut_el_JER_up.out&
#nohup ./submitJob.py -C FullCut -T "-l el -e JER_dn" -S sample -R 95 > fullcut_el_JER_dn.out&
nohup ./submitJob.py -C FullCut -T "-l el -e JEC_up" -S sample -R 205 > fullcut_el_JEC_up.out&
nohup ./submitJob.py -C FullCut -T "-l el -e JEC_dn" -S sample -R 205 > fullcut_el_JEC_dn.out&
#nohup ./submitJob.py -C FullCut -T "-l mu -e JER_up" -S sample -R 95 > fullcut_mu_JER_up.out&
#nohup ./submitJob.py -C FullCut -T "-l mu -e JER_dn" -S sample -R 95 > fullcut_mu_JER_dn.out&
nohup ./submitJob.py -C FullCut -T "-l mu -e JEC_up" -S sample -R 205 > fullcut_mu_JEC_up.out&
nohup ./submitJob.py -C FullCut -T "-l mu -e JEC_dn" -S sample -R 205 > fullcut_mu_JEC_dn.out&
