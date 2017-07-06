rm data/Systematics.xml
rm tex/*_*.tex

./SamsAnalysis LYSO_hitRate forward
./SamsAnalysis CSI_Doped_hitRate forward

./SamsAnalysis BGO_dose forward
./SamsAnalysis BGO_dose backward

./SamsAnalysis HE3_rate

python SystematicPlotter.py 0 0 png

cd tex
source makeList.sh
pdflatex SamsAnalysis.tex >> /dev/null 2>&1
pdflatex SamsAnalysis.tex >> /dev/null 2>&1
cd $OLDPWD