rm data/Systematics.xml

./SamsAnalysis LYSO_hitRate forward
./SamsAnalysis CSI_Doped_hitRate forward

./SamsAnalysis BGO_dose forward
./SamsAnalysis BGO_dose backward

./SamsAnalysis HE3_rate

python SystematicPlotter.py 0 0 png