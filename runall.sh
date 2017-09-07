rm data/Systematics.xml
rm tex/*_*.tex
rm -r figs/*

./SamsAnalysis LYSO_hitRate forward
./SamsAnalysis LYSO_hitRate backward

./SamsAnalysis CSI_Pure_hitRate forward
./SamsAnalysis CSI_Pure_hitRate backward

./SamsAnalysis BGO_dose forward
./SamsAnalysis BGO_dose backward

./SamsAnalysis HE3_rate

./SamsAnalysis PIN_dose

python SystematicPlotter.py 1 0 png
python SystematicPlotter.py 1 0 pdf

#cd tex
#source makeList.sh
#pdflatex SamsAnalysis.tex >> /dev/null 2>&1
#pdflatex SamsAnalysis.tex >> /dev/null 2>&1
#cd $OLDPWD