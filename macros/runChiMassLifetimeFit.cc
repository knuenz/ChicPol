/*
 * runChiMassLifetimeFit.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: valentinknuenz
 */

#ifndef __CINT__
#endif

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

#include "chiMassLifetimeFit.cc"
#include "TROOT.h"

//========================================================
// code to read input arguments
template<typename T>
void fromSplit(const std::string& key, const std::string &arg, T& out)
{
  const char delim = '=';
  // Skip if key or delimiter not there
  if ((arg.find(key) == std::string::npos) ||
      (arg.find(delim) == std::string::npos))
    return;

  std::string skey, sval;
  std::stringstream sstr(arg);
  std::getline(sstr, skey, delim); // Dummy read to skip key
  std::getline(sstr, sval, delim); // Get value
  T tout;
  if (!(std::istringstream(sval) >> std::boolalpha >> tout))
    return;
  out = tout;
  std::cout << std::boolalpha << skey << ": "  << out << std::endl;
}

// Special version for string without the conversion
template<>
void fromSplit(const std::string& key, const std::string &arg, std::string &out)
{
  const char delim = '=';
  // Skip if key or delimiter not there
  if ((arg.find(key) == std::string::npos) ||
      (arg.find(delim) == std::string::npos))
    return;
  std::string skey, sval;
  std::stringstream sstr(arg);
  std::getline(sstr, skey, delim); // Dummy read to skip key
  std::getline(sstr, sval, delim); // Get value
  out = sval;
  std::cout << skey << ": "  << out << std::endl;
}


//===================================================
int main(int argc, char* argv[]){

  // Set defaults
    int rapMin = 999,
			 	rapMax = 999,
			 	ptMin = 999,
			 	ptMax = 999,
			 	nState = 999;

	 bool runChiMassFitOnly = false;
 	 bool MC = false;

    // Loop over argument list
    for (int i=1; i < argc; i++)
      {
	std::string arg = argv[i];
    	fromSplit("runChiMassFitOnly", arg, runChiMassFitOnly);
        fromSplit("rapMin", arg, rapMin);
        fromSplit("rapMax", arg, rapMax);
        fromSplit("ptMin", arg, ptMin);
        fromSplit("ptMax", arg, ptMax);
        fromSplit("nState", arg, nState);
        fromSplit("MC", arg, MC);
      }

    std::cout << "-----------------------\n"
	      << "Fitting mass/lifetime for \n"
	      << "y bins " << rapMin << " - " << rapMax << "\n"
	      << "and pT bins "  << ptMin << " - " << ptMax << "\n"
	      << "-----------------------" << std::endl;

    for(int iRap = rapMin; iRap <= rapMax; iRap++){
      for(int iPT = ptMin; iPT <= ptMax; iPT++){

    		std::stringstream tempFrom;
    		tempFrom << "tmpFiles/backupWorkSpace/ws_MassLifetimeFit_Jpsi_rap" << iRap << "_pt" << iPT << ".root";
    		const std::string infilenameFrom = tempFrom.str().c_str();

    		std::stringstream tempTo;
    		tempTo << "tmpFiles/backupWorkSpace/ws_MassLifetimeFit_Chi_rap" << iRap << "_pt" << iPT << ".root";
    		const std::string infilenameTo = tempTo.str().c_str();

    		cout<<"copy file "<<infilenameFrom.c_str()<<"to "<<infilenameTo.c_str()<<endl;
    		gSystem->CopyFile(infilenameFrom.c_str(),infilenameTo.c_str(),kTRUE);
    		cout<<"copy file finished"<<endl;

      	  	chiMassLifetimeFit(infilenameTo.c_str(), iRap, iPT, nState, runChiMassFitOnly, MC);

      }
    }

    return 0;
}



