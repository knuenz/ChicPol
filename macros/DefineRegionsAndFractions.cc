/*
 * DefineRegionsAndFractions.cc
 *
 *  Created on: Jan 30, 2014
 *      Author: valentinknuenz
 */

#include "rootIncludes.inc"
#include "commonVar.h"
#include "RooGenericPdf.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgList.h"

using namespace RooFit;

void DefineRegionsAndFractions(const std::string &infilename, int rapBin, int ptBin, int nState, bool runChiMassFitOnly, bool FixRegionsToInclusiveFit, int rapFixTo, int ptFixTo){

    TFile *infile = new TFile(infilename.c_str(), "UPDATE");
    if(!infile){
        std::cout << "Error: failed to open file with dataset" << std::endl;
    }
    RooWorkspace *ws=(RooWorkspace *)infile->Get("ws_masslifetime");
    if(!ws){
        std::cout << "Error: failed to open workspace " << std::endl;
    }



    RooAbsPdf *fullPdf;
    RooAbsPdf *fullMassPdf;

    fullPdf = (RooAbsPdf*)ws->pdf("ML_fullModel");
    fullMassPdf = (RooAbsPdf*)ws->pdf("M_fullModel");
	RooAbsPdf *Prompt = (RooAbsPdf*)ws->pdf("L_TotalPromptLifetime");



    std::stringstream binName;
    binName << "data_rap" << rapBin << "_pt" << ptBin;
    RooDataSet *data = (RooDataSet*)ws->data(binName.str().c_str());
    double data_ev = data->numEntries();
    std::cout<<"Number of Events in dataset: "<<data_ev<<endl;

    data->Print("v");

    double ev = ws->var("NumEvE")->getVal();
    //double ev = data_ev;


    //-------------------------------- Plotting -----------------------------------------
    std::cout<< "plotting" <<std::endl;

    // get variables
    RooRealVar *mass = (RooRealVar*)ws->var("chicMass");
    RooRealVar *ct = (RooRealVar*)ws->var("Jpsict");
    RooRealVar *ctErr = (RooRealVar*)ws->var("JpsictErr");
    RooRealVar *cbmass1 =  (RooRealVar*)ws->var("CBmass1");
    RooRealVar *cbsigma1 = (RooRealVar*)ws->var("CBsigma1");

    RooAbsReal *cbmass2;
	if(ws->function("CBmass2")==NULL) cbmass2 =  (RooRealVar*)ws->var("CBmass2");
	else cbmass2 =  (RooRealVar*)ws->function("CBmass2");
    RooAbsReal *cbsigma2;
	if(ws->function("CBsigma2")==NULL) cbsigma2 =  (RooRealVar*)ws->var("CBsigma2");
	else cbsigma2 =  (RooRealVar*)ws->function("CBsigma2");

    // calculate mean and sigma for defining regions
    double mean1 = cbmass1->getVal();
    double mean2 = cbmass2->getVal();
    double sigma1 = cbsigma1->getVal();
    double sigma2 = cbsigma2->getVal();

    std::cout<< "mean2" << mean2<<std::endl;
    std::cout<< "sigma2" << sigma2<<std::endl;

    //DEFINE MASS REGIONS
    double sig1MaxMass;//  = mean1+sigma1*onia::nSigChi1MassHigh;
    double sig1MinMass;//  = mean1-sigma1*onia::nSigChi1MassLow;
    double sig2MaxMass;//  = mean2+sigma2*onia::nSigChi2MassHigh;
    double sig2MinMass;//  = mean2-sigma2*onia::nSigChi2MassLow;
    double lsbMaxMass;//   = mean1 - sigma1*onia::nSigChiBkgLow;
    double lsbMinMass;//   = onia::massChiSBMin;
    double rsbMaxMass;//   = onia::massChiSBMax;
    double rsbMinMass;//   = mean2 + sigma2*onia::nSigChiBkgHigh;

    lsbMinMass=onia::massChiSBMin;
    rsbMaxMass=onia::massChiSBMax;



    double PRMinFixTo=0.;
    double PRMaxFixTo=0.;
    double NPMinFixTo=0.;
    double NPMaxFixTo=0.;

    if(!FixRegionsToInclusiveFit){

		RooAbsReal* real_fChic_region;
		double startmass, deltamass, testmass, buffintegral;

		int nM=1e5;
		deltamass=1e-5;

		startmass=mean1;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass-iM*deltamass;
			mass->setRange("testregion", onia::massChiSBMin, testmass);
			real_fChic_region = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(buffintegral<onia::fChi1MassLow) {
				sig1MinMass=testmass+deltamass/2.;
				cout<<"Found sig1MinMass = "<<sig1MinMass<<" after "<<iM<<" evaluations, fChic1 below is "<<buffintegral<<endl;
				break;
			}
		}

		startmass=sig1MinMass;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass-iM*deltamass;
			mass->setRange("testregion", onia::massChiSBMin, testmass);
			real_fChic_region = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(buffintegral<onia::fChiBkgLow) {
				lsbMaxMass=testmass+deltamass/2.;
				cout<<"Found lsbMaxMass = "<<lsbMaxMass<<" after "<<iM<<" evaluations, fChic1 below is "<<buffintegral<<endl;
				break;
			}
		}

		startmass=mean1;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass+iM*deltamass;
			mass->setRange("testregion", testmass, onia::massChiSBMax);
			real_fChic_region = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(buffintegral<onia::fChi1MassHigh) {
				sig1MaxMass=testmass-deltamass/2.;
				cout<<"Found sig1MaxMass = "<<sig1MaxMass<<" after "<<iM<<" evaluations, fChic1 below is "<<buffintegral<<endl;
				break;
			}
		}

		startmass=mean2;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass-iM*deltamass;
			mass->setRange("testregion", onia::massChiSBMin, testmass);
			real_fChic_region = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(testmass<sig1MaxMass) {
				sig2MinMass=sig1MaxMass;
				cout<<"MaxSR1 reached -> adjusting MinSR2: Found sig2MinMass = "<<sig2MinMass<<" after "<<iM<<" evaluations, fChic2 below is "<<buffintegral<<endl;
				break;
			}
			if(buffintegral<onia::fChi2MassLow) {
				sig2MinMass=testmass+deltamass/2.;
				cout<<"Found sig2MinMass = "<<sig2MinMass<<" after "<<iM<<" evaluations, fChic2 below is "<<buffintegral<<endl;
				break;
			}
		}

		startmass=mean2;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass+iM*deltamass;
			mass->setRange("testregion", testmass, onia::massChiSBMax);
			real_fChic_region = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(buffintegral<onia::fChi2MassHigh) {
				sig2MaxMass=testmass-deltamass/2.;
				cout<<"Found sig2MaxMass = "<<sig2MaxMass<<" after "<<iM<<" evaluations, fChic2 below is "<<buffintegral<<endl;
				break;
			}
		}

		startmass=sig2MaxMass;
		for(int iM=0;iM<nM;iM++){
			testmass=startmass+iM*deltamass;
			mass->setRange("testregion", testmass, onia::massChiSBMax);
			real_fChic_region = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
			buffintegral=real_fChic_region->getVal();
			if(buffintegral<onia::fChiBkgHigh) {
				rsbMinMass=testmass-deltamass/2.;
				cout<<"Found rsbMinMass = "<<rsbMinMass<<" after "<<iM<<" evaluations, fChic2 below is "<<buffintegral<<endl;
				break;
			}
		}

    }
    else{

		std::stringstream FixToFile;
		FixToFile << "tmpFiles/backupWorkSpace/ws_DefineRegionsAndFractions_Chi_rap" << rapFixTo << "_pt" << ptFixTo << ".root";
		const std::string infilenameFixTo = FixToFile.str().c_str();

		TFile *infileFixTo = new TFile(infilenameFixTo.c_str(), "UPDATE");
		if(!infileFixTo){
			std::cout << "Error: failed to open FixTo file with dataset" << std::endl;
		}
		RooWorkspace *wsFixTo=(RooWorkspace *)infileFixTo->Get("ws_masslifetime");
		if(!wsFixTo){
			std::cout << "Error: failed to open FixTo workspace " << std::endl;
		}

	    sig1MaxMass=wsFixTo->var("var_sig1MaxMass")->getVal();
	    sig1MinMass=wsFixTo->var("var_sig1MinMass")->getVal();
	    sig2MaxMass=wsFixTo->var("var_sig2MaxMass")->getVal();
	    sig2MinMass=wsFixTo->var("var_sig2MinMass")->getVal();
	    lsbMaxMass=wsFixTo->var("var_lsbMaxMass")->getVal();
	    rsbMinMass=wsFixTo->var("var_rsbMinMass")->getVal();

	    PRMinFixTo=wsFixTo->var("var_PRMin")->getVal();
	    PRMaxFixTo=wsFixTo->var("var_PRMax")->getVal();
	    NPMinFixTo=wsFixTo->var("var_NPMin")->getVal();
	    NPMaxFixTo=wsFixTo->var("var_NPMax")->getVal();

    }

    //if( sig2MinMass<sig1MaxMass ){
  	//  cout<<"adjusting lower border of SR2 due to overlap with SR1"<<endl;
  	//  //sig2MinMass=(sigma2*onia::nSigChi2MassLow*mean1+sigma1*onia::nSigChi1MassHigh*mean2)/(sigma1*onia::nSigChi1MassHigh+sigma2*onia::nSigChi2MassLow);
  	//  sig2MinMass=sig1MaxMass;
    //  real_fChic_region = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("testregion"));
    //  buffintegral=real_fChic_region->getVal();
  	//  cout<<"fChic2 below is "<<buffintegral<<endl;
    //}


    mass->setRange("SR1", sig1MinMass, sig1MaxMass);
    mass->setRange("SR2", sig2MinMass, sig2MaxMass);
    mass->setRange("LSB", lsbMinMass, lsbMaxMass);
    mass->setRange("RSB", rsbMinMass, rsbMaxMass);

    ws->var("chicMass")->setRange("SR1", sig1MinMass, sig1MaxMass);
    ws->var("chicMass")->setRange("SR2", sig2MinMass, sig2MaxMass);
    ws->var("chicMass")->setRange("LSB", lsbMinMass, lsbMaxMass);
    ws->var("chicMass")->setRange("RSB", rsbMinMass, rsbMaxMass);

    //DEFINE LIFETIME REGIONS
    //TODO: define regions

    //Calculate sigma of ctau from resolution function

    std::stringstream cutSR;
    cutSR << "chicMass > " << sig1MinMass << " && chicMass < " << sig2MaxMass;

    RooAbsData* dataSR = data->reduce(Cut(cutSR.str().c_str()));

	RooDataSet *dataJpsictErr = (RooDataSet*)dataSR->reduce(SelectVars(RooArgSet(*ctErr)),Name("dataJpsictErr"));




	RooDataSet *PromptPseudoData = Prompt->generate(*ct,ProtoData(*dataJpsictErr));

	int nbinsSigmaDef=200;
	//TH2F* hist2D = (TH2F*)PromptPseudoData->createHistogram("hist2D",*ct,Binning(nbinsSigmaDef),YVar(*ctErr,Binning(nbinsSigmaDef)));
	//TH1F* hist = (TH1F*)hist2D->ProjectionX();
	TH1F* hist1D = (TH1F*)PromptPseudoData->createHistogram("hist1D",*ct,Binning(nbinsSigmaDef));

	//hist->Scale(1./hist->Integral());
	//hist->SetLineColor(kRed);
	//hist->SetMarkerColor(kRed);
	//hist->GetXaxis()->SetLimits(-.2,.2);
	hist1D->Scale(1./hist1D->Integral());
	hist1D->SetLineColor(kRed);
	hist1D->SetMarkerColor(kRed);
	hist1D->GetXaxis()->SetLimits(-.2,.2);

	double ctres = hist1D->GetRMS();
	double err_ctres = hist1D->GetRMSError();

    RooRealVar var_ctres("var_ctres","var_ctres",ctres); var_ctres.setError(err_ctres); if(!ws->var("var_ctres")) ws->import(var_ctres); else{ ws->var("var_ctres")->setVal(ctres); ws->var("var_ctres")->setError(err_ctres); }
    RooRealVar var_err_ctres("var_err_ctres","var_err_ctres",err_ctres); if(!ws->var("var_err_ctres")) ws->import(var_err_ctres); else ws->var("var_err_ctres")->setVal(err_ctres);

	cout<<"ctau resolution   = "<<ctres*1000<<" +- "<<err_ctres*1000<<" micron"<<endl;

	// define sigma of prompt p.d.f., got from fit the trend
	// define function y = a + b * pT
	double a = 0.073, b = 0.0027;
	//proper decay length
	double L_decay = a + b * ws->var("var_chicMeanPt")->getVal();
	//pseudo-proper decay length
	double l_sigma = L_decay * onia::MpsiPDG / ws->var("var_chicMeanPt")->getVal() ;

	cout<<"   ->  "<<ctres/l_sigma<<" with respect to 2011 definition"<<endl;

    double PRMin=-onia::nSigLifetimePR*ctres;
    double PRMax=onia::nSigLifetimePR*ctres;
    double NPMin=onia::nSigLifetimeNP*ctres;
    double NPMax=onia::ctVarMax;

    if(FixRegionsToInclusiveFit){

    	PRMin=PRMinFixTo;
    	PRMax=PRMaxFixTo;
    	NPMin=NPMinFixTo;
    	NPMax=NPMaxFixTo;

    }

    ct->setRange("PR", PRMin, PRMax);
    ct->setRange("NP", NPMin, NPMax);

    ws->var("Jpsict")->setRange("PR", PRMin, PRMax);
    ws->var("Jpsict")->setRange("NP", NPMin, NPMax);








    bool debugInt=false;

    if(debugInt){


	RooFitResult* fitRlt = dynamic_cast<RooFitResult*>(ws->obj(Form("fitresult_rap%d_pt%d",rapBin,ptBin)));
	assert ( 0 != fitRlt);


    RooAbsReal* real_fPRTESTChicInPR = ws->pdf("L_TotalPromptLifetime")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fPRTESTChicInNP = ws->pdf("L_TotalPromptLifetime")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fPRTESTChicInPR=real_fPRTESTChicInPR->getVal();
    double fPRTESTChicInNP=real_fPRTESTChicInNP->getVal();
    cout<<"PRinPR ws->pdf->createIntegral    = "<<fPRTESTChicInPR   <<endl;
    cout<<"PRinNP ws->pdf->createIntegral    = "<<fPRTESTChicInNP   <<endl;
    RooAbsReal* real_fPRTESTChicInTESTPR = Prompt->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fPRTESTChicInTESTNP = Prompt->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fPRTESTChicInTESTPR=real_fPRTESTChicInTESTPR->getVal();
    double fPRTESTChicInTESTNP=real_fPRTESTChicInTESTNP->getVal();
    cout<<"PRinPR pdf->createIntegral = "<<fPRTESTChicInPR   <<endl;
    cout<<"PRinNP pdf->createIntegral = "<<fPRTESTChicInNP   <<endl;

	TH1F* histTEST = (TH1F*)Prompt->createHistogram("histTEST",*ct,Binning(10000000));
	int PRbinmin=histTEST->FindBin(PRMin);
	int PRbinmax=histTEST->FindBin(PRMax);
	int NPbinmax=histTEST->FindBin(NPMax);
	histTEST->Scale(1./histTEST->Integral());
	cout<<"PRinPR pdf->createHistogram->Integral = "<<histTEST->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->createHistogram->Integral = "<<histTEST->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *PromptPseudoData4 = Prompt->generate(*ct,1000000);

	TH1F* histTEST4 = (TH1F*)PromptPseudoData4->createHistogram("histTEST4",*ct,Binning(10000000));
	PRbinmin=histTEST4->FindBin(PRMin);
	PRbinmax=histTEST4->FindBin(PRMax);
	NPbinmax=histTEST4->FindBin(NPMax);
	histTEST4->Scale(1./histTEST4->Integral());
	cout<<"PRinPR pdf->generate(noData)->createHistogram->Integral = "<<histTEST4->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(noData)->createHistogram->Integral = "<<histTEST4->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *dataJpsictErrAll = (RooDataSet*)data->reduce(SelectVars(RooArgSet(*ctErr)),Name("dataJpsictErr"));
	RooDataSet *PromptPseudoData2 = Prompt->generate(*ct,1000000,ProtoData(*dataJpsictErrAll, true));

	TH1F* histTEST2 = (TH1F*)PromptPseudoData2->createHistogram("histTEST2",*ct,Binning(10000));
	PRbinmin=histTEST2->FindBin(PRMin);
	PRbinmax=histTEST2->FindBin(PRMax);
	NPbinmax=histTEST2->FindBin(NPMax);
	histTEST2->Scale(1./histTEST2->Integral());
	cout<<"PRinPR pdf->generate(ctErrAlldata)->createHistogram->Integral = "<<histTEST2->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrAlldata)->createHistogram->Integral = "<<histTEST2->Integral(PRbinmax, NPbinmax)<<endl;

	TH2F* histTEST2D2 = (TH2F*)PromptPseudoData2->createHistogram("histTEST2D2",*ct,Binning(10000),YVar(*ctErr,Binning(10000)));
	TH1F* histTEST2D1D2 = (TH1F*)histTEST2D2->ProjectionX();
	PRbinmin=histTEST2D1D2->FindBin(PRMin);
	PRbinmax=histTEST2D1D2->FindBin(PRMax);
	NPbinmax=histTEST2D1D2->FindBin(NPMax);
	histTEST2D1D2->Scale(1./histTEST2D1D2->Integral());
	cout<<"PRinPR pdf->generate(ctErrAlldata)->createHistogram2D->1D->Integral = "<<histTEST2D1D2->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrAlldata)->createHistogram2D->1D->Integral = "<<histTEST2D1D2->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *PromptPseudoData9 = ws->pdf("L_TotalPromptLifetime")->generate(*ct,1000000,ProtoData(*dataJpsictErrAll, true));

	TH1F* histTEST9 = (TH1F*)PromptPseudoData9->createHistogram("histTEST9",*ct,Binning(10000));
	PRbinmin=histTEST9->FindBin(PRMin);
	PRbinmax=histTEST9->FindBin(PRMax);
	NPbinmax=histTEST9->FindBin(NPMax);
	histTEST9->Scale(1./histTEST9->Integral());
	cout<<"PRinPR ws->pdf->generate(ctErrAlldata)->createHistogram->Integral = "<<histTEST9->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP ws->pdf->generate(ctErrAlldata)->createHistogram->Integral = "<<histTEST9->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *dataJpsictErrAll08 = (RooDataSet*)data->reduce(SelectVars(RooArgSet(*ctErr)),Name("dataJpsictErr"), Cut("TMath::Abs(Jpsict)<0.2"));
	RooDataSet *PromptPseudoData8 = Prompt->generate(*ct,1000000,ProtoData(*dataJpsictErrAll08, true));

	TH1F* histTEST8 = (TH1F*)PromptPseudoData8->createHistogram("histTEST8",*ct,Binning(10000));
	PRbinmin=histTEST8->FindBin(PRMin);
	PRbinmax=histTEST8->FindBin(PRMax);
	NPbinmax=histTEST8->FindBin(NPMax);
	histTEST8->Scale(1./histTEST8->Integral());
	cout<<"PRinPR pdf->generate(ctErrAlldata02)->createHistogram->Integral = "<<histTEST8->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrAlldata02)->createHistogram->Integral = "<<histTEST8->Integral(PRbinmax, NPbinmax)<<endl;


	RooDataSet *PromptPseudoData3 = Prompt->generate(*ct,1000000,ProtoData(*dataJpsictErr, true));

	TH1F* histTEST3 = (TH1F*)PromptPseudoData3->createHistogram("histTEST3",*ct,Binning(10000000));
	PRbinmin=histTEST3->FindBin(PRMin);
	PRbinmax=histTEST3->FindBin(PRMax);
	NPbinmax=histTEST3->FindBin(NPMax);
	histTEST3->Scale(1./histTEST3->Integral());
	cout<<"PRinPR pdf->generate(ctErrSRdata)->createHistogram->Integral = "<<histTEST3->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrSRdata)->createHistogram->Integral = "<<histTEST3->Integral(PRbinmax, NPbinmax)<<endl;


	RooDataSet *dataJpsictErrAllLargeError = (RooDataSet*)data->reduce(SelectVars(RooArgSet(*ctErr)),Name("dataJpsictErr"),Cut("JpsictErr>0.03"));
	RooDataSet *PromptPseudoData5 = Prompt->generate(*ct,1000000,ProtoData(*dataJpsictErrAllLargeError, true));

	TH1F* histTEST5 = (TH1F*)PromptPseudoData5->createHistogram("histTEST5",*ct,Binning(10000000));
	PRbinmin=histTEST5->FindBin(PRMin);
	PRbinmax=histTEST5->FindBin(PRMax);
	NPbinmax=histTEST5->FindBin(NPMax);
	histTEST5->Scale(1./histTEST5->Integral());
	cout<<"PRinPR pdf->generate(ctErrAlldataLargeError)->createHistogram->Integral = "<<histTEST5->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrAlldataLargeError)->createHistogram->Integral = "<<histTEST5->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *dataJpsictErrAllSmallError = (RooDataSet*)data->reduce(SelectVars(RooArgSet(*ctErr)),Name("dataJpsictErr"),Cut("JpsictErr<0.0125"));
	RooDataSet *PromptPseudoData6 = Prompt->generate(*ct,1000000,ProtoData(*dataJpsictErrAllSmallError, true));

	TH1F* histTEST6 = (TH1F*)PromptPseudoData6->createHistogram("histTEST6",*ct,Binning(10000000));
	PRbinmin=histTEST6->FindBin(PRMin);
	PRbinmax=histTEST6->FindBin(PRMax);
	NPbinmax=histTEST6->FindBin(NPMax);
	histTEST6->Scale(1./histTEST6->Integral());
	cout<<"PRinPR pdf->generate(ctErrAlldataSmallError)->createHistogram->Integral = "<<histTEST6->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(ctErrAlldataSmallError)->createHistogram->Integral = "<<histTEST6->Integral(PRbinmax, NPbinmax)<<endl;

	RooDataSet *datamassAll = (RooDataSet*)data->reduce(SelectVars(RooArgSet(*mass)),Name("dataJpsictErr"));
	RooDataSet *PromptPseudoData7 = Prompt->generate(*ct,1000000,ProtoData(*datamassAll, true));

	TH1F* histTEST7 = (TH1F*)PromptPseudoData7->createHistogram("histTEST7",*ct,Binning(10000000));
	PRbinmin=histTEST7->FindBin(PRMin);
	PRbinmax=histTEST7->FindBin(PRMax);
	NPbinmax=histTEST7->FindBin(NPMax);
	histTEST7->Scale(1./histTEST7->Integral());
	cout<<"PRinPR pdf->generate(masAlldata)->createHistogram->Integral = "<<histTEST7->Integral(PRbinmin, PRbinmax)<<endl;
	cout<<"PRinNP pdf->generate(masAlldata)->createHistogram->Integral = "<<histTEST7->Integral(PRbinmax, NPbinmax)<<endl;





    }











    std::cout << "-------------- mass range --------------\n"
            << "signal range chi1: " << sig1MinMass << " - " << sig1MaxMass << "\n"
            << "mean1: " << mean1 << ", sigma1: " << sigma1 << "\n"
            << "signal range chi2: " << sig2MinMass << " - " << sig2MaxMass << "\n"
            << "mean2: " << mean2 << ", sigma2: " << sigma2 << "\n"
            << "LSB max: " << lsbMaxMass << ", RSB min: " << rsbMinMass << "\n"
            << "----------------------------------------" << std::endl;

    std::cout << "-------------- lifetime range --------------\n"
            << "l_sigma = " << ctres*1000 << " micron\n"
            << "PR: " << PRMin << " - " << PRMax << "mm \n"
            << "NP: " << NPMin << " - " << NPMax << "mm \n"
             << "----------------------------------------" << std::endl;

    // define data in different regions
    std::stringstream cutSR1, cutSR2, cutLSB, cutRSB;
    cutSR1 << "chicMass > " << sig1MinMass << " && chicMass < " << sig1MaxMass;
    cutSR2 << "chicMass > " << sig2MinMass << " && chicMass < " << sig2MaxMass;
    cutLSB << "chicMass > " << onia::massChiSBMin << " && chicMass < " << lsbMaxMass;
    cutRSB << "chicMass > " << rsbMinMass << " && chicMass < " << onia::massChiSBMax;

    std::stringstream binNameSR1, binNameSR2, binNameLSB, binNameRSB;
    binNameSR1  << "data_rap" << rapBin << "_pt" << ptBin << "_SR1";
    binNameSR2  << "data_rap" << rapBin << "_pt" << ptBin << "_SR2";
    binNameLSB << "data_rap" << rapBin << "_pt" << ptBin << "_LSB";
    binNameRSB << "data_rap" << rapBin << "_pt" << ptBin << "_RSB";

    RooAbsData* dataSR1 = data->reduce(Cut(cutSR1.str().c_str()));
    RooAbsData* dataSR2 = data->reduce(Cut(cutSR2.str().c_str()));
    RooAbsData* dataLSB = data->reduce(Cut(cutLSB.str().c_str()));
    RooAbsData* dataRSB = data->reduce(Cut(cutRSB.str().c_str()));



    double nBackground=ev*ws->var("fracBackground")->getVal();
    double nChic=ev-nBackground;
    double nChic0=nChic*ws->var("fracSignal_chic0")->getVal();
    double nChic1=nChic*ws->var("fracSignal_chic1")->getVal();
    double nChic2=nChic-nChic0-nChic1;
    double nChic0NP=nChic0*ws->var("fracNP_chic0")->getVal();
    double nChic0PR=nChic0-nChic0NP;
    double nChic1NP=nChic1*ws->var("fracNP_chic1")->getVal();
    double nChic1PR=nChic1-nChic1NP;
    double nChic2NP=nChic2*ws->var("fracNP_chic2")->getVal();
    double nChic2PR=nChic2-nChic2NP;


    cout<<"FULL MASS REGION:"<<endl;
    cout<<"Total number of events = "<<ev<<endl;
    cout<<"nBackground = "<<nBackground<<endl;
    cout<<"nChic       = "<<nChic      <<endl;
    cout<<"nChic0      = "<<nChic0     <<endl;
    cout<<"nChic1      = "<<nChic1     <<endl;
    cout<<"nChic2      = "<<nChic2     <<endl;
    cout<<"nChic0NP    = "<<nChic0NP   <<endl;
    cout<<"nChic0PR    = "<<nChic0PR   <<endl;
    cout<<"nChic1NP    = "<<nChic1NP   <<endl;
    cout<<"nChic1PR    = "<<nChic1PR   <<endl;
    cout<<"nChic2NP    = "<<nChic2NP   <<endl;
    cout<<"nChic2PR    = "<<nChic2PR   <<endl;



    cout<<"FRACTIONS OF INDIVIDUAL COMPONENT-PDFs IN EACH MASS REGION"<<endl;

    RooAbsReal* real_fTotInLSB = fullMassPdf->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("LSB"));
    RooAbsReal* real_fTotInRSB = fullMassPdf->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("RSB"));
    RooAbsReal* real_fTotInSR1 = fullMassPdf->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR1"));
    RooAbsReal* real_fTotInSR2 = fullMassPdf->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR2"));
    double fTotInLSB=real_fTotInLSB->getVal();
    double fTotInRSB=real_fTotInRSB->getVal();
    double fTotInSR1=real_fTotInSR1->getVal();
    double fTotInSR2=real_fTotInSR2->getVal();
    cout<<"fTotInLSB    = "<<fTotInLSB   <<endl;
    cout<<"fTotInRSB    = "<<fTotInRSB   <<endl;
    cout<<"fTotInSR1    = "<<fTotInSR1   <<endl;
    cout<<"fTotInSR2    = "<<fTotInSR2   <<endl;

    RooAbsReal* real_fBackgroundInLSB = ws->pdf("M_background")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("LSB"));
    RooAbsReal* real_fBackgroundInRSB = ws->pdf("M_background")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("RSB"));
    RooAbsReal* real_fBackgroundInSR1 = ws->pdf("M_background")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR1"));
    RooAbsReal* real_fBackgroundInSR2 = ws->pdf("M_background")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR2"));
    double fBackgroundInLSB=real_fBackgroundInLSB->getVal();
    double fBackgroundInRSB=real_fBackgroundInRSB->getVal();
    double fBackgroundInSR1=real_fBackgroundInSR1->getVal();
    double fBackgroundInSR2=real_fBackgroundInSR2->getVal();
    cout<<"fBackgroundInLSB    = "<<fBackgroundInLSB   <<endl;
    cout<<"fBackgroundInRSB    = "<<fBackgroundInRSB   <<endl;
    cout<<"fBackgroundInSR1    = "<<fBackgroundInSR1   <<endl;
    cout<<"fBackgroundInSR2    = "<<fBackgroundInSR2   <<endl;

    RooAbsReal* real_fChic0InLSB = ws->pdf("M_chic0")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("LSB"));
    RooAbsReal* real_fChic0InRSB = ws->pdf("M_chic0")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("RSB"));
    RooAbsReal* real_fChic0InSR1 = ws->pdf("M_chic0")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR1"));
    RooAbsReal* real_fChic0InSR2 = ws->pdf("M_chic0")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR2"));
    double fChic0InLSB=real_fChic0InLSB->getVal();
    double fChic0InRSB=real_fChic0InRSB->getVal();
    double fChic0InSR1=real_fChic0InSR1->getVal();
    double fChic0InSR2=real_fChic0InSR2->getVal();
    cout<<"fChic0InLSB    = "<<fChic0InLSB   <<endl;
    cout<<"fChic0InRSB    = "<<fChic0InRSB   <<endl;
    cout<<"fChic0InSR1    = "<<fChic0InSR1   <<endl;
    cout<<"fChic0InSR2    = "<<fChic0InSR2   <<endl;

    RooAbsReal* real_fChic1InLSB = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("LSB"));
    RooAbsReal* real_fChic1InRSB = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("RSB"));
    RooAbsReal* real_fChic1InSR1 = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR1"));
    RooAbsReal* real_fChic1InSR2 = ws->pdf("M_chic1")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR2"));
    double fChic1InLSB=real_fChic1InLSB->getVal();
    double fChic1InRSB=real_fChic1InRSB->getVal();
    double fChic1InSR1=real_fChic1InSR1->getVal();
    double fChic1InSR2=real_fChic1InSR2->getVal();
    cout<<"fChic1InLSB    = "<<fChic1InLSB   <<endl;
    cout<<"fChic1InRSB    = "<<fChic1InRSB   <<endl;
    cout<<"fChic1InSR1    = "<<fChic1InSR1   <<endl;
    cout<<"fChic1InSR2    = "<<fChic1InSR2   <<endl;

    RooAbsReal* real_fChic2InLSB = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("LSB"));
    RooAbsReal* real_fChic2InRSB = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("RSB"));
    RooAbsReal* real_fChic2InSR1 = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR1"));
    RooAbsReal* real_fChic2InSR2 = ws->pdf("M_chic2")->createIntegral(RooArgSet(*mass), NormSet(RooArgSet(*mass)), Range("SR2"));
    double fChic2InLSB=real_fChic2InLSB->getVal();
    double fChic2InRSB=real_fChic2InRSB->getVal();
    double fChic2InSR1=real_fChic2InSR1->getVal();
    double fChic2InSR2=real_fChic2InSR2->getVal();
    cout<<"fChic2InLSB    = "<<fChic2InLSB   <<endl;
    cout<<"fChic2InRSB    = "<<fChic2InRSB   <<endl;
    cout<<"fChic2InSR1    = "<<fChic2InSR1   <<endl;
    cout<<"fChic2InSR2    = "<<fChic2InSR2   <<endl;

    cout<<"BACKGROUND FRACTIONS"<<endl;

    double fracBackgroundInLSB=nBackground*fBackgroundInLSB / (nBackground*fBackgroundInLSB + nChic0*fChic0InLSB + nChic1*fChic1InLSB + nChic2*fChic2InLSB);
    double fracBackgroundInRSB=nBackground*fBackgroundInRSB / (nBackground*fBackgroundInRSB + nChic0*fChic0InRSB + nChic1*fChic1InRSB + nChic2*fChic2InRSB);
    double fracBackgroundInSR1=nBackground*fBackgroundInSR1 / (nBackground*fBackgroundInSR1 + nChic0*fChic0InSR1 + nChic1*fChic1InSR1 + nChic2*fChic2InSR1);
    double fracBackgroundInSR2=nBackground*fBackgroundInSR2 / (nBackground*fBackgroundInSR2 + nChic0*fChic0InSR2 + nChic1*fChic1InSR2 + nChic2*fChic2InSR2);
    cout<<"fracBackgroundInLSB    = "<<fracBackgroundInLSB   <<endl;
    cout<<"fracBackgroundInRSB    = "<<fracBackgroundInRSB   <<endl;
    cout<<"fracBackgroundInSR1    = "<<fracBackgroundInSR1   <<endl;
    cout<<"fracBackgroundInSR2    = "<<fracBackgroundInSR2   <<endl;

    cout<<"SIGNAL CONTAMINATIONS"<<endl;

    double fracChic0InLSB=nChic0*fChic0InLSB / (nBackground*fBackgroundInLSB + nChic0*fChic0InLSB + nChic1*fChic1InLSB + nChic2*fChic2InLSB);
    double fracChic0InRSB=nChic0*fChic0InRSB / (nBackground*fBackgroundInRSB + nChic0*fChic0InRSB + nChic1*fChic1InRSB + nChic2*fChic2InRSB);
    double fracChic0InSR1=nChic0*fChic0InSR1 / (nBackground*fBackgroundInSR1 + nChic0*fChic0InSR1 + nChic1*fChic1InSR1 + nChic2*fChic2InSR1);
    double fracChic0InSR2=nChic0*fChic0InSR2 / (nBackground*fBackgroundInSR2 + nChic0*fChic0InSR2 + nChic1*fChic1InSR2 + nChic2*fChic2InSR2);
    cout<<"fracChic0InLSB    = "<<fracChic0InLSB   <<endl;
    cout<<"fracChic0InRSB    = "<<fracChic0InRSB   <<endl;
    cout<<"fracChic0InSR1    = "<<fracChic0InSR1   <<endl;
    cout<<"fracChic0InSR2    = "<<fracChic0InSR2   <<endl;

    double fracChic1InLSB=nChic1*fChic1InLSB / (nBackground*fBackgroundInLSB + nChic0*fChic0InLSB + nChic1*fChic1InLSB + nChic2*fChic2InLSB);
    double fracChic1InRSB=nChic1*fChic1InRSB / (nBackground*fBackgroundInRSB + nChic0*fChic0InRSB + nChic1*fChic1InRSB + nChic2*fChic2InRSB);
    double fracChic1InSR1=nChic1*fChic1InSR1 / (nBackground*fBackgroundInSR1 + nChic0*fChic0InSR1 + nChic1*fChic1InSR1 + nChic2*fChic2InSR1);
    double fracChic1InSR2=nChic1*fChic1InSR2 / (nBackground*fBackgroundInSR2 + nChic0*fChic0InSR2 + nChic1*fChic1InSR2 + nChic2*fChic2InSR2);
    cout<<"fracChic1InLSB    = "<<fracChic1InLSB   <<endl;
    cout<<"fracChic1InRSB    = "<<fracChic1InRSB   <<endl;
    cout<<"fracChic1InSR1    = "<<fracChic1InSR1   <<endl;
    cout<<"fracChic1InSR2    = "<<fracChic1InSR2   <<endl;

    double fracChic2InLSB=nChic2*fChic2InLSB / (nBackground*fBackgroundInLSB + nChic0*fChic0InLSB + nChic1*fChic1InLSB + nChic2*fChic2InLSB);
    double fracChic2InRSB=nChic2*fChic2InRSB / (nBackground*fBackgroundInRSB + nChic0*fChic0InRSB + nChic1*fChic1InRSB + nChic2*fChic2InRSB);
    double fracChic2InSR1=nChic2*fChic2InSR1 / (nBackground*fBackgroundInSR1 + nChic0*fChic0InSR1 + nChic1*fChic1InSR1 + nChic2*fChic2InSR1);
    double fracChic2InSR2=nChic2*fChic2InSR2 / (nBackground*fBackgroundInSR2 + nChic0*fChic0InSR2 + nChic1*fChic1InSR2 + nChic2*fChic2InSR2);
    cout<<"fracChic2InLSB    = "<<fracChic2InLSB   <<endl;
    cout<<"fracChic2InRSB    = "<<fracChic2InRSB   <<endl;
    cout<<"fracChic2InSR1    = "<<fracChic2InSR1   <<endl;
    cout<<"fracChic2InSR2    = "<<fracChic2InSR2   <<endl;



    cout<<"FRACTIONS OF INDIVIDUAL COMPONENT-PDFs IN EACH LIFETIME REGION"<<endl;

    RooAbsReal* real_fBackgroundInPR = ws->pdf("L_background")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fBackgroundInNP = ws->pdf("L_background")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fBackgroundInPR=real_fBackgroundInPR->getVal();
    double fBackgroundInNP=real_fBackgroundInNP->getVal();
    cout<<"fBackgroundInPR    = "<<fBackgroundInPR   <<endl;
    cout<<"fBackgroundInNP    = "<<fBackgroundInNP   <<endl;

    RooAbsReal* real_fPRChicInPR = ws->pdf("L_TotalPromptLifetime")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fPRChicInNP = ws->pdf("L_TotalPromptLifetime")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fPRChicInPR=real_fPRChicInPR->getVal();
    double fPRChicInNP=real_fPRChicInNP->getVal();
    cout<<"fPRChicInPR    = "<<fPRChicInPR   <<endl;
    cout<<"fPRChicInNP    = "<<fPRChicInNP   <<endl;

    RooAbsReal* real_fNPChic0InPR = ws->pdf("L_chic0_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fNPChic0InNP = ws->pdf("L_chic0_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPChic0InPR=real_fNPChic0InPR->getVal();
    double fNPChic0InNP=real_fNPChic0InNP->getVal();
    cout<<"fNPChic0InPR    = "<<fNPChic0InPR   <<endl;
    cout<<"fNPChic0InNP    = "<<fNPChic0InNP   <<endl;

    RooAbsReal* real_fNPChic1InPR = ws->pdf("L_chic1_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fNPChic1InNP = ws->pdf("L_chic1_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPChic1InPR=real_fNPChic1InPR->getVal();
    double fNPChic1InNP=real_fNPChic1InNP->getVal();
    cout<<"fNPChic1InPR    = "<<fNPChic1InPR   <<endl;
    cout<<"fNPChic1InNP    = "<<fNPChic1InNP   <<endl;

    RooAbsReal* real_fNPChic2InPR = ws->pdf("L_chic2_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    RooAbsReal* real_fNPChic2InNP = ws->pdf("L_chic2_NP")->createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPChic2InPR=real_fNPChic2InPR->getVal();
    double fNPChic2InNP=real_fNPChic2InNP->getVal();
    cout<<"fNPChic2InPR    = "<<fNPChic2InPR   <<endl;
    cout<<"fNPChic2InNP    = "<<fNPChic2InNP   <<endl;



    cout<<"FRACTIONS OF INDIVIDUAL COMPONENTS IN EACH MASS-LIFETIME REGION"<<endl;

    double nPRChic0InPRLSB = nChic0PR*fChic0InLSB*fPRChicInPR;
    double nPRChic0InPRRSB = nChic0PR*fChic0InRSB*fPRChicInPR;
    double nPRChic0InPRSR1 = nChic0PR*fChic0InSR1*fPRChicInPR;
    double nPRChic0InPRSR2 = nChic0PR*fChic0InSR2*fPRChicInPR;
    double nPRChic0InNPLSB = nChic0PR*fChic0InLSB*fPRChicInNP;
    double nPRChic0InNPRSB = nChic0PR*fChic0InRSB*fPRChicInNP;
    double nPRChic0InNPSR1 = nChic0PR*fChic0InSR1*fPRChicInNP;
    double nPRChic0InNPSR2 = nChic0PR*fChic0InSR2*fPRChicInNP;

    double nNPChic0InPRLSB = nChic0NP*fChic0InLSB*fNPChic0InPR;
    double nNPChic0InPRRSB = nChic0NP*fChic0InRSB*fNPChic0InPR;
    double nNPChic0InPRSR1 = nChic0NP*fChic0InSR1*fNPChic0InPR;
    double nNPChic0InPRSR2 = nChic0NP*fChic0InSR2*fNPChic0InPR;
    double nNPChic0InNPLSB = nChic0NP*fChic0InLSB*fNPChic0InNP;
    double nNPChic0InNPRSB = nChic0NP*fChic0InRSB*fNPChic0InNP;
    double nNPChic0InNPSR1 = nChic0NP*fChic0InSR1*fNPChic0InNP;
    double nNPChic0InNPSR2 = nChic0NP*fChic0InSR2*fNPChic0InNP;

    double nPRChic1InPRLSB = nChic1PR*fChic1InLSB*fPRChicInPR;
    double nPRChic1InPRRSB = nChic1PR*fChic1InRSB*fPRChicInPR;
    double nPRChic1InPRSR1 = nChic1PR*fChic1InSR1*fPRChicInPR;
    double nPRChic1InPRSR2 = nChic1PR*fChic1InSR2*fPRChicInPR;
    double nPRChic1InNPLSB = nChic1PR*fChic1InLSB*fPRChicInNP;
    double nPRChic1InNPRSB = nChic1PR*fChic1InRSB*fPRChicInNP;
    double nPRChic1InNPSR1 = nChic1PR*fChic1InSR1*fPRChicInNP;
    double nPRChic1InNPSR2 = nChic1PR*fChic1InSR2*fPRChicInNP;

    double nNPChic1InPRLSB = nChic1NP*fChic1InLSB*fNPChic1InPR;
    double nNPChic1InPRRSB = nChic1NP*fChic1InRSB*fNPChic1InPR;
    double nNPChic1InPRSR1 = nChic1NP*fChic1InSR1*fNPChic1InPR;
    double nNPChic1InPRSR2 = nChic1NP*fChic1InSR2*fNPChic1InPR;
    double nNPChic1InNPLSB = nChic1NP*fChic1InLSB*fNPChic1InNP;
    double nNPChic1InNPRSB = nChic1NP*fChic1InRSB*fNPChic1InNP;
    double nNPChic1InNPSR1 = nChic1NP*fChic1InSR1*fNPChic1InNP;
    double nNPChic1InNPSR2 = nChic1NP*fChic1InSR2*fNPChic1InNP;

    double nPRChic2InPRLSB = nChic2PR*fChic2InLSB*fPRChicInPR;
    double nPRChic2InPRRSB = nChic2PR*fChic2InRSB*fPRChicInPR;
    double nPRChic2InPRSR1 = nChic2PR*fChic2InSR1*fPRChicInPR;
    double nPRChic2InPRSR2 = nChic2PR*fChic2InSR2*fPRChicInPR;
    double nPRChic2InNPLSB = nChic2PR*fChic2InLSB*fPRChicInNP;
    double nPRChic2InNPRSB = nChic2PR*fChic2InRSB*fPRChicInNP;
    double nPRChic2InNPSR1 = nChic2PR*fChic2InSR1*fPRChicInNP;
    double nPRChic2InNPSR2 = nChic2PR*fChic2InSR2*fPRChicInNP;

    double nNPChic2InPRLSB = nChic2NP*fChic2InLSB*fNPChic2InPR;
    double nNPChic2InPRRSB = nChic2NP*fChic2InRSB*fNPChic2InPR;
    double nNPChic2InPRSR1 = nChic2NP*fChic2InSR1*fNPChic2InPR;
    double nNPChic2InPRSR2 = nChic2NP*fChic2InSR2*fNPChic2InPR;
    double nNPChic2InNPLSB = nChic2NP*fChic2InLSB*fNPChic2InNP;
    double nNPChic2InNPRSB = nChic2NP*fChic2InRSB*fNPChic2InNP;
    double nNPChic2InNPSR1 = nChic2NP*fChic2InSR1*fNPChic2InNP;
    double nNPChic2InNPSR2 = nChic2NP*fChic2InSR2*fNPChic2InNP;

    double nBackgroundInPRLSB = nBackground*fBackgroundInLSB*fBackgroundInPR;
    double nBackgroundInPRRSB = nBackground*fBackgroundInRSB*fBackgroundInPR;
    double nBackgroundInPRSR1 = nBackground*fBackgroundInSR1*fBackgroundInPR;
    double nBackgroundInPRSR2 = nBackground*fBackgroundInSR2*fBackgroundInPR;
    double nBackgroundInNPLSB = nBackground*fBackgroundInLSB*fBackgroundInNP;
    double nBackgroundInNPRSB = nBackground*fBackgroundInRSB*fBackgroundInNP;
    double nBackgroundInNPSR1 = nBackground*fBackgroundInSR1*fBackgroundInNP;
    double nBackgroundInNPSR2 = nBackground*fBackgroundInSR2*fBackgroundInNP;

    double nPRChic1InPRSR1_Err = TMath::Sqrt(nPRChic1InPRSR1);
    double nPRChic2InPRSR2_Err = TMath::Sqrt(nPRChic2InPRSR2);

    cout<<"Components in PR LSB"<<endl;
    double nTotInPRLSB = nPRChic0InPRLSB + nPRChic1InPRLSB + nPRChic2InPRLSB + nNPChic0InPRLSB + nNPChic1InPRLSB + nNPChic2InPRLSB + nBackgroundInPRLSB;
    double fracPRChic0InPRLSB = nPRChic0InPRLSB / nTotInPRLSB ;
    double fracNPChic0InPRLSB = nNPChic0InPRLSB / nTotInPRLSB ;
    double fracPRChic1InPRLSB = nPRChic1InPRLSB / nTotInPRLSB ;
    double fracNPChic1InPRLSB = nNPChic1InPRLSB / nTotInPRLSB ;
    double fracPRChic2InPRLSB = nPRChic2InPRLSB / nTotInPRLSB ;
    double fracNPChic2InPRLSB = nNPChic2InPRLSB / nTotInPRLSB ;
    double fracBackgroundInPRLSB = nBackgroundInPRLSB / nTotInPRLSB ;
    cout<<"fracPRChic0InPRLSB    = "<<fracPRChic0InPRLSB   <<endl;
    cout<<"fracNPChic0InPRLSB    = "<<fracNPChic0InPRLSB   <<endl;
    cout<<"fracPRChic1InPRLSB    = "<<fracPRChic1InPRLSB   <<endl;
    cout<<"fracNPChic1InPRLSB    = "<<fracNPChic1InPRLSB   <<endl;
    cout<<"fracPRChic2InPRLSB    = "<<fracPRChic2InPRLSB   <<endl;
    cout<<"fracNPChic2InPRLSB    = "<<fracNPChic2InPRLSB   <<endl;
    cout<<"fracBackgroundInPRLSB = "<<fracBackgroundInPRLSB<<endl;

    cout<<"Components in PR RSB"<<endl;
    double nTotInPRRSB = nPRChic0InPRRSB + nPRChic1InPRRSB + nPRChic2InPRRSB + nNPChic0InPRRSB + nNPChic1InPRRSB + nNPChic2InPRRSB + nBackgroundInPRRSB;
    double fracPRChic0InPRRSB = nPRChic0InPRRSB / nTotInPRRSB ;
    double fracNPChic0InPRRSB = nNPChic0InPRRSB / nTotInPRRSB ;
    double fracPRChic1InPRRSB = nPRChic1InPRRSB / nTotInPRRSB ;
    double fracNPChic1InPRRSB = nNPChic1InPRRSB / nTotInPRRSB ;
    double fracPRChic2InPRRSB = nPRChic2InPRRSB / nTotInPRRSB ;
    double fracNPChic2InPRRSB = nNPChic2InPRRSB / nTotInPRRSB ;
    double fracBackgroundInPRRSB = nBackgroundInPRRSB / nTotInPRRSB ;
    cout<<"fracPRChic0InPRRSB    = "<<fracPRChic0InPRRSB   <<endl;
    cout<<"fracNPChic0InPRRSB    = "<<fracNPChic0InPRRSB   <<endl;
    cout<<"fracPRChic1InPRRSB    = "<<fracPRChic1InPRRSB   <<endl;
    cout<<"fracNPChic1InPRRSB    = "<<fracNPChic1InPRRSB   <<endl;
    cout<<"fracPRChic2InPRRSB    = "<<fracPRChic2InPRRSB   <<endl;
    cout<<"fracNPChic2InPRRSB    = "<<fracNPChic2InPRRSB   <<endl;
    cout<<"fracBackgroundInPRRSB = "<<fracBackgroundInPRRSB<<endl;

    cout<<"Components in PR SR1"<<endl;
    double nTotInPRSR1 = nPRChic0InPRSR1 + nPRChic1InPRSR1 + nPRChic2InPRSR1 + nNPChic0InPRSR1 + nNPChic1InPRSR1 + nNPChic2InPRSR1 + nBackgroundInPRSR1;
    double fracPRChic0InPRSR1 = nPRChic0InPRSR1 / nTotInPRSR1 ;
    double fracNPChic0InPRSR1 = nNPChic0InPRSR1 / nTotInPRSR1 ;
    double fracPRChic1InPRSR1 = nPRChic1InPRSR1 / nTotInPRSR1 ;
    double fracNPChic1InPRSR1 = nNPChic1InPRSR1 / nTotInPRSR1 ;
    double fracPRChic2InPRSR1 = nPRChic2InPRSR1 / nTotInPRSR1 ;
    double fracNPChic2InPRSR1 = nNPChic2InPRSR1 / nTotInPRSR1 ;
    double fracBackgroundInPRSR1 = nBackgroundInPRSR1 / nTotInPRSR1 ;
    cout<<"fracPRChic0InPRSR1    = "<<fracPRChic0InPRSR1   <<endl;
    cout<<"fracNPChic0InPRSR1    = "<<fracNPChic0InPRSR1   <<endl;
    cout<<"fracPRChic1InPRSR1    = "<<fracPRChic1InPRSR1   <<endl;
    cout<<"fracNPChic1InPRSR1    = "<<fracNPChic1InPRSR1   <<endl;
    cout<<"fracPRChic2InPRSR1    = "<<fracPRChic2InPRSR1   <<endl;
    cout<<"fracNPChic2InPRSR1    = "<<fracNPChic2InPRSR1   <<endl;
    cout<<"fracBackgroundInPRSR1 = "<<fracBackgroundInPRSR1<<endl;

    cout<<"Components in PR SR2"<<endl;
    double nTotInPRSR2 = nPRChic0InPRSR2 + nPRChic1InPRSR2 + nPRChic2InPRSR2 + nNPChic0InPRSR2 + nNPChic1InPRSR2 + nNPChic2InPRSR2 + nBackgroundInPRSR2;
    double fracPRChic0InPRSR2 = nPRChic0InPRSR2 / nTotInPRSR2 ;
    double fracNPChic0InPRSR2 = nNPChic0InPRSR2 / nTotInPRSR2 ;
    double fracPRChic1InPRSR2 = nPRChic1InPRSR2 / nTotInPRSR2 ;
    double fracNPChic1InPRSR2 = nNPChic1InPRSR2 / nTotInPRSR2 ;
    double fracPRChic2InPRSR2 = nPRChic2InPRSR2 / nTotInPRSR2 ;
    double fracNPChic2InPRSR2 = nNPChic2InPRSR2 / nTotInPRSR2 ;
    double fracBackgroundInPRSR2 = nBackgroundInPRSR2 / nTotInPRSR2 ;
    cout<<"fracPRChic0InPRSR2    = "<<fracPRChic0InPRSR2   <<endl;
    cout<<"fracNPChic0InPRSR2    = "<<fracNPChic0InPRSR2   <<endl;
    cout<<"fracPRChic1InPRSR2    = "<<fracPRChic1InPRSR2   <<endl;
    cout<<"fracNPChic1InPRSR2    = "<<fracNPChic1InPRSR2   <<endl;
    cout<<"fracPRChic2InPRSR2    = "<<fracPRChic2InPRSR2   <<endl;
    cout<<"fracNPChic2InPRSR2    = "<<fracNPChic2InPRSR2   <<endl;
    cout<<"fracBackgroundInPRSR2 = "<<fracBackgroundInPRSR2<<endl;





    cout<<"Components in NP LSB"<<endl;
    double nTotInNPLSB = nPRChic0InNPLSB + nPRChic1InNPLSB + nPRChic2InNPLSB + nNPChic0InNPLSB + nNPChic1InNPLSB + nNPChic2InNPLSB + nBackgroundInNPLSB;
    double fracPRChic0InNPLSB = nPRChic0InNPLSB / nTotInNPLSB ;
    double fracNPChic0InNPLSB = nNPChic0InNPLSB / nTotInNPLSB ;
    double fracPRChic1InNPLSB = nPRChic1InNPLSB / nTotInNPLSB ;
    double fracNPChic1InNPLSB = nNPChic1InNPLSB / nTotInNPLSB ;
    double fracPRChic2InNPLSB = nPRChic2InNPLSB / nTotInNPLSB ;
    double fracNPChic2InNPLSB = nNPChic2InNPLSB / nTotInNPLSB ;
    double fracBackgroundInNPLSB = nBackgroundInNPLSB / nTotInNPLSB ;
    cout<<"fracPRChic0InNPLSB    = "<<fracPRChic0InNPLSB   <<endl;
    cout<<"fracNPChic0InNPLSB    = "<<fracNPChic0InNPLSB   <<endl;
    cout<<"fracPRChic1InNPLSB    = "<<fracPRChic1InNPLSB   <<endl;
    cout<<"fracNPChic1InNPLSB    = "<<fracNPChic1InNPLSB   <<endl;
    cout<<"fracPRChic2InNPLSB    = "<<fracPRChic2InNPLSB   <<endl;
    cout<<"fracNPChic2InNPLSB    = "<<fracNPChic2InNPLSB   <<endl;
    cout<<"fracBackgroundInNPLSB = "<<fracBackgroundInNPLSB<<endl;

    cout<<"Components in NP RSB"<<endl;
    double nTotInNPRSB = nPRChic0InNPRSB + nPRChic1InNPRSB + nPRChic2InNPRSB + nNPChic0InNPRSB + nNPChic1InNPRSB + nNPChic2InNPRSB + nBackgroundInNPRSB;
    double fracPRChic0InNPRSB = nPRChic0InNPRSB / nTotInNPRSB ;
    double fracNPChic0InNPRSB = nNPChic0InNPRSB / nTotInNPRSB ;
    double fracPRChic1InNPRSB = nPRChic1InNPRSB / nTotInNPRSB ;
    double fracNPChic1InNPRSB = nNPChic1InNPRSB / nTotInNPRSB ;
    double fracPRChic2InNPRSB = nPRChic2InNPRSB / nTotInNPRSB ;
    double fracNPChic2InNPRSB = nNPChic2InNPRSB / nTotInNPRSB ;
    double fracBackgroundInNPRSB = nBackgroundInNPRSB / nTotInNPRSB ;
    cout<<"fracPRChic0InNPRSB    = "<<fracPRChic0InNPRSB   <<endl;
    cout<<"fracNPChic0InNPRSB    = "<<fracNPChic0InNPRSB   <<endl;
    cout<<"fracPRChic1InNPRSB    = "<<fracPRChic1InNPRSB   <<endl;
    cout<<"fracNPChic1InNPRSB    = "<<fracNPChic1InNPRSB   <<endl;
    cout<<"fracPRChic2InNPRSB    = "<<fracPRChic2InNPRSB   <<endl;
    cout<<"fracNPChic2InNPRSB    = "<<fracNPChic2InNPRSB   <<endl;
    cout<<"fracBackgroundInNPRSB = "<<fracBackgroundInNPRSB<<endl;

    cout<<"Components in NP SR1"<<endl;
    double nTotInNPSR1 = nPRChic0InNPSR1 + nPRChic1InNPSR1 + nPRChic2InNPSR1 + nNPChic0InNPSR1 + nNPChic1InNPSR1 + nNPChic2InNPSR1 + nBackgroundInNPSR1;
    double fracPRChic0InNPSR1 = nPRChic0InNPSR1 / nTotInNPSR1 ;
    double fracNPChic0InNPSR1 = nNPChic0InNPSR1 / nTotInNPSR1 ;
    double fracPRChic1InNPSR1 = nPRChic1InNPSR1 / nTotInNPSR1 ;
    double fracNPChic1InNPSR1 = nNPChic1InNPSR1 / nTotInNPSR1 ;
    double fracPRChic2InNPSR1 = nPRChic2InNPSR1 / nTotInNPSR1 ;
    double fracNPChic2InNPSR1 = nNPChic2InNPSR1 / nTotInNPSR1 ;
    double fracBackgroundInNPSR1 = nBackgroundInNPSR1 / nTotInNPSR1 ;
    cout<<"fracPRChic0InNPSR1    = "<<fracPRChic0InNPSR1   <<endl;
    cout<<"fracNPChic0InNPSR1    = "<<fracNPChic0InNPSR1   <<endl;
    cout<<"fracPRChic1InNPSR1    = "<<fracPRChic1InNPSR1   <<endl;
    cout<<"fracNPChic1InNPSR1    = "<<fracNPChic1InNPSR1   <<endl;
    cout<<"fracPRChic2InNPSR1    = "<<fracPRChic2InNPSR1   <<endl;
    cout<<"fracNPChic2InNPSR1    = "<<fracNPChic2InNPSR1   <<endl;
    cout<<"fracBackgroundInNPSR1 = "<<fracBackgroundInNPSR1<<endl;

    cout<<"Components in NP SR2"<<endl;
    double nTotInNPSR2 = nPRChic0InNPSR2 + nPRChic1InNPSR2 + nPRChic2InNPSR2 + nNPChic0InNPSR2 + nNPChic1InNPSR2 + nNPChic2InNPSR2 + nBackgroundInNPSR2;
    double fracPRChic0InNPSR2 = nPRChic0InNPSR2 / nTotInNPSR2 ;
    double fracNPChic0InNPSR2 = nNPChic0InNPSR2 / nTotInNPSR2 ;
    double fracPRChic1InNPSR2 = nPRChic1InNPSR2 / nTotInNPSR2 ;
    double fracNPChic1InNPSR2 = nNPChic1InNPSR2 / nTotInNPSR2 ;
    double fracPRChic2InNPSR2 = nPRChic2InNPSR2 / nTotInNPSR2 ;
    double fracNPChic2InNPSR2 = nNPChic2InNPSR2 / nTotInNPSR2 ;
    double fracBackgroundInNPSR2 = nBackgroundInNPSR2 / nTotInNPSR2 ;
    cout<<"fracPRChic0InNPSR2    = "<<fracPRChic0InNPSR2   <<endl;
    cout<<"fracNPChic0InNPSR2    = "<<fracNPChic0InNPSR2   <<endl;
    cout<<"fracPRChic1InNPSR2    = "<<fracPRChic1InNPSR2   <<endl;
    cout<<"fracNPChic1InNPSR2    = "<<fracNPChic1InNPSR2   <<endl;
    cout<<"fracPRChic2InNPSR2    = "<<fracPRChic2InNPSR2   <<endl;
    cout<<"fracNPChic2InNPSR2    = "<<fracNPChic2InNPSR2   <<endl;
    cout<<"fracBackgroundInNPSR2 = "<<fracBackgroundInNPSR2<<endl;


    //TODO: add fracs to ws


    //add variables to ws. If they exist, change value
    RooRealVar var_sig1MaxMass("var_sig1MaxMass","var_sig1MaxMass",sig1MaxMass); if(!ws->var("var_sig1MaxMass")) ws->import(var_sig1MaxMass); else ws->var("var_sig1MaxMass")->setVal(sig1MaxMass);
    RooRealVar var_sig1MinMass("var_sig1MinMass","var_sig1MinMass",sig1MinMass); if(!ws->var("var_sig1MinMass")) ws->import(var_sig1MinMass); else ws->var("var_sig1MinMass")->setVal(sig1MinMass);
    RooRealVar var_sig2MaxMass("var_sig2MaxMass","var_sig2MaxMass",sig2MaxMass); if(!ws->var("var_sig2MaxMass")) ws->import(var_sig2MaxMass); else ws->var("var_sig2MaxMass")->setVal(sig2MaxMass);
    RooRealVar var_sig2MinMass("var_sig2MinMass","var_sig2MinMass",sig2MinMass); if(!ws->var("var_sig2MinMass")) ws->import(var_sig2MinMass); else ws->var("var_sig2MinMass")->setVal(sig2MinMass);
    RooRealVar var_lsbMaxMass("var_lsbMaxMass","var_lsbMaxMass",lsbMaxMass); if(!ws->var("var_lsbMaxMass")) ws->import(var_lsbMaxMass); else ws->var("var_lsbMaxMass")->setVal(lsbMaxMass);
    RooRealVar var_lsbMinMass("var_lsbMinMass","var_lsbMinMass",lsbMinMass); if(!ws->var("var_lsbMinMass")) ws->import(var_lsbMinMass); else ws->var("var_lsbMinMass")->setVal(lsbMinMass);
    RooRealVar var_rsbMaxMass("var_rsbMaxMass","var_rsbMaxMass",rsbMaxMass); if(!ws->var("var_rsbMaxMass")) ws->import(var_rsbMaxMass); else ws->var("var_rsbMaxMass")->setVal(rsbMaxMass);
    RooRealVar var_rsbMinMass("var_rsbMinMass","var_rsbMinMass",rsbMinMass); if(!ws->var("var_rsbMinMass")) ws->import(var_rsbMinMass); else ws->var("var_rsbMinMass")->setVal(rsbMinMass);

    RooRealVar var_PRMin("var_PRMin","var_PRMin",PRMin); if(!ws->var("var_PRMin")) ws->import(var_PRMin); else ws->var("var_PRMin")->setVal(PRMin);
    RooRealVar var_PRMax("var_PRMax","var_PRMax",PRMax); if(!ws->var("var_PRMax")) ws->import(var_PRMax); else ws->var("var_PRMax")->setVal(PRMax);
    RooRealVar var_NPMin("var_NPMin","var_NPMin",NPMin); if(!ws->var("var_NPMin")) ws->import(var_NPMin); else ws->var("var_NPMin")->setVal(NPMin);
    RooRealVar var_NPMax("var_NPMax","var_NPMax",NPMax); if(!ws->var("var_NPMax")) ws->import(var_NPMax); else ws->var("var_NPMax")->setVal(NPMax);


    RooRealVar var_ev("var_ev","var_ev",ev); if(!ws->var("var_ev")) ws->import(var_ev); else ws->var("var_ev")->setVal(ev);
    RooRealVar var_data_ev("var_data_ev","var_data_ev",data_ev); if(!ws->var("var_data_ev")) ws->import(var_data_ev); else ws->var("var_data_ev")->setVal(data_ev);
    RooRealVar var_nBackground("var_nBackground","var_nBackground",nBackground); if(!ws->var("var_nBackground")) ws->import(var_nBackground); else ws->var("var_nBackground")->setVal(nBackground);
    RooRealVar var_nChic("var_nChic","var_nChic",nChic); if(!ws->var("var_nChic")) ws->import(var_nChic); else ws->var("var_nChic")->setVal(nChic);
    RooRealVar var_nChic0("var_nChic0","var_nChic0",nChic0); if(!ws->var("var_nChic0")) ws->import(var_nChic0); else ws->var("var_nChic0")->setVal(nChic0);
    RooRealVar var_nChic1("var_nChic1","var_nChic1",nChic1); if(!ws->var("var_nChic1")) ws->import(var_nChic1); else ws->var("var_nChic1")->setVal(nChic1);
    RooRealVar var_nChic2("var_nChic2","var_nChic2",nChic2); if(!ws->var("var_nChic2")) ws->import(var_nChic2); else ws->var("var_nChic2")->setVal(nChic2);
    RooRealVar var_nChic0NP("var_nChic0NP","var_nChic0NP",nChic0NP); if(!ws->var("var_nChic0NP")) ws->import(var_nChic0NP); else ws->var("var_nChic0NP")->setVal(nChic0NP);
    RooRealVar var_nChic0PR("var_nChic0PR","var_nChic0PR",nChic0PR); if(!ws->var("var_nChic0PR")) ws->import(var_nChic0PR); else ws->var("var_nChic0PR")->setVal(nChic0PR);
    RooRealVar var_nChic1NP("var_nChic1NP","var_nChic1NP",nChic1NP); if(!ws->var("var_nChic1NP")) ws->import(var_nChic1NP); else ws->var("var_nChic1NP")->setVal(nChic1NP);
    RooRealVar var_nChic1PR("var_nChic1PR","var_nChic1PR",nChic1PR); if(!ws->var("var_nChic1PR")) ws->import(var_nChic1PR); else ws->var("var_nChic1PR")->setVal(nChic1PR);
    RooRealVar var_nChic2NP("var_nChic2NP","var_nChic2NP",nChic2NP); if(!ws->var("var_nChic2NP")) ws->import(var_nChic2NP); else ws->var("var_nChic2NP")->setVal(nChic2NP);
    RooRealVar var_nChic2PR("var_nChic2PR","var_nChic2PR",nChic2PR); if(!ws->var("var_nChic2PR")) ws->import(var_nChic2PR); else ws->var("var_nChic2PR")->setVal(nChic2PR);

    RooRealVar var_fTotInLSB("var_fTotInLSB","var_fTotInLSB",fTotInLSB); if(!ws->var("var_fTotInLSB")) ws->import(var_fTotInLSB); else ws->var("var_fTotInLSB")->setVal(fTotInLSB);
    RooRealVar var_fTotInRSB("var_fTotInRSB","var_fTotInRSB",fTotInRSB); if(!ws->var("var_fTotInRSB")) ws->import(var_fTotInRSB); else ws->var("var_fTotInRSB")->setVal(fTotInRSB);
    RooRealVar var_fTotInSR1("var_fTotInSR1","var_fTotInSR1",fTotInSR1); if(!ws->var("var_fTotInSR1")) ws->import(var_fTotInSR1); else ws->var("var_fTotInSR1")->setVal(fTotInSR1);
    RooRealVar var_fTotInSR2("var_fTotInSR2","var_fTotInSR2",fTotInSR2); if(!ws->var("var_fTotInSR2")) ws->import(var_fTotInSR2); else ws->var("var_fTotInSR2")->setVal(fTotInSR2);

    RooRealVar var_fBackgroundInLSB("var_fBackgroundInLSB","var_fBackgroundInLSB",fBackgroundInLSB); if(!ws->var("var_fBackgroundInLSB")) ws->import(var_fBackgroundInLSB); else ws->var("var_fBackgroundInLSB")->setVal(fBackgroundInLSB);
    RooRealVar var_fBackgroundInRSB("var_fBackgroundInRSB","var_fBackgroundInRSB",fBackgroundInRSB); if(!ws->var("var_fBackgroundInRSB")) ws->import(var_fBackgroundInRSB); else ws->var("var_fBackgroundInRSB")->setVal(fBackgroundInRSB);
    RooRealVar var_fBackgroundInSR1("var_fBackgroundInSR1","var_fBackgroundInSR1",fBackgroundInSR1); if(!ws->var("var_fBackgroundInSR1")) ws->import(var_fBackgroundInSR1); else ws->var("var_fBackgroundInSR1")->setVal(fBackgroundInSR1);
    RooRealVar var_fBackgroundInSR2("var_fBackgroundInSR2","var_fBackgroundInSR2",fBackgroundInSR2); if(!ws->var("var_fBackgroundInSR2")) ws->import(var_fBackgroundInSR2); else ws->var("var_fBackgroundInSR2")->setVal(fBackgroundInSR2);

    RooRealVar var_fChic0InLSB("var_fChic0InLSB","var_fChic0InLSB",fChic0InLSB); if(!ws->var("var_fChic0InLSB")) ws->import(var_fChic0InLSB); else ws->var("var_fChic0InLSB")->setVal(fChic0InLSB);
    RooRealVar var_fChic0InRSB("var_fChic0InRSB","var_fChic0InRSB",fChic0InRSB); if(!ws->var("var_fChic0InRSB")) ws->import(var_fChic0InRSB); else ws->var("var_fChic0InRSB")->setVal(fChic0InRSB);
    RooRealVar var_fChic0InSR1("var_fChic0InSR1","var_fChic0InSR1",fChic0InSR1); if(!ws->var("var_fChic0InSR1")) ws->import(var_fChic0InSR1); else ws->var("var_fChic0InSR1")->setVal(fChic0InSR1);
    RooRealVar var_fChic0InSR2("var_fChic0InSR2","var_fChic0InSR2",fChic0InSR2); if(!ws->var("var_fChic0InSR2")) ws->import(var_fChic0InSR2); else ws->var("var_fChic0InSR2")->setVal(fChic0InSR2);

    RooRealVar var_fChic1InLSB("var_fChic1InLSB","var_fChic1InLSB",fChic1InLSB); if(!ws->var("var_fChic1InLSB")) ws->import(var_fChic1InLSB); else ws->var("var_fChic1InLSB")->setVal(fChic1InLSB);
    RooRealVar var_fChic1InRSB("var_fChic1InRSB","var_fChic1InRSB",fChic1InRSB); if(!ws->var("var_fChic1InRSB")) ws->import(var_fChic1InRSB); else ws->var("var_fChic1InRSB")->setVal(fChic1InRSB);
    RooRealVar var_fChic1InSR1("var_fChic1InSR1","var_fChic1InSR1",fChic1InSR1); if(!ws->var("var_fChic1InSR1")) ws->import(var_fChic1InSR1); else ws->var("var_fChic1InSR1")->setVal(fChic1InSR1);
    RooRealVar var_fChic1InSR2("var_fChic1InSR2","var_fChic1InSR2",fChic1InSR2); if(!ws->var("var_fChic1InSR2")) ws->import(var_fChic1InSR2); else ws->var("var_fChic1InSR2")->setVal(fChic1InSR2);

    RooRealVar var_fChic2InLSB("var_fChic2InLSB","var_fChic2InLSB",fChic2InLSB); if(!ws->var("var_fChic2InLSB")) ws->import(var_fChic2InLSB); else ws->var("var_fChic2InLSB")->setVal(fChic2InLSB);
    RooRealVar var_fChic2InRSB("var_fChic2InRSB","var_fChic2InRSB",fChic2InRSB); if(!ws->var("var_fChic2InRSB")) ws->import(var_fChic2InRSB); else ws->var("var_fChic2InRSB")->setVal(fChic2InRSB);
    RooRealVar var_fChic2InSR1("var_fChic2InSR1","var_fChic2InSR1",fChic2InSR1); if(!ws->var("var_fChic2InSR1")) ws->import(var_fChic2InSR1); else ws->var("var_fChic2InSR1")->setVal(fChic2InSR1);
    RooRealVar var_fChic2InSR2("var_fChic2InSR2","var_fChic2InSR2",fChic2InSR2); if(!ws->var("var_fChic2InSR2")) ws->import(var_fChic2InSR2); else ws->var("var_fChic2InSR2")->setVal(fChic2InSR2);

    RooRealVar var_fracBackgroundInLSB("var_fracBackgroundInLSB","var_fracBackgroundInLSB",fracBackgroundInLSB); if(!ws->var("var_fracBackgroundInLSB")) ws->import(var_fracBackgroundInLSB); else ws->var("var_fracBackgroundInLSB")->setVal(fracBackgroundInLSB);
    RooRealVar var_fracBackgroundInRSB("var_fracBackgroundInRSB","var_fracBackgroundInRSB",fracBackgroundInRSB); if(!ws->var("var_fracBackgroundInRSB")) ws->import(var_fracBackgroundInRSB); else ws->var("var_fracBackgroundInRSB")->setVal(fracBackgroundInRSB);
    RooRealVar var_fracBackgroundInSR1("var_fracBackgroundInSR1","var_fracBackgroundInSR1",fracBackgroundInSR1); if(!ws->var("var_fracBackgroundInSR1")) ws->import(var_fracBackgroundInSR1); else ws->var("var_fracBackgroundInSR1")->setVal(fracBackgroundInSR1);
    RooRealVar var_fracBackgroundInSR2("var_fracBackgroundInSR2","var_fracBackgroundInSR2",fracBackgroundInSR2); if(!ws->var("var_fracBackgroundInSR2")) ws->import(var_fracBackgroundInSR2); else ws->var("var_fracBackgroundInSR2")->setVal(fracBackgroundInSR2);

    RooRealVar var_fracChic0InLSB("var_fracChic0InLSB","var_fracChic0InLSB",fracChic0InLSB); if(!ws->var("var_fracChic0InLSB")) ws->import(var_fracChic0InLSB); else ws->var("var_fracChic0InLSB")->setVal(fracChic0InLSB);
    RooRealVar var_fracChic0InRSB("var_fracChic0InRSB","var_fracChic0InRSB",fracChic0InRSB); if(!ws->var("var_fracChic0InRSB")) ws->import(var_fracChic0InRSB); else ws->var("var_fracChic0InRSB")->setVal(fracChic0InRSB);
    RooRealVar var_fracChic0InSR1("var_fracChic0InSR1","var_fracChic0InSR1",fracChic0InSR1); if(!ws->var("var_fracChic0InSR1")) ws->import(var_fracChic0InSR1); else ws->var("var_fracChic0InSR1")->setVal(fracChic0InSR1);
    RooRealVar var_fracChic0InSR2("var_fracChic0InSR2","var_fracChic0InSR2",fracChic0InSR2); if(!ws->var("var_fracChic0InSR2")) ws->import(var_fracChic0InSR2); else ws->var("var_fracChic0InSR2")->setVal(fracChic0InSR2);

    RooRealVar var_fracChic1InLSB("var_fracChic1InLSB","var_fracChic1InLSB",fracChic1InLSB); if(!ws->var("var_fracChic1InLSB")) ws->import(var_fracChic1InLSB); else ws->var("var_fracChic1InLSB")->setVal(fracChic1InLSB);
    RooRealVar var_fracChic1InRSB("var_fracChic1InRSB","var_fracChic1InRSB",fracChic1InRSB); if(!ws->var("var_fracChic1InRSB")) ws->import(var_fracChic1InRSB); else ws->var("var_fracChic1InRSB")->setVal(fracChic1InRSB);
    RooRealVar var_fracChic1InSR1("var_fracChic1InSR1","var_fracChic1InSR1",fracChic1InSR1); if(!ws->var("var_fracChic1InSR1")) ws->import(var_fracChic1InSR1); else ws->var("var_fracChic1InSR1")->setVal(fracChic1InSR1);
    RooRealVar var_fracChic1InSR2("var_fracChic1InSR2","var_fracChic1InSR2",fracChic1InSR2); if(!ws->var("var_fracChic1InSR2")) ws->import(var_fracChic1InSR2); else ws->var("var_fracChic1InSR2")->setVal(fracChic1InSR2);

    RooRealVar var_fracChic2InLSB("var_fracChic2InLSB","var_fracChic2InLSB",fracChic2InLSB); if(!ws->var("var_fracChic2InLSB")) ws->import(var_fracChic2InLSB); else ws->var("var_fracChic2InLSB")->setVal(fracChic2InLSB);
    RooRealVar var_fracChic2InRSB("var_fracChic2InRSB","var_fracChic2InRSB",fracChic2InRSB); if(!ws->var("var_fracChic2InRSB")) ws->import(var_fracChic2InRSB); else ws->var("var_fracChic2InRSB")->setVal(fracChic2InRSB);
    RooRealVar var_fracChic2InSR1("var_fracChic2InSR1","var_fracChic2InSR1",fracChic2InSR1); if(!ws->var("var_fracChic2InSR1")) ws->import(var_fracChic2InSR1); else ws->var("var_fracChic2InSR1")->setVal(fracChic2InSR1);
    RooRealVar var_fracChic2InSR2("var_fracChic2InSR2","var_fracChic2InSR2",fracChic2InSR2); if(!ws->var("var_fracChic2InSR2")) ws->import(var_fracChic2InSR2); else ws->var("var_fracChic2InSR2")->setVal(fracChic2InSR2);

    RooRealVar var_fBackgroundInPR("var_fBackgroundInPR","var_fBackgroundInPR",fBackgroundInPR); if(!ws->var("var_fBackgroundInPR")) ws->import(var_fBackgroundInPR); else ws->var("var_fBackgroundInPR")->setVal(fBackgroundInPR);
    RooRealVar var_fBackgroundInNP("var_fBackgroundInNP","var_fBackgroundInNP",fBackgroundInNP); if(!ws->var("var_fBackgroundInNP")) ws->import(var_fBackgroundInNP); else ws->var("var_fBackgroundInNP")->setVal(fBackgroundInNP);
    RooRealVar var_fPRChicInPR("var_fPRChicInPR","var_fPRChicInPR",fPRChicInPR); if(!ws->var("var_fPRChicInPR")) ws->import(var_fPRChicInPR); else ws->var("var_fPRChicInPR")->setVal(fPRChicInPR);
    RooRealVar var_fPRChicInNP("var_fPRChicInNP","var_fPRChicInNP",fPRChicInNP); if(!ws->var("var_fPRChicInNP")) ws->import(var_fPRChicInNP); else ws->var("var_fPRChicInNP")->setVal(fPRChicInNP);
    RooRealVar var_fNPChic0InPR("var_fNPChic0InPR","var_fNPChic0InPR",fNPChic0InPR); if(!ws->var("var_fNPChic0InPR")) ws->import(var_fNPChic0InPR); else ws->var("var_fNPChic0InPR")->setVal(fNPChic0InPR);
    RooRealVar var_fNPChic0InNP("var_fNPChic0InNP","var_fNPChic0InNP",fNPChic0InNP); if(!ws->var("var_fNPChic0InNP")) ws->import(var_fNPChic0InNP); else ws->var("var_fNPChic0InNP")->setVal(fNPChic0InNP);
    RooRealVar var_fNPChic1InPR("var_fNPChic1InPR","var_fNPChic1InPR",fNPChic1InPR); if(!ws->var("var_fNPChic1InPR")) ws->import(var_fNPChic1InPR); else ws->var("var_fNPChic1InPR")->setVal(fNPChic1InPR);
    RooRealVar var_fNPChic1InNP("var_fNPChic1InNP","var_fNPChic1InNP",fNPChic1InNP); if(!ws->var("var_fNPChic1InNP")) ws->import(var_fNPChic1InNP); else ws->var("var_fNPChic1InNP")->setVal(fNPChic1InNP);
    RooRealVar var_fNPChic2InPR("var_fNPChic2InPR","var_fNPChic2InPR",fNPChic2InPR); if(!ws->var("var_fNPChic2InPR")) ws->import(var_fNPChic2InPR); else ws->var("var_fNPChic2InPR")->setVal(fNPChic2InPR);
    RooRealVar var_fNPChic2InNP("var_fNPChic2InNP","var_fNPChic2InNP",fNPChic2InNP); if(!ws->var("var_fNPChic2InNP")) ws->import(var_fNPChic2InNP); else ws->var("var_fNPChic2InNP")->setVal(fNPChic2InNP);

    RooRealVar var_fracPRChic0InPRLSB("var_fracPRChic0InPRLSB","var_fracPRChic0InPRLSB",fracPRChic0InPRLSB); if(!ws->var("var_fracPRChic0InPRLSB")) ws->import(var_fracPRChic0InPRLSB); else ws->var("var_fracPRChic0InPRLSB")->setVal(fracPRChic0InPRLSB);
    RooRealVar var_fracNPChic0InPRLSB("var_fracNPChic0InPRLSB","var_fracNPChic0InPRLSB",fracNPChic0InPRLSB); if(!ws->var("var_fracNPChic0InPRLSB")) ws->import(var_fracNPChic0InPRLSB); else ws->var("var_fracNPChic0InPRLSB")->setVal(fracNPChic0InPRLSB);
    RooRealVar var_fracPRChic1InPRLSB("var_fracPRChic1InPRLSB","var_fracPRChic1InPRLSB",fracPRChic1InPRLSB); if(!ws->var("var_fracPRChic1InPRLSB")) ws->import(var_fracPRChic1InPRLSB); else ws->var("var_fracPRChic1InPRLSB")->setVal(fracPRChic1InPRLSB);
    RooRealVar var_fracNPChic1InPRLSB("var_fracNPChic1InPRLSB","var_fracNPChic1InPRLSB",fracNPChic1InPRLSB); if(!ws->var("var_fracNPChic1InPRLSB")) ws->import(var_fracNPChic1InPRLSB); else ws->var("var_fracNPChic1InPRLSB")->setVal(fracNPChic1InPRLSB);
    RooRealVar var_fracPRChic2InPRLSB("var_fracPRChic2InPRLSB","var_fracPRChic2InPRLSB",fracPRChic2InPRLSB); if(!ws->var("var_fracPRChic2InPRLSB")) ws->import(var_fracPRChic2InPRLSB); else ws->var("var_fracPRChic2InPRLSB")->setVal(fracPRChic2InPRLSB);
    RooRealVar var_fracNPChic2InPRLSB("var_fracNPChic2InPRLSB","var_fracNPChic2InPRLSB",fracNPChic2InPRLSB); if(!ws->var("var_fracNPChic2InPRLSB")) ws->import(var_fracNPChic2InPRLSB); else ws->var("var_fracNPChic2InPRLSB")->setVal(fracNPChic2InPRLSB);
    RooRealVar var_fracBackgroundInPRLSB("var_fracBackgroundInPRLSB","var_fracBackgroundInPRLSB",fracBackgroundInPRLSB); if(!ws->var("var_fracBackgroundInPRLSB")) ws->import(var_fracBackgroundInPRLSB); else ws->var("var_fracBackgroundInPRLSB")->setVal(fracBackgroundInPRLSB);

    RooRealVar var_fracPRChic0InPRRSB("var_fracPRChic0InPRRSB","var_fracPRChic0InPRRSB",fracPRChic0InPRRSB); if(!ws->var("var_fracPRChic0InPRRSB")) ws->import(var_fracPRChic0InPRRSB); else ws->var("var_fracPRChic0InPRRSB")->setVal(fracPRChic0InPRRSB);
    RooRealVar var_fracNPChic0InPRRSB("var_fracNPChic0InPRRSB","var_fracNPChic0InPRRSB",fracNPChic0InPRRSB); if(!ws->var("var_fracNPChic0InPRRSB")) ws->import(var_fracNPChic0InPRRSB); else ws->var("var_fracNPChic0InPRRSB")->setVal(fracNPChic0InPRRSB);
    RooRealVar var_fracPRChic1InPRRSB("var_fracPRChic1InPRRSB","var_fracPRChic1InPRRSB",fracPRChic1InPRRSB); if(!ws->var("var_fracPRChic1InPRRSB")) ws->import(var_fracPRChic1InPRRSB); else ws->var("var_fracPRChic1InPRRSB")->setVal(fracPRChic1InPRRSB);
    RooRealVar var_fracNPChic1InPRRSB("var_fracNPChic1InPRRSB","var_fracNPChic1InPRRSB",fracNPChic1InPRRSB); if(!ws->var("var_fracNPChic1InPRRSB")) ws->import(var_fracNPChic1InPRRSB); else ws->var("var_fracNPChic1InPRRSB")->setVal(fracNPChic1InPRRSB);
    RooRealVar var_fracPRChic2InPRRSB("var_fracPRChic2InPRRSB","var_fracPRChic2InPRRSB",fracPRChic2InPRRSB); if(!ws->var("var_fracPRChic2InPRRSB")) ws->import(var_fracPRChic2InPRRSB); else ws->var("var_fracPRChic2InPRRSB")->setVal(fracPRChic2InPRRSB);
    RooRealVar var_fracNPChic2InPRRSB("var_fracNPChic2InPRRSB","var_fracNPChic2InPRRSB",fracNPChic2InPRRSB); if(!ws->var("var_fracNPChic2InPRRSB")) ws->import(var_fracNPChic2InPRRSB); else ws->var("var_fracNPChic2InPRRSB")->setVal(fracNPChic2InPRRSB);
    RooRealVar var_fracBackgroundInPRRSB("var_fracBackgroundInPRRSB","var_fracBackgroundInPRRSB",fracBackgroundInPRRSB); if(!ws->var("var_fracBackgroundInPRRSB")) ws->import(var_fracBackgroundInPRRSB); else ws->var("var_fracBackgroundInPRRSB")->setVal(fracBackgroundInPRRSB);

    RooRealVar var_fracPRChic0InPRSR1("var_fracPRChic0InPRSR1","var_fracPRChic0InPRSR1",fracPRChic0InPRSR1); if(!ws->var("var_fracPRChic0InPRSR1")) ws->import(var_fracPRChic0InPRSR1); else ws->var("var_fracPRChic0InPRSR1")->setVal(fracPRChic0InPRSR1);
    RooRealVar var_fracNPChic0InPRSR1("var_fracNPChic0InPRSR1","var_fracNPChic0InPRSR1",fracNPChic0InPRSR1); if(!ws->var("var_fracNPChic0InPRSR1")) ws->import(var_fracNPChic0InPRSR1); else ws->var("var_fracNPChic0InPRSR1")->setVal(fracNPChic0InPRSR1);
    RooRealVar var_fracPRChic1InPRSR1("var_fracPRChic1InPRSR1","var_fracPRChic1InPRSR1",fracPRChic1InPRSR1); if(!ws->var("var_fracPRChic1InPRSR1")) ws->import(var_fracPRChic1InPRSR1); else ws->var("var_fracPRChic1InPRSR1")->setVal(fracPRChic1InPRSR1);
    RooRealVar var_fracNPChic1InPRSR1("var_fracNPChic1InPRSR1","var_fracNPChic1InPRSR1",fracNPChic1InPRSR1); if(!ws->var("var_fracNPChic1InPRSR1")) ws->import(var_fracNPChic1InPRSR1); else ws->var("var_fracNPChic1InPRSR1")->setVal(fracNPChic1InPRSR1);
    RooRealVar var_fracPRChic2InPRSR1("var_fracPRChic2InPRSR1","var_fracPRChic2InPRSR1",fracPRChic2InPRSR1); if(!ws->var("var_fracPRChic2InPRSR1")) ws->import(var_fracPRChic2InPRSR1); else ws->var("var_fracPRChic2InPRSR1")->setVal(fracPRChic2InPRSR1);
    RooRealVar var_fracNPChic2InPRSR1("var_fracNPChic2InPRSR1","var_fracNPChic2InPRSR1",fracNPChic2InPRSR1); if(!ws->var("var_fracNPChic2InPRSR1")) ws->import(var_fracNPChic2InPRSR1); else ws->var("var_fracNPChic2InPRSR1")->setVal(fracNPChic2InPRSR1);
    RooRealVar var_fracBackgroundInPRSR1("var_fracBackgroundInPRSR1","var_fracBackgroundInPRSR1",fracBackgroundInPRSR1); if(!ws->var("var_fracBackgroundInPRSR1")) ws->import(var_fracBackgroundInPRSR1); else ws->var("var_fracBackgroundInPRSR1")->setVal(fracBackgroundInPRSR1);

    RooRealVar var_fracPRChic0InPRSR2("var_fracPRChic0InPRSR2","var_fracPRChic0InPRSR2",fracPRChic0InPRSR2); if(!ws->var("var_fracPRChic0InPRSR2")) ws->import(var_fracPRChic0InPRSR2); else ws->var("var_fracPRChic0InPRSR2")->setVal(fracPRChic0InPRSR2);
    RooRealVar var_fracNPChic0InPRSR2("var_fracNPChic0InPRSR2","var_fracNPChic0InPRSR2",fracNPChic0InPRSR2); if(!ws->var("var_fracNPChic0InPRSR2")) ws->import(var_fracNPChic0InPRSR2); else ws->var("var_fracNPChic0InPRSR2")->setVal(fracNPChic0InPRSR2);
    RooRealVar var_fracPRChic1InPRSR2("var_fracPRChic1InPRSR2","var_fracPRChic1InPRSR2",fracPRChic1InPRSR2); if(!ws->var("var_fracPRChic1InPRSR2")) ws->import(var_fracPRChic1InPRSR2); else ws->var("var_fracPRChic1InPRSR2")->setVal(fracPRChic1InPRSR2);
    RooRealVar var_fracNPChic1InPRSR2("var_fracNPChic1InPRSR2","var_fracNPChic1InPRSR2",fracNPChic1InPRSR2); if(!ws->var("var_fracNPChic1InPRSR2")) ws->import(var_fracNPChic1InPRSR2); else ws->var("var_fracNPChic1InPRSR2")->setVal(fracNPChic1InPRSR2);
    RooRealVar var_fracPRChic2InPRSR2("var_fracPRChic2InPRSR2","var_fracPRChic2InPRSR2",fracPRChic2InPRSR2); if(!ws->var("var_fracPRChic2InPRSR2")) ws->import(var_fracPRChic2InPRSR2); else ws->var("var_fracPRChic2InPRSR2")->setVal(fracPRChic2InPRSR2);
    RooRealVar var_fracNPChic2InPRSR2("var_fracNPChic2InPRSR2","var_fracNPChic2InPRSR2",fracNPChic2InPRSR2); if(!ws->var("var_fracNPChic2InPRSR2")) ws->import(var_fracNPChic2InPRSR2); else ws->var("var_fracNPChic2InPRSR2")->setVal(fracNPChic2InPRSR2);
    RooRealVar var_fracBackgroundInPRSR2("var_fracBackgroundInPRSR2","var_fracBackgroundInPRSR2",fracBackgroundInPRSR2); if(!ws->var("var_fracBackgroundInPRSR2")) ws->import(var_fracBackgroundInPRSR2); else ws->var("var_fracBackgroundInPRSR2")->setVal(fracBackgroundInPRSR2);



    RooRealVar var_fracPRChic0InNPLSB("var_fracPRChic0InNPLSB","var_fracPRChic0InNPLSB",fracPRChic0InNPLSB); if(!ws->var("var_fracPRChic0InNPLSB")) ws->import(var_fracPRChic0InNPLSB); else ws->var("var_fracPRChic0InNPLSB")->setVal(fracPRChic0InNPLSB);
    RooRealVar var_fracNPChic0InNPLSB("var_fracNPChic0InNPLSB","var_fracNPChic0InNPLSB",fracNPChic0InNPLSB); if(!ws->var("var_fracNPChic0InNPLSB")) ws->import(var_fracNPChic0InNPLSB); else ws->var("var_fracNPChic0InNPLSB")->setVal(fracNPChic0InNPLSB);
    RooRealVar var_fracPRChic1InNPLSB("var_fracPRChic1InNPLSB","var_fracPRChic1InNPLSB",fracPRChic1InNPLSB); if(!ws->var("var_fracPRChic1InNPLSB")) ws->import(var_fracPRChic1InNPLSB); else ws->var("var_fracPRChic1InNPLSB")->setVal(fracPRChic1InNPLSB);
    RooRealVar var_fracNPChic1InNPLSB("var_fracNPChic1InNPLSB","var_fracNPChic1InNPLSB",fracNPChic1InNPLSB); if(!ws->var("var_fracNPChic1InNPLSB")) ws->import(var_fracNPChic1InNPLSB); else ws->var("var_fracNPChic1InNPLSB")->setVal(fracNPChic1InNPLSB);
    RooRealVar var_fracPRChic2InNPLSB("var_fracPRChic2InNPLSB","var_fracPRChic2InNPLSB",fracPRChic2InNPLSB); if(!ws->var("var_fracPRChic2InNPLSB")) ws->import(var_fracPRChic2InNPLSB); else ws->var("var_fracPRChic2InNPLSB")->setVal(fracPRChic2InNPLSB);
    RooRealVar var_fracNPChic2InNPLSB("var_fracNPChic2InNPLSB","var_fracNPChic2InNPLSB",fracNPChic2InNPLSB); if(!ws->var("var_fracNPChic2InNPLSB")) ws->import(var_fracNPChic2InNPLSB); else ws->var("var_fracNPChic2InNPLSB")->setVal(fracNPChic2InNPLSB);
    RooRealVar var_fracBackgroundInNPLSB("var_fracBackgroundInNPLSB","var_fracBackgroundInNPLSB",fracBackgroundInNPLSB); if(!ws->var("var_fracBackgroundInNPLSB")) ws->import(var_fracBackgroundInNPLSB); else ws->var("var_fracBackgroundInNPLSB")->setVal(fracBackgroundInNPLSB);

    RooRealVar var_fracPRChic0InNPRSB("var_fracPRChic0InNPRSB","var_fracPRChic0InNPRSB",fracPRChic0InNPRSB); if(!ws->var("var_fracPRChic0InNPRSB")) ws->import(var_fracPRChic0InNPRSB); else ws->var("var_fracPRChic0InNPRSB")->setVal(fracPRChic0InNPRSB);
    RooRealVar var_fracNPChic0InNPRSB("var_fracNPChic0InNPRSB","var_fracNPChic0InNPRSB",fracNPChic0InNPRSB); if(!ws->var("var_fracNPChic0InNPRSB")) ws->import(var_fracNPChic0InNPRSB); else ws->var("var_fracNPChic0InNPRSB")->setVal(fracNPChic0InNPRSB);
    RooRealVar var_fracPRChic1InNPRSB("var_fracPRChic1InNPRSB","var_fracPRChic1InNPRSB",fracPRChic1InNPRSB); if(!ws->var("var_fracPRChic1InNPRSB")) ws->import(var_fracPRChic1InNPRSB); else ws->var("var_fracPRChic1InNPRSB")->setVal(fracPRChic1InNPRSB);
    RooRealVar var_fracNPChic1InNPRSB("var_fracNPChic1InNPRSB","var_fracNPChic1InNPRSB",fracNPChic1InNPRSB); if(!ws->var("var_fracNPChic1InNPRSB")) ws->import(var_fracNPChic1InNPRSB); else ws->var("var_fracNPChic1InNPRSB")->setVal(fracNPChic1InNPRSB);
    RooRealVar var_fracPRChic2InNPRSB("var_fracPRChic2InNPRSB","var_fracPRChic2InNPRSB",fracPRChic2InNPRSB); if(!ws->var("var_fracPRChic2InNPRSB")) ws->import(var_fracPRChic2InNPRSB); else ws->var("var_fracPRChic2InNPRSB")->setVal(fracPRChic2InNPRSB);
    RooRealVar var_fracNPChic2InNPRSB("var_fracNPChic2InNPRSB","var_fracNPChic2InNPRSB",fracNPChic2InNPRSB); if(!ws->var("var_fracNPChic2InNPRSB")) ws->import(var_fracNPChic2InNPRSB); else ws->var("var_fracNPChic2InNPRSB")->setVal(fracNPChic2InNPRSB);
    RooRealVar var_fracBackgroundInNPRSB("var_fracBackgroundInNPRSB","var_fracBackgroundInNPRSB",fracBackgroundInNPRSB); if(!ws->var("var_fracBackgroundInNPRSB")) ws->import(var_fracBackgroundInNPRSB); else ws->var("var_fracBackgroundInNPRSB")->setVal(fracBackgroundInNPRSB);

    RooRealVar var_fracPRChic0InNPSR1("var_fracPRChic0InNPSR1","var_fracPRChic0InNPSR1",fracPRChic0InNPSR1); if(!ws->var("var_fracPRChic0InNPSR1")) ws->import(var_fracPRChic0InNPSR1); else ws->var("var_fracPRChic0InNPSR1")->setVal(fracPRChic0InNPSR1);
    RooRealVar var_fracNPChic0InNPSR1("var_fracNPChic0InNPSR1","var_fracNPChic0InNPSR1",fracNPChic0InNPSR1); if(!ws->var("var_fracNPChic0InNPSR1")) ws->import(var_fracNPChic0InNPSR1); else ws->var("var_fracNPChic0InNPSR1")->setVal(fracNPChic0InNPSR1);
    RooRealVar var_fracPRChic1InNPSR1("var_fracPRChic1InNPSR1","var_fracPRChic1InNPSR1",fracPRChic1InNPSR1); if(!ws->var("var_fracPRChic1InNPSR1")) ws->import(var_fracPRChic1InNPSR1); else ws->var("var_fracPRChic1InNPSR1")->setVal(fracPRChic1InNPSR1);
    RooRealVar var_fracNPChic1InNPSR1("var_fracNPChic1InNPSR1","var_fracNPChic1InNPSR1",fracNPChic1InNPSR1); if(!ws->var("var_fracNPChic1InNPSR1")) ws->import(var_fracNPChic1InNPSR1); else ws->var("var_fracNPChic1InNPSR1")->setVal(fracNPChic1InNPSR1);
    RooRealVar var_fracPRChic2InNPSR1("var_fracPRChic2InNPSR1","var_fracPRChic2InNPSR1",fracPRChic2InNPSR1); if(!ws->var("var_fracPRChic2InNPSR1")) ws->import(var_fracPRChic2InNPSR1); else ws->var("var_fracPRChic2InNPSR1")->setVal(fracPRChic2InNPSR1);
    RooRealVar var_fracNPChic2InNPSR1("var_fracNPChic2InNPSR1","var_fracNPChic2InNPSR1",fracNPChic2InNPSR1); if(!ws->var("var_fracNPChic2InNPSR1")) ws->import(var_fracNPChic2InNPSR1); else ws->var("var_fracNPChic2InNPSR1")->setVal(fracNPChic2InNPSR1);
    RooRealVar var_fracBackgroundInNPSR1("var_fracBackgroundInNPSR1","var_fracBackgroundInNPSR1",fracBackgroundInNPSR1); if(!ws->var("var_fracBackgroundInNPSR1")) ws->import(var_fracBackgroundInNPSR1); else ws->var("var_fracBackgroundInNPSR1")->setVal(fracBackgroundInNPSR1);

    RooRealVar var_fracPRChic0InNPSR2("var_fracPRChic0InNPSR2","var_fracPRChic0InNPSR2",fracPRChic0InNPSR2); if(!ws->var("var_fracPRChic0InNPSR2")) ws->import(var_fracPRChic0InNPSR2); else ws->var("var_fracPRChic0InNPSR2")->setVal(fracPRChic0InNPSR2);
    RooRealVar var_fracNPChic0InNPSR2("var_fracNPChic0InNPSR2","var_fracNPChic0InNPSR2",fracNPChic0InNPSR2); if(!ws->var("var_fracNPChic0InNPSR2")) ws->import(var_fracNPChic0InNPSR2); else ws->var("var_fracNPChic0InNPSR2")->setVal(fracNPChic0InNPSR2);
    RooRealVar var_fracPRChic1InNPSR2("var_fracPRChic1InNPSR2","var_fracPRChic1InNPSR2",fracPRChic1InNPSR2); if(!ws->var("var_fracPRChic1InNPSR2")) ws->import(var_fracPRChic1InNPSR2); else ws->var("var_fracPRChic1InNPSR2")->setVal(fracPRChic1InNPSR2);
    RooRealVar var_fracNPChic1InNPSR2("var_fracNPChic1InNPSR2","var_fracNPChic1InNPSR2",fracNPChic1InNPSR2); if(!ws->var("var_fracNPChic1InNPSR2")) ws->import(var_fracNPChic1InNPSR2); else ws->var("var_fracNPChic1InNPSR2")->setVal(fracNPChic1InNPSR2);
    RooRealVar var_fracPRChic2InNPSR2("var_fracPRChic2InNPSR2","var_fracPRChic2InNPSR2",fracPRChic2InNPSR2); if(!ws->var("var_fracPRChic2InNPSR2")) ws->import(var_fracPRChic2InNPSR2); else ws->var("var_fracPRChic2InNPSR2")->setVal(fracPRChic2InNPSR2);
    RooRealVar var_fracNPChic2InNPSR2("var_fracNPChic2InNPSR2","var_fracNPChic2InNPSR2",fracNPChic2InNPSR2); if(!ws->var("var_fracNPChic2InNPSR2")) ws->import(var_fracNPChic2InNPSR2); else ws->var("var_fracNPChic2InNPSR2")->setVal(fracNPChic2InNPSR2);
    RooRealVar var_fracBackgroundInNPSR2("var_fracBackgroundInNPSR2","var_fracBackgroundInNPSR2",fracBackgroundInNPSR2); if(!ws->var("var_fracBackgroundInNPSR2")) ws->import(var_fracBackgroundInNPSR2); else ws->var("var_fracBackgroundInNPSR2")->setVal(fracBackgroundInNPSR2);


//Define fullPdf with correct fractions in each mass region

    RooAddPdf ML_fullModel_SR1= RooAddPdf("ML_fullModel_SR1","ML_fullModel_SR1",RooArgList(*ws->pdf("ML_background"),*ws->pdf("ML_chic0"),*ws->pdf("ML_chic1"),*ws->pdf("ML_chic2")),RooArgList(var_fracBackgroundInSR1, var_fracChic0InSR1, var_fracChic1InSR1, var_fracChic2InSR1)); ws->import(ML_fullModel_SR1);
    RooAddPdf ML_fullModel_SR2= RooAddPdf("ML_fullModel_SR2","ML_fullModel_SR2",RooArgList(*ws->pdf("ML_background"),*ws->pdf("ML_chic0"),*ws->pdf("ML_chic1"),*ws->pdf("ML_chic2")),RooArgList(var_fracBackgroundInSR2, var_fracChic0InSR2, var_fracChic1InSR2, var_fracChic2InSR2)); ws->import(ML_fullModel_SR2);
    RooAddPdf ML_fullModel_LSB= RooAddPdf("ML_fullModel_LSB","ML_fullModel_LSB",RooArgList(*ws->pdf("ML_background"),*ws->pdf("ML_chic0"),*ws->pdf("ML_chic1"),*ws->pdf("ML_chic2")),RooArgList(var_fracBackgroundInLSB, var_fracChic0InLSB, var_fracChic1InLSB, var_fracChic2InLSB)); ws->import(ML_fullModel_LSB);
    RooAddPdf ML_fullModel_RSB= RooAddPdf("ML_fullModel_RSB","ML_fullModel_RSB",RooArgList(*ws->pdf("ML_background"),*ws->pdf("ML_chic0"),*ws->pdf("ML_chic1"),*ws->pdf("ML_chic2")),RooArgList(var_fracBackgroundInRSB, var_fracChic0InRSB, var_fracChic1InRSB, var_fracChic2InRSB)); ws->import(ML_fullModel_RSB);


    RooAbsReal* real_fPRLSBInLSB = ML_fullModel_LSB.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    double fPRLSBInLSB=real_fPRLSBInLSB->getVal();
    cout<<"fPRLSBInLSB    = "<<fPRLSBInLSB   <<endl;
    RooAbsReal* real_fNPLSBInLSB = ML_fullModel_LSB.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPLSBInLSB=real_fNPLSBInLSB->getVal();
    cout<<"fNPLSBInLSB    = "<<fNPLSBInLSB   <<endl;
    RooAbsReal* real_fPRRSBInRSB = ML_fullModel_RSB.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    double fPRRSBInRSB=real_fPRRSBInRSB->getVal();
    cout<<"fPRRSBInRSB    = "<<fPRRSBInRSB   <<endl;
    RooAbsReal* real_fNPRSBInRSB = ML_fullModel_RSB.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPRSBInRSB=real_fNPRSBInRSB->getVal();
    cout<<"fNPRSBInRSB    = "<<fNPRSBInRSB   <<endl;
    RooAbsReal* real_fPRSR1InSR1 = ML_fullModel_SR1.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    double fPRSR1InSR1=real_fPRSR1InSR1->getVal();
    cout<<"fPRSR1InSR1    = "<<fPRSR1InSR1   <<endl;
    RooAbsReal* real_fNPSR1InSR1 = ML_fullModel_SR1.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPSR1InSR1=real_fNPSR1InSR1->getVal();
    cout<<"fNPSR1InSR1    = "<<fNPSR1InSR1   <<endl;
    RooAbsReal* real_fPRSR2InSR2 = ML_fullModel_SR2.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("PR"));
    double fPRSR2InSR2=real_fPRSR2InSR2->getVal();
    cout<<"fPRSR2InSR2    = "<<fPRSR2InSR2   <<endl;
    RooAbsReal* real_fNPSR2InSR2 = ML_fullModel_SR2.createIntegral(RooArgSet(*ct), NormSet(RooArgSet(*ct)), Range("NP"));
    double fNPSR2InSR2=real_fNPSR2InSR2->getVal();
    cout<<"fNPSR2InSR2    = "<<fNPSR2InSR2   <<endl;

    RooRealVar var_fPRLSBInLSB("var_fPRLSBInLSB","var_fPRLSBInLSB",fPRLSBInLSB); if(!ws->var("var_fPRLSBInLSB")) ws->import(var_fPRLSBInLSB); else ws->var("var_fPRLSBInLSB")->setVal(fPRLSBInLSB);
    RooRealVar var_fNPLSBInLSB("var_fNPLSBInLSB","var_fNPLSBInLSB",fNPLSBInLSB); if(!ws->var("var_fNPLSBInLSB")) ws->import(var_fNPLSBInLSB); else ws->var("var_fNPLSBInLSB")->setVal(fNPLSBInLSB);
    RooRealVar var_fPRRSBInRSB("var_fPRRSBInRSB","var_fPRRSBInRSB",fPRRSBInRSB); if(!ws->var("var_fPRRSBInRSB")) ws->import(var_fPRRSBInRSB); else ws->var("var_fPRRSBInRSB")->setVal(fPRRSBInRSB);
    RooRealVar var_fNPRSBInRSB("var_fNPRSBInRSB","var_fNPRSBInRSB",fNPRSBInRSB); if(!ws->var("var_fNPRSBInRSB")) ws->import(var_fNPRSBInRSB); else ws->var("var_fNPRSBInRSB")->setVal(fNPRSBInRSB);
    RooRealVar var_fPRSR1InSR1("var_fPRSR1InSR1","var_fPRSR1InSR1",fPRSR1InSR1); if(!ws->var("var_fPRSR1InSR1")) ws->import(var_fPRSR1InSR1); else ws->var("var_fPRSR1InSR1")->setVal(fPRSR1InSR1);
    RooRealVar var_fNPSR1InSR1("var_fNPSR1InSR1","var_fNPSR1InSR1",fNPSR1InSR1); if(!ws->var("var_fNPSR1InSR1")) ws->import(var_fNPSR1InSR1); else ws->var("var_fNPSR1InSR1")->setVal(fNPSR1InSR1);
    RooRealVar var_fPRSR2InSR2("var_fPRSR2InSR2","var_fPRSR2InSR2",fPRSR2InSR2); if(!ws->var("var_fPRSR2InSR2")) ws->import(var_fPRSR2InSR2); else ws->var("var_fPRSR2InSR2")->setVal(fPRSR2InSR2);
    RooRealVar var_fNPSR2InSR2("var_fNPSR2InSR2","var_fNPSR2InSR2",fNPSR2InSR2); if(!ws->var("var_fNPSR2InSR2")) ws->import(var_fNPSR2InSR2); else ws->var("var_fNPSR2InSR2")->setVal(fNPSR2InSR2);

    RooRealVar var_nPRChic1InPRSR1("var_nPRChic1InPRSR1","var_nPRChic1InPRSR1",nPRChic1InPRSR1); var_nPRChic1InPRSR1.setError(nPRChic1InPRSR1_Err); if(!ws->var("var_nPRChic1InPRSR1")) ws->import(var_nPRChic1InPRSR1); else {ws->var("var_nPRChic1InPRSR1")->setVal(nPRChic1InPRSR1); ws->var("var_nPRChic1InPRSR1")->setError(nPRChic1InPRSR1_Err);}
    RooRealVar var_nPRChic2InPRSR2("var_nPRChic2InPRSR2","var_nPRChic2InPRSR2",nPRChic2InPRSR2); var_nPRChic2InPRSR2.setError(nPRChic2InPRSR2_Err); if(!ws->var("var_nPRChic2InPRSR2")) ws->import(var_nPRChic2InPRSR2); else {ws->var("var_nPRChic2InPRSR2")->setVal(nPRChic2InPRSR2); ws->var("var_nPRChic2InPRSR2")->setError(nPRChic2InPRSR2_Err);}





   //ws->var("var_sig1MaxMass")->setError(err_sig1MaxMass);


    std::cout<< "write ws" <<std::endl;
    ws->Write();
    std::cout<< "print ws" <<std::endl;
    ws->Print("v");

    infile->Close();

}




