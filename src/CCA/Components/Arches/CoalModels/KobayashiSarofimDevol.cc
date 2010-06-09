#include <CCA/Components/Arches/CoalModels/KobayashiSarofimDevol.h>
#include <CCA/Components/Arches/TransportEqns/EqnFactory.h>
#include <CCA/Components/Arches/TransportEqns/EqnBase.h>
#include <CCA/Components/Arches/TransportEqns/DQMOMEqn.h>
#include <CCA/Components/Arches/ArchesLabel.h>
#include <CCA/Components/Arches/Directives.h>

#include <Core/ProblemSpec/ProblemSpec.h>
#include <CCA/Ports/Scheduler.h>
#include <Core/Grid/SimulationState.h>
#include <Core/Grid/Variables/VarTypes.h>
#include <Core/Grid/Variables/CCVariable.h>
#include <Core/Exceptions/InvalidValue.h>
#include <Core/Parallel/Parallel.h>

//===========================================================================

using namespace std;
using namespace Uintah; 

//---------------------------------------------------------------------------
// Builder:
KobayashiSarofimDevolBuilder::KobayashiSarofimDevolBuilder( const std::string         & modelName,
                                                            const vector<std::string> & reqICLabelNames,
                                                            const vector<std::string> & reqScalarLabelNames,
                                                            const ArchesLabel         * fieldLabels,
                                                            SimulationStateP          & sharedState,
                                                            int qn ) :
  ModelBuilder( modelName, reqICLabelNames, reqScalarLabelNames, fieldLabels, sharedState, qn )
{
}

KobayashiSarofimDevolBuilder::~KobayashiSarofimDevolBuilder(){}

ModelBase* KobayashiSarofimDevolBuilder::build() {
  return scinew KobayashiSarofimDevol( d_modelName, d_sharedState, d_fieldLabels, d_icLabels, d_scalarLabels, d_quadNode );
}
// End Builder
//---------------------------------------------------------------------------

KobayashiSarofimDevol::KobayashiSarofimDevol( std::string modelName, 
                                              SimulationStateP& sharedState,
                                              const ArchesLabel* fieldLabels,
                                              vector<std::string> icLabelNames, 
                                              vector<std::string> scalarLabelNames,
                                              int qn ) 
: Devolatilization(modelName, sharedState, fieldLabels, icLabelNames, scalarLabelNames, qn)
{
  // Values from Ubhayakar (1976):
  A1  =  3.7e5;       // [=] 1/s; k1 pre-exponential factor
  A2  =  1.46e13;     // [=] 1/s; k2 pre-exponential factor
  E1  =  17600;      // [=] kcal/kmol;  k1 activation energy
  E2  =  60000;      // [=] kcal/kmol;  k2 activation energy

  /*
  // Values from Kobayashi (1976):
  A1  =  2.0e5;       // [=] 1/s; pre-exponential factor for k1
  A2  =  1.3e7;       // [=] 1/s; pre-exponential factor for k2
  E1  =  -25000;      // [=] kcal/kmol;  k1 activation energy
  E2  =  -40000;      // [=] kcal/kmol;  k2 activation energy
  */

  R   =  1.987;       // [=] kcal/kmol; ideal gas constant

  // Y values from white book:
  Y1_ = 0.3; // volatile fraction from proximate analysis
  Y2_ = 1.0; // fraction devolatilized at higher temperatures

  /*
  // Y values from Ubhayakar (1976):
  Y1_ = 0.39; // volatile fraction from proximate analysis
  Y2_ = 0.80; // fraction devolatilized at higher temperatures
  */

  compute_part_temp = false;

}

KobayashiSarofimDevol::~KobayashiSarofimDevol()
{}

//-----------------------------------------------------------------------------
//Problem Setup
//-----------------------------------------------------------------------------
void 
KobayashiSarofimDevol::problemSetup(const ProblemSpecP& params)
{
  // call parent's method first
  Devolatilization::problemSetup(params);

  ProblemSpecP db = params; 
  compute_part_temp = false;

  string label_name;
  string role_name;
  string temp_label_name;
  
  string temp_ic_name;
  string temp_ic_name_full;

  // -----------------------------------------------------------------
  // Look for required internal coordinates
  ProblemSpecP db_icvars = params->findBlock("ICVars");
  if (db_icvars) {
    for (ProblemSpecP variable = db_icvars->findBlock("variable"); variable != 0; variable = variable->findNextBlock("variable") ) {

      variable->getAttribute("label",label_name);
      variable->getAttribute("role", role_name);

      temp_label_name = label_name;
      
      std::stringstream out;
      out << d_quadNode;
      string node = out.str();
      temp_label_name += "_qn";
      temp_label_name += node;

      // user specifies "role" of each internal coordinate

      if ( role_name == "raw_coal_mass" ) {
        LabelToRoleMap[temp_label_name] = role_name;
      } else if( role_name == "particle_temperature" ) {  
        LabelToRoleMap[temp_label_name] = role_name;
        compute_part_temp = true;
      } else {
        std::string errmsg;
        errmsg = "Invalid variable role for Kobayashi Sarofim Devolatilization model: must be \"particle_temperature\" or \"raw_coal_mass\", you specified \"" + role_name + "\".";
        throw InvalidValue(errmsg,__FILE__,__LINE__);
      }
    }
  }

  // fix the d_icLabels to point to the correct quadrature node (since there is 1 model per quad node)
  for ( vector<std::string>::iterator iString = d_icLabels.begin(); 
        iString != d_icLabels.end(); ++iString) {

    temp_ic_name        = (*iString);
    temp_ic_name_full   = temp_ic_name;

    std::stringstream out;
    out << d_quadNode;
    string node = out.str();
    temp_ic_name_full += "_qn";
    temp_ic_name_full += node;

    std::replace( d_icLabels.begin(), d_icLabels.end(), temp_ic_name, temp_ic_name_full);
  }

  DQMOMEqnFactory& dqmom_eqn_factory = DQMOMEqnFactory::self();

  // assign labels for each required internal coordinate
  for (vector<std::string>::iterator iter = d_icLabels.begin(); 
      iter != d_icLabels.end(); iter++) { 

    map<string, string>::iterator iMap = LabelToRoleMap.find(*iter);

    if ( iMap != LabelToRoleMap.end() ) {
      if ( iMap->second == "particle_temperature") {
        if (dqmom_eqn_factory.find_scalar_eqn(*iter) ) {
          EqnBase& t_current_eqn = dqmom_eqn_factory.retrieve_scalar_eqn(*iter);
          DQMOMEqn& current_eqn = dynamic_cast<DQMOMEqn&>(t_current_eqn);
          d_particle_temperature_label = current_eqn.getTransportEqnLabel();
          d_pt_scaling_factor = current_eqn.getScalingConstant();
        } else {
          std::string errmsg = "ARCHES: KobayashiSarofimDevol: Invalid variable given in <variable> tag for KobayashiSarofimDevol model";
          errmsg += "\nCould not find given particle temperature variable \"";
          errmsg += *iter;
          errmsg += "\" in EqnFactory or in DQMOMEqnFactory.";
          throw InvalidValue(errmsg,__FILE__,__LINE__);
        }

      } else if ( iMap->second == "raw_coal_mass") {
        if (dqmom_eqn_factory.find_scalar_eqn(*iter) ) {
          EqnBase& t_current_eqn = dqmom_eqn_factory.retrieve_scalar_eqn(*iter);
          DQMOMEqn& current_eqn = dynamic_cast<DQMOMEqn&>(t_current_eqn);
          d_raw_coal_mass_label = current_eqn.getTransportEqnLabel();
          d_rc_scaling_factor = current_eqn.getScalingConstant();
        } else {
          std::string errmsg = "ARCHES: KobayashiSarofimDevol: Invalid variable given in <variable> tag for KobayashiSarofimDevol model";
          errmsg += "\nCould not find given raw coal mass variable \"";
          errmsg += *iter;
          errmsg += "\" in DQMOMEqnFactory.";
          throw InvalidValue(errmsg,__FILE__,__LINE__);
        }
      }

    } else {
      // can't find this required variable in the labels-to-roles map!
      std::string errmsg = "ARCHES: KobayashiSarofimDevol: You specified that the variable \"" + *iter + 
                           "\" was required, but you did not specify a role for it!\n";
      throw InvalidValue( errmsg, __FILE__, __LINE__);
    }
  }

}



//-----------------------------------------------------------------------------
//Schedule the calculation of the Model 
//-----------------------------------------------------------------------------
void 
KobayashiSarofimDevol::sched_computeModel( const LevelP& level, SchedulerP& sched, int timeSubStep )
{
  std::string taskname = "KobayashiSarofimDevol::computeModel";
  Task* tsk = scinew Task(taskname, this, &KobayashiSarofimDevol::computeModel);

  Ghost::GhostType gn = Ghost::None;

  d_timeSubStep = timeSubStep; 

  if (d_timeSubStep == 0 && !d_labelSchedInit) {
    // Every model term needs to set this flag after the varLabel is computed. 
    // transportEqn.cleanUp should reinitialize this flag at the end of the time step. 
    d_labelSchedInit = true;

    tsk->computes(d_modelLabel);
    tsk->computes(d_gasLabel); 
  } else {
    tsk->modifies(d_modelLabel);
    tsk->modifies(d_gasLabel);  
  }

  tsk->requires(Task::OldDW, d_weight_label, gn, 0);

  tsk->requires(Task::OldDW, d_raw_coal_mass_label, gn, 0);

  if(compute_part_temp) {
    tsk->requires(Task::OldDW, d_particle_temperature_label, gn, 0);
  } else {
    tsk->requires(Task::OldDW, d_fieldLabels->d_tempINLabel, gn, 0);
  }

  sched->addTask(tsk, level->eachPatch(), d_sharedState->allArchesMaterials()); 

}



//-----------------------------------------------------------------------------
//Actually compute the source term 
//-----------------------------------------------------------------------------
void
KobayashiSarofimDevol::computeModel( const ProcessorGroup * pc, 
                                     const PatchSubset    * patches, 
                                     const MaterialSubset * matls, 
                                     DataWarehouse        * old_dw, 
                                     DataWarehouse        * new_dw )
{
  for( int p=0; p < patches->size(); p++ ) {  // Patch loop

    //Ghost::GhostType  gaf = Ghost::AroundFaces;
    Ghost::GhostType  gac = Ghost::AroundCells;
    Ghost::GhostType  gn  = Ghost::None;

    const Patch* patch = patches->get(p);
    int archIndex = 0;
    int matlIndex = d_fieldLabels->d_sharedState->getArchesMaterial(archIndex)->getDWIndex(); 

    CCVariable<double> devol_rate;
    if( new_dw->exists( d_modelLabel, matlIndex, patch ) ) {
      new_dw->getModifiable( devol_rate, d_modelLabel, matlIndex, patch ); 
    } else {
      new_dw->allocateAndPut( devol_rate, d_modelLabel, matlIndex, patch );
      devol_rate.initialize(0.0);
    }

    CCVariable<double> gas_devol_rate; 
    if (new_dw->exists( d_gasLabel, matlIndex, patch )){
      new_dw->getModifiable( gas_devol_rate, d_gasLabel, matlIndex, patch ); 
    } else {
      new_dw->allocateAndPut( gas_devol_rate, d_gasLabel, matlIndex, patch ); 
      gas_devol_rate.initialize(0.0);
    }

    constCCVariable<double> temperature; // holds gas OR particle temperature...
    if (compute_part_temp) {
      old_dw->get( temperature, d_particle_temperature_label, matlIndex, patch, gn, 0 );
    } else {
      old_dw->get( temperature, d_fieldLabels->d_tempINLabel, matlIndex, patch, gac, 1 );
    }
 
    constCCVariable<double> wa_raw_coal_mass;
    old_dw->get( wa_raw_coal_mass, d_raw_coal_mass_label, matlIndex, patch, gn, 0 );

    constCCVariable<double> weight;
    old_dw->get( weight, d_weight_label, matlIndex, patch, gn, 0 );

#if !defined(VERIFY_KOBAYASHI_MODEL)

    for (CellIterator iter=patch->getCellIterator(); !iter.done(); iter++){

      IntVector c = *iter; 

      // weight - check if small
      bool weight_is_small = (weight[c] < d_w_small);

      double unscaled_weight;
      if (weight_is_small) {
        unscaled_weight = 0.0;
      } else {
        unscaled_weight = weight[c]*d_w_scaling_factor;
      }

      // temperature - particle or gas
      double unscaled_temperature;
      if (weight_is_small) {
        // no particles
        unscaled_temperature = 0.0;
      } else {
        if (compute_part_temp) {
          // particle temp
          unscaled_temperature = temperature[c]*d_pt_scaling_factor/weight[c];
        } else {
          // gas temp
          unscaled_temperature = temperature[c];
        }
      }
 
      // raw coal mass - de-scaled, de-weighted
      double scaled_raw_coal_mass;
      double unscaled_raw_coal_mass;
      if (weight_is_small) {
        // no particles
        scaled_raw_coal_mass = 0.0;
        unscaled_raw_coal_mass = 0.0;
      } else {
        scaled_raw_coal_mass = wa_raw_coal_mass[c]/weight[c];
        unscaled_raw_coal_mass = scaled_raw_coal_mass*d_rc_scaling_factor;
      }

      // devol_rate: particle source
      double devol_rate_;

      // gase_devol_rate: gas source
      double gas_devol_rate_;

#else
      bool weight_is_small = false;
      double unscaled_weight = 1e6;
      double unscaled_temperature = 2000;
      double unscaled_raw_coal_mass = 1e-8;
      double scaled_raw_coal_mass = unscaled_raw_coal_mass;
      double devol_rate_;
      double gas_devol_rate_;
#endif

      if (weight_is_small) {
        devol_rate_ = 0.0;
        gas_devol_rate_ = 0.0;
      } else {
        k1 = A1*exp(-E1/(R*unscaled_temperature)); // [=] 1/s
        k2 = A2*exp(-E2/(R*unscaled_temperature)); // [=] 1/s
        
        double testVal_part = -(k1+k2)*scaled_raw_coal_mass;
        if( testVal_part < -1e-16 ) {
          devol_rate_ = testVal_part;
        } else {
          devol_rate_ = 0.0;
        }

        double testVal_gas = (Y1_*k1 + Y2_*k2)*unscaled_raw_coal_mass*unscaled_weight;
        if( testVal_gas > 1e-16 ) {
          gas_devol_rate_ = testVal_gas;
        } else {
          gas_devol_rate_ = 0.0;
        }

      }

#if defined(VERIFY_KOBAYASHI_MODEL)
      proc0cout << "****************************************************************" << endl;
      proc0cout << "Verification error, Kobayashi-Sarofim Devolatilization model:   " << endl;
      proc0cout << endl;

      double error; 
      
      error = ( (-0.04053)-(devol_rate_) )/(-0.04053);
      if( fabs(error) < 0.01 ) {
        proc0cout << "Verification for particle model term successful:" << endl;
        proc0cout << "    Percent error is " << fabs(error)*100 << ", which is less than 1 percent." << endl;
      } else {
        proc0cout << "WARNING: VERIFICATION FOR PARTICLE MODEL TERM FAILED!!! " << endl;
        proc0cout << "    Verification value  = -0.04053" << endl;
        proc0cout << "    Calculated value    = " << devol_rate_ << endl;
        proc0cout << "    Percent error = " << fabs(error)*100 << ", which is greater than 1 percent." << endl;
      }

      proc0cout << endl;

      error = ( (40500.3) - (gas_devol_rate_) )/(40500.3);
      if( fabs(error) < 0.01 ) {
        proc0cout << "Verification for gas model term successful:" << endl;
        proc0cout << "    Percent error = " << fabs(error)*100 << ", which is less than 1 percent." << endl;
      } else {
        proc0cout << "WARNING: VERIFICATION FOR GAS MODEL TERM FAILED!!! " << endl;
        proc0cout << "    Verification value  = 40500.3" << endl;
        proc0cout << "    Calculated value    = " << gas_devol_rate_ << endl;
        proc0cout << "    Percent error = " << fabs(error)*100 << ", which is greater than 1 percent." << endl;
      }

      proc0cout << endl;
      proc0cout << "****************************************************************" << endl;

#else
      devol_rate[c] = devol_rate_;
      gas_devol_rate[c] = gas_devol_rate_;

    }//end cell loop
#endif
  
  }//end patch loop
}



//---------------------------------------------------------------------------
//Actually do the dummy initialization
//---------------------------------------------------------------------------
/*
@details
This method intentionally left blank. 

@seealso Devolatilization::dummyInit
*/
void
KobayashiSarofimDevol::dummyInit( const ProcessorGroup* pc,
                                  const PatchSubset* patches, 
                                  const MaterialSubset* matls, 
                                  DataWarehouse* old_dw, 
                                  DataWarehouse* new_dw )
{
}



//---------------------------------------------------------------------------
//Schedule the initialization of some variables 
//---------------------------------------------------------------------------
/**
@details
This method intentionally left blank. 

@seealso Devolatilization::sched_initVars
*/
void 
KobayashiSarofimDevol::sched_initVars( const LevelP& level, SchedulerP& sched )
{
}



//---------------------------------------------------------------------------
//Initialize variables
//---------------------------------------------------------------------------
/**
@details
This method intentionally left blank. 

@seealso Devolatilization::initVars
*/
void
KobayashiSarofimDevol::initVars( const ProcessorGroup * pc, 
                                 const PatchSubset    * patches, 
                                 const MaterialSubset * matls, 
                                 DataWarehouse        * old_dw, 
                                 DataWarehouse        * new_dw )
{
}

