//----- NewStaticMixingTable.cc --------------------------------------------------

#include <Packages/Uintah/CCA/Components/Arches/Mixing/NewStaticMixingTable.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/InletStream.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/KDTree.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/VectorTable.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/ReactionModel.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/StanjanEquilibriumReactionModel.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/ILDMReactionModel.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/ChemkinInterface.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/MixRxnTable.h>
#include <Packages/Uintah/CCA/Components/Arches/Mixing/MixRxnTableInfo.h>
#include <Packages/Uintah/Core/ProblemSpec/ProblemSpecP.h>
#include <Packages/Uintah/Core/ProblemSpec/ProblemSpec.h>
#include <Packages/Uintah/Core/Exceptions/InvalidValue.h>
#include <iostream>
#include <math.h>
#include <Core/Math/MiscMath.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace Uintah;
using namespace SCIRun;
//****************************************************************************
// Default constructor for NewStaticMixingTable
//****************************************************************************
NewStaticMixingTable::NewStaticMixingTable():MixingModel()
{
	//srand ( time(NULL) );
}

//****************************************************************************
// Destructor
//****************************************************************************
NewStaticMixingTable::~NewStaticMixingTable()
{
}

//****************************************************************************
// Problem Setup for NewStaticMixingTable
//****************************************************************************
void 
NewStaticMixingTable::problemSetup(const ProblemSpecP& params)
{
  std::string d_inputfile;
  ProblemSpecP db = params->findBlock("NewStaticMixingTable");
  db->require("adiabatic",d_adiabatic);
  db->require("rxnvars",d_numRxnVars);
  db->require("mixstatvars",d_numMixStatVars);
  db->require("inputfile",d_inputfile);
  if ((db->findBlock("h_fuel"))&&(db->findBlock("h_air"))) {
    db->require("h_fuel",d_H_fuel);
    db->require("h_air",d_H_air);
    d_adiab_enth_inputs = true;
  }
  else
    d_adiab_enth_inputs = false;
  // Set up for MixingModel table
  d_numMixingVars = 1;
  // Define mixing table, which includes call reaction model constructor
  readMixingTable(d_inputfile);
}

      

//****************************************************************************
// Actually compute the properties here
//****************************************************************************
void 
NewStaticMixingTable::computeProps(const InletStream& inStream,
			     Stream& outStream)
{
  double small=1.0e-10;
  // Extracting the independent variables from the in stream 
  double mixFrac = inStream.d_mixVars[0];
  double mixFracVars = 0.0;
  if (inStream.d_mixVarVariance.size() != 0)
    mixFracVars = inStream.d_mixVarVariance[0];
  if(mixFrac > 1.0)
	mixFrac=1.0;
  else if (mixFrac < small)
	mixFrac=0.0;
  if(mixFracVars < small)
	mixFracVars=0.0;
  double var_limit=mixFracVars/((mixFrac*(1.0-mixFrac))+small);
  if(var_limit > 0.9)
  	mixFracVars=(2.0/3.0)*mixFracVars;
  // Heat loss for adiabatic case
  double current_heat_loss=0.0;
  double zero_heat_loss=0.0;
  double zero_mixFracVars=0.0;
  //Absolute enthalpy
  double enthalpy=0.0;
  // Adiabatic enthalpy
  double adia_enthalpy=0.0;
  double interp_adiab_enthalpy = 0.0;
  if (d_adiab_enth_inputs)
    interp_adiab_enthalpy = d_H_fuel*mixFrac+d_H_air*(1.0-mixFrac);
  // Sensible enthalpy
  double sensible_enthalpy=0.0;
  if(!d_adiabatic){
	sensible_enthalpy=tableLookUp(mixFrac, mixFracVars, zero_heat_loss, Hs_index);
	enthalpy=inStream.d_enthalpy;
	if (!(Enthalpy_index == -1))
          adia_enthalpy=tableLookUp(mixFrac, zero_mixFracVars, zero_heat_loss, Enthalpy_index);
	else if (d_adiab_enth_inputs)
	  adia_enthalpy=interp_adiab_enthalpy;
	else {
	  cout << "No way provided to compute adiabatic enthalpy" << endl;
	  exit (1);
	}

        if (inStream.d_initEnthalpy)
          	current_heat_loss = zero_heat_loss;
        else
  		current_heat_loss=(adia_enthalpy-enthalpy)/(sensible_enthalpy+small);
        if(current_heat_loss < -1.0 || current_heat_loss > 1.0){
		cout<< "Heat loss is exceeding the bounds: "<<current_heat_loss << endl;
		cout<< "Absolute enthalpy is : "<< enthalpy << endl;
		cout<< "Adiabatic enthalpy is : "<< adia_enthalpy << endl;
		cout<< "Sensible enthalpy is : "<< sensible_enthalpy << endl;
  		cout<< "Mixture fraction is :  "<< mixFrac << endl;
  		cout<< "Mixture fraction variance is :  "<< mixFracVars << endl;
	}
	if(fabs(current_heat_loss) < small)
		current_heat_loss = zero_heat_loss;
        if(current_heat_loss > heatLoss[d_heatlosscount-1])
		current_heat_loss = heatLoss[d_heatlosscount-1];
	else if (current_heat_loss < heatLoss[0])
		current_heat_loss = heatLoss[0];
  }
  // Looking for the properties corresponding to the heat loss
  
  outStream.d_temperature=tableLookUp(mixFrac, mixFracVars, current_heat_loss, T_index);  
  outStream.d_density=tableLookUp(mixFrac, mixFracVars, current_heat_loss, Rho_index);  
  outStream.d_cp=tableLookUp(mixFrac, mixFracVars, current_heat_loss, Cp_index);  
  if (!(Enthalpy_index == -1))
    outStream.d_enthalpy=tableLookUp(mixFrac, mixFracVars, current_heat_loss, Enthalpy_index);  
  else if (d_adiab_enth_inputs)
    outStream.d_enthalpy=interp_adiab_enthalpy;
  else
    outStream.d_enthalpy=0.0;
  outStream.d_co2=tableLookUp(mixFrac, mixFracVars, current_heat_loss, co2_index);  
  outStream.d_h2o=tableLookUp(mixFrac, mixFracVars, current_heat_loss, h2o_index);  
  /*if((outStream.d_temperature - 293.0) <= -0.01 || (outStream.d_density - 1.20002368329336) >= 0.001){
  	cout<<"Temperature for properties outbound is:  "<<outStream.d_temperature<<endl;
  	cout<<"Density for properties outbound is:  "<<outStream.d_density<<endl;
  	cout<<"Mixture fraction for properties outbound  is :  "<<mixFrac<<endl;
  	cout<<"Mixture fraction variance for properties outbound is :  "<<mixFracVars<<endl;
  	cout<<"Heat loss for properties outbound is :  "<<current_heat_loss<<endl;
  }*/
}



double NewStaticMixingTable::tableLookUp(double mixfrac, double mixfracVars, double current_heat_loss, int var_index)
{
  double small = 1.0e-10;
  // compute index
  // nx - mixfrac, ny - mixfracVars, var_index - index of the variable
  // Enthalpy loss lookup
  double fmg, fpmg,s1,s2,var_value;
  int nhl_lo=0, nhl_hi=0;
  double dhl_lo=0.0, dhl_hi=0.0;
  if(current_heat_loss==0.0  && d_heatlosscount ==1){
	nhl_lo=0;
	nhl_hi=0;
        dhl_lo=0.0;
        dhl_hi=1.0;
  }
  else{
	for(int hl_index=0; hl_index < d_heatlosscount-1; hl_index++){
		dhl_lo = heatLoss[hl_index]-current_heat_loss;
		dhl_hi = heatLoss[hl_index+1]-current_heat_loss;
		if((dhl_lo*dhl_hi) == 0.0 && hl_index != 0){
			nhl_lo=hl_index+1;
			nhl_hi=nhl_lo;
                        break;
		}
		else if((dhl_lo*dhl_hi) <= 0.0){
			nhl_lo=hl_index;
			nhl_hi=nhl_lo+1;
                        break;
		}
	}
  }
  /*cout<< "nhl_lo is :" << nhl_lo<<endl;
  cout<< "nhl_hi is :" << nhl_hi<<endl;
  cout<< "dhl_lo is :" << dhl_lo<<endl;
  cout<< "dhl_hi is :" << dhl_hi<<endl;*/

  // Main loop
  for(int m_index = nhl_lo; m_index <= nhl_hi; m_index++){ 
  int nx_lo, nx_hi;

  //Non-unifrom mixture fraction 
  double df1,df2; 
  for(int index=0; index < d_mixfraccount-1; index++){
	df1 = meanMix[var_index][index]-mixfrac;
	df2 = meanMix[var_index][index+1]-mixfrac;
	if((df1*df2) == 0.0 && index != 0){
		nx_lo=index+1;
		nx_hi=nx_lo;
                break;
	}
	else if((df1*df2) <= 0.0){
		nx_lo=index;
		nx_hi=nx_lo+1;
                break;
	}
  }

  //cout<<"nx_lo index is  : "<<nx_lo<<endl;
  //cout<<"nx_hi index is  : "<<nx_hi<<endl;

  
  // variance is uniform table
  double max_curr_Zvar = mixfrac*(1.0-mixfrac);
  double max_Zvar = min(mixfracVars,max_curr_Zvar);
  double g, gi1, gp, gi2;
  //Index for variances
  int k1,k2,k1p,k2p;
  //Weighing factors for variance
  double dk1,dk2,dk1p,dk2p;
  if(meanMix[var_index][nx_lo]<=small || fabs(meanMix[var_index][nx_lo]-1.0)<=small){
	// Set the values to get the first entry
	g=0.0;
        k1=0;
	k1p=1;
	dk1=0.0;
	dk1p=1.0;
  }
  else{
	gi1=max_Zvar*meanMix[var_index][nx_lo]*(1.0-meanMix[var_index][nx_lo])/(max_curr_Zvar+small);
	// Normalizing
        g=gi1/(meanMix[var_index][nx_lo]*(1.0-meanMix[var_index][nx_lo]));
	// Finding the table entry
  	for(int index=0; index < d_mixvarcount-1; index++){
		dk1 = variance[index]-g;
		dk1p = variance[index+1]-g;
		if((dk1*dk1p) == 0.0 && index != 0){
			k1=index+1;
			k1p=k1;
                	break;
		}
		else if((dk1*dk1p) <= 0.0){
			k1=index;
			k1p=k1+1;
                	break;
		}
  	}
  }

  if(meanMix[var_index][nx_hi]<=small || fabs(meanMix[var_index][nx_hi]-1.0)<=small){
	// Set the values to get the first entry
	gp=0.0;
        k2=0;
	k2p=1;
	dk2=0.0;
	dk2p=1.0;
  }
  else{
	gi2=max_Zvar*meanMix[var_index][nx_hi]*(1.0-meanMix[var_index][nx_hi])/(max_curr_Zvar+small);
	// Normalizing
        gp=gi2/(meanMix[var_index][nx_hi]*(1.0-meanMix[var_index][nx_hi]));
	// Finding the table entry
  	for(int index=0; index < d_mixvarcount-1; index++){
		dk2 = variance[index]-gp;
		dk2p = variance[index+1]-gp;
		if((dk2*dk2p) == 0.0 && index != 0){
			k2=index+1;
			k2p=k2;
                	break;
		}
		else if((dk2*dk2p) <= 0.0){
			k2=index;
			k2p=k2+1;
                	break;
		}
  	}

  }
  //cout<<" G value is :"<<g<<endl;
  //cout<<"GP value is :"<<gp<<endl;
  //Interpolating the values
  fmg=(g-double(k1))*table[var_index][m_index*d_mixfraccount*d_mixvarcount+k1p*d_mixfraccount+nx_lo]-(g-double(k1p))*table[var_index][m_index*d_mixfraccount*d_mixvarcount+k1*d_mixfraccount+nx_lo];
  //cout<< "FMG is :"<<fmg<<endl;
  fpmg=(gp-double(k2))*table[var_index][m_index*d_mixfraccount*d_mixvarcount+k2p*d_mixfraccount+nx_hi]-(gp-double(k2p))*table[var_index][m_index*d_mixfraccount*d_mixvarcount+k2*d_mixfraccount+nx_hi];
  //cout<< "FPMG is : "<<fpmg<<endl;
  if(nhl_lo==nhl_hi){
	s1=(df1*fpmg-df2*fmg)/(df1-df2);
        s2=s1;
	break;
  }
  else if(m_index == nhl_lo)
  	s1=(df1*fpmg-df2*fmg)/(df1-df2);
  else
  	s2=(df1*fpmg-df2*fmg)/(df1-df2);
	
  }
  //cout<<"value of S1 is "<<s1<<endl;
  //cout<<"value of S2 is "<<s2<<endl;

  var_value = (dhl_lo*s2-dhl_hi*s1)/(dhl_lo-dhl_hi);
  return var_value; 
}


void NewStaticMixingTable::readMixingTable(std::string inputfile)
{
  cout << "Preparing to read the inputfile:   " << inputfile << endl;
  ifstream fd(inputfile.c_str());
  if(fd.fail()){
	cout<<" Unable to open the given input file " << inputfile << endl;
	exit(1);
  }
  fd >> d_indepvarscount;
  cout<< "d_indepvars count: " << d_indepvarscount << endl;
  indepvars_names = vector<string>(d_indepvarscount);
  Hl_index = -1;
  F_index = -1;
  Fvar_index = -1;
  
  for (int ii = 0; ii < d_indepvarscount; ii++) {
    fd >> indepvars_names[ii];
    if(indepvars_names[ii]== "Hl")
	    Hl_index = ii;
    else if(indepvars_names[ii]== "F")
	    F_index = ii;
    else if(indepvars_names[ii]== "Fvar")
	    Fvar_index = ii;
    cout<<indepvars_names[ii]<<endl;
  }
  
  eachindepvarcount = vector<int>(d_indepvarscount);
  for (int ii = 0; ii < d_indepvarscount; ii++)
    fd >> eachindepvarcount[ii];

  d_heatlosscount = 1;
  d_mixfraccount = 1;
  d_mixvarcount = 1;
  if (!(Hl_index == -1)) d_heatlosscount = eachindepvarcount[Hl_index];
  if (!(F_index == -1)) d_mixfraccount = eachindepvarcount[F_index];
  if (!(Fvar_index == -1)) d_mixvarcount = eachindepvarcount[Fvar_index];
  cout << d_heatlosscount << " " << d_mixfraccount << " " << d_mixvarcount << endl;

  // Total number of variables in the table: non-adaibatic table has sensibile enthalpy too
  fd >> d_varscount;
  cout<< "d_vars count: " << d_varscount << endl;
  vars_names= vector<string>(d_varscount);
  Rho_index = -1;
  T_index = -1;
  Cp_index = -1;
  Enthalpy_index = -1;
  Hs_index = -1;
  co2_index = -1;
  h2o_index = -1;
  for (int ii = 0; ii < d_varscount; ii++) {
    fd >> vars_names[ii];
    if(vars_names[ii]== "Rho" || vars_names[ii]== "density")
	    Rho_index = ii;
    else if(vars_names[ii]== "T" || vars_names[ii]== "Temp")
	    T_index = ii;
    else if(vars_names[ii]== "Cp" || vars_names[ii]== "heat_capac")
	    Cp_index = ii;
    else if(vars_names[ii]== "Entalpy")
	    Enthalpy_index = ii;
    else if(vars_names[ii]== "Hs" || vars_names[ii]== "sensible_h")
	    Hs_index = ii;
    else if(vars_names[ii]== "CO2")
	    co2_index = ii;
    else if(vars_names[ii]== "H2O")
	    h2o_index = ii;
    cout<<vars_names[ii]<<endl;
  }
  if ((Hs_index == -1)&&(!(d_adiabatic))) {
    cout << "No Hs found in table" << endl;
    exit(1);
  }
  cout<<"CO2 index is " << co2_index<<endl;
  cout<<"H2O index is " << h2o_index<<endl;

  // Not sure if we care about units in runtime, read them just in case
  vars_units= vector<string>(d_varscount);
  for (int ii = 0; ii < d_varscount; ii++) {
    fd >> vars_units[ii];
  }
  
  // Intitializing the mixture fraction table meanMix
  meanMix = vector < vector<double> >(d_varscount);
  for(int ii=0; ii<d_varscount; ii++)
	meanMix[ii]=vector<double>(d_mixfraccount);

  // Enthalpy loss vector
  heatLoss=vector<double>(d_heatlosscount);
  
  // Allocating the table space 
  int size = d_heatlosscount*d_mixfraccount*d_mixvarcount;
  table = vector <vector <double> > (d_varscount);
  for (int ii = 0; ii < d_varscount; ii++)
    table[ii] = vector <double> (size);

  // Reading heat loss values
  for (int mm=0; mm< d_heatlosscount; mm++){
	fd >> heatLoss[mm];
	//cout << heatLoss[mm] << endl;
  }
  // Non-dimensionalized variance values normalized by maximum mixture fraction: f_mean*(1.0-f_mean)
  variance=vector<double>(d_mixvarcount);
  // Reading variance values
  for (int mm=0; mm< d_mixvarcount; mm++){
	fd >> variance[mm];
	//cout << variance[mm] << endl;
  }

  //Reading the data
  // Variables loop 
  for (int kk=0; kk< d_varscount; kk++){
    // Reading mixture fraction values
    for (int ii=0; ii< d_mixfraccount; ii++){
      fd >> meanMix[kk][ii];
      //cout << meanMix[kk][ii] << "      ";
    }
    // Enthaply loss loop
    for (int mm=0; mm< d_heatlosscount; mm++){
      // Variance loop
      for (int jj=0;jj<d_mixvarcount; jj++){
  	// Mixture fraction loop 
  	for (int ii=0; ii< d_mixfraccount; ii++){
	  fd >> table[kk][mm*(d_mixfraccount*d_mixvarcount)+jj*d_mixfraccount+ii];
	  //cout << table[kk][mm*(d_mixfraccount*d_mixvarcount)+jj*d_mixfraccount+ii]<< "  ";
  	}// End of mixture fraction loop
      } // End of variance loop
    }//End of enthalpy loss loop
  }// End of Variables loop

  // Closing the file pointer
  fd.close();
  cout << "Table reading is successful" << endl;
}
