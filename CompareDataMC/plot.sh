echo "\hline\multirow{2}{*}{PU re-weight}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u pileup_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u pileup_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{b-tag scale factor (b and c quark)}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u bctag_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u bctag_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{b-tag scale factor (light quark)}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u lighttag_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u lighttag_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Lepton}"; 
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u lepton_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u lepton_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{JER}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u JER_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u JER_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{JEC}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u JEC_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u JEC_dn
echo "\\\\\\"

echo "\hline\multicolumn{6}{|c|}{Theoretical uncertainties}\\\\\\\\"
echo "\hline\multirow{2}{*}{PDF}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u PDF_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u PDF_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{$\mu_R$ and $\mu_F$}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u muFmuR_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u muFmuR_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{ISR}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u ISR_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u ISR_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{FSR}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u FSR_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u FSR_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Color Reconnection}"
echo "& CR1"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u CP5CR1
echo "\\\\\\"
echo "& CR2"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u CP5CR2
echo "\\\\\\"

echo "\hline\multirow{2}{*}{ME-PS matching}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u hdampUP
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u hdampDOWN
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Underlying event}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u CP5up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u CP5down
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Flavour response}"
echo "& \$+1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u FR_up
echo "\\\\\\"
echo "& \$-1 \sigma$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u FR_dn
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Top mass variation}"
echo "& \$+1 \GeV$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u mtop1755
echo "\\\\\\"
echo "& \$-1 \GeV$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u mtop1695
echo "\\\\\\"

echo "\hline\multirow{1}{*}{Bkg. template}"
echo "& -"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u background
echo "\\\\\\"

echo "\hline\multirow{2}{*}{Per event resolution}"
echo "& \$+10\%$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u resolution_10
echo "\\\\\\"
echo "& \$-10\%$"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u resolution_-10
echo "\\\\\\"

echo "\hline\multirow{1}{*}{W+HF enriched}"
echo "& -"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u WHF_10
echo "\\\\\\"

echo "\hline\multirow{1}{*}{w/o Top PT reweighting}"
echo "& -"
python temp_cut.py -y $1 -l $2 -c 20 -o 150 --seed 20 -p -u woTopPT
echo "\\\\\\"

