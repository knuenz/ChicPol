#!/bin/sh

homedir=$PWD
cd ..
cd ..
basedir=$PWD
cd macros/polFit
storagedir=`more storagedir`/Data #please define the directory storagedir in the file macros/polFit/storagedir
#storagedir=$basedir/Psi/Data
datadir_Start=${basedir}/macros/DataFiles

########## INPUTS ##########

#Take Care of Mean pT in ToyMC.h
NSigma=3.00 #needed in 2 decimal accuracy (x.yz)

for nState in 5;do

cp ../../interface/rootIncludes.inc               rootIncludes.inc
cp ../../interface/commonVar_Psi$[nState-3]S.h    commonVar.h
cp ../../interface/ToyMC_Psi$[nState-3]S.h        ToyMC.h
cp ../../interface/effsAndCuts_Psi$[nState-3]S.h  effsAndCuts.h
touch polRapPtPlot.cc
make

#for JobID in Psi$[nState-3]S_${NSigma}Sigma_11Dec2012; do
for JobID in Psi$[nState-3]S_${NSigma}Sigma_11Dec2012_noRhoFactor; do

DataID=_FrameworkTest_5Dec2012

FidCuts=11
if [ $nState -eq 4 ]
then
ptBinMin=1
ptBinMax=12
fi
if [ $nState -eq 5 ]
then
ptBinMin=2
ptBinMax=6
fi

MPValgo=3 		#1...mean,2...gauss,3...gauss-loop with chi2<2

additionalName=MPV${MPValgo}

############################


TreeID=Psi$[nState-3]S

datadir=${datadir_Start}/SetOfCuts${FidCuts}${DataID}/Psi$[nState-3]S/tmpFiles

frameSig=1
polScenSig=3

frameBkg=1
polScenBkg=3

nGenerations=10


rapBinMin=1 #don't change
if [ $nState -eq 4 ] 
then
rapBinMax=2 #don't change
fi
if [ $nState -eq 5 ] 
then
rapBinMax=3 #don't change
fi

Jobdir=${storagedir}/${JobID}

mkdir ${basedir}/macros/polFit/FiguresData
mkdir ${Jobdir}
mkdir ${Jobdir}/Figures
mkdir ${Jobdir}/Figures/${TreeID}
mkdir ${Jobdir}/Figures/${TreeID}/Figures
mkdir ${basedir}/macros/polFit/FiguresData/${JobID}
mkdir ${basedir}/macros/polFit/FiguresData/${JobID}/${TreeID}

#cp ${basedir}/macros/polFit/polGenRecFitPlot.cc ${Jobdir}/polGenRecFitPlot.cc
#cp ${basedir}/macros/polFit/polRapPtPlot.cc ${Jobdir}/polRapPtPlot.cc
#cp ${basedir}/macros/polFit/PlotFinalResults.cc ${Jobdir}/PlotFinalResults.cc
#cp ${basedir}/macros/polFit/Makefile ${Jobdir}/Makefile
#cp ${basedir}/macros/polFit/polGen.C ${Jobdir}/polGen.C
#cp ${basedir}/macros/polFit/polRec.C ${Jobdir}/polRec.C
#cp ${basedir}/macros/polFit/polFit.C ${Jobdir}/polFit.C
#cp ${basedir}/macros/polFit/polPlot.C ${Jobdir}/polPlot.C
#
#cp ../../interface/rootIncludes.inc ${Jobdir}/rootIncludes.inc
#cp ../../interface/commonVar_Psi$[nState-3]S.h ${Jobdir}/commonVar.h
#cp ../../interface/ToyMC_Psi$[nState-3]S.h ${Jobdir}/ToyMC.h
#cp ../../interface/effsAndCuts_Psi$[nState-3]S.h ${Jobdir}/effsAndCuts.h

#cd ${Jobdir}
#touch polRapPtPlot.cc
#make
#cp ${basedir}/macros/polFit/polRapPtPlot polRapPtPlot_${TreeID}

echo 'copy finished'

for nSigma in 3 2 1;do #3 2 1

cd ${Jobdir}
cp ${basedir}/macros/polFit/polRapPtPlot polRapPtPlot_${TreeID}

./polRapPtPlot_${TreeID} ${nSigma}nSigma ${ptBinMin}ptBinMin ${ptBinMax}ptBinMax ${rapBinMin}rapBinMin ${rapBinMax}rapBinMax ${frameSig}frameSig ${polScenSig}polScen ${MPValgo}MPValgo ${nGenerations}nGenerations ${TreeID}=TreeID realdata ${Jobdir}=dirstruct ${nState}nState ${datadir}=realdatadir

mv ${Jobdir}/TGraphResults_${TreeID}_temp.root ${Jobdir}/TGraphResults_${TreeID}.root 
cp ${Jobdir}/TGraphResults_${TreeID}.root ${Jobdir}/TGraphResults_${TreeID}_${nSigma}sigma.root 

cd ${Jobdir}/Figures/${TreeID}

cp ${basedir}/latex/DataResults_vs_RapPt.tex .
cp ${basedir}/latex/IndividualFitResults.tex ../../.
mv ${Jobdir}/ToyNumericalResults.tex .

pdflatex ToyNumericalResults.tex
mv ToyNumericalResults.pdf ${basedir}/macros/polFit/FiguresData/${JobID}/${TreeID}/DataNumericalResults_${additionalName}.pdf
rm *.aux
rm *.log


pdflatex DataResults_vs_RapPt.tex
mv DataResults_vs_RapPt.pdf ${basedir}/macros/polFit/FiguresData/${JobID}/${TreeID}/DataResults_vs_RapPt_${additionalName}.pdf

rm *.aux
rm *.log
rm DataResults_vs_RapPt.tex

rap_=${rapBinMin}
while [ $rap_ -le ${rapBinMax} ]
do
pT_=${ptBinMin}
while [ $pT_ -le ${ptBinMax} ]
do

cd ${Jobdir}/Figures/${TreeID}

filename=../lph_vs_lth_${TreeID}_rap${rap_}_pT${pT_}.pdf
if test -s "$filename"
then
cd ../..
	pdflatex "\newcommand\TreeBinID{${TreeID}_rap${rap_}_pT${pT_}}\input{IndividualFitResults.tex}"
	mv IndividualFitResults.pdf ${basedir}/macros/polFit/FiguresData/${JobID}/${TreeID}/IndividualFitResults_rap${rap_}pt${pT_}_${additionalName}.pdf
fi


pT_=$[pT_+1]
done
rap_=$[rap_+1]
done


done


cd ${Jobdir}


done

rm polRapPtPlot_${TreeID}
rm IndividualFitResults.tex
rm *.aux
rm *.log
cd ${basedir}/macros/polFit
rm polRapPtPlot
done
