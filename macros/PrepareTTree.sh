#!/bin/chs

############ INPUTS ####################

source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh

Cdir=$PWD

cd ..
basedir=$PWD
cd macros

# input arguments
for nState in 6;do    #1,2,3,Upsi(1S,2S,3S); 4=Jpsi, 5=PsiPrime, 6=chic1, 7=chic2
for FidCuts in 11;do #defines the set of cuts to be used, see macros/polFit/effsAndCuts.h
cd $Cdir

COPY_AND_COMPILE=0

rapMin=1     #takes bins, not actual values
rapMax=1     #if you only want to process 1 y bin, rapMax = rapMin
ptMin=0      #takes bins, not acutal values
ptMax=0      #if you only want to process 1 pt bin, ptMax = ptMin

Plotting=1   #plotting macro: 1 = plot all, 2 = plot mass, 3 = plot lifetime
			 #plotting macro: 4 = plot lifetimeSR1, 5 = plot lifetimeSR2, 6 = plot lifetimeLSB, 7 = plot lifetimeRSB, 8 = plot lifetimeFullRegion

PlottingJpsi=2   #plotting macro: 1 = plot all, 2 = plot mass, 3 = plot lifetimeSBs, 4= lifetimeSR, 5= PlotMassRap, 6= plot lifetime, 7=pedagogical

PlottingDataDists=2 #0...all, 1...1D plots, 2...2D plots

runChiMassFitOnly=false
correctCtau=false   #correct pseudo-proper lifetime
rejectCowboys=false
RequestTrigger=true
MC=false
drawRapPt2D=false  #draw Rap-Pt 2D map of Psi
FixRegionsToInclusiveFit=false
rapFixTo=1
ptFixTo=1
doFractionUncer=true

#PlotFitPar:::
AddInclusiveResult=false #Inclusive defined by rapFixTo, ptFixTo


doCtauUncer=true
PolLSB=false       #measure polarization of the left sideband
PolRSB=false       #measure polarization of the right sideband
PolNP=false        #measure polarization of the non prompt events
forceBinning=true  #set binning of Psi1S consistently to non prompt binning and Psi2S consistently to background binning
folding=true       #folding is applied to all background histograms
normApproach=false #normalization 
ctauScen=3         #0:default(1s:2.5,2s:2.0), 1:(1s:3.5,2s:3.0), 2:(1s:1.5,2s:1.0), 3:100mm 1S and 2S, 
FracLSB=-1         #-1:defalut, 0, 100
scaleFracBg=false
fitMassPR=false
fitMassNP=false

DataID=Psi$[nState-3]S_ctauScen0_FracLSB-1_16Mar2013
polDataPath=${basedir}/Psi/Data/${DataID}


#Define JobID
#JobID=April15_FullData_PtBinning_BkgLTaddDS
#JobID=MC_April9
#JobID=MC_March25
#JobID=March18_FullData_PtBinning

#JobID=April23_WorkshopFollowUp
#JobID=April23_WorkshopFollowUp_ScaledJpsictErr
#JobID=April23_WorkshopFollowUp_2rapBins
#JobID=April24_JpsiMassRap
#JobID=MC_plusArtificialBG_April25
#JobID=May2_WorkshopFollowUp #_CtauTest
#JobID=May5_WorkshopFollowUp_PlotsOldSolution
#JobID=May7_WorkshopFollowUp_DeformCtauErr
#JobID=MeetingFollowUp_May11_freeCBnPsi
#JobID=DebugCtau_ModelCtauErr2011_May26
#JobID=DebugCtau_June4_PunziCheck
JobID=LastFitImprovments_June7_ConstrainBGl_ConstrainHighPt
#JobID=DebugCtau_June17_PunziCheck2011


################ EXECUTABLES #################

#following flags decide if the step is executed (1) or not (0):
#IMPORTANT: for MC set execute_runWorkspace, execute_MassFit and execute_runLifetimeFit to 0
execute_runChiData=0			           		#independent of rapMin, rapMax, ptMin, ptMax
execute_runWorkspace=0	    					#independent of rapMin, rapMax, ptMin, ptMax
execute_runMassFit=0				    	    #can be executed for different pt and y bins
execute_runLifetimeFit=0    				    #can be executed for different pt and y bins
execute_runPlotJpsiMassLifetime=1    			#can be executed for different pt and y bins
execute_PlotJpsiFitPar=0              			#can be executed for different pt and y bins
execute_runChiMassLifetimeFit=0		  	    	#can be executed for different pt and y bins
execute_runDefineRegionsAndFractions=0			#can be executed for different pt and y bins
execute_runPlotMassLifetime=0    				#can be executed for different pt and y bins
execut_PlotFitPar=0              				#can be executed for different pt and y bins
execute_runPlotDataDistributions=0 		 		#This step only has to be executed once for each set of cuts (indep. of FracLSB and nSigma)

execute_runBkgHistos=0           				#can be executed for different pt and y bins
execute_PlotCosThetaPhiBG=0 		 			#This step only has to be executed once for each set of cuts (indep. of FracLSB and nSigma)
execute_PlotCosThetaPhiDistribution=0 			#This step only has to be executed once for each set of cuts (indep. of FracLSB and nSigma)

#################################
#PsiRelics:::
execute_runData=0			           #independent of rapMin, rapMax, ptMin, ptMax


# input files
# In case of more input Files: define inputTreeX and adapt the line starting with inputTrees, at the moment up to 4 files implemented
if [ ${nState} -eq 4 ] 
then
inputTree1=/scratch/knuenz/Polarization/RootInput/Psi/TTree_Onia2MuMu_v30_PromptRecoAB_10May2012_Jpsi.root
if [ ${MC} = 'true' ]
then
inputTree1=/scratch/ikratsch/Polarization/Jpsi/InputFiles/TTree_Psi1S_Gun_Pt9p5_70p5_19Dec2012.root
fi
fi

if [ ${nState} -eq 5 ]
then
inputTree1=/scratch/knuenz/Polarization/RootInput/Psi/TTree_Onia2MuMu_v30_PromptRecoAB_10May2012_Psi.root
if [ ${MC} = 'true' ]
then
inputTree1=/scratch/ikratsch/Polarization/Jpsi/InputFiles/TTree_Psi2S_Gun_Pt6p5_50p5_19Dec2012.root
fi
fi


if [ ${nState} -eq 6 ] 
then
inputTree1=/scratch/knuenz/Polarization/RootInput/ChicPol/chic_rootuple_subFeb2014.root
if [ ${MC} = 'true' ]
then
inputTree1=/scratch/knuenz/Polarization/RootInput/ChicPol/chic_rootuple_MC_15M_sel.root
fi
fi




# Make directories
CutDir=${Cdir}/DataFiles/SetOfCuts${FidCuts}_${JobID}

WorkDir=${CutDir}
mkdir -p ${CutDir}
mkdir -p ${WorkDir}
cp ../interface/commonVar.h ${WorkDir}/commonVar.h 

mkdir -p DataFiles
mkdir -p ${WorkDir}/tmpFiles/backupWorkSpace
mkdir -p ${WorkDir}/Figures
mkdir -p ${WorkDir}/PDF
mkdir -p ${WorkDir}/Fit


if [ ${COPY_AND_COMPILE} -eq 1 ]
then

# Copy files to directory
cp Makefile ${WorkDir}/Makefile
cp ../interface/rootIncludes.inc ${WorkDir}/rootIncludes.inc

cp runData.cc ${WorkDir}/runData.cc
cp runChiData.cc ${WorkDir}/runChiData.cc
cp PolData.C ${WorkDir}/PolData.C
cp PolData.h ${WorkDir}/PolData.h
cp PolChiData.C ${WorkDir}/PolChiData.C
cp PolChiData.h ${WorkDir}/PolChiData.h
cp ../interface/effsAndCuts.h ${WorkDir}/effsAndCuts.h

cp runWorkspace.cc ${WorkDir}/runWorkspace.cc
cp createWorkspace.C ${WorkDir}/createWorkspace.C

cp runMassFit.cc ${WorkDir}/runMassFit.cc
cp massFit.cc ${WorkDir}/massFit.cc

cp runChiMassLifetimeFit.cc ${WorkDir}/runChiMassLifetimeFit.cc
cp chiMassLifetimeFit.cc ${WorkDir}/chiMassLifetimeFit.cc
cp runDefineRegionsAndFractions.cc ${WorkDir}/runDefineRegionsAndFractions.cc
cp DefineRegionsAndFractions.cc ${WorkDir}/DefineRegionsAndFractions.cc



cp runLifetimeFit.cc ${WorkDir}/runLifetimeFit.cc
cp lifetimeFit.cc ${WorkDir}/lifetimeFit.cc
cp ../interface/calculatePar.cc ${WorkDir}/calculatePar.cc
cp ../interface/RooUtils.h ${WorkDir}/RooUtils.h

cp runPlotMassLifetime.cc ${WorkDir}/runPlotMassLifetime.cc
cp PlotMassLifetime.cc ${WorkDir}/PlotMassLifetime.cc

cp runPlotJpsiMassLifetime.cc ${WorkDir}/runPlotJpsiMassLifetime.cc
cp PlotJpsiMassLifetime.cc ${WorkDir}/PlotJpsiMassLifetime.cc

cp runPlotDataDistributions.cc ${WorkDir}/runPlotDataDistributions.cc
cp PlotDataDistributions.cc ${WorkDir}/PlotDataDistributions.cc

cp PlotFitPar.cc ${WorkDir}/PlotFitPar.cc
cp PlotJpsiFitPar.cc ${WorkDir}/PlotJpsiFitPar.cc

cp runBkgHistos.cc ${WorkDir}/runBkgHistos.cc
cp bkgHistos.C ${WorkDir}/bkgHistos.C
cp calcPol.C ${WorkDir}/calcPol.C

cp PlotCosThetaPhiBG.cc ${WorkDir}/PlotCosThetaPhiBG.cc
cp PlotCosThetaPhiDistribution.cc ${WorkDir}/PlotCosThetaPhiDistribution.cc

#cp ../latex/Mass_fitParameter.tex ${WorkDir}/Mass_fitParameter.tex
#cp ../latex/Lifetime_fitParameter.tex ${WorkDir}/Lifetime_fitParameter.tex
#cp ../latex/myStyle.tex ${WorkDir}/myStyle.tex
#cp ../latex/evaluateCtau.tex ${WorkDir}/evaluateCtau.tex
#cp ../latex/NumEvents.tex ${WorkDir}/NumEvents.tex

#cp ../latex/cosThetaPhi_BG.tex        ${WorkDir}/cosThetaPhi_BG.tex
#cp ../latex/cosThetaPhi_BG_highct.tex ${WorkDir}/cosThetaPhi_BG_highct.tex
#cp ../latex/cosThetaPhi_NPBG.tex      ${WorkDir}/cosThetaPhi_NPBG.tex
#cp ../latex/cosThetaPhi_TBG.tex       ${WorkDir}/cosThetaPhi_TBG.tex
#cp ../latex/cosThetaPhi.tex           ${WorkDir}/cosThetaPhi.tex
#cp ../latex/MassLifetime.tex       ${WorkDir}/MassLifetime.tex

cd ${WorkDir}

make

fi

cd ${WorkDir}

inputTrees="inputTree=${inputTree1} inputTree=${inputTree2} inputTree=${inputTree3} inputTree=${inputTree4}"
if [ ${execute_runData} -eq 1 ]
then
./runData ${inputTrees} rejectCowboys=${rejectCowboys} FidCuts=${FidCuts} nState=${nState} MC=${MC} RequestTrigger=${RequestTrigger}
fi

if [ ${execute_runChiData} -eq 1 ]
then
./runChiData ${inputTrees} rejectCowboys=${rejectCowboys} FidCuts=${FidCuts} nState=${nState} MC=${MC} RequestTrigger=${RequestTrigger} correctCtau=${correctCtau}
fi

if [ ${execute_runWorkspace} -eq 1 ]
then
./runWorkspace nState=${nState} correctCtau=${correctCtau} drawRapPt2D=${drawRapPt2D}
fi

if [ ${execute_runMassFit} -eq 1 ]
then
cp runMassFit runMassFit_rap${rapMin}_pt${ptMin}
./runMassFit_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} fitMassPR=${fitMassPR} fitMassNP=${fitMassNP} MC=${MC}
rm runMassFit_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_runLifetimeFit} -eq 1 ]
then
cp runLifetimeFit runLifetimeFit_rap${rapMin}_pt${ptMin}
./runLifetimeFit_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} MC=${MC}
rm runLifetimeFit_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_runPlotJpsiMassLifetime} -eq 1 ]
then
cp runPlotJpsiMassLifetime runPlotJpsiMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin}
./runPlotJpsiMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} PlottingJpsi=${PlottingJpsi}
rm runPlotJpsiMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_PlotJpsiFitPar} -eq 1 ]
then
./PlotJpsiFitPar nState=${nState} doCtauUncer=${doCtauUncer} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} rapFixTo=${rapFixTo} ptFixTo=${ptFixTo} AddInclusiveResult=${AddInclusiveResult}
fi

if [ ${execute_runChiMassLifetimeFit} -eq 1 ]
then
cp runChiMassLifetimeFit runChiMassLifetimeFit_rap${rapMin}_pt${ptMin}
./runChiMassLifetimeFit_rap${rapMin}_pt${ptMin} runChiMassFitOnly=${runChiMassFitOnly} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} MC=${MC}
rm runChiMassLifetimeFit_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_runDefineRegionsAndFractions} -eq 1 ]
then
rootfile=fit_Chi_rap${rapMin}_pt${ptMin}.root
#cp tmpFiles/backupWorkSpace/ws_MassLifetimeFit_Chi_rap${rapMin}_pt${ptMin}.root tmpFiles/backupWorkSpace/ws_DefineRegionsAndFractions_Chi_rap${rapMin}_pt${ptMin}.root
cp runDefineRegionsAndFractions runDefineRegionsAndFractions_rap${rapMin}_pt${ptMin}
./runDefineRegionsAndFractions_rap${rapMin}_pt${ptMin} runChiMassFitOnly=${runChiMassFitOnly} doFractionUncer=${doFractionUncer} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} FixRegionsToInclusiveFit=${FixRegionsToInclusiveFit} rapFixTo=${rapFixTo} ptFixTo=${ptFixTo}
rm runDefineRegionsAndFractions_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_runPlotDataDistributions} -eq 1 ]
then
cp runPlotDataDistributions runPlotDataDistributions_$[nState-3]S_rap${rapMin}_pt${ptMin}
mkdir Figures/PlotDataDistributions
./runPlotDataDistributions_$[nState-3]S_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} PlottingDataDists=${PlottingDataDists}
rm runPlotDataDistributions_$[nState-3]S_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_runPlotMassLifetime} -eq 1 ]
then
cp runPlotMassLifetime runPlotMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin}
./runPlotMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} Plotting=${Plotting}
rm runPlotMassLifetime_$[nState-3]S_rap${rapMin}_pt${ptMin}
fi

if [ ${execut_PlotFitPar} -eq 1 ]
then
./PlotFitPar nState=${nState} doCtauUncer=${doCtauUncer} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} rapFixTo=${rapFixTo} ptFixTo=${ptFixTo} AddInclusiveResult=${AddInclusiveResult}
#pdflatex Lifetime_fitParameter.tex
#pdflatex Mass_fitParameter.tex
#pdflatex evaluateCtau.tex
#pdflatex evaluateCtau.tex
#pdflatex NumEvents.tex
#pdflatex NumEvents.tex
#mv Lifetime_fitParameter.pdf PDF/Lifetime_fitParameter.pdf
#mv Mass_fitParameter.pdf PDF/Mass_fitParameter.pdf
#mv evaluateCtau.pdf PDF/evaluateCtau.pdf
#mv NumEvents.pdf PDF/NumEvents.pdf
fi

if [ ${execute_runBkgHistos} -eq 1 ]
then
cp runBkgHistos runBkgHistos_$[nState-3]S_rap${rapMin}_pt${ptMin}
./runBkgHistos_$[nState-3]S_rap${rapMin}_pt${ptMin} rapMin=${rapMin} rapMax=${rapMax} ptMin=${ptMin} ptMax=${ptMax} nState=${nState} MC=${MC} doCtauUncer=${doCtauUncer} PolLSB=${PolLSB} PolRSB=${PolRSB} PolNP=${PolNP} ctauScen=${ctauScen} FracLSB=${FracLSB} forceBinning=${forceBinning} folding=${folding} normApproach=${normApproach} scaleFracBg=${scaleFracBg} ${polDataPath}=polDataPath
rm runBkgHistos_$[nState-3]S_rap${rapMin}_pt${ptMin}
fi

if [ ${execute_PlotCosThetaPhiBG} -eq 1 ]
then
./PlotCosThetaPhiBG nState=${nState}
#pdflatex cosThetaPhi_$[nState-3]S_BG.tex
#pdflatex cosThetaPhi_$[nState-3]S_BG_highct.tex
#pdflatex cosThetaPhi_$[nState-3]S_NPBG.tex
#pdflatex cosThetaPhi_$[nState-3]S_TBG.tex
#mv cosThetaPhi_$[nState-3]S_BG.pdf PDF/cosThetaPhi_$[nState-3]S_BG.pdf
#mv cosThetaPhi_$[nState-3]S_BG_highct.pdf PDF/cosThetaPhi_$[nState-3]S_BG_highct.pdf
#mv cosThetaPhi_$[nState-3]S_NPBG.pdf PDF/cosThetaPhi_$[nState-3]S_NPBG.pdf
#mv cosThetaPhi_$[nState-3]S_TBG.pdf PDF/cosThetaPhi_$[nState-3]S_TBG.pdf
fi

if [ ${execute_PlotCosThetaPhiDistribution} -eq 1 ]
then
./PlotCosThetaPhiDistribution ${nState}nState ${WorkDir}=DataPath
#pdflatex cosThetaPhi_$[nState-3]S.tex
#mv cosThetaPhi_$[nState-3]S.pdf PDF/cosThetaPhi_$[nState-3]S.pdf
fi


#rm runData
#rm runChiData
#rm runWorkspace
#rm runMassFit
#rm runLifetimeFit
#rm runPlotMassLifetime
#rm runBkgHistos
#rm PlotFitPar
#rm PlotCosThetaPhiBG
#rm PlotCosThetaPhiDistribution
#rm *.tex
#rm *.aux
#rm *.log
#rm *.so
#rm *.d
#rm *.nav 
#rm *.out 
#rm *.snm 
#rm *.toc 

done
done

