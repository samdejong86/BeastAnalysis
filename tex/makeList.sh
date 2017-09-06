rm -rf files.tex
prefix="../figs/"
suffix=".pdf"
echo '\newcommand*{\ListOfFiles}{' >> files.tex

for f in ../figs/*
do
    if [[ $f == *"Results"* ]]; then
	continue
    fi
    for g in $f/* 
    do
	echo ${g%$suffix}, >> files.tex
    done
    echo ${f#$prefix}, >> files.tex
done
sed -i '$ s/.$/}/' files.tex