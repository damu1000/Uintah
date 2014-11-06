/*
 * The MIT License
 *
 * Copyright (c) 1997-2014 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifdef __APPLE__
// This is a hack.  gcc 3.3 #undefs isnan in the cmath header, which
// make the isnan function not work.  This define makes the cmath header
// not get included since we do not need it anyway.
#  define _CPP_CMATH
#endif
#include <CCA/Components/ICE_sm/ICE_sm.h>
#include <CCA/Components/ICE_sm/ConservationTest.h>
#include <CCA/Components/ICE_sm/Diffusion.h>
#include <CCA/Components/ICE_sm/ICEMaterial.h>
#include <CCA/Components/ICE_sm/BoundaryCond.h>
#include <CCA/Components/ICE_sm/Advection/AdvectionFactory.h>
#include <CCA/Components/ICE_sm/EOS/EquationOfState.h>
#include <CCA/Components/ICE_sm/SpecificHeatModel/SpecificHeat.h>

#include <CCA/Ports/DataWarehouse.h>
#include <CCA/Ports/Scheduler.h>
#include <Core/Grid/DbgOutput.h>
#include <Core/Grid/Patch.h>
#include <Core/Grid/Variables/PerPatch.h>
#include <Core/Grid/SimulationState.h>
#include <Core/Grid/SimulationTime.h>
#include <Core/Grid/Task.h>
#include <Core/Grid/Variables/VarTypes.h>
#include <Core/Grid/Variables/CellIterator.h>
#include <Core/Grid/BoundaryConditions/BCUtils.h>

#include <Core/Exceptions/ParameterNotFound.h>
#include <Core/Exceptions/ProblemSetupException.h>
#include <Core/Exceptions/InvalidValue.h>
#include <Core/Parallel/Parallel.h>
#include <Core/Parallel/ProcessorGroup.h>

#include <Core/Math/FastMatrix.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Util/DebugStream.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <cfloat>

using namespace std;
using namespace Uintah;

//__________________________________
//  To turn on normal output
//  setenv SCI_DEBUG "ICE_NORMAL_COUT:+,ICE_DOING_COUT:+"
//  ICE_DOING_COUT:   dumps when tasks are scheduled and performed
//  default is OF
static DebugStream iceCout("ICE_SM_DOING_COUT", false);


ICE_sm::ICE_sm(const ProcessorGroup* myworld, const bool doAMR) :
  UintahParallelComponent(myworld)
{
  lb   = scinew ICELabel();

  d_useCompatibleFluxes   = true;
  d_viscousFlow           = false;
  
  d_EVIL_NUM                = -9.99e30;                                      
  d_SMALL_NUM               = 1.0e-100;                                     
  d_clampSpecificVolume     = false;
  
  d_conservationTest         = scinew conservationTest_flags();
  d_conservationTest->onOff = false;           
  d_press_matl    = 0;
  d_press_matlSet = 0;
}
//______________________________________________________________________
//
//______________________________________________________________________
ICE_sm::~ICE_sm()
{
  iceCout << d_myworld->myrank() << " Doing: ICE destructor " << endl;

  delete d_conservationTest;
  delete lb;
  delete d_advector;
  
  if (d_press_matl && d_press_matl->removeReference()){
    delete d_press_matl;
  }
  if (d_press_matlSet && d_press_matlSet->removeReference()){
    delete d_press_matlSet;
  }
}

bool ICE_sm::restartableTimesteps()
{
  return true;
}

double ICE_sm::recomputeTimestep(double current_dt)
{
  return current_dt * 0.75;
}

/* _____________________________________________________________________
 Function~  ICE_sm::problemSetup--
_____________________________________________________________________*/
void ICE_sm::problemSetup(const ProblemSpecP& prob_spec, 
                       const ProblemSpecP& restart_prob_spec,
                       GridP& grid, 
                       SimulationStateP&   sharedState)
{
  iceCout << d_myworld->myrank() << " Doing ICE_sm::problemSetup " << "\t\t\t ICE" << endl;
  d_sharedState = sharedState;
  d_press_matl = scinew MaterialSubset();
  d_press_matl->add(0);
  d_press_matl->addReference();
  
  d_press_matlSet  = scinew MaterialSet();
  d_press_matlSet->add(0);
  d_press_matlSet->addReference();
  
  dataArchiver = dynamic_cast<Output*>(getPort("output"));
  if(!dataArchiver){
    throw InternalError("ICE:couldn't get output port", __FILE__, __LINE__);
  }


  //__________________________________
  // Pull out from CFD-ICE section
  ProblemSpecP cfd_ps = prob_spec->findBlock("CFD");

  if(!cfd_ps){
    throw ProblemSetupException(                                                                    
     "\n Could not find the <CFD> section in the input file\n",__FILE__, __LINE__);    
  }

  cfd_ps->require("cfl",d_CFL);
  
  ProblemSpecP cfd_ice_ps = cfd_ps->findBlock("ICE");
  if(!cfd_ice_ps){
    throw ProblemSetupException(                                                                    
     "\n Could not find the <CFD> <ICE> section in the input file\n",__FILE__, __LINE__);    
  }
   
  cfd_ice_ps->get("ClampSpecificVolume",        d_clampSpecificVolume);
  
  d_advector = AdvectionFactory::create(cfd_ice_ps, d_useCompatibleFluxes, d_OrderOfAdvection);
 
  
  //__________________________________
  // Pull out Initial Conditions
  ProblemSpecP mat_ps = 0;

  if (prob_spec->findBlockWithOutAttribute("MaterialProperties")){
    mat_ps = prob_spec->findBlockWithOutAttribute("MaterialProperties");
  }else if (restart_prob_spec){
    mat_ps = 
      restart_prob_spec->findBlockWithOutAttribute("MaterialProperties");
  }
  
  ProblemSpecP ice_mat_ps   = mat_ps->findBlock("ICE");  

  for (ProblemSpecP ps = ice_mat_ps->findBlock("material"); ps != 0;
    ps = ps->findNextBlock("material") ) {
    string index("");
    ps->getAttribute("index",index);
    std::stringstream id(index);

    const int DEFAULT_VALUE = -1;

    int index_val = DEFAULT_VALUE;
    id >> index_val;

    if( !id ) {
      // stringstream parsing failed... on many (most) systems, the
      // original value assigned to index_val would be left
      // intact... but on some systems it inserts garbage,
      // so we have to manually restore the value.
      index_val = DEFAULT_VALUE;
    }
    //cout_norm << "Material attribute = " << index_val << endl;

    // Extract out the type of EOS and the associated parameters
    ICEMaterial *mat = scinew ICEMaterial(ps, sharedState);
    // When doing restart, we need to make sure that we load the materials
    // in the same order that they were initially created.  Restarts will
    // ALWAYS have an index number as in <material index = "0">.
    // Index_val = -1 means that we don't register the material by its 
    // index number.
    if (index_val > -1){
      sharedState->registerICEMaterial(mat,index_val);
    }else{
      sharedState->registerICEMaterial(mat);
    } 
  }  

  //__________________________________
  //  conservationTest
  if (dataArchiver->isLabelSaved("TotalMass") ){
    d_conservationTest->mass     = true;
    d_conservationTest->onOff    = true;
  }
  if (dataArchiver->isLabelSaved("TotalMomentum") ){
    d_conservationTest->momentum = true;
    d_conservationTest->onOff    = true;
  }
  if (dataArchiver->isLabelSaved("TotalIntEng")   || 
      dataArchiver->isLabelSaved("KineticEnergy") ){
    d_conservationTest->energy   = true;
    d_conservationTest->onOff    = true;
  }
  
          
  //__________________________________
  //  boundary condition warnings
  BC_bulletproofing(prob_spec,sharedState);
}

/*______________________________________________________________________
 Function~  ICE_sm::outputProblemSpec--
 Purpose~   outputs material state
 _____________________________________________________________________*/
void ICE_sm::outputProblemSpec(ProblemSpecP& root_ps)
{
  iceCout << d_myworld->myrank() << " Doing ICE_sm::outputProblemSpec " << "\t\t\t ICE" << endl;

  ProblemSpecP root = root_ps->getRootNode();

  ProblemSpecP mat_ps = 0;
  mat_ps = root->findBlockWithOutAttribute("MaterialProperties");

  if (mat_ps == 0)
    mat_ps = root->appendChild("MaterialProperties");

  ProblemSpecP ice_ps = mat_ps->appendChild("ICE");
  for (int i = 0; i < d_sharedState->getNumICEMatls();i++) {
    ICEMaterial* mat = d_sharedState->getICEMaterial(i);
    mat->outputProblemSpec(ice_ps);
  }
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleInitialize--
 Notes:     This task actually schedules several tasks.
_____________________________________________________________________*/
void ICE_sm::scheduleInitialize(const LevelP& level,SchedulerP& sched)
{
  iceCout << d_myworld->myrank() << " Doing ICE_sm::scheduleInitialize \t\t\t\tL-"
             <<level->getIndex() << endl;
  
  Task* t = scinew Task("ICE_sm::actuallyInitialize",
                  this, &ICE_sm::actuallyInitialize);

  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  
  t->computes(lb->vel_CCLabel);
  t->computes(lb->rho_CCLabel); 
  t->computes(lb->temp_CCLabel);
  t->computes(lb->sp_vol_CCLabel);
  t->computes(lb->vol_frac_CCLabel);
  t->computes(lb->rho_micro_CCLabel);
  t->computes(lb->speedSound_CCLabel);
  t->computes(lb->thermalCondLabel);
  t->computes(lb->viscosityLabel);
  t->computes(lb->gammaLabel);
  t->computes(lb->specific_heatLabel);
  t->computes(lb->press_CCLabel,     d_press_matl, oims);
  t->computes(lb->initialGuessLabel, d_press_matl, oims); 
  const MaterialSet* ice_matls = d_sharedState->allICEMaterials();
    
  sched->addTask(t, level->eachPatch(), ice_matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm::restartInitialize--
 Purpose:   Set variables that are normally set during the initialization
            phase, but get wiped clean when you restart
_____________________________________________________________________*/
void ICE_sm::restartInitialize()
{
  iceCout << d_myworld->myrank() << " Doing restartInitialize "<< "\t\t\t ICE" << endl;
  
  //__________________________________
  // ICE: Material specific flags
  int numMatls = d_sharedState->getNumICEMatls();
  for (int m = 0; m < numMatls; m++ ) {
    ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m); 
    
    if(ice_matl->getViscosity() > 0.0){
      d_viscousFlow = true;
    }
    
  }
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputeStableTimestep--
_____________________________________________________________________*/
void ICE_sm::scheduleComputeStableTimestep(const LevelP& level,
                                      SchedulerP& sched)
{
  Task* t = 0;
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeStableTimestep \t\t\t\tL-"
             <<level->getIndex() << endl;
             
  t = scinew Task("ICE_sm::actuallyComputeStableTimestep",
            this, &ICE_sm::actuallyComputeStableTimestep);

  
  Ghost::GhostType  gac = Ghost::AroundCells;
  Ghost::GhostType  gn = Ghost::None;
  const MaterialSet* ice_matls = d_sharedState->allICEMaterials();
                            
  t->requires(Task::NewDW, lb->vel_CCLabel,        gac, 1, true);  
  t->requires(Task::NewDW, lb->speedSound_CCLabel, gac, 1, true);
  t->requires(Task::NewDW, lb->thermalCondLabel,   gn,  0, true);
  t->requires(Task::NewDW, lb->gammaLabel,         gn,  0, true);
  t->requires(Task::NewDW, lb->specific_heatLabel, gn,  0, true);   
  t->requires(Task::NewDW, lb->sp_vol_CCLabel,     gn,  0, true);   
  t->requires(Task::NewDW, lb->viscosityLabel,     gn,  0, true);        
  
  t->computes(d_sharedState->get_delt_label(),level.get_rep());
  sched->addTask(t,level->eachPatch(), ice_matls); 
}
/* _____________________________________________________________________
 Function~  ICE_sm::scheduleTimeAdvance--
_____________________________________________________________________*/
void
ICE_sm::scheduleTimeAdvance( const LevelP& level, SchedulerP& sched)
{
  MALLOC_TRACE_TAG_SCOPE("ICE_sm::scheduleTimeAdvance()");
  // for AMR, we need to reset the initial Delt otherwise some unsuspecting level will
  // get the init delt when it didn't compute delt on L0.
  
  iceCout << d_myworld->myrank() << " --------------------------------------------------------L-" 
             <<level->getIndex()<< endl;
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleTimeAdvance\t\t\t\tL-" <<level->getIndex()<< endl;
  const PatchSet* patches = level->eachPatch();
  const MaterialSet* ice_matls = d_sharedState->allICEMaterials();
  const MaterialSet* all_matls = d_sharedState->allMaterials();  

  const MaterialSubset* ice_matls_sub = ice_matls->getUnion();
                                                          
  scheduleComputeThermoTransportProperties(sched, level,  ice_matls);
  
  scheduleComputePressure(                sched, patches, d_press_matl,
                                                          all_matls);    

  scheduleComputeVel_FC(                   sched, patches,ice_matls_sub, 
                                                          d_press_matl,    
                                                          all_matls);                    

  scheduleComputeDelPressAndUpdatePressCC(sched, patches,d_press_matl,     
                                                           ice_matls_sub,   
                                                           all_matls);     

  
  scheduleComputePressFC(                 sched, patches, d_press_matl,
                                                          all_matls);

  scheduleVelTau_CC(                      sched, patches, ice_matls);

  scheduleViscousShearStress(             sched, patches, ice_matls);

  scheduleAccumulateMomentumSourceSinks(  sched, patches, d_press_matl,
                                                          ice_matls_sub,
                                                          all_matls);
                                                          
  scheduleAccumulateEnergySourceSinks(    sched, patches, ice_matls_sub,
                                                          d_press_matl,
                                                          all_matls);

  scheduleComputeLagrangianValues(        sched, patches, all_matls);

                                                           
  scheduleComputeLagrangianSpecificVolume(sched, patches, ice_matls_sub,
                                                          d_press_matl,
                                                          all_matls);
                                   
  scheduleAdvectAndAdvanceInTime(         sched, patches, ice_matls_sub,
                                                          all_matls);
                                                          
  scheduleConservedtoPrimitive_Vars(      sched, patches, ice_matls_sub,
                                                          all_matls,
                                                          "afterAdvection");
}
/* _____________________________________________________________________
 Function~  ICE_sm::scheduleFinalizeTimestep--
  This is called after scheduleTimeAdvance and the scheduleCoarsen
_____________________________________________________________________*/
void
ICE_sm::scheduleFinalizeTimestep( const LevelP& level, SchedulerP& sched)
{
  iceCout << "----------------------------"<<endl;  
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleFinalizeTimestep\t\t\t\t\tL-" <<level->getIndex()<< endl;
  const PatchSet* patches = level->eachPatch();
  const MaterialSet* ice_matls = d_sharedState->allICEMaterials();
  const MaterialSet* all_matls = d_sharedState->allMaterials();  
  const MaterialSubset* ice_matls_sub = ice_matls->getUnion();


  scheduleConservedtoPrimitive_Vars(      sched, patches, ice_matls_sub,
                                                          all_matls,
                                                          "finalizeTimestep");                                                          
                                                          
  scheduleTestConservation(               sched, patches, ice_matls_sub,
                                                          all_matls);
                                                          
  iceCout << "---------------------------------------------------------"<<endl;
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputeThermoTransportProperties--
_____________________________________________________________________*/
void ICE_sm::scheduleComputeThermoTransportProperties(SchedulerP& sched,
                                const LevelP& level,
                                const MaterialSet* ice_matls)
{ 
  Task* t;
  iceCout << d_myworld->myrank() << " ICE_sm::schedulecomputeThermoTransportProperties" 
             << "\t\t\tL-"<< level->getIndex()<< endl;
             
  t = scinew Task("ICE_sm::computeThermoTransportProperties", 
            this, &ICE_sm::computeThermoTransportProperties); 
                       
  t->requires(Task::OldDW,lb->temp_CCLabel, ice_matls->getUnion(), Ghost::None, 0);  

  t->computes(lb->viscosityLabel);
  t->computes(lb->thermalCondLabel);
  t->computes(lb->gammaLabel);
  t->computes(lb->specific_heatLabel);
  
  sched->addTask(t, level->eachPatch(), ice_matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputePressure--
_____________________________________________________________________*/
void ICE_sm::scheduleComputePressure(SchedulerP& sched,
                                  const PatchSet* patches,
                                  const MaterialSubset* press_matl,
                                  const MaterialSet* ice_matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  Task* t = 0;
  
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeEquilibrationPressure" 
             << "\t\t\tL-" << levelIndex<< endl;

  t = scinew Task("ICE_sm::computeEquilPressure_1_matl",
            this, &ICE_sm::computeEquilPressure_1_matl);       


  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  Ghost::GhostType  gn = Ghost::None;
  t->requires( Task::OldDW, lb->delTLabel, getLevel(patches));  
  t->requires( Task::OldDW, lb->press_CCLabel, press_matl, oims, gn);
  t->requires( Task::OldDW, lb->rho_CCLabel,               gn);
  t->requires( Task::OldDW, lb->temp_CCLabel,              gn); 
  t->requires( Task::OldDW, lb->sp_vol_CCLabel,            gn);
  t->requires( Task::NewDW, lb->gammaLabel,                gn);
  t->requires( Task::NewDW, lb->specific_heatLabel,        gn);
  
  t->computes( lb->f_theta_CCLabel); 
  t->computes( lb->speedSound_CCLabel);
  t->computes( lb->vol_frac_CCLabel);
  t->computes( lb->sp_vol_CCLabel);
  t->computes( lb->rho_CCLabel);
  t->computes( lb->compressibilityLabel);
  t->computes( lb->sumKappaLabel,        press_matl, oims);
  t->computes( lb->press_equil_CCLabel,  press_matl, oims);
  
  sched->addTask(t, patches, ice_matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputeVel_FC--
_____________________________________________________________________*/
void ICE_sm::scheduleComputeVel_FC(SchedulerP& sched,
                                const PatchSet* patches,
                                const MaterialSubset* ice_matls,
                                const MaterialSubset* press_matl,
                                const MaterialSet* all_matls)
{ 
  int levelIndex = getLevel(patches)->getIndex();
  Task* t = 0;

  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeVel_FC" 
             << "\t\t\t\t\tL-" << levelIndex<< endl;

  t = scinew Task("ICE_sm::computeVel_FC",
            this, &ICE_sm::computeVel_FC);

  Ghost::GhostType  gac = Ghost::AroundCells;
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  t->requires(Task::OldDW, lb->delTLabel, getLevel(patches));  
  t->requires(Task::NewDW, lb->press_equil_CCLabel, press_matl, oims, gac,1);
  t->requires(Task::NewDW,lb->sp_vol_CCLabel,    /*all_matls*/ gac,1);
  t->requires(Task::NewDW,lb->rho_CCLabel,       /*all_matls*/ gac,1);
  t->requires(Task::OldDW,lb->vel_CCLabel,         ice_matls,  gac,1);
  
  t->computes(lb->uvel_FCLabel);
  t->computes(lb->vvel_FCLabel);
  t->computes(lb->wvel_FCLabel);
  t->computes(lb->grad_P_XFCLabel);
  t->computes(lb->grad_P_YFCLabel);
  t->computes(lb->grad_P_ZFCLabel);
  sched->addTask(t, patches, all_matls);
}


/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputeDelPressAndUpdatePressCC--
_____________________________________________________________________*/
void ICE_sm::scheduleComputeDelPressAndUpdatePressCC(SchedulerP& sched,
                                            const PatchSet* patches,
                                            const MaterialSubset* press_matl,
                                            const MaterialSubset* ice_matls,
                                            const MaterialSet* matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeDelPressAndUpdatePressCC" 
             << "\t\t\tL-"<< levelIndex<< endl;
  Task *task = scinew Task("ICE_sm::computeDelPressAndUpdatePressCC",
                            this, &ICE_sm::computeDelPressAndUpdatePressCC);
  Ghost::GhostType  gac = Ghost::AroundCells;
  Ghost::GhostType  gn = Ghost::None;  
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  task->requires( Task::OldDW, lb->delTLabel,getLevel(patches));   
  task->requires( Task::NewDW, lb->vol_frac_CCLabel,   gac,2);
  task->requires( Task::NewDW, lb->uvel_FCMELabel,     gac,2);
  task->requires( Task::NewDW, lb->vvel_FCMELabel,     gac,2);
  task->requires( Task::NewDW, lb->wvel_FCMELabel,     gac,2);
  task->requires( Task::NewDW, lb->sp_vol_CCLabel,     gn);
  task->requires( Task::NewDW, lb->rho_CCLabel,        gn);    
  task->requires( Task::NewDW, lb->speedSound_CCLabel, gn);
  task->requires( Task::NewDW, lb->sumKappaLabel,      press_matl,oims,gn);
  task->requires( Task::NewDW, lb->press_equil_CCLabel,press_matl,oims,gn);
  //__________________________________ 
  task->computes(lb->press_CCLabel,        press_matl, oims);
  task->computes(lb->delP_DilatateLabel,   press_matl, oims);
  task->computes(lb->delP_MassXLabel,      press_matl, oims);
  task->computes(lb->term2Label,           press_matl, oims);
  task->computes(lb->sum_rho_CCLabel,      press_matl, oims);
  task->computes(lb->vol_fracX_FCLabel);
  task->computes(lb->vol_fracY_FCLabel);
  task->computes(lb->vol_fracZ_FCLabel);
  
  sched->setRestartable(true);
  sched->addTask(task, patches, matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleComputePressFC--
_____________________________________________________________________*/
void ICE_sm::scheduleComputePressFC(SchedulerP& sched,
                             const PatchSet* patches,
                             const MaterialSubset* press_matl,
                             const MaterialSet* matls)
{ 
  int levelIndex = getLevel(patches)->getIndex();
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputePressFC" 
             << "\t\t\t\t\tL-"<< levelIndex<< endl;
                                
  Task* task = scinew Task("ICE_sm::computePressFC",
                     this, &ICE_sm::computePressFC);
                     
  Ghost::GhostType  gac = Ghost::AroundCells;
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  task->requires(Task::NewDW,lb->press_CCLabel,   press_matl,oims, gac,1);
  task->requires(Task::NewDW,lb->sum_rho_CCLabel, press_matl,oims, gac,1);

  task->computes(lb->pressX_FCLabel, press_matl, oims);
  task->computes(lb->pressY_FCLabel, press_matl, oims);
  task->computes(lb->pressZ_FCLabel, press_matl, oims);

  sched->addTask(task, patches, matls);
}

//______________________________________________________________________
//

void ICE_sm::scheduleVelTau_CC( SchedulerP& sched,
                             const PatchSet* patches,              
                             const MaterialSet* ice_matls )         
{ 
  if( !d_viscousFlow ){
    return;
  }
  printSchedule(patches,iceCout,"ICE_sm::scheduleVelTau_CC");
                                
  Task* t = scinew Task("ICE_sm::VelTau_CC",
                  this, &ICE_sm::VelTau_CC);
                  
  Ghost::GhostType  gn= Ghost::None;
  t->requires( Task::OldDW, lb->vel_CCLabel, gn, 0 );
  t->computes( lb->velTau_CCLabel );
  
  sched->addTask(t, patches, ice_matls);
}

//______________________________________________________________________
//
void ICE_sm::scheduleViscousShearStress(SchedulerP& sched,
                                     const PatchSet* patches,
                                     const MaterialSet* ice_matls)
{ 
  int levelIndex = getLevel(patches)->getIndex();
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleViscousShearStress" 
             << "\t\t\t\t\tL-"<< levelIndex<< endl;
                                
  Task* t = scinew Task("ICE_sm::viscousShearStress",
                  this, &ICE_sm::viscousShearStress);
                     
  Ghost::GhostType  gac = Ghost::AroundCells;

  if(d_viscousFlow){
    t->requires( Task::NewDW, lb->viscosityLabel,   gac, 2);
    t->requires( Task::NewDW, lb->velTau_CCLabel,   gac, 2);  
    t->requires( Task::NewDW, lb->rho_CCLabel,      gac, 2);
    t->requires( Task::NewDW, lb->vol_frac_CCLabel, gac, 2);
    
    t->computes( lb->tau_X_FCLabel );
    t->computes( lb->tau_Y_FCLabel );
    t->computes( lb->tau_Z_FCLabel );
  }

  t->computes( lb->viscous_src_CCLabel );
  sched->addTask(t, patches, ice_matls);
}


/* _____________________________________________________________________
 Function~  ICE_sm::scheduleAccumulateMomentumSourceSinks--
_____________________________________________________________________*/
void
ICE_sm::scheduleAccumulateMomentumSourceSinks(SchedulerP& sched,
                                           const PatchSet* patches,
                                           const MaterialSubset* press_matl,
                                           const MaterialSubset* ice_matls,
                                           const MaterialSet* matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  Task* t;
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleAccumulateMomentumSourceSinks" 
             << "\t\t\tL-"<< levelIndex<< endl;
              
  t = scinew Task("ICE_sm::accumulateMomentumSourceSinks", 
            this, &ICE_sm::accumulateMomentumSourceSinks);

  t->requires(Task::OldDW, lb->delTLabel,getLevel(patches));  
  Ghost::GhostType  gac = Ghost::AroundCells;
  Ghost::GhostType  gn  = Ghost::None;
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  
  t->requires( Task::NewDW, lb->pressX_FCLabel,   press_matl,    oims, gac, 1);
  t->requires( Task::NewDW, lb->pressY_FCLabel,   press_matl,    oims, gac, 1);
  t->requires( Task::NewDW, lb->pressZ_FCLabel,   press_matl,    oims, gac, 1);
  t->requires( Task::NewDW, lb->viscous_src_CCLabel, ice_matls, gn, 0);
  t->requires( Task::NewDW, lb->rho_CCLabel,         gn, 0);
  t->requires( Task::NewDW, lb->vol_frac_CCLabel,    gn, 0);

  t->computes(lb->mom_source_CCLabel);
  sched->addTask(t, patches, matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm::scheduleAccumulateEnergySourceSinks--
_____________________________________________________________________*/
void ICE_sm::scheduleAccumulateEnergySourceSinks(SchedulerP& sched,
                                         const PatchSet* patches,
                                         const MaterialSubset* ice_matls,
                                         const MaterialSubset* press_matl,
                                         const MaterialSet* matls)

{
  int levelIndex = getLevel(patches)->getIndex();
  Task* t;              // EQ
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleAccumulateEnergySourceSinks" 
             << "\t\t\tL-" << levelIndex << endl;

  t = scinew Task("ICE_sm::accumulateEnergySourceSinks",
            this, &ICE_sm::accumulateEnergySourceSinks);
                     

  Ghost::GhostType  gac = Ghost::AroundCells;
  Ghost::GhostType  gn  = Ghost::None;
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  t->requires(Task::OldDW, lb->delTLabel,getLevel(patches));  
  t->requires(Task::NewDW, lb->press_CCLabel,     press_matl,oims, gn);
  t->requires(Task::NewDW, lb->delP_DilatateLabel,press_matl,oims, gn);
  t->requires(Task::NewDW, lb->compressibilityLabel,               gn);
  t->requires(Task::OldDW, lb->temp_CCLabel,      ice_matls, gac,1);
  t->requires(Task::NewDW, lb->thermalCondLabel,  ice_matls, gac,1);
  t->requires(Task::NewDW, lb->rho_CCLabel,                  gac,1);
  t->requires(Task::NewDW, lb->sp_vol_CCLabel,               gac,1);
  t->requires(Task::NewDW, lb->vol_frac_CCLabel,             gac,1);
  
  t->computes(lb->int_eng_source_CCLabel);
  t->computes(lb->heatCond_src_CCLabel);
  sched->addTask(t, patches, matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm:: scheduleComputeLagrangianValues--
 Note:      Only loop over ICE materials  
_____________________________________________________________________*/
void ICE_sm::scheduleComputeLagrangianValues(SchedulerP& sched,
                                     const PatchSet* patches,
                                     const MaterialSet* ice_matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeLagrangianValues" 
             << "\t\t\t\tL-"<< levelIndex<< endl;
             
  Task* t = scinew Task("ICE_sm::computeLagrangianValues",
                      this,&ICE_sm::computeLagrangianValues);
  Ghost::GhostType  gn  = Ghost::None;
  t->requires(Task::NewDW,lb->specific_heatLabel,      gn); 
  t->requires(Task::NewDW,lb->rho_CCLabel,             gn);
  t->requires(Task::OldDW,lb->vel_CCLabel,             gn);
  t->requires(Task::OldDW,lb->temp_CCLabel,            gn);
  t->requires(Task::NewDW,lb->mom_source_CCLabel,      gn);
  t->requires(Task::NewDW,lb->int_eng_source_CCLabel,  gn);

  t->computes(lb->mom_L_CCLabel);
  t->computes(lb->int_eng_L_CCLabel);
  t->computes(lb->mass_L_CCLabel);
 
  sched->addTask(t, patches, ice_matls);
}

/* _____________________________________________________________________
 Function~  ICE_sm:: scheduleComputeLagrangianSpecificVolume--
_____________________________________________________________________*/
void ICE_sm::scheduleComputeLagrangianSpecificVolume(SchedulerP& sched,
                                            const PatchSet* patches,
                                            const MaterialSubset* ice_matls,
                                            const MaterialSubset* press_matl,
                                            const MaterialSet* matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  Task* t = 0;
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleComputeLagrangianSpecificVolume" 
             << "\t\t\tL-"<< levelIndex<< endl;
  t = scinew Task("ICE_sm::computeLagrangianSpecificVolume",
             this,&ICE_sm::computeLagrangianSpecificVolume);

  Ghost::GhostType  gn  = Ghost::None;  
  Ghost::GhostType  gac = Ghost::AroundCells;
  Task::MaterialDomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.

  t->requires(Task::OldDW, lb->delTLabel,getLevel(patches));  
  t->requires(Task::NewDW, lb->rho_CCLabel,               gn);
  t->requires(Task::NewDW, lb->sp_vol_CCLabel,            gn);    
  t->requires(Task::NewDW, lb->Tdot_CCLabel,              gn);  
  t->requires(Task::NewDW, lb->f_theta_CCLabel,           gn);
  t->requires(Task::NewDW, lb->compressibilityLabel,      gn);  
  t->requires(Task::NewDW, lb->vol_frac_CCLabel,          gac,1);
  
  t->requires(Task::OldDW, lb->temp_CCLabel,        ice_matls, gn);
  t->requires(Task::NewDW, lb->specific_heatLabel,  ice_matls, gn); 

  t->requires(Task::NewDW, lb->delP_DilatateLabel,  press_matl,oims,gn);
  t->requires(Task::NewDW, lb->press_CCLabel,       press_matl,oims,gn);

  t->computes(lb->sp_vol_L_CCLabel);                             
  t->computes(lb->sp_vol_src_CCLabel);                        

  sched->setRestartable(true);
  sched->addTask(t, patches, matls);
}


/* _____________________________________________________________________
 Function~  ICE_sm::scheduleAdvectAndAdvanceInTime--
_____________________________________________________________________*/
void ICE_sm::scheduleAdvectAndAdvanceInTime(SchedulerP& sched,
                                    const PatchSet* patch_set,
                                    const MaterialSubset* ice_matlsub,
                                    const MaterialSet* ice_matls)
{
  int levelIndex = getLevel(patch_set)->getIndex();
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleAdvectAndAdvanceInTime" 
             << "\t\t\t\tL-"<< levelIndex << endl;
             
  Task* task = scinew Task("ICE_sm::advectAndAdvanceInTime",
                           this, &ICE_sm::advectAndAdvanceInTime);
  task->requires(Task::OldDW, lb->delTLabel,getLevel(patch_set));
  Ghost::GhostType  gac  = Ghost::AroundCells;
  task->requires(Task::NewDW, lb->uvel_FCMELabel,      gac,2);
  task->requires(Task::NewDW, lb->vvel_FCMELabel,      gac,2);
  task->requires(Task::NewDW, lb->wvel_FCMELabel,      gac,2);
  task->requires(Task::NewDW, lb->mom_L_ME_CCLabel,    gac,2);
  task->requires(Task::NewDW, lb->mass_L_CCLabel,      gac,2);
  task->requires(Task::NewDW, lb->eng_L_ME_CCLabel,    gac,2);
  task->requires(Task::NewDW, lb->sp_vol_L_CCLabel,    gac,2);
  
  task->computes(lb->mass_advLabel);
  task->computes(lb->mom_advLabel);
  task->computes(lb->eng_advLabel);
  task->computes(lb->sp_vol_advLabel);     

  sched->setRestartable(true);
  sched->addTask(task, patch_set, ice_matls);
}
/* _____________________________________________________________________
 Function~  ICE_sm::scheduleConservedtoPrimitive_Vars--
_____________________________________________________________________*/
void ICE_sm::scheduleConservedtoPrimitive_Vars(SchedulerP& sched,
                                    const PatchSet* patch_set,
                                    const MaterialSubset* ice_matlsub,
                                    const MaterialSet* ice_matls,
                                    const string& where)
{
  ASSERT( where == "afterAdvection" || where == "finalizeTimestep");
  
  int levelIndex = getLevel(patch_set)->getIndex();
  int numLevels = getLevel(patch_set)->getGrid()->numLevels();
  // single level problems we only need to perform this task once
  // immediately after advecton
  if(numLevels == 1 && where == "finalizeTimestep")  
    return;
    
  // On the finest level we only need to perform this task once
  // immediately after advecton
  if(levelIndex + 1 == numLevels && where ==  "finalizeTimestep")
    return;

  // from another taskgraph
  bool fat = false;
  if (where == "finalizeTimestep")
    fat = true;

  //---------------------------  
  iceCout << d_myworld->myrank() << " ICE_sm::scheduleConservedtoPrimitive_Vars" 
             << "\t\t\tL-"<< levelIndex << endl;

  string name = "ICE_sm::conservedtoPrimitive_Vars:" + where;

  Task* task = scinew Task(name, this, &ICE_sm::conservedtoPrimitive_Vars);
  task->requires(Task::OldDW, lb->delTLabel,getLevel(patch_set));     
  Ghost::GhostType  gn   = Ghost::None;
  task->requires(Task::NewDW, lb->mass_advLabel,      gn,0);
  task->requires(Task::NewDW, lb->mom_advLabel,       gn,0);
  task->requires(Task::NewDW, lb->eng_advLabel,       gn,0);
  task->requires(Task::NewDW, lb->sp_vol_advLabel,    gn,0);
  
  task->requires(Task::NewDW, lb->specific_heatLabel, gn, 0, fat);
  task->requires(Task::NewDW, lb->speedSound_CCLabel, gn, 0, fat);
  task->requires(Task::NewDW, lb->vol_frac_CCLabel,   gn, 0, fat);
  task->requires(Task::NewDW, lb->gammaLabel,         gn, 0, fat);
                                 
  task->modifies(lb->rho_CCLabel,     fat);
  task->modifies(lb->sp_vol_CCLabel,  fat);               
  if( where == "afterAdvection"){
    task->computes(lb->temp_CCLabel);
    task->computes(lb->vel_CCLabel);
    task->computes(lb->machLabel);
  }
  if( where == "finalizeTimestep"){
    task->modifies(lb->temp_CCLabel,  fat);
    task->modifies(lb->vel_CCLabel,   fat);
    task->modifies(lb->machLabel,     fat);
  } 
  
  sched->addTask(task, patch_set, ice_matls);
}
/* _____________________________________________________________________
 Function~  ICE_sm::scheduleTestConservation--
_____________________________________________________________________*/
void ICE_sm::scheduleTestConservation(SchedulerP& sched,
                                   const PatchSet* patches,
                                   const MaterialSubset* ice_matls,
                                   const MaterialSet* all_matls)
{
  int levelIndex = getLevel(patches)->getIndex();
  if(d_conservationTest->onOff && levelIndex == 0) {
    iceCout << d_myworld->myrank() << " ICE_sm::scheduleTestConservation" 
               << "\t\t\t\t\tL-"<< levelIndex<< endl;
    
    Task* t= scinew Task("ICE_sm::TestConservation",
                   this, &ICE_sm::TestConservation);

    Ghost::GhostType  gn  = Ghost::None;
    t->requires(Task::OldDW, lb->delTLabel,getLevel(patches)); 
    t->requires(Task::NewDW,lb->rho_CCLabel,        ice_matls, gn);
    t->requires(Task::NewDW,lb->vel_CCLabel,        ice_matls, gn);
    t->requires(Task::NewDW,lb->temp_CCLabel,       ice_matls, gn);
    t->requires(Task::NewDW,lb->specific_heatLabel, ice_matls, gn);
    t->requires(Task::NewDW,lb->uvel_FCMELabel,     ice_matls, gn);
    t->requires(Task::NewDW,lb->vvel_FCMELabel,     ice_matls, gn);
    t->requires(Task::NewDW,lb->wvel_FCMELabel,     ice_matls, gn);
    
                                 // A L L  M A T L S         
    t->requires(Task::NewDW,lb->mom_L_CCLabel,           gn);         
    t->requires(Task::NewDW,lb->int_eng_L_CCLabel,       gn);    
    t->requires(Task::NewDW,lb->mom_L_ME_CCLabel,        gn);         
    t->requires(Task::NewDW,lb->eng_L_ME_CCLabel,        gn); 
    
    if(d_conservationTest->exchange){
      t->computes(lb->mom_exch_errorLabel);
      t->computes(lb->eng_exch_errorLabel);
    }
    if(d_conservationTest->mass){
      t->computes(lb->TotalMassLabel);
    }
    if(d_conservationTest->energy){
    t->computes(lb->KineticEnergyLabel);
    t->computes(lb->TotalIntEngLabel);
    }
    if(d_conservationTest->momentum){
      t->computes(lb->TotalMomentumLabel);
    }
    sched->addTask(t, patches, all_matls);
  }
}

/* _____________________________________________________________________
 Function~  ICE_sm::actuallyComputeStableTimestep--
_____________________________________________________________________*/
void ICE_sm::actuallyComputeStableTimestep(const ProcessorGroup*,  
                                    const PatchSubset* patches,
                                    const MaterialSubset* /*matls*/,
                                    DataWarehouse* /*old_dw*/,
                                    DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    iceCout << d_myworld->myrank() << " Doing Compute Stable Timestep on patch " << patch->getID() 
         << "\t\t ICE \tL-" <<level->getIndex()<< endl;
      
    Vector dx = patch->dCell();
    double delX = dx.x();
    double delY = dx.y();
    double delZ = dx.z();
    double delt_CFL;
    double delt_diff;
    double delt;
    double inv_sum_invDelx_sqr = 1.0/( 1.0/(delX * delX) 
                                     + 1.0/(delY * delY) 
                                     + 1.0/(delZ * delZ) );
    constCCVariable<double> speedSound, sp_vol_CC, thermalCond, viscosity;
    constCCVariable<double> cv, gamma;
    constCCVariable<Vector> vel_CC;
    Ghost::GhostType  gn  = Ghost::None; 
    Ghost::GhostType  gac = Ghost::AroundCells;

    IntVector badCell(0,0,0);
    delt_CFL  = 1000.0; 
    delt_diff = 1000;
    delt      = 1000;

    for (int m = 0; m < d_sharedState->getNumICEMatls(); m++) {
      Material* matl = d_sharedState->getICEMaterial(m);
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
      int indx = matl->getDWIndex(); 
      new_dw->get(speedSound, lb->speedSound_CCLabel, indx,patch,gac, 1);
      new_dw->get(vel_CC,     lb->vel_CCLabel,        indx,patch,gac, 1);
      new_dw->get(sp_vol_CC,  lb->sp_vol_CCLabel,     indx,patch,gn,  0);
      new_dw->get(viscosity,  lb->viscosityLabel,     indx,patch,gn,  0);
      new_dw->get(thermalCond,lb->thermalCondLabel,   indx,patch,gn,  0);
      new_dw->get(gamma,      lb->gammaLabel,         indx,patch,gn,  0);
      new_dw->get(cv,         lb->specific_heatLabel, indx,patch,gn,  0);
      
      for(CellIterator iter=patch->getCellIterator(); !iter.done(); iter++){
        IntVector c = *iter;
        double speed_Sound = speedSound[c];
        double A = d_CFL*delX/(speed_Sound + 
                                     fabs(vel_CC[c].x())+d_SMALL_NUM);
        double B = d_CFL*delY/(speed_Sound + 
                                     fabs(vel_CC[c].y())+d_SMALL_NUM);
        double C = d_CFL*delZ/(speed_Sound + 
                                     fabs(vel_CC[c].z())+d_SMALL_NUM);
        delt_CFL = std::min(A, delt_CFL);
        delt_CFL = std::min(B, delt_CFL);
        delt_CFL = std::min(C, delt_CFL);
        if (A < 1e-20 || B < 1e-20 || C < 1e-20) {
          if (badCell == IntVector(0,0,0)) {
            badCell = c;
          }
          cout << d_myworld->myrank() << " Bad cell " << c << " (" << patch->getID() << "-" << level->getIndex() << "): " << vel_CC[c]<< endl;
        }
        
        // cout << " Aggressive delT Based on currant number "<< delt_CFL << endl;
        //__________________________________
        // stability constraint due to diffusion
        //  I C E  O N L Y
        double thermalCond_test = ice_matl->getThermalConductivity();
        double viscosity_test   = ice_matl->getViscosity();
        if (thermalCond_test !=0 || viscosity_test !=0) {

          for(CellIterator iter=patch->getCellIterator(); !iter.done(); iter++){
            IntVector c = *iter;
            double cp = cv[c] * gamma[c];
            double inv_thermalDiffusivity = cp/(sp_vol_CC[c] * thermalCond[c]);
            double kinematicViscosity = viscosity[c] * sp_vol_CC[c];
            double inv_diffusionCoeff = min(inv_thermalDiffusivity, 1.0/kinematicViscosity);
            double A = d_CFL * 0.5 * inv_sum_invDelx_sqr * inv_diffusionCoeff;
            delt_diff = std::min(A, delt_diff);
            if (delt_diff < 1e-20 && badCell == IntVector(0,0,0)) {
              badCell = c;
            }
          }
        }  //
        // cout << "delT based on diffusion  "<< delt_diff<<endl;
        delt = std::min(delt_CFL, delt_diff);
      } // aggressive Timestep 
    }  // matl loop   

    const Level* level = getLevel(patches);
    //__________________________________
    //  Bullet proofing
    if(delt < 1e-20) { 
      ostringstream warn;
      warn << "ERROR ICE:(L-"<< level->getIndex()
           << "):ComputeStableTimestep: delT < 1e-20 on cell " << badCell;
      throw InvalidValue(warn.str(), __FILE__, __LINE__);
    }
    new_dw->put(delt_vartype(delt), lb->delTLabel, level);
  }  // patch loop
}

/* _____________________________________________________________________ 
 Function~  ICE_sm::actuallyInitialize--
 Purpose~  Initialize CC variables and the pressure  
 Note that rho_micro, sp_vol, temp and velocity must be defined 
 everywhere in the domain
_____________________________________________________________________*/ 
void ICE_sm::actuallyInitialize(const ProcessorGroup*, 
                          const PatchSubset* patches,
                          const MaterialSubset* /*matls*/,
                          DataWarehouse*, 
                          DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  int L_indx = level->getIndex();
  
  
  //__________________________________
  // find max index of all the ICE matls
  // you could have a 1 matl problem with a starting indx of 2
  int max_indx = -100;
  
  int numICEMatls = d_sharedState->getNumICEMatls();
  for (int m = 0; m < numICEMatls; m++ ){
    ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
    int indx= ice_matl->getDWIndex();
    max_indx = max(max_indx, indx);
  }
  d_max_iceMatl_indx = max_indx;
  max_indx +=1;   

  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::actuallyInitialize" );
    
    int numMatls    = d_sharedState->getNumICEMatls();
    int numALLMatls = d_sharedState->getNumMatls();
    Vector grav     = getGravity();

    StaticArray<CCVariable<double>   > rho_micro(max_indx);
    StaticArray<CCVariable<double>   > sp_vol_CC(max_indx);
    StaticArray<CCVariable<double>   > rho_CC(max_indx); 
    StaticArray<CCVariable<double>   > Temp_CC(max_indx);
    StaticArray<CCVariable<double>   > speedSound(max_indx);
    StaticArray<CCVariable<double>   > vol_frac_CC(max_indx);
    StaticArray<CCVariable<Vector>   > vel_CC(max_indx);
    StaticArray<CCVariable<double>   > cv(max_indx);
    StaticArray<CCVariable<double>   > gamma(max_indx);
    CCVariable<double>    press_CC, imp_initialGuess, vol_frac_sum;
    
    new_dw->allocateAndPut(press_CC,         lb->press_CCLabel,     0,patch);
    new_dw->allocateAndPut(imp_initialGuess, lb->initialGuessLabel, 0,patch);
    new_dw->allocateTemporary(vol_frac_sum,patch);
    imp_initialGuess.initialize(0.0); 
    vol_frac_sum.initialize(0.0);

    //__________________________________
    //  Thermo and transport properties
    for (int m = 0; m < numMatls; m++ ) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx= ice_matl->getDWIndex();
      CCVariable<double> viscosity, thermalCond;
      new_dw->allocateAndPut(viscosity,    lb->viscosityLabel,    indx,patch);
      new_dw->allocateAndPut(thermalCond,  lb->thermalCondLabel,  indx,patch);
      new_dw->allocateAndPut(cv[indx],     lb->specific_heatLabel,indx,patch);
      new_dw->allocateAndPut(gamma[indx],  lb->gammaLabel,        indx,patch);

      gamma[indx].initialize( ice_matl->getGamma());
      cv[indx].initialize(    ice_matl->getSpecificHeat());    
      viscosity.initialize  ( ice_matl->getViscosity());
      thermalCond.initialize( ice_matl->getThermalConductivity());
      
      if(ice_matl->getViscosity() > 0.0){
        d_viscousFlow = true;
      }
       
    }

  //__________________________________
  // Note:
  // The press_CC isn't material dependent even though
  // we loop over numMatls below. This is done so we don't need additional
  // machinery to grab the pressure inside a geom_object
    for (int m = 0; m < numMatls; m++ ) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx= ice_matl->getDWIndex();
      new_dw->allocateAndPut(rho_micro[indx],  lb->rho_micro_CCLabel, indx,patch); 
      new_dw->allocateAndPut(sp_vol_CC[indx],  lb->sp_vol_CCLabel,    indx,patch); 
      new_dw->allocateAndPut(rho_CC[indx],     lb->rho_CCLabel,       indx,patch); 
      new_dw->allocateAndPut(Temp_CC[indx],    lb->temp_CCLabel,      indx,patch); 
      new_dw->allocateAndPut(speedSound[indx], lb->speedSound_CCLabel,indx,patch); 
      new_dw->allocateAndPut(vol_frac_CC[indx],lb->vol_frac_CCLabel,  indx,patch);
      new_dw->allocateAndPut(vel_CC[indx],     lb->vel_CCLabel,       indx,patch);
    }
    
    press_CC.initialize(0);
    
    for (int m = 0; m < numMatls; m++ ) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();
      ice_matl->initializeCells(rho_micro[indx],  rho_CC[indx],
                                Temp_CC[indx],    speedSound[indx], 
                                vol_frac_CC[indx], vel_CC[indx], 
                                press_CC, numALLMatls, patch, new_dw);

      setBC( rho_CC[indx],     "Density",     patch, indx, new_dw );
      setBC( rho_micro[indx],  "Density",     patch, indx, new_dw );
      setBC( Temp_CC[indx],    "Temperature", patch, indx, new_dw );
      setBC( speedSound[indx], "zeroNeumann", patch, indx, new_dw ); 
      setBC( vel_CC[indx],     "Velocity",    patch, indx, new_dw ); 
      setBC( press_CC,         "Pressure",    patch, 0,    new_dw );
            
      SpecificHeat *cvModel = ice_matl->getSpecificHeatModel();
      if(cvModel != 0) {
        for (CellIterator iter = patch->getExtraCellIterator();!iter.done();iter++){
          IntVector c = *iter;
          gamma[indx][c] = cvModel->getGamma(Temp_CC[indx][c]); 
          cv[indx][c]    = cvModel->getSpecificHeat(Temp_CC[indx][c]); 
        }
      } 
            
      for (CellIterator iter = patch->getExtraCellIterator();!iter.done();iter++){
        IntVector c = *iter;
        sp_vol_CC[indx][c] = 1.0/rho_micro[indx][c];

        //needed for LODI BCs
        vol_frac_CC[indx][c] = rho_CC[indx][c]*sp_vol_CC[indx][c];
        vol_frac_sum[c] += vol_frac_CC[indx][c];

        double dp_drho, dp_de, c_2, press_tmp;
        ice_matl->getEOS()->computePressEOS(rho_micro[indx][c],gamma[indx][c],
                                          cv[indx][c], Temp_CC[indx][c], press_tmp,
                                          dp_drho, dp_de);
          
        c_2 = dp_drho + dp_de * press_CC[c]/(rho_micro[indx][c] * rho_micro[indx][c]);
        speedSound[indx][c] = sqrt(c_2);
      }
      //____ B U L L E T   P R O O F I N G----
      IntVector neg_cell;
      ostringstream warn, base;
      base <<"ERROR ICE:(L-"<<L_indx<<"):actuallyInitialize, mat "<< indx <<" cell ";
      
      if( !areAllValuesPositive(press_CC, neg_cell) ) {
        warn << base.str()<< neg_cell << " press_CC is negative\n";
        throw ProblemSetupException(warn.str(), __FILE__, __LINE__ );
      }
      if( !areAllValuesPositive(rho_CC[indx], neg_cell) ) {
        warn << base.str()<< neg_cell << " rho_CC is negative\n";
        throw ProblemSetupException(warn.str(), __FILE__, __LINE__ );
      }
      if( !areAllValuesPositive(Temp_CC[indx], neg_cell) ) {
        warn << base.str()<< neg_cell << " Temp_CC is negative\n";
        throw ProblemSetupException(warn.str(), __FILE__, __LINE__ );
      }
      if( !areAllValuesPositive(sp_vol_CC[indx], neg_cell) ) {
        warn << base.str()<< neg_cell << " sp_vol_CC is negative\n";
        throw ProblemSetupException(warn.str(), __FILE__, __LINE__ );
      }
    }   // numMatls
    
    // make sure volume fractions sum to 1
    for (CellIterator iter = patch->getExtraCellIterator();!iter.done();iter++){
      if(vol_frac_sum[*iter] > 1.0 + 1e-10 || vol_frac_sum[*iter] < 1.0 - 1e-10){
        ostringstream warn, base;
        base <<"ERROR ICE:(L-"<<L_indx<<"):actuallyInitialize";
        warn << base.str() << "Cell: " << *iter << " Volume fractions did not sum to 1. Sum=" << vol_frac_sum[*iter] << "\n";
        throw ProblemSetupException(warn.str(), __FILE__, __LINE__ );
      }
    }
  }  // patch loop 
}

/* _____________________________________________________________________
 Function~  ICE_sm::computeThermoTransportProperties
 Purpose~   
 _____________________________________________________________________  */
void ICE_sm::computeThermoTransportProperties(const ProcessorGroup*,
                                              const PatchSubset* patches,
                                              const MaterialSubset* /*ice_matls*/,
                                              DataWarehouse* old_dw,
                                              DataWarehouse* new_dw)
{ 

  const Level* level = getLevel(patches);
  int levelIndex = level->getIndex();
 
 
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    
    iceCout << " ---------------------------------------------- L-"<< levelIndex<< endl;
    printTask(patches, patch, iceCout, "Doing ICE_sm::computeThermoTransportProperties" );
   
    int numMatls = d_sharedState->getNumICEMatls();
    
    for (int m = 0; m < numMatls; m++) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();

      constCCVariable<double> temp_CC;
      old_dw->get(temp_CC, lb->temp_CCLabel, indx,patch,Ghost::None,0);

      CCVariable<double> viscosity, thermalCond, gamma, cv;
      
      new_dw->allocateAndPut(thermalCond, lb->thermalCondLabel,  indx, patch);  
      new_dw->allocateAndPut(viscosity,   lb->viscosityLabel,    indx, patch);
      new_dw->allocateAndPut(cv,          lb->specific_heatLabel,indx, patch);
      new_dw->allocateAndPut(gamma,       lb->gammaLabel,        indx, patch); 
      
      viscosity.initialize  ( ice_matl->getViscosity());
      thermalCond.initialize( ice_matl->getThermalConductivity());
      gamma.initialize  (     ice_matl->getGamma());
      cv.initialize(          ice_matl->getSpecificHeat());
      SpecificHeat *cvModel = ice_matl->getSpecificHeatModel();
      
      if(cvModel != 0) {
        // loop through cells and compute pointwise
        for(CellIterator iter = patch->getCellIterator();!iter.done();iter++) {
          IntVector c = *iter;
          cv[c] = cvModel->getSpecificHeat(temp_CC[c]);
          gamma[c] = cvModel->getGamma(temp_CC[c]);
        }
      }
    }

  }
}

 
/* _____________________________________________________________________ 
 Function~  ICE_sm::computeEquilPressure_1_matl--
 Purpose~   Simple EOS evaluation
_____________________________________________________________________*/ 
void ICE_sm::computeEquilPressure_1_matl(const ProcessorGroup*,  
                                      const PatchSubset* patches,
                                      const MaterialSubset* matls,
                                      DataWarehouse* old_dw, 
                                      DataWarehouse* new_dw)
{
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::computeEquilPressure_1_matl" );

    CCVariable<double> vol_frac, sp_vol_new; 
    CCVariable<double> speedSound, f_theta, kappa;
    CCVariable<double> press_eq, sumKappa, sum_imp_delP, rho_CC_new;
    constCCVariable<double> Temp,rho_CC, sp_vol_CC, cv, gamma;   
    StaticArray<CCVariable<double> > rho_micro(1);
    
    Ghost::GhostType  gn = Ghost::None;
    ICEMaterial* ice_matl = d_sharedState->getICEMaterial(0);   
    int indx = ice_matl->getDWIndex();
    
    //__________________________________
    old_dw->get(Temp,      lb->temp_CCLabel,      indx,patch, gn,0);
    old_dw->get(rho_CC,    lb->rho_CCLabel,       indx,patch, gn,0);
    old_dw->get(sp_vol_CC, lb->sp_vol_CCLabel,    indx,patch, gn,0);
    new_dw->get(cv,        lb->specific_heatLabel,indx,patch, gn,0);
    new_dw->get(gamma,     lb->gammaLabel,        indx,patch, gn,0);
       
    new_dw->allocateTemporary(rho_micro[0],  patch);

    new_dw->allocateAndPut(press_eq,     lb->press_equil_CCLabel, 0,  patch);
    new_dw->allocateAndPut(sumKappa,     lb->sumKappaLabel,       0,  patch);
    new_dw->allocateAndPut(sum_imp_delP, lb->sum_imp_delPLabel,   0,  patch);
    new_dw->allocateAndPut(kappa,        lb->compressibilityLabel,indx,patch);
    new_dw->allocateAndPut(vol_frac,     lb->vol_frac_CCLabel,   indx,patch);    
    new_dw->allocateAndPut(sp_vol_new,   lb->sp_vol_CCLabel,     indx,patch);     
    new_dw->allocateAndPut(f_theta,      lb->f_theta_CCLabel,    indx,patch);
    new_dw->allocateAndPut(speedSound,   lb->speedSound_CCLabel, indx,patch);       
    sum_imp_delP.initialize(0.0);       

    new_dw->allocateAndPut(rho_CC_new,   lb->rho_CCLabel,        indx,patch);

    //______________________________________________________________________
    //  Main loop
    for (CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++) {
      IntVector c = *iter;
      vol_frac[c]      = 1.0;
      rho_micro[0][c]  = rho_CC_new[c] = rho_CC[c];
      sp_vol_new[c]    = 1.0/rho_CC[c];
      double dp_drho, dp_de, c_2;
      //__________________________________
      // evaluate EOS
      ice_matl->getEOS()->computePressEOS(rho_micro[0][c],gamma[c],
                                          cv[c], Temp[c], press_eq[c],
                                          dp_drho, dp_de);
                                          
      c_2 = dp_drho + dp_de * press_eq[c]/(rho_micro[0][c] * rho_micro[0][c]);
      speedSound[c] = sqrt(c_2);
      
      //  compute f_theta  
      kappa[c]    = sp_vol_new[c]/(speedSound[c]*speedSound[c]);
      sumKappa[c] = kappa[c];
      f_theta[c]  = 1.0;
    }
    //__________________________________
    // - apply Boundary conditions   
    setBC(press_eq, "Pressure", patch, 0, new_dw);

  }  // patch loop
}     

/* _____________________________________________________________________
 Function~  ICE_sm::computeFaceCenteredVelocities--
 Purpose~   compute the face centered velocities minus the exchange
            contribution.
_____________________________________________________________________*/
template<class T> void ICE_sm::computeVelFace(int dir, 
                                              CellIterator it,
                                              IntVector adj_offset,
                                              double dx,
                                              double delT, double gravity,
                                              constCCVariable<double>& rho_CC,
                                              constCCVariable<double>& sp_vol_CC,
                                              constCCVariable<Vector>& vel_CC,
                                              constCCVariable<double>& press_CC,
                                              T& vel_FC,
                                              T& grad_P_FC,
                                              bool include_acc)
{
  double inv_dx = 1.0/dx;

  double one_or_zero=1.;
  if(!include_acc){
    one_or_zero=0.0;
  }
  
  for(;!it.done(); it++){
    IntVector R = *it;
    IntVector L = R + adj_offset; 

    double rho_FC = rho_CC[L] + rho_CC[R];
#if SCI_ASSERTION_LEVEL >=2
    if (rho_FC <= 0.0) {
      cout << d_myworld->myrank() << " rho_fc <= 0: " << rho_FC << " with L= " << L << " (" 
           << rho_CC[L] << ") R= " << R << " (" << rho_CC[R]<< ")\n";
    }
#endif
    ASSERT(rho_FC > 0.0);

    //__________________________________
    // interpolation to the face
    double term1 = (rho_CC[L] * vel_CC[L][dir] +
                    rho_CC[R] * vel_CC[R][dir])/(rho_FC);            
    //__________________________________
    // pressure term           
    double sp_vol_brack = 2.*(sp_vol_CC[L] * sp_vol_CC[R])/
                             (sp_vol_CC[L] + sp_vol_CC[R]); 
                             
    grad_P_FC[R] = (press_CC[R] - press_CC[L]) * inv_dx;
    double term2 = delT * sp_vol_brack * grad_P_FC[R];
     
    //__________________________________
    // gravity term
    double term3 =  delT * gravity;
    
    vel_FC[R] = term1 - one_or_zero*term2 + one_or_zero*term3;
  } 
}
                  
//______________________________________________________________________
//                       
void ICE_sm::computeVel_FC(const ProcessorGroup*,  
                        const PatchSubset* patches,                
                        const MaterialSubset* /*matls*/,           
                        DataWarehouse* old_dw,                     
                        DataWarehouse* new_dw)                     
{
  const Level* level = getLevel(patches);
  
  for(int p = 0; p<patches->size(); p++){
    const Patch* patch = patches->get(p);
    printTask(patches, patch, iceCout, "Doing ICE_sm::computeVel_FCVel" );

    int numMatls = d_sharedState->getNumMatls();
    
    Vector dx      = patch->dCell();
    Vector gravity = getGravity();
    
    constCCVariable<double> press_CC;
    Ghost::GhostType  gac = Ghost::AroundCells; 
    new_dw->get(press_CC,lb->press_equil_CCLabel, 0, patch,gac, 1);
    
    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);   
     
    // Compute the face centered velocities
    for(int m = 0; m < numMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      int indx = matl->getDWIndex();
      constCCVariable<double> rho_CC, sp_vol_CC;
      constCCVariable<Vector> vel_CC;

      new_dw->get(rho_CC, lb->rho_CCLabel, indx, patch, gac, 1);
      old_dw->get(vel_CC, lb->vel_CCLabel, indx, patch, gac, 1); 
      new_dw->get(sp_vol_CC, lb->sp_vol_CCLabel,indx,patch, gac, 1);
              
      SFCXVariable<double> uvel_FC, grad_P_XFC;
      SFCYVariable<double> vvel_FC, grad_P_YFC;
      SFCZVariable<double> wvel_FC, grad_P_ZFC;

      new_dw->allocateAndPut(uvel_FC, lb->uvel_FCLabel, indx, patch);
      new_dw->allocateAndPut(vvel_FC, lb->vvel_FCLabel, indx, patch);
      new_dw->allocateAndPut(wvel_FC, lb->wvel_FCLabel, indx, patch);
      
      // debugging variables
      new_dw->allocateAndPut(grad_P_XFC, lb->grad_P_XFCLabel, indx, patch);
      new_dw->allocateAndPut(grad_P_YFC, lb->grad_P_YFCLabel, indx, patch);
      new_dw->allocateAndPut(grad_P_ZFC, lb->grad_P_ZFCLabel, indx, patch);   
      
      IntVector lowIndex(patch->getExtraSFCXLowIndex());
      uvel_FC.initialize(0.0, lowIndex,patch->getExtraSFCXHighIndex());
      vvel_FC.initialize(0.0, lowIndex,patch->getExtraSFCYHighIndex());
      wvel_FC.initialize(0.0, lowIndex,patch->getExtraSFCZHighIndex());
      
      grad_P_XFC.initialize(0.0);
      grad_P_YFC.initialize(0.0);
      grad_P_ZFC.initialize(0.0);
      
      vector<IntVector> adj_offset(3);
      adj_offset[0] = IntVector(-1, 0, 0);    // X faces
      adj_offset[1] = IntVector(0, -1, 0);    // Y faces
      adj_offset[2] = IntVector(0,  0, -1);   // Z faces     

      CellIterator XFC_iterator = patch->getSFCXIterator();
      CellIterator YFC_iterator = patch->getSFCYIterator();
      CellIterator ZFC_iterator = patch->getSFCZIterator();

      bool include_acc = true;

      //__________________________________
      //  Compute vel_FC for each face
      computeVelFace<SFCXVariable<double> >(0, XFC_iterator,
                                       adj_offset[0],dx[0],delT,gravity[0],
                                       rho_CC,sp_vol_CC,vel_CC,press_CC,
                                       uvel_FC, grad_P_XFC, include_acc);

      computeVelFace<SFCYVariable<double> >(1, YFC_iterator,
                                       adj_offset[1],dx[1],delT,gravity[1],
                                       rho_CC,sp_vol_CC,vel_CC,press_CC,
                                       vvel_FC, grad_P_YFC, include_acc);

      computeVelFace<SFCZVariable<double> >(2, ZFC_iterator,
                                       adj_offset[2],dx[2],delT,gravity[2],
                                       rho_CC,sp_vol_CC,vel_CC,press_CC,
                                       wvel_FC, grad_P_ZFC, include_acc);

      //__________________________________
      // (*)vel_FC BC are updated in 
      // ICE_sm::addExchangeContributionToFCVel()

    } // matls loop
  }  // patch loop
}

/*_____________________________________________________________________
 Function~  ICE_sm::computeDelPressAndUpdatePressCC--
 Purpose~
   This function calculates the change in pressure explicitly. 
 Note:  Units of delp_Dilatate and delP_MassX are [Pa]
 Reference:  Multimaterial Formalism eq. 1.5
 _____________________________________________________________________  */
void ICE_sm::computeDelPressAndUpdatePressCC(const ProcessorGroup*,  
                                          const PatchSubset* patches,
                                          const MaterialSubset* /*matls*/,
                                          DataWarehouse* old_dw, 
                                          DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);  

    printTask(patches, patch, iceCout, "Doing ICE_sm::computeDelPressAndUpdatePressCC" );

    int numMatls  = d_sharedState->getNumMatls();
    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);
    
    bool newGrid = d_sharedState->isRegridTimestep();
    Advector* advector = d_advector->clone(new_dw,patch,newGrid );   

    CCVariable<double> q_advected;
    CCVariable<double> delP_Dilatate;
    CCVariable<double> delP_MassX;
    CCVariable<double> sum_rho_CC;
    CCVariable<double> press_CC;
    CCVariable<double> term1, term2;
    
    constCCVariable<double>sumKappa, press_equil;
    
    StaticArray<constCCVariable<double> > sp_vol_CC(numMatls);
   
    const IntVector gc(1,1,1);
    Ghost::GhostType  gn  = Ghost::None;
    Ghost::GhostType  gac = Ghost::AroundCells;
    new_dw->get(sumKappa,                lb->sumKappaLabel,      0,patch,gn,0);
    new_dw->get(press_equil,             lb->press_equil_CCLabel,0,patch,gn,0);
    new_dw->allocateAndPut( press_CC,    lb->press_CCLabel,      0, patch);   
    new_dw->allocateAndPut(delP_Dilatate,lb->delP_DilatateLabel, 0, patch);   
    new_dw->allocateAndPut(delP_MassX,   lb->delP_MassXLabel,    0, patch);
    new_dw->allocateAndPut(term2,        lb->term2Label,         0, patch);
    new_dw->allocateAndPut(sum_rho_CC,   lb->sum_rho_CCLabel,    0, patch); 

    new_dw->allocateTemporary(q_advected, patch);
    new_dw->allocateTemporary(term1,      patch);
    
    term1.initialize(0.);
    term2.initialize(0.);
    sum_rho_CC.initialize(0.0); 
    delP_Dilatate.initialize(0.0);
    delP_MassX.initialize(0.0);

    for(int m = 0; m < numMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      int indx = matl->getDWIndex();
      constCCVariable<double> speedSound;
      constCCVariable<double> vol_frac;
      constCCVariable<double> rho_CC;
      constSFCXVariable<double> uvel_FC;
      constSFCYVariable<double> vvel_FC;
      constSFCZVariable<double> wvel_FC;
      
      new_dw->get(uvel_FC,     lb->uvel_FCMELabel,     indx,patch,gac, 2);   
      new_dw->get(vvel_FC,     lb->vvel_FCMELabel,     indx,patch,gac, 2);   
      new_dw->get(wvel_FC,     lb->wvel_FCMELabel,     indx,patch,gac, 2);   
      new_dw->get(vol_frac,    lb->vol_frac_CCLabel,   indx,patch,gac, 2);   
      new_dw->get(rho_CC,      lb->rho_CCLabel,        indx,patch,gn,0);
      new_dw->get(sp_vol_CC[m],lb->sp_vol_CCLabel,     indx,patch,gn,0);
      new_dw->get(speedSound,  lb->speedSound_CCLabel, indx,patch,gn,0);
      
      SFCXVariable<double> vol_fracX_FC;
      SFCYVariable<double> vol_fracY_FC;
      SFCZVariable<double> vol_fracZ_FC;

      new_dw->allocateAndPut(vol_fracX_FC, lb->vol_fracX_FCLabel,  indx,patch);
      new_dw->allocateAndPut(vol_fracY_FC, lb->vol_fracY_FCLabel,  indx,patch);
      new_dw->allocateAndPut(vol_fracZ_FC, lb->vol_fracZ_FCLabel,  indx,patch);
      
           
      // lowIndex is the same for all vel_FC
      IntVector lowIndex(patch->getExtraSFCXLowIndex());
      double nan= getNan();
      vol_fracX_FC.initialize(nan, lowIndex,patch->getExtraSFCXHighIndex());
      vol_fracY_FC.initialize(nan, lowIndex,patch->getExtraSFCYHighIndex());
      vol_fracZ_FC.initialize(nan, lowIndex,patch->getExtraSFCZHighIndex()); 
                 
      //__________________________________
      // Advection preprocessing
      // - divide vol_frac_cc/vol
      bool bulletProof_test=true;
      advectVarBasket* varBasket = scinew advectVarBasket();
      
      advector->inFluxOutFluxVolume(uvel_FC,vvel_FC,wvel_FC,delT,patch,indx,
                                    bulletProof_test, new_dw, varBasket); 
      //__________________________________
      //   advect vol_frac
      varBasket->doRefluxing = false;  // don't need to reflux here
      advector->advectQ(vol_frac, patch, q_advected, varBasket,  
                        vol_fracX_FC, vol_fracY_FC,  vol_fracZ_FC, new_dw);
                        
      delete varBasket; 
      
      for(CellIterator iter=patch->getCellIterator(); !iter.done();iter++) {
        IntVector c = *iter;
        term2[c] -= q_advected[c]; 
      }
         
      //__________________________________
      //  compute sum_rho_CC used by press_FC
      for(CellIterator iter=patch->getExtraCellIterator(); !iter.done();iter++){
        IntVector c = *iter;
        sum_rho_CC[c] += rho_CC[c];
      } 
    }  //matl loop
    delete advector;
    
    //__________________________________
    //  add delP to press_equil
    //  AMR:  hit the extra cells, BC aren't set an you need a valid pressure there
    // THIS COULD BE TROUBLE
    for(CellIterator iter = patch->getExtraCellIterator(); !iter.done(); iter++) { 
      IntVector c = *iter;
      press_CC[c] = press_equil[c];
    }

    for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++) { 
      IntVector c = *iter;
      double inv_sumKappa = 1.0/sumKappa[c];
      delP_MassX[c]    =  term1[c] * inv_sumKappa;
      delP_Dilatate[c] = -term2[c] * inv_sumKappa;
      press_CC[c]      =  press_equil[c] + delP_MassX[c] + delP_Dilatate[c];
      press_CC[c]      = max(1.0e-12, press_CC[c]);  // CLAMP
//      delP_Dilatate[c] = press_CC[c] - delP_MassX[c] - press_equil[c];
    }

    //__________________________________
    //  set boundary conditions
    setBC(press_CC, "Pressure", patch, 0, new_dw );    

  }  // patch loop
}

/* _____________________________________________________________________  
 Function~  ICE_sm::computePressFC--
 Purpose~
    This function calculates the face centered pressure on each of the 
    cell faces for every cell in the computational domain and a single 
    layer of ghost cells. 
  _____________________________________________________________________  */
template <class T> void ICE_sm::computePressFace(CellIterator iter, 
                                              IntVector adj_offset,
                                              constCCVariable<double>& sum_rho,
                                              constCCVariable<double>& press_CC,
                                              T& press_FC)
{
  for(;!iter.done(); iter++){
    IntVector R = *iter;
    IntVector L = R + adj_offset; 

    press_FC[R] = (press_CC[R] * sum_rho[L] + press_CC[L] * sum_rho[R])/
      (sum_rho[R] + sum_rho[L]);
  }
}
 
//______________________________________________________________________
//
void ICE_sm::computePressFC(const ProcessorGroup*,   
                      const PatchSubset* patches,
                      const MaterialSubset* /*matls*/,
                      DataWarehouse*,
                      DataWarehouse* new_dw)
{
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    
    printTask(patches, patch, iceCout, "Doing press_face_MM" );
    
    Ghost::GhostType  gac = Ghost::AroundCells;
    
    constCCVariable<double> press_CC;
    constCCVariable<double> sum_rho_CC;
    new_dw->get(press_CC,  lb->press_CCLabel,   0, patch, gac, 1);
    new_dw->get(sum_rho_CC,lb->sum_rho_CCLabel, 0, patch, gac, 1);
    
    SFCXVariable<double> pressX_FC;
    SFCYVariable<double> pressY_FC;
    SFCZVariable<double> pressZ_FC;
    new_dw->allocateAndPut(pressX_FC, lb->pressX_FCLabel, 0, patch);
    new_dw->allocateAndPut(pressY_FC, lb->pressY_FCLabel, 0, patch);
    new_dw->allocateAndPut(pressZ_FC, lb->pressZ_FCLabel, 0, patch);
    
    vector<IntVector> adj_offset(3);
    adj_offset[0] = IntVector(-1, 0, 0);    // X faces
    adj_offset[1] = IntVector(0, -1, 0);    // Y faces
    adj_offset[2] = IntVector(0,  0, -1);   // Z faces
         
    //__________________________________
    //  For each face compute the pressure
    computePressFace<SFCXVariable<double> >(patch->getSFCXIterator(),
                                       adj_offset[0], sum_rho_CC, press_CC,
                                       pressX_FC);

    computePressFace<SFCYVariable<double> >(patch->getSFCYIterator(),
                                       adj_offset[1], sum_rho_CC, press_CC,
                                       pressY_FC);

    computePressFace<SFCZVariable<double> >(patch->getSFCZIterator(),
                                       adj_offset[2], sum_rho_CC, press_CC,
                                       pressZ_FC); 
  }  // patch loop
}



//______________________________________________________________________
//    See comments in ICE_sm::VelTau_CC()
//______________________________________________________________________
void ICE_sm::computeVelTau_CCFace( const Patch* patch,
                                const Patch::FaceType face,
                                constCCVariable<Vector>& vel_CC,
                                CCVariable<Vector>& velTau_CC)
{
  CellIterator iterLimits=patch->getFaceIterator(face, Patch::ExtraMinusEdgeCells);
  IntVector oneCell = patch->faceDirection(face);
  
  for(CellIterator iter = iterLimits;!iter.done();iter++){
    IntVector c = *iter;        // extra cell index
    IntVector in = c - oneCell; // interior cell index
    
    velTau_CC[c] = 2. * vel_CC[c] - vel_CC[in];
  }
}

//______________________________________________________________________
//  Modify the vel_CC in the extra cells so that it behaves 
//     vel_FC[FC] = (vel_CC(c) + vel_CC(ec) )/2        (1)
//  
//  Note that at the edge of the domain we assume that vel_FC = vel_CC[ec]
//  so (1) becomes:
//     vel_CC[ec]    = (vel_CC(c) + velTau_CC(ec) )/2
//            or
//     velTau_CC[ec] = (2 * vel_CC(ec) - velTau_CC(c) );
//      
//  You need this so the viscous shear stress terms tau_xy = tau_yx.
//             
//               |           |
//    ___________|___________|_______________
//               |           |
//         o     |     o     |      o         Vel_CC
//               |     c     |  
//               |           |    
//               |           |  
//    ===========|====FC=====|==============      Edge of computational Domain
//               |           | 
//               |           |
//         *     |     o     |      o          Vel_CC in extraCell
//               |    ec     |
//               |           |
//    ___________|___________|_______________
//
//   A fundamental assumption is that the boundary conditions
//  have been vel_CC[ec] in the old_dw.
//______________________________________________________________________
void ICE_sm::VelTau_CC(const ProcessorGroup*,  
                    const PatchSubset* patches,              
                    const MaterialSubset* /*matls*/,         
                    DataWarehouse* old_dw,                   
                    DataWarehouse* new_dw)                   
{
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    printTask(patches, patch, iceCout, "Doing VelTau_CC" );
    
    //__________________________________
    //  ICE matl loop 
    int numMatls = d_sharedState->getNumICEMatls();
    Ghost::GhostType  gn  = Ghost::None;
      
    for (int m = 0; m < numMatls; m++) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();
      
      constCCVariable<Vector> vel_CC;
      CCVariable<Vector>   velTau_CC;
      old_dw->get(             vel_CC,    lb->vel_CCLabel,    indx, patch, gn, 0);           
      new_dw->allocateAndPut( velTau_CC,  lb->velTau_CCLabel, indx, patch);
      
      velTau_CC.copyData( vel_CC );           // copy interior values over
      
      if( patch->hasBoundaryFaces() ){
      
        // Iterate over the faces encompassing the domain
        vector<Patch::FaceType> bf;
        patch->getBoundaryFaces(bf);
        
        for( vector<Patch::FaceType>::const_iterator iter = bf.begin(); iter != bf.end(); ++iter ){
          Patch::FaceType face = *iter;

          //__________________________________
          //           X faces
          if (face == Patch::xminus || face == Patch::xplus) {
            computeVelTau_CCFace( patch, face, vel_CC, velTau_CC );
            continue;
          }

          //__________________________________
          //           Y faces
          if (face == Patch::yminus || face == Patch::yplus) {
            computeVelTau_CCFace( patch, face, vel_CC, velTau_CC );
            continue;
          }

          //__________________________________
          //           Z faces
          if (face == Patch::zminus || face == Patch::zplus) {
            computeVelTau_CCFace( patch, face, vel_CC, velTau_CC );
            continue;
          }

        }  // face loop
      }  // has boundary face
    }  // matl loop
  }  // patch loop
}



//______________________________________________________________________
//
void ICE_sm::viscousShearStress(const ProcessorGroup*,  
                             const PatchSubset* patches,
                             const MaterialSubset* /*matls*/,
                             DataWarehouse* old_dw, 
                             DataWarehouse* new_dw)
{
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    printTask(patches, patch, iceCout, "Doing viscousShearStress" );
      
    IntVector right, left, top, bottom, front, back;
 
    Vector dx    = patch->dCell();
    double areaX = dx.y() * dx.z();
    double areaY = dx.x() * dx.z();
    double areaZ = dx.x() * dx.y();
    
    //__________________________________
    //  Matl loop 
    int numMatls = d_sharedState->getNumICEMatls();
    
    for (int m = 0; m < numMatls; m++) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();
      
      CCVariable<Vector>   viscous_src;
      new_dw->allocateAndPut( viscous_src,  lb->viscous_src_CCLabel,  indx, patch);
      viscous_src.initialize( Vector(0.,0.,0.) );

      //__________________________________
      // Compute Viscous diffusion for this matl
      if( d_viscousFlow ){
        constCCVariable<double>   vol_frac;
        constCCVariable<double>   rho_CC;
        
        Ghost::GhostType  gac = Ghost::AroundCells;
        new_dw->get(vol_frac,  lb->vol_frac_CCLabel, indx,patch,gac,2);
        new_dw->get(rho_CC,    lb->rho_CCLabel,      indx,patch,gac,2);
      
        SFCXVariable<Vector> tau_X_FC, Ttau_X_FC;
        SFCYVariable<Vector> tau_Y_FC, Ttau_Y_FC;
        SFCZVariable<Vector> tau_Z_FC, Ttau_Z_FC;  
        
        new_dw->allocateAndPut(tau_X_FC, lb->tau_X_FCLabel, indx,patch);      
        new_dw->allocateAndPut(tau_Y_FC, lb->tau_Y_FCLabel, indx,patch);      
        new_dw->allocateAndPut(tau_Z_FC, lb->tau_Z_FCLabel, indx,patch);   

        tau_X_FC.initialize( Vector(0.0) );  // DEFAULT VALUE
        tau_Y_FC.initialize( Vector(0.0) );
        tau_Z_FC.initialize( Vector(0.0) );
        
        //__________________________________
        //  compute the shear stress terms  
        double viscosity_test = ice_matl->getViscosity();
        
        if(viscosity_test != 0.0) {
          CCVariable<double>        viscosity;
          constCCVariable<double>   viscosity_org;
          constCCVariable<Vector>   velTau_CC;

          new_dw->get(viscosity_org, lb->viscosityLabel, indx, patch, gac,2); 
          new_dw->get(velTau_CC,        lb->velTau_CCLabel,    indx, patch, gac,2); 

          // don't alter the original value
          new_dw->allocateTemporary(viscosity, patch, gac, 2);
          viscosity.copyData(viscosity_org);
          
          // Use temporary arrays to eliminate the communication of shear stress components
          // across the network.  Normally you would compute them in a separate task and then
          // require them with ghostCells to compute the divergence.
          SFCXVariable<Vector> Ttau_X_FC;
          SFCYVariable<Vector> Ttau_Y_FC;
          SFCZVariable<Vector> Ttau_Z_FC;  
          
          Ghost::GhostType  gac = Ghost::AroundCells;
          new_dw->allocateTemporary(Ttau_X_FC, patch, gac, 1);
          new_dw->allocateTemporary(Ttau_Y_FC, patch, gac, 1);
          new_dw->allocateTemporary(Ttau_Z_FC, patch, gac, 1);
          
          Vector evilNum(-9e30);
          Ttau_X_FC.initialize( evilNum );
          Ttau_Y_FC.initialize( evilNum );
          Ttau_Z_FC.initialize( evilNum );

          computeTauComponents( patch, vol_frac, velTau_CC,viscosity, Ttau_X_FC, Ttau_Y_FC, Ttau_Z_FC);

          for(CellIterator iter = patch->getCellIterator(); !iter.done();iter++){
            IntVector c = *iter;
            right    = c + IntVector(1,0,0);    left     = c + IntVector(0,0,0);
            top      = c + IntVector(0,1,0);    bottom   = c + IntVector(0,0,0);
            front    = c + IntVector(0,0,1);    back     = c + IntVector(0,0,0);       

            viscous_src[c].x(  (Ttau_X_FC[right].x() - Ttau_X_FC[left].x())  * areaX +
                               (Ttau_Y_FC[top].x()   - Ttau_Y_FC[bottom].x())* areaY +    
                               (Ttau_Z_FC[front].x() - Ttau_Z_FC[back].x())  * areaZ  );  

            viscous_src[c].y(  (Ttau_X_FC[right].y() - Ttau_X_FC[left].y())  * areaX +
                               (Ttau_Y_FC[top].y()   - Ttau_Y_FC[bottom].y())* areaY +      
                               (Ttau_Z_FC[front].y() - Ttau_Z_FC[back].y())  * areaZ  );          

            viscous_src[c].z(  (Ttau_X_FC[right].z() - Ttau_X_FC[left].z())  * areaX +
                               (Ttau_Y_FC[top].z()   - Ttau_Y_FC[bottom].z())* areaY +
                               (Ttau_Z_FC[front].z() - Ttau_Z_FC[back].z())  * areaZ  );
          }
          // copy the temporary data
          tau_X_FC.copyPatch( Ttau_X_FC, tau_X_FC.getLowIndex(), tau_X_FC.getHighIndex() );
          tau_Y_FC.copyPatch( Ttau_Y_FC, tau_Y_FC.getLowIndex(), tau_Y_FC.getHighIndex() );
          tau_Z_FC.copyPatch( Ttau_Z_FC, tau_Z_FC.getLowIndex(), tau_Z_FC.getHighIndex() );
          
        }  // hasViscosity
      }  // ice_matl
    }  // matl loop
  }  // patch loop
}


/* _____________________________________________________________________
 Purpose~   accumulate all of the sources/sinks of momentum
 _____________________________________________________________________  */
void ICE_sm::accumulateMomentumSourceSinks(const ProcessorGroup*,  
                                        const PatchSubset* patches,
                                        const MaterialSubset* /*matls*/,
                                        DataWarehouse* old_dw, 
                                        DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::accumulateMomentumSourceSinks" );
      
    IntVector right, left, top, bottom, front, back;

    delt_vartype delT; 
    old_dw->get(delT, d_sharedState->get_delt_label(),level);
 
    Vector dx      = patch->dCell();
    double vol     = dx.x() * dx.y() * dx.z();
    Vector gravity = getGravity();
    
    double areaX = dx.y() * dx.z();
    double areaY = dx.x() * dx.z();
    double areaZ = dx.x() * dx.y();
    
    constSFCXVariable<double> pressX_FC;
    constSFCYVariable<double> pressY_FC;
    constSFCZVariable<double> pressZ_FC;
    
    Ghost::GhostType  gac = Ghost::AroundCells;
    Ghost::GhostType  gn  = Ghost::None;
    new_dw->get(pressX_FC,lb->pressX_FCLabel, 0, patch, gac, 1);
    new_dw->get(pressY_FC,lb->pressY_FCLabel, 0, patch, gac, 1);
    new_dw->get(pressZ_FC,lb->pressZ_FCLabel, 0, patch, gac, 1);

    //__________________________________
    //  Matl loop 
    int numMatls  = d_sharedState->getNumMatls();
    for(int m = 0; m < numMatls; m++) {
      Material* matl        = d_sharedState->getMaterial( m );
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
      int indx = matl->getDWIndex();
  
      constCCVariable<double>  vol_frac;
      constCCVariable<double>  rho_CC;
      CCVariable<Vector>   mom_source;
      new_dw->get(vol_frac,  lb->vol_frac_CCLabel, indx,patch,gn,0);
      new_dw->get(rho_CC,    lb->rho_CCLabel,      indx,patch,gn,0);
      
      new_dw->allocateAndPut(mom_source,  lb->mom_source_CCLabel,  indx, patch);
      mom_source.initialize( Vector(0.,0.,0.) );
      
      //__________________________________
      //  accumulate sources
      for(CellIterator iter = patch->getCellIterator(); !iter.done();iter++){
        IntVector c = *iter;

        right    = c + IntVector(1,0,0);    left     = c + IntVector(0,0,0);
        top      = c + IntVector(0,1,0);    bottom   = c + IntVector(0,0,0);
        front    = c + IntVector(0,0,1);    back     = c + IntVector(0,0,0);
          
        double press_src_X = ( pressX_FC[right] - pressX_FC[left] )   * vol_frac[c];
        double press_src_Y = ( pressY_FC[top]   - pressY_FC[bottom] ) * vol_frac[c]; 
        double press_src_Z = ( pressZ_FC[front] - pressZ_FC[back] )   * vol_frac[c];
        
        mom_source[c].x( -press_src_X * areaX ); 
        mom_source[c].y( -press_src_Y * areaY );    
        mom_source[c].z( -press_src_Z * areaZ );
      }
      
      //__________________________________
      //  ICE _matls: 
      if(ice_matl){
        constCCVariable<Vector> viscous_src;
        new_dw->get(viscous_src, lb->viscous_src_CCLabel, indx, patch,gn,0);
      
        for(CellIterator iter = patch->getCellIterator(); !iter.done();iter++){
          IntVector c = *iter;
          double mass = rho_CC[c] * vol;

          mom_source[c] = (mom_source[c] + viscous_src[c] + mass * gravity );
        }
        
      }  //ice_matl
      
      //__________________________________
      //  All Matls
      for(CellIterator iter = patch->getCellIterator(); !iter.done();iter++){
        IntVector c = *iter;
        mom_source[c] *= delT; 
      }
      
    }  // matls loop
  }  //patches
}

/* _____________________________________________________________________ 
 Function~  ICE_sm::accumulateEnergySourceSinks--
 Purpose~   This function accumulates all of the sources/sinks of energy 
 Currently the kinetic energy isn't included.
 _____________________________________________________________________  */
void ICE_sm::accumulateEnergySourceSinks(const ProcessorGroup*,  
                                  const PatchSubset* patches,
                                  const MaterialSubset* /*matls*/,
                                  DataWarehouse* old_dw, 
                                  DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    printTask(patches, patch, iceCout, "Doing ICE_sm::accumulateEnergySourceSinks" );

    int numMatls = d_sharedState->getNumMatls();

    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);
    Vector dx = patch->dCell();
    double vol=dx.x()*dx.y()*dx.z();
    
    constCCVariable<double> sp_vol_CC;
    constCCVariable<double> kappa;
    constCCVariable<double> vol_frac;
    constCCVariable<double> press_CC;
    constCCVariable<double> delP_Dilatate;
    constCCVariable<double> matl_press;
    constCCVariable<double> rho_CC;

    Ghost::GhostType  gn  = Ghost::None;
    Ghost::GhostType  gac = Ghost::AroundCells;
    new_dw->get(press_CC,     lb->press_CCLabel,      0, patch,gn, 0);
    new_dw->get(delP_Dilatate,lb->delP_DilatateLabel, 0, patch,gn, 0);

    for(int m = 0; m < numMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl); 

      int indx    = matl->getDWIndex();   
      CCVariable<double> int_eng_source;
      CCVariable<double> heatCond_src;
      
      new_dw->get(sp_vol_CC,  lb->sp_vol_CCLabel,      indx,patch, gac,1);
      new_dw->get(rho_CC,     lb->rho_CCLabel,         indx,patch, gac,1);
      new_dw->get(kappa,      lb->compressibilityLabel,indx,patch, gn, 0);
      new_dw->get(vol_frac,   lb->vol_frac_CCLabel,    indx,patch, gac,1);
       
      new_dw->allocateAndPut(int_eng_source,  lb->int_eng_source_CCLabel,indx,patch);
      new_dw->allocateAndPut(heatCond_src,    lb->heatCond_src_CCLabel,  indx,patch);
      int_eng_source.initialize(0.0);
      heatCond_src.initialize(0.0);
     
      //__________________________________
      //  Source due to conduction ICE only
      if(ice_matl){
        double thermalCond_test = ice_matl->getThermalConductivity();
        if(thermalCond_test != 0.0 ){
          constCCVariable<double> Temp_CC;
          constCCVariable<double> thermalCond;
          new_dw->get(thermalCond, lb->thermalCondLabel, indx,patch,gac,1); 
          old_dw->get(Temp_CC,     lb->temp_CCLabel,     indx,patch,gac,1); 

          bool use_vol_frac = true; // include vol_frac in diffusion calc.
          scalarDiffusionOperator(new_dw, patch, use_vol_frac, Temp_CC,
                                  vol_frac, heatCond_src, thermalCond, delT);
        }
      }
                                     
      //__________________________________
      //   Compute source from volume dilatation
      //   Exclude contribution from delP_MassX
      for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++){
        IntVector c = *iter;
        double A = vol * vol_frac[c] * kappa[c] * press_CC[c];
        int_eng_source[c] += A * delP_Dilatate[c] + heatCond_src[c];
      }

    }  // matl loop
  }  // patch loop
}

/* _____________________________________________________________________
 Function~  ICE_sm::computeLagrangianValues--
 Computes lagrangian mass momentum and energy
 Note:    Only loop over ICE materials, mom_L, massL and int_eng_L
 _____________________________________________________________________  */
void ICE_sm::computeLagrangianValues(const ProcessorGroup*,  
                                  const PatchSubset* patches,
                                  const MaterialSubset* /*matls*/,
                                  DataWarehouse* old_dw, 
                                  DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);

  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::computeLagrangianValues" );

    int numALLMatls = d_sharedState->getNumMatls();
    Vector  dx = patch->dCell();
    double vol = dx.x()*dx.y()*dx.z();    
    
    //__________________________________ 
    //  Compute the Lagrangian quantities
    for(int m = 0; m < numALLMatls; m++) {
     Material* matl = d_sharedState->getMaterial( m );
     int indx = matl->getDWIndex();
     ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
     CCVariable<Vector> mom_L; 
     CCVariable<double> int_eng_L; 
     CCVariable<double> mass_L;
     double tiny_rho = 1.e-12;

     constCCVariable<double> rho_CC, temp_CC, cv, int_eng_source;
     constCCVariable<Vector> vel_CC, mom_source, mom_comb;
     tiny_rho = ice_matl->getTinyRho();

     Ghost::GhostType  gn = Ghost::None;
     new_dw->get(cv,             lb->specific_heatLabel,    indx,patch,gn,0);
     new_dw->get(rho_CC,         lb->rho_CCLabel,           indx,patch,gn,0);  
     old_dw->get(vel_CC,         lb->vel_CCLabel,           indx,patch,gn,0);  
     old_dw->get(temp_CC,        lb->temp_CCLabel,          indx,patch,gn,0);  
     new_dw->get(mom_source,     lb->mom_source_CCLabel,    indx,patch,gn,0);
     new_dw->get(int_eng_source, lb->int_eng_source_CCLabel,indx,patch,gn,0);  
     new_dw->allocateAndPut(mom_L,     lb->mom_L_CCLabel,     indx,patch);
     new_dw->allocateAndPut(int_eng_L, lb->int_eng_L_CCLabel, indx,patch);
     new_dw->allocateAndPut(mass_L,    lb->mass_L_CCLabel,    indx,patch);
     //__________________________________
     //  NO mass exchange
       for(CellIterator iter = patch->getExtraCellIterator(); !iter.done(); iter++) {
        IntVector c = *iter;
         double mass = rho_CC[c] * vol;
         mass_L[c] = mass;
         mom_L[c] = vel_CC[c] * mass + mom_source[c];
         int_eng_L[c] = mass*cv[c] * temp_CC[c] + int_eng_source[c];
       }

       //____ B U L L E T   P R O O F I N G----
       // catch negative internal energies
       // ignore BP if timestep restart has already been requested
       IntVector neg_cell;
       bool tsr = new_dw->timestepRestarted();

       if (!areAllValuesPositive(int_eng_L, neg_cell) && !tsr ) {
        ostringstream warn;
        int idx = level->getIndex();
        warn<<"ICE:(L-"<<idx<<"):computeLagrangianValues, mat "<<indx<<" cell "
            <<neg_cell<<" Negative int_eng_L: " << int_eng_L[neg_cell] <<  "\n";
        throw InvalidValue(warn.str(), __FILE__, __LINE__);
       }
    }  // end numALLMatl loop
  }  // patch loop
}
/* _____________________________________________________________________
 Function~  ICE_sm::computeLagrangianSpecificVolume--
 _____________________________________________________________________  */
void ICE_sm::computeLagrangianSpecificVolume(const ProcessorGroup*,  
                                          const PatchSubset* patches,
                                          const MaterialSubset* /*matls*/,
                                          DataWarehouse* old_dw,
                                          DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::computeLagrangianSpecificVolume" );

    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);

    int numALLMatls = d_sharedState->getNumMatls();
    Vector  dx = patch->dCell();
    double vol = dx.x()*dx.y()*dx.z();
    Ghost::GhostType  gn  = Ghost::None;
    Ghost::GhostType  gac = Ghost::AroundCells;

    StaticArray<constCCVariable<double> > Tdot(numALLMatls);
    StaticArray<constCCVariable<double> > vol_frac(numALLMatls);
    StaticArray<constCCVariable<double> > Temp_CC(numALLMatls);
    StaticArray<CCVariable<double> > alpha(numALLMatls);
    constCCVariable<double> rho_CC, f_theta, sp_vol_CC, cv;
    constCCVariable<double> delP, P;
    constCCVariable<double> TMV_CC;
    CCVariable<double> sum_therm_exp;

    new_dw->allocateTemporary(sum_therm_exp,patch);
    new_dw->get(delP, lb->delP_DilatateLabel, 0, patch,gn, 0);
    new_dw->get(P,    lb->press_CCLabel,      0, patch,gn, 0);
    sum_therm_exp.initialize(0.);

    for(int m = 0; m < numALLMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      int indx = matl->getDWIndex();
      
      new_dw->get(Tdot[m],    lb->Tdot_CCLabel,     indx,patch, gn,0);
      new_dw->get(vol_frac[m],lb->vol_frac_CCLabel, indx,patch, gac, 1);
      old_dw->get(Temp_CC[m], lb->temp_CCLabel,     indx,patch, gn,0);
      new_dw->allocateTemporary(alpha[m],patch);
     
    }

    //__________________________________
    // Sum of thermal expansion
    for(int m = 0; m < numALLMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
      int indx = matl->getDWIndex();
      
      new_dw->get(sp_vol_CC, lb->sp_vol_CCLabel,    indx,patch,gn, 0);
      new_dw->get(cv,        lb->specific_heatLabel,indx,patch,gn, 0);

      for(CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
        IntVector c = *iter;
        alpha[m][c] = ice_matl->getEOS()->getAlpha(Temp_CC[m][c],sp_vol_CC[c],P[c],cv[c]);
        sum_therm_exp[c] += vol_frac[m][c]*alpha[m][c]*Tdot[m][c];
      } 
    }

    //__________________________________ 
    for(int m = 0; m < numALLMatls; m++) {
      Material* matl = d_sharedState->getMaterial( m );
      int indx = matl->getDWIndex();
      CCVariable<double> sp_vol_L, sp_vol_src;
      constCCVariable<double> kappa;
      
      new_dw->allocateAndPut(sp_vol_L,  lb->sp_vol_L_CCLabel,   indx,patch);
      new_dw->allocateAndPut(sp_vol_src,lb->sp_vol_src_CCLabel, indx,patch);
      sp_vol_src.initialize(0.);
      double tiny_rho = 1.e-12;
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
      if (ice_matl) {
        tiny_rho = ice_matl->getTinyRho();
      }

      new_dw->get(sp_vol_CC,  lb->sp_vol_CCLabel,     indx,patch,gn, 0);
      new_dw->get(rho_CC,     lb->rho_CCLabel,        indx,patch,gn, 0);
      new_dw->get(f_theta,    lb->f_theta_CCLabel,    indx,patch,gn, 0);
      new_dw->get(kappa,      lb->compressibilityLabel,indx,patch,gn, 0);

      //__________________________________
      //  compute sp_vol_L * mass
      for(CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
        IntVector c = *iter;
        sp_vol_L[c] = (rho_CC[c] * vol)*sp_vol_CC[c];
      }
      
      //__________________________________
      //  add the sources to sp_vol_L
      for(CellIterator iter=patch->getCellIterator();!iter.done();iter++){
        IntVector c = *iter;
        //__________________________________
        //  term1
        double term1 = -vol_frac[m][c] * kappa[c] * vol * delP[c];
        double term2 = delT * vol *
                             (vol_frac[m][c] * alpha[m][c] * Tdot[m][c] -
                                               f_theta[c] * sum_therm_exp[c]);

        // This is actually mass * sp_vol
        double src = term1 + term2;
        sp_vol_L[c]  += src;
        sp_vol_src[c] = src/(rho_CC[c] * vol);
      }

      if(d_clampSpecificVolume){
        for(CellIterator iter=patch->getCellIterator();!iter.done();iter++){
          IntVector c = *iter;
/*`==========TESTING==========*/
          sp_vol_L[c] = max(sp_vol_L[c], tiny_rho * vol * sp_vol_CC[c]);
/*==========TESTING==========`*/
        }
      }

      //__________________________________
      // Apply boundary conditions
      setSpecificVolBC(sp_vol_L, "SpecificVol", true ,rho_CC,vol_frac[m], patch, indx);
      

      //____ B U L L E T   P R O O F I N G----
      // ignore BP if timestep restart has already been requested
      IntVector neg_cell;
      bool tsr = new_dw->timestepRestarted();
      
      if (!areAllValuesPositive(sp_vol_L, neg_cell) && !tsr) {
        cout << "\nICE:WARNING......Negative specific Volume"<< endl;
        cout << "cell              "<< neg_cell << " level " <<  level->getIndex() << endl;
        cout << "matl              "<< indx << endl;
        cout << "sum_thermal_exp   "<< sum_therm_exp[neg_cell] << endl;
        cout << "sp_vol_src        "<< sp_vol_src[neg_cell] << endl;
        cout << "mass sp_vol_L     "<< sp_vol_L[neg_cell] << endl;
        cout << "mass sp_vol_L_old "
             << (rho_CC[neg_cell]*vol*sp_vol_CC[neg_cell]) << endl;
        cout << "-----------------------------------"<<endl;
//        ostringstream warn;
//        int L = level->getIndex();
//        warn<<"ERROR ICE:("<<L<<"):computeLagrangianSpecificVolumeRF, mat "<<indx
//            << " cell " <<neg_cell << " sp_vol_L is negative\n";
//        throw InvalidValue(warn.str(), __FILE__, __LINE__);
        new_dw->abortTimestep();
        new_dw->restartTimestep();
     }
    }  // end numALLMatl loop
  }  // patch loop

}


///______________________________________________________________________
//   Single material version of momentum and heat exchange.
//   It sets the boundary conditions.  Do this for speed
void ICE_sm::addExchangeToMomentumAndEnergy_1matl(const ProcessorGroup*,
                                               const PatchSubset* patches,
                                               const MaterialSubset*,
                                               DataWarehouse* old_dw,
                                               DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::addExchangeToMomentumAndEnergy_1matl" );

    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);

    CCVariable<double>  Temp_CC;
    constCCVariable<double>  gamma;
    constCCVariable<Vector>  mom_L;
    constCCVariable<double>  int_eng_L;
    constCCVariable<double>  cv;

    // Create variables for the results
    CCVariable<Vector>  mom_L_ME;
    CCVariable<Vector>  vel_CC;
    CCVariable<double>  int_eng_L_ME;
    CCVariable<double>  Tdot;
    constCCVariable<double>  mass_L;
    constCCVariable<double>  old_temp;
    
    Ghost::GhostType  gn = Ghost::None;
    ICEMaterial* ice_matl = d_sharedState->getICEMaterial(0);   
    int indx = ice_matl->getDWIndex();
    
    old_dw->get(old_temp,  lb->temp_CCLabel,      indx, patch, gn, 0);   
    new_dw->get(cv,        lb->specific_heatLabel,indx, patch, gn, 0);   
    new_dw->get(gamma,     lb->gammaLabel,        indx, patch, gn, 0);   
    new_dw->get(mass_L,    lb->mass_L_CCLabel,    indx, patch, gn, 0);   
    new_dw->get(mom_L,     lb->mom_L_CCLabel,     indx, patch, gn, 0);   
    new_dw->get(int_eng_L, lb->int_eng_L_CCLabel, indx, patch, gn, 0);   
    
    new_dw->allocateAndPut(Tdot,        lb->Tdot_CCLabel,    indx,patch);
    new_dw->allocateAndPut(mom_L_ME,    lb->mom_L_ME_CCLabel,indx,patch);
    new_dw->allocateAndPut(int_eng_L_ME,lb->eng_L_ME_CCLabel,indx,patch);
    
    new_dw->allocateTemporary(vel_CC,  patch);
    new_dw->allocateTemporary(Temp_CC, patch);

    //__________________________________
    // Convert momenta to velocities and internal energy to Temp
    for(CellIterator iter = patch->getExtraCellIterator(); !iter.done();iter++){
      IntVector c = *iter;
      Temp_CC[c] = int_eng_L[c]/(mass_L[c]*cv[c]);  
      vel_CC[c]  = mom_L[c]/mass_L[c];              
    }

    //__________________________________
    //  Apply boundary conditions
    setBC(vel_CC, "Velocity",   patch,             indx, new_dw);
    setBC(Temp_CC,"Temperature",gamma, cv, patch,  indx, new_dw );
    
    //__________________________________
    // Convert vars. primitive-> flux 
    for(CellIterator iter = patch->getExtraCellIterator(); !iter.done();iter++){
      IntVector c = *iter;
      int_eng_L_ME[c] = Temp_CC[c]*cv[c] * mass_L[c];
      mom_L_ME[c]     = vel_CC[c]        * mass_L[c];
      Tdot[c]         = (Temp_CC[c] - old_temp[c])/delT;
    }
  } //patches
}

 
/* _____________________________________________________________________ 
 Function~  ICE_sm::advectAndAdvanceInTime--
 Purpose~
   This task calculates the The cell-centered, time n+1, mass, momentum
   internal energy, sp_vol
 _____________________________________________________________________  */
void ICE_sm::advectAndAdvanceInTime(const ProcessorGroup* /*pg*/,
                                 const PatchSubset* patches,
                                 const MaterialSubset* /*matls*/,
                                 DataWarehouse* old_dw,
                                 DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  
  // the advection calculations care about the position of the old dw subcycle
  double AMR_subCycleProgressVar = getSubCycleProgress(old_dw);

  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    printTask(patches, patch, iceCout, "Doing ICE_sm::advectAndAdvanceInTime" );
    iceCout << " progressVar " << AMR_subCycleProgressVar << endl;

    delt_vartype delT;
    old_dw->get(delT, d_sharedState->get_delt_label(),level);

    bool newGrid = d_sharedState->isRegridTimestep();
    Advector* advector = d_advector->clone(new_dw,patch,newGrid );


    CCVariable<double>  q_advected;
    CCVariable<Vector>  qV_advected; 
    new_dw->allocateTemporary(q_advected,   patch);
    new_dw->allocateTemporary(qV_advected,  patch);

    int numMatls = d_sharedState->getNumICEMatls();

    for (int m = 0; m < numMatls; m++ ) {
      Material* matl = d_sharedState->getICEMaterial( m );
      int indx = matl->getDWIndex(); 

      CCVariable<double> mass_adv, int_eng_adv, sp_vol_adv;
      CCVariable<Vector> mom_adv;
      constCCVariable<double> int_eng_L_ME, mass_L,sp_vol_L;
      constCCVariable<Vector> mom_L_ME;
      constSFCXVariable<double > uvel_FC;
      constSFCYVariable<double > vvel_FC;
      constSFCZVariable<double > wvel_FC;

      Ghost::GhostType  gac = Ghost::AroundCells;
      new_dw->get(uvel_FC,     lb->uvel_FCMELabel,        indx,patch,gac,2);  
      new_dw->get(vvel_FC,     lb->vvel_FCMELabel,        indx,patch,gac,2);  
      new_dw->get(wvel_FC,     lb->wvel_FCMELabel,        indx,patch,gac,2);  

      new_dw->get(mass_L,      lb->mass_L_CCLabel,        indx,patch,gac,2);
      new_dw->get(mom_L_ME,    lb->mom_L_ME_CCLabel,      indx,patch,gac,2);
      new_dw->get(sp_vol_L,    lb->sp_vol_L_CCLabel,      indx,patch,gac,2);
      new_dw->get(int_eng_L_ME,lb->eng_L_ME_CCLabel,      indx,patch,gac,2);

      new_dw->allocateAndPut(mass_adv,    lb->mass_advLabel,   indx,patch);          
      new_dw->allocateAndPut(mom_adv,     lb->mom_advLabel,    indx,patch);
      new_dw->allocateAndPut(int_eng_adv, lb->eng_advLabel,    indx,patch); 
      new_dw->allocateAndPut(sp_vol_adv,  lb->sp_vol_advLabel, indx,patch); 

      mass_adv.initialize(0.0);
      mom_adv.initialize(Vector(0.0,0.0,0.0));
      int_eng_adv.initialize(0.0);
      sp_vol_adv.initialize(0.0);
      q_advected.initialize(0.0);  
      qV_advected.initialize(Vector(0.0,0.0,0.0)); 
      
      //__________________________________
      // common variables that get passed into the advection operators
      advectVarBasket* varBasket = scinew advectVarBasket();
      varBasket->new_dw = new_dw;
      varBasket->old_dw = old_dw;
      varBasket->indx   = indx;
      varBasket->patch  = patch;
      varBasket->level  = level;
      varBasket->lb     = lb;
      varBasket->useCompatibleFluxes = d_useCompatibleFluxes;

      //__________________________________
      //   Advection preprocessing
      bool bulletProof_test=true;
      advector->inFluxOutFluxVolume(uvel_FC, vvel_FC, wvel_FC, delT, patch,indx,
                                    bulletProof_test, new_dw, varBasket); 
      //__________________________________
      // mass
      advector->advectMass(mass_L, q_advected,  varBasket);

      for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++) {
        IntVector c = *iter;
        mass_adv[c]  = (mass_L[c] + q_advected[c]);
      }   
      //__________________________________
      // momentum
      varBasket->is_Q_massSpecific   = true;
      varBasket->desc = "mom";
      advector->advectQ(mom_L_ME,mass_L,qV_advected, varBasket);

      for(CellIterator iter = patch->getCellIterator(); !iter.done();  iter++){
        IntVector c = *iter;
        mom_adv[c] = (mom_L_ME[c] + qV_advected[c]) ;
      }
      //__________________________________
      // internal energy
      varBasket->is_Q_massSpecific = true;
      varBasket->desc = "int_eng";
      advector->advectQ(int_eng_L_ME, mass_L, q_advected, varBasket);

      for(CellIterator iter = patch->getCellIterator(); !iter.done();  iter++){
        IntVector c = *iter;
        int_eng_adv[c] = (int_eng_L_ME[c] + q_advected[c]) ;
      }            
      //__________________________________
      // sp_vol[m] * mass
      varBasket->is_Q_massSpecific = true;
      varBasket->desc = "sp_vol";
      advector->advectQ(sp_vol_L,mass_L, q_advected, varBasket); 

      for(CellIterator iter = patch->getCellIterator(); !iter.done();  iter++){
        IntVector c = *iter;
        sp_vol_adv[c] = (sp_vol_L[c] + q_advected[c]) ;
      } 
      delete varBasket;
    }  // ice_matls loop
    delete advector;
  }  // patch loop
}
/* _____________________________________________________________________ 
 Function~  ICE_sm::conservedtoPrimitive_Vars
 Purpose~ This task computes the primitive variables (rho,T,vel,sp_vol,...)
          at time n+1, from the conserved variables mass, momentum, energy...
 _____________________________________________________________________  */
void ICE_sm::conservedtoPrimitive_Vars(const ProcessorGroup* /*pg*/,
                                    const PatchSubset* patches,
                                    const MaterialSubset* /*matls*/,
                                    DataWarehouse* old_dw,
                                    DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  int L_indx = level->getIndex();

  for(int p=0;p<patches->size();p++){ 
    const Patch* patch = patches->get(p);
    
    printTask(patches, patch, iceCout, "Doing ICE_sm::conservedtoPrimitive_Vars" );

    Vector dx = patch->dCell();
    double invvol = 1.0/(dx.x()*dx.y()*dx.z());
    Ghost::GhostType  gn  = Ghost::None;
    int numMatls = d_sharedState->getNumICEMatls();

    for (int m = 0; m < numMatls; m++ ) {
      Material* matl = d_sharedState->getICEMaterial( m );
      int indx = matl->getDWIndex();
      
      CCVariable<double> rho_CC, temp_CC, sp_vol_CC,mach;
      CCVariable<Vector> vel_CC;
      constCCVariable<double> int_eng_adv, mass_adv,sp_vol_adv,speedSound, cv;
      constCCVariable<double> gamma, vol_frac;
      constCCVariable<Vector> mom_adv;

      new_dw->get(gamma,       lb->gammaLabel,         indx,patch,gn,0);
      new_dw->get(speedSound,  lb->speedSound_CCLabel, indx,patch,gn,0);
      new_dw->get(vol_frac,    lb->vol_frac_CCLabel,   indx,patch,gn,0);
      new_dw->get(cv,          lb->specific_heatLabel, indx,patch,gn,0);

      new_dw->get(mass_adv,    lb->mass_advLabel,      indx,patch,gn,0); 
      new_dw->get(mom_adv,     lb->mom_advLabel,       indx,patch,gn,0); 
      new_dw->get(sp_vol_adv,  lb->sp_vol_advLabel,    indx,patch,gn,0); 
      new_dw->get(int_eng_adv, lb->eng_advLabel,       indx,patch,gn,0); 
      
      new_dw->getModifiable(sp_vol_CC, lb->sp_vol_CCLabel,indx,patch);
      new_dw->getModifiable(rho_CC,    lb->rho_CCLabel,   indx,patch);

      new_dw->allocateAndPut(temp_CC,lb->temp_CCLabel,  indx,patch);          
      new_dw->allocateAndPut(vel_CC, lb->vel_CCLabel,   indx,patch);
      new_dw->allocateAndPut(mach,   lb->machLabel,     indx,patch);  

      rho_CC.initialize(-d_EVIL_NUM);
      temp_CC.initialize(-d_EVIL_NUM);
      vel_CC.initialize(Vector(0.0,0.0,0.0)); 

      //__________________________________
      // Backout primitive quantities from 
      // the conserved ones.
      for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++) {
        IntVector c = *iter;
        double inv_mass_adv = 1.0/mass_adv[c];
        rho_CC[c]    = mass_adv[c] * invvol;
        vel_CC[c]    = mom_adv[c]    * inv_mass_adv;
        sp_vol_CC[c] = sp_vol_adv[c] * inv_mass_adv;
      } 
      
      //__________________________________
      // Backout primitive quantities from 
      // the conserved ones.
      for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++) {
        IntVector c = *iter;
        temp_CC[c] = int_eng_adv[c]/ (mass_adv[c]*cv[c]);
      }
      
      //__________________________________
      // set the boundary conditions
      setBC(rho_CC, "Density",                patch, indx, new_dw);                  
      setBC(vel_CC, "Velocity",               patch, indx, new_dw);                  
      setBC(temp_CC,"Temperature",gamma, cv,  patch, indx, new_dw );                 
            
      setSpecificVolBC(sp_vol_CC, "SpecificVol", false,rho_CC,vol_frac, patch, indx); 

      //__________________________________
      // Compute Auxilary quantities
      for(CellIterator iter = patch->getExtraCellIterator(); !iter.done(); iter++){
        IntVector c = *iter;
        mach[c]  = vel_CC[c].length()/speedSound[c];
      }

      //____ B U L L E T   P R O O F I N G----
      // ignore BP if timestep restart has already been requested
      IntVector neg_cell;
      bool tsr = new_dw->timestepRestarted();
      
      ostringstream base, warn;
      base <<"ERROR ICE:(L-"<<L_indx<<"):conservedtoPrimitive_Vars, mat "<< indx <<" cell ";
      if (!areAllValuesPositive(rho_CC, neg_cell) && !tsr) {
        warn << base.str() << neg_cell << " negative rho_CC\n ";
        throw InvalidValue(warn.str(), __FILE__, __LINE__);
      }
      if (!areAllValuesPositive(temp_CC, neg_cell) && !tsr) {
        warn << base.str() << neg_cell << " negative temp_CC\n ";
        throw InvalidValue(warn.str(), __FILE__, __LINE__);
      }
      if (!areAllValuesPositive(sp_vol_CC, neg_cell) && !tsr) {
       warn << base.str() << neg_cell << " negative sp_vol_CC\n ";        
       throw InvalidValue(warn.str(), __FILE__, __LINE__);
      } 
    }  // ice_matls loop
  }  // patch loop
}
/*_______________________________________________________________________
 Function:  TestConservation--
 Purpose:   Test for conservation of mass, momentum, energy.   
            Test to see if the exchange process is conserving
_______________________________________________________________________ */
void ICE_sm::TestConservation(const ProcessorGroup*,  
                           const PatchSubset* patches,
                           const MaterialSubset* /*matls*/,
                           DataWarehouse* old_dw,
                           DataWarehouse* new_dw)
{
  const Level* level = getLevel(patches);
  delt_vartype delT;
  old_dw->get(delT, d_sharedState->get_delt_label(),level);
     
  double total_mass     = 0.0;      
  double total_KE       = 0.0;      
  double total_int_eng  = 0.0;      
  Vector total_mom(0.0, 0.0, 0.0);  
  Vector mom_exch_error(0,0,0);     
  double eng_exch_error = 0;        
          
  for(int p=0; p<patches->size(); p++)  {
    const Patch* patch = patches->get(p);
    
    printTask(patches, patch, iceCout, "Doing ICE_sm::TestConservation" );
         
    Vector dx = patch->dCell();
    double cell_vol = dx.x()*dx.y()*dx.z();

    int numICEmatls = d_sharedState->getNumICEMatls();
    Ghost::GhostType  gn  = Ghost::None;
    //__________________________________
    // get face centered velocities to 
    // to compute what's being fluxed through the domain
    StaticArray<constSFCXVariable<double> >uvel_FC(numICEmatls);
    StaticArray<constSFCYVariable<double> >vvel_FC(numICEmatls);
    StaticArray<constSFCZVariable<double> >wvel_FC(numICEmatls);
    for (int m = 0; m < numICEmatls; m++ ) {
      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();
      new_dw->get(uvel_FC[m], lb->uvel_FCMELabel, indx,patch,gn,0);
      new_dw->get(vvel_FC[m], lb->vvel_FCMELabel, indx,patch,gn,0);
      new_dw->get(wvel_FC[m], lb->wvel_FCMELabel, indx,patch,gn,0);
    }
    
    //__________________________________
    // conservation of mass
    constCCVariable<double> rho_CC;
    StaticArray<CCVariable<double> > mass(numICEmatls);   
    for (int m = 0; m < numICEmatls; m++ ) {

      ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
      int indx = ice_matl->getDWIndex();
      new_dw->allocateTemporary(mass[m],patch);
      new_dw->get(rho_CC, lb->rho_CCLabel,   indx, patch, gn,0);

      for (CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
        IntVector c = *iter;
        mass[m][c] = rho_CC[c] * cell_vol;
      }
    }
    
    if(d_conservationTest->mass){    
      for (int m = 0; m < numICEmatls; m++ ) {
        double mat_mass = 0;
        conservationTest<double>(patch, delT, mass[m], 
                                 uvel_FC[m], vvel_FC[m], wvel_FC[m],mat_mass);
        total_mass += mat_mass; 
      }
    }
    //__________________________________
    // conservation of momentum
    if(d_conservationTest->momentum){
      CCVariable<Vector> mom;
      constCCVariable<Vector> vel_CC;
      new_dw->allocateTemporary(mom,patch);

      for (int m = 0; m < numICEmatls; m++ ) {

        ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
        int indx = ice_matl->getDWIndex();
        new_dw->get(vel_CC, lb->vel_CCLabel,   indx, patch, gn,0);

        for (CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
          IntVector c = *iter;
          mom[c] = mass[m][c] * vel_CC[c];
        }
        
        Vector mat_mom(0,0,0);
        conservationTest<Vector>(patch, delT, mom,
                                  uvel_FC[m],vvel_FC[m],wvel_FC[m], mat_mom);
        total_mom += mat_mom;
      } 
    }
    //__________________________________
    // conservation of internal_energy
    if(d_conservationTest->energy){
      CCVariable<double> int_eng;
      constCCVariable<double> temp_CC;
      constCCVariable<double> cv;
      new_dw->allocateTemporary(int_eng,patch);

      for (int m = 0; m < numICEmatls; m++ ) {

        ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
        int indx = ice_matl->getDWIndex();
        new_dw->get(temp_CC, lb->temp_CCLabel,      indx, patch, gn,0);
        new_dw->get(cv,      lb->specific_heatLabel,indx, patch, gn,0);
        
        for (CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
          IntVector c = *iter;
          int_eng[c] = mass[m][c] * cv[c] * temp_CC[c];
        }
        
        double mat_int_eng(0);
        
        conservationTest<double>(patch, delT, int_eng,
                                 uvel_FC[m],vvel_FC[m],wvel_FC[m], mat_int_eng);
        total_int_eng += mat_int_eng;
      }
    }
    //__________________________________
    // conservation of kinetic_energy
    if(d_conservationTest->energy){
      CCVariable<double> KE;
      constCCVariable<Vector> vel_CC;
      new_dw->allocateTemporary(KE,patch);

      for (int m = 0; m < numICEmatls; m++ ) {

        ICEMaterial* ice_matl = d_sharedState->getICEMaterial(m);
        int indx = ice_matl->getDWIndex();
        new_dw->get(vel_CC, lb->vel_CCLabel,indx, patch, gn,0);
        
        for (CellIterator iter=patch->getExtraCellIterator();!iter.done();iter++){
          IntVector c = *iter;
          double vel_mag = vel_CC[c].length();
          KE[c] = 0.5 * mass[m][c] * vel_mag * vel_mag;
        }
        
        double mat_KE(0);
        conservationTest<double>(patch, delT, KE,
                                  uvel_FC[m],vvel_FC[m],wvel_FC[m], mat_KE);
        total_KE += mat_KE;
      }
    }
    //__________________________________
    // conservation during the exchange process
    if(d_conservationTest->exchange){
      Vector sum_mom_L_CC     = Vector(0.0, 0.0, 0.0);
      Vector sum_mom_L_ME_CC  = Vector(0.0, 0.0, 0.0);
      double sum_int_eng_L_CC = 0.0;
      double sum_eng_L_ME_CC  = 0.0;

      int numALLmatls = d_sharedState->getNumMatls();
      for(int m = 0; m < numALLmatls; m++) {
        Material* matl = d_sharedState->getMaterial( m );
        int indx = matl->getDWIndex();
        constCCVariable<double> int_eng_L_CC, eng_L_ME_CC;
        constCCVariable<Vector> mom_L_CC, mom_L_ME_CC;
        new_dw->get(mom_L_CC,     lb->mom_L_CCLabel,     indx, patch,gn, 0);
        new_dw->get(int_eng_L_CC, lb->int_eng_L_CCLabel, indx, patch,gn, 0);
        new_dw->get(mom_L_ME_CC,  lb->mom_L_ME_CCLabel,  indx, patch,gn, 0);
        new_dw->get(eng_L_ME_CC,  lb->eng_L_ME_CCLabel,  indx, patch,gn, 0); 

        for (CellIterator iter=patch->getCellIterator(); !iter.done();iter++){
          IntVector c = *iter;
          sum_mom_L_CC     += mom_L_CC[c];     
          sum_mom_L_ME_CC  += mom_L_ME_CC[c];  
          sum_int_eng_L_CC += int_eng_L_CC[c]; 
          sum_eng_L_ME_CC  += eng_L_ME_CC[c];  
        }
      }
      mom_exch_error = sum_mom_L_CC     - sum_mom_L_ME_CC;
      eng_exch_error = sum_int_eng_L_CC - sum_eng_L_ME_CC;
    }
  }  // patch loop
  if(d_conservationTest->mass){
    new_dw->put(sum_vartype(total_mass),        lb->TotalMassLabel);
  }
  if(d_conservationTest->exchange){
    new_dw->put(sumvec_vartype(mom_exch_error), lb->mom_exch_errorLabel);  
    new_dw->put(sum_vartype(eng_exch_error),    lb->eng_exch_errorLabel);  
  }
  if(d_conservationTest->energy){
    new_dw->put(sum_vartype(total_KE),          lb->KineticEnergyLabel);  
    new_dw->put(sum_vartype(total_int_eng),     lb->TotalIntEngLabel);    
  }
  if(d_conservationTest->momentum){
    new_dw->put(sumvec_vartype(total_mom),      lb->TotalMomentumLabel);
  }
}

/*_____________________________________________________________________
 Function~  ICE_sm::upwindCell--
 purpose:   find the upwind cell in each direction  This is a knock off
            of Bucky's logic
 _____________________________________________________________________  */
IntVector ICE_sm::upwindCell_X(const IntVector& c, 
                            const double& var,              
                            double is_logical_R_face )     
{
  double  plus_minus_half = 0.5 * (var + d_SMALL_NUM)/fabs(var + d_SMALL_NUM);
  int one_or_zero = int(-0.5 - plus_minus_half + is_logical_R_face); 
  IntVector tmp = c + IntVector(one_or_zero,0,0);
  return tmp;
}

IntVector ICE_sm::upwindCell_Y(const IntVector& c, 
                            const double& var,              
                            double is_logical_R_face )     
{
  double  plus_minus_half = 0.5 * (var + d_SMALL_NUM)/fabs(var + d_SMALL_NUM);
  int one_or_zero = int(-0.5 - plus_minus_half + is_logical_R_face); 
  IntVector tmp = c + IntVector(0,one_or_zero,0);
  return tmp;
}

IntVector ICE_sm::upwindCell_Z(const IntVector& c, 
                            const double& var,              
                            double is_logical_R_face )     
{
  double  plus_minus_half = 0.5 * (var + d_SMALL_NUM)/fabs(var + d_SMALL_NUM);
  int one_or_zero = int(-0.5 - plus_minus_half + is_logical_R_face); 
  IntVector tmp = c + IntVector(0,0,one_or_zero);
  return tmp;
}



