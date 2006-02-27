#include <Packages/Uintah/CCA/Components/ICE/ICE.h>
#include <Packages/Uintah/CCA/Components/ICE/AMRICE.h>
#include <Packages/Uintah/CCA/Components/ICE/ICEMaterial.h>
#include <Packages/Uintah/CCA/Components/MPM/ConstitutiveModel/MPMMaterial.h>
#include <Packages/Uintah/CCA/Components/ICE/BoundaryCond.h>
#include <Packages/Uintah/CCA/Ports/LoadBalancer.h>
#include <Packages/Uintah/CCA/Ports/Scheduler.h>

#include <Packages/Uintah/Core/Grid/Task.h>
#include <Packages/Uintah/Core/Grid/SimulationState.h>
#include <Packages/Uintah/Core/Grid/Variables/CellIterator.h>
#include <Packages/Uintah/Core/Grid/Variables/VarTypes.h>
#include <Packages/Uintah/Core/Grid/Variables/AMRInterpolate.h>
#include <Packages/Uintah/Core/Exceptions/ConvergenceFailure.h>
#include <Packages/Uintah/Core/Exceptions/ProblemSetupException.h> 
#include <Packages/Uintah/Core/Exceptions/InvalidValue.h>
#include <Packages/Uintah/Core/Parallel/ProcessorGroup.h>
#include <Core/Util/DebugStream.h>
#include <Core/Exceptions/InternalError.h>

using namespace SCIRun;
using namespace Uintah;

static DebugStream cout_doing("ICE_DOING_COUT", false);
static DebugStream cout_dbg("impAMRICE_DBG", false);


/* _____________________________________________________________________
 Function~  ICE::scheduleLockstepTimeAdvance--
_____________________________________________________________________*/
void
ICE::scheduleLockstepTimeAdvance( const GridP& grid, SchedulerP& sched)
{
  int maxLevel = grid->numLevels();

  const MaterialSet* ice_matls = d_sharedState->allICEMaterials();
  const MaterialSet* mpm_matls = d_sharedState->allMPMMaterials();
  const MaterialSet* all_matls = d_sharedState->allMaterials();  

  MaterialSubset* one_matl = d_press_matl;
  const MaterialSubset* ice_matls_sub = ice_matls->getUnion();
  const MaterialSubset* mpm_matls_sub = mpm_matls->getUnion();
  double AMR_subCycleProgressVar = 0; 
  
  cout_doing << "--------------------------------------------------------"<< endl;
  cout_doing << "ICE::scheduleLockstepTimeAdvance"<< endl;  
  
  
  //__________________________________
  //
  for(int L = 0; L<maxLevel; L++){
    LevelP level = grid->getLevel(L);
    const PatchSet* patches = level->eachPatch();
    
    if(!doICEOnLevel(level->getIndex(), level->getGrid()->numLevels())){
      return;
    }

    // for AMR, we need to reset the initial Delt otherwise some unsuspecting level will
    // get the init delt when it didn't compute delt on L0.
    if (d_sharedState->getCurrentTopLevelTimeStep() > 1){
      d_initialDt = 10000.0;
    }


   if(d_turbulence){
      // The turblence model is also called directly from
      // accumlateMomentumSourceSinks.  This method just allows other
      // quantities (such as variance) to be computed
      d_turbulence->scheduleTurbulence1(sched, patches, ice_matls);
    }
    vector<PatchSubset*> maxMach_PSS(Patch::numFaces);
    scheduleMaxMach_on_Lodi_BC_Faces(       sched, level,   ice_matls, 
                                                            maxMach_PSS);

    scheduleComputeThermoTransportProperties(sched, level,  ice_matls);

    scheduleComputePressure(                sched, patches, d_press_matl,
                                                            all_matls); 

    scheduleComputeTempFC(                   sched, patches, ice_matls_sub,  
                                                             mpm_matls_sub,
                                                             all_matls);    

    scheduleComputeModelSources(             sched, level,   all_matls);

    scheduleUpdateVolumeFraction(            sched, level,   d_press_matl,
                                                             all_matls);


    scheduleComputeVel_FC(                   sched, patches,ice_matls_sub, 
                                                            mpm_matls_sub, 
                                                            d_press_matl,    
                                                            all_matls,     
                                                            false);        

    scheduleAddExchangeContributionToFCVel( sched, patches,ice_matls_sub,
                                                           all_matls,
                                                           false);
  }

//______________________________________________________________________
// MULTI-LEVEL IMPLICIT/EXPLICIT PRESSURE SOLVE
  if(d_impICE) {        //  I M P L I C I T
  
    bool recursion = false;

    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      const PatchSet* patches = level->eachPatch();

      scheduleSetupRHS(       sched,            patches,  one_matl, 
                                                          all_matls, 
                                                          recursion,
                                                          "computes");    
    }
    
    // correct the rhs at the coarse fine interfaces
    // compute maxRHS
    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      scheduleAddReflux_RHS(     sched,         level,     one_matl,
                                                           all_matls,
                                                           true);
                                                           
      scheduleCompute_maxRHS(    sched,         level,     one_matl,
                                                           all_matls);
    }
    scheduleMultiLevelPressureSolve(sched, grid,   0,
                                                   one_matl,
                                                   d_press_matl,
                                                   ice_matls_sub,
                                                   mpm_matls_sub,
                                                   all_matls);
    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      const PatchSet* patches = level->eachPatch();

      scheduleComputeDel_P(                   sched,  level, patches,  
                                                             one_matl,
                                                             d_press_matl,
                                                             all_matls);     
    }
  }
  //__________________________________
  if(!d_impICE){         //  E X P L I C I T (for debugging)
    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      const PatchSet* patches = level->eachPatch();
      scheduleComputeDelPressAndUpdatePressCC(sched, patches,d_press_matl,     
                                                           ice_matls_sub,  
                                                           mpm_matls_sub,  
                                                           all_matls);
    }  
    for(int L = maxLevel-1; L> 0; L--){
      const LevelP coarseLevel = grid->getLevel(L-1);
      scheduleCoarsen_delP(  sched,  coarseLevel,  d_press_matl, 
                                                   lb->delP_DilatateLabel);
    }                                                              
  }
//______________________________________________________________________

  for(int L = 0; L<maxLevel; L++){
    LevelP level = grid->getLevel(L);
    const PatchSet* patches = level->eachPatch();
    
    if(!doICEOnLevel(level->getIndex(), level->getGrid()->numLevels())){
      continue;
    }    
    
    scheduleComputePressFC(                 sched, patches, d_press_matl,
                                                            all_matls);

    scheduleAccumulateMomentumSourceSinks(  sched, patches, d_press_matl,
                                                            ice_matls_sub,
                                                            mpm_matls_sub,
                                                            all_matls);
    scheduleAccumulateEnergySourceSinks(    sched, patches, ice_matls_sub,
                                                            mpm_matls_sub,
                                                            d_press_matl,
                                                            all_matls);

    scheduleComputeLagrangianValues(        sched, patches, all_matls);

    scheduleAddExchangeToMomentumAndEnergy( sched, patches, ice_matls_sub,
                                                            mpm_matls_sub,
                                                            d_press_matl,
                                                            all_matls);

    scheduleComputeLagrangianSpecificVolume(sched, patches, ice_matls_sub,
                                                            mpm_matls_sub, 
                                                            d_press_matl,
                                                            all_matls);

    scheduleComputeLagrangian_Transported_Vars(sched, patches,
                                                            all_matls);

    scheduleAdvectAndAdvanceInTime(         sched, patches, AMR_subCycleProgressVar,
                                                            ice_matls_sub,
                                                            mpm_matls_sub,
                                                            d_press_matl,
                                                            all_matls);

    scheduleTestConservation(               sched, patches, ice_matls_sub,
                                                            all_matls); 
  }
  //__________________________________
  //  coarsen and refineInterface
  if(d_doAMR){
    for(int L = maxLevel-1; L> 0; L--){ // from finer to coarser levels
      LevelP coarseLevel = grid->getLevel(L-1);
      scheduleCoarsen(coarseLevel, sched);
    }
    for(int L = 1; L<maxLevel; L++){   // from coarser to finer levels
      LevelP fineLevel = grid->getLevel(L);
      scheduleRefineInterface(fineLevel, sched, 1, 1);
    }
  }

#if 0
    if(d_canAddICEMaterial){
      //  This checks to see if the model on THIS patch says that it's
      //  time to add a new material
      scheduleCheckNeedAddMaterial(           sched, level,   all_matls);

      //  This one checks to see if the model on ANY patch says that it's
      //  time to add a new material
      scheduleSetNeedAddMaterialFlag(         sched, level,   all_matls);
    }
#endif
    cout_doing << "---------------------------------------------------------"<<endl;
}


/*___________________________________________________________________
 Function~  ICE::scheduleMultiLevelPressureSolve--
_____________________________________________________________________*/
void ICE::scheduleMultiLevelPressureSolve(  SchedulerP& sched,
                                          const GridP grid,
                                          const PatchSet*,
                                          const MaterialSubset* one_matl,
                                          const MaterialSubset* press_matl,
                                          const MaterialSubset* ice_matls,
                                          const MaterialSubset* mpm_matls,
                                          const MaterialSet* all_matls)
{
  cout_doing << d_myworld->myrank() <<
                " ICE::scheduleMultiLevelPressureSolve" << endl;
  
  Task* t = scinew Task("ICE::multiLevelPressureSolve", 
                   this, &ICE::multiLevelPressureSolve,
                   grid, sched.get_rep(), ice_matls, mpm_matls);
 
  t->hasSubScheduler();
  Ghost::GhostType  gac = Ghost::AroundCells;  
  Ghost::GhostType  gn  = Ghost::None;
  Task::DomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  Task::DomainSpec nd = Task::NormalDomain;   //normal patches (need to specify when we use oims)

  for (int L = 0; L < grid->numLevels(); L++) {
    //__________________________________
    // The computes and requires when you're looking up 
    // from implicitPressure solve
    // OldDW = ParentOldDW
    // NewDW = ParentNewDW

    const LevelP level = grid->getLevel(L);
    const PatchSubset* patches = level->allPatches()->getUnion();
    //__________________________________
    // common Variables
    //  t->requires( Task::OldDW, lb->delTLabel);    AMR
    t->requires( Task::NewDW, lb->vol_frac_CCLabel,     patches,  gac,2); 
    t->requires( Task::NewDW, lb->sp_vol_CCLabel,       patches,  gac,1);
    t->requires( Task::NewDW, lb->rhsLabel,             patches, nd, one_matl,   oims,gn,0);

    //__________________________________
    // SetupRHS
    if(d_models.size() > 0){  
      t->requires(Task::NewDW,lb->modelMass_srcLabel, patches, gn,0);
    } 
    t->requires( Task::NewDW, lb->speedSound_CCLabel, patches, gn,0);
    t->requires( Task::NewDW, lb->max_RHSLabel,       patches, gn,0);
    
    //__________________________________
    // setup Matrix
    t->requires( Task::NewDW, lb->sp_volX_FCLabel,   patches,  gac,1);            
    t->requires( Task::NewDW, lb->sp_volY_FCLabel,   patches,  gac,1);            
    t->requires( Task::NewDW, lb->sp_volZ_FCLabel,   patches,  gac,1);            
    t->requires( Task::NewDW, lb->vol_fracX_FCLabel, patches,  gac,1);            
    t->requires( Task::NewDW, lb->vol_fracY_FCLabel, patches,  gac,1);            
    t->requires( Task::NewDW, lb->vol_fracZ_FCLabel, patches,  gac,1);            
    t->requires( Task::NewDW, lb->sumKappaLabel,  patches, nd, press_matl,oims,gn,0);   
        
    //__________________________________
    // Update Pressure
    t->requires( Task::NewDW, lb->press_equil_CCLabel, press_matl, oims, gac,1);  
    t->requires( Task::NewDW, lb->sum_imp_delPLabel,   press_matl, oims, gac,1);  
    
#if 0 // fix me
    computesRequires_CustomBCs(t, "implicitPressureSolve", lb, ice_matls,
                               d_customBC_var_basket);
#endif
    
    //__________________________________
    // ImplicitVel_FC
    t->requires(Task::OldDW,lb->vel_CCLabel, patches, ice_matls,  gac,1);    
    t->requires(Task::NewDW,lb->vel_CCLabel, patches, mpm_matls,  gac,1);
    
    //__________________________________
    //  what's produced from this task
    t->computes(lb->press_CCLabel,      patches, nd, press_matl,oims);
    t->computes(lb->grad_dp_XFCLabel,   patches, nd, press_matl,oims);
    t->computes(lb->grad_dp_YFCLabel,   patches, nd, press_matl,oims);
    t->computes(lb->grad_dp_ZFCLabel,   patches, nd, press_matl,oims);
    t->modifies(lb->sum_imp_delPLabel,  patches, nd, press_matl,oims);
    t->modifies(lb->term2Label,         patches, nd, one_matl,  oims);   
    t->modifies(lb->rhsLabel,           patches, nd, one_matl,  oims);
    
    t->modifies(lb->uvel_FCMELabel, patches);
    t->modifies(lb->vvel_FCMELabel, patches);
    t->modifies(lb->wvel_FCMELabel, patches);
    
    t->modifies(lb->vol_fracX_FCLabel, patches);
    t->modifies(lb->vol_fracY_FCLabel, patches);
    t->modifies(lb->vol_fracZ_FCLabel, patches); 
    
    t->modifies(lb->vol_frac_X_FC_fluxLabel, patches);
    t->modifies(lb->vol_frac_Y_FC_fluxLabel, patches);
    t->modifies(lb->vol_frac_Z_FC_fluxLabel, patches); 
  }
  t->setType(Task::OncePerProc);
  LoadBalancer* loadBal = sched->getLoadBalancer();
  const PatchSet* perprocPatches = loadBal->createPerProcessorPatchSet(grid);

  sched->addTask(t, perprocPatches, all_matls);
  cout << d_myworld->myrank() << " proc_patches are " << *perprocPatches << "\n";

}
/*___________________________________________________________________ 
 Function~  ICE::multiLevelPressureSolve-- 
_____________________________________________________________________*/
void ICE::multiLevelPressureSolve(const ProcessorGroup* pg,
                                  const PatchSubset* patches, 
                                  const MaterialSubset*,       
                                  DataWarehouse* ParentOldDW,    
                                  DataWarehouse* ParentNewDW,    
                                  GridP grid,                 
                                  Scheduler* sched,
                                  const MaterialSubset* ice_matls,
                                  const MaterialSubset* mpm_matls)
{
  // this function will be called exactly once per processor, regardless of the number of patches assigned
  // get the patches our processor is responsible for

  cout_doing << d_myworld->myrank() << " ICE::MultiLevelPressureSolve on patch " << *patches << endl;
  SchedulerP schedulerP(sched);
  //__________________________________
  // define Matl sets and subsets
  const MaterialSet* all_matls = d_sharedState->allMaterials();
  MaterialSubset* one_matl    = d_press_matl;
  MaterialSet* press_matlSet  = scinew MaterialSet();
  press_matlSet->add(0);
  press_matlSet->addReference(); 
  
  //__________________________________
  //  turn off parentDW scrubbing
  DataWarehouse::ScrubMode ParentOldDW_scrubmode =
                           ParentOldDW->setScrubbing(DataWarehouse::ScrubNone);
  DataWarehouse::ScrubMode ParentNewDW_scrubmode =
                           ParentNewDW->setScrubbing(DataWarehouse::ScrubNone);
  //__________________________________
  // create a new subscheduler
  SchedulerP subsched = sched->createSubScheduler();
  subsched->setRestartable(true); 
  subsched->initialize(3, 1, ParentOldDW, ParentNewDW);
  subsched->clearMappings();
  subsched->mapDataWarehouse(Task::ParentOldDW, 0);
  subsched->mapDataWarehouse(Task::ParentNewDW, 1);
  subsched->mapDataWarehouse(Task::OldDW, 2);
  subsched->mapDataWarehouse(Task::NewDW, 3);
  
  subsched->advanceDataWarehouse(grid);
  DataWarehouse* subOldDW = subsched->get_dw(2);
  DataWarehouse* subNewDW = subsched->get_dw(3);

  int maxLevel = grid->numLevels();

  //__________________________________
  //  Move data from parentOldDW to subSchedNewDW.
  // on all the levels
  delt_vartype dt;
  subNewDW = subsched->get_dw(3);
  ParentOldDW->get(dt, d_sharedState->get_delt_label());
  subNewDW->put(dt, d_sharedState->get_delt_label());
   
  max_vartype max_RHS_old;
  ParentNewDW->get(max_RHS_old, lb->max_RHSLabel);
  subNewDW->put(   max_RHS_old, lb->max_RHSLabel);
     
  subNewDW->transferFrom(ParentNewDW,lb->sum_imp_delPLabel, patches, d_press_matl); 
  subNewDW->transferFrom(ParentNewDW,lb->rhsLabel,          patches, one_matl);
  
  const MaterialSubset* all_matls_s = all_matls->getUnion();
  subNewDW->transferFrom(ParentNewDW,lb->uvel_FCLabel,      patches, all_matls_s); 
  subNewDW->transferFrom(ParentNewDW,lb->vvel_FCLabel,      patches, all_matls_s); 
  subNewDW->transferFrom(ParentNewDW,lb->wvel_FCLabel,      patches, all_matls_s);
    
  //__________________________________
  //  Iteration Loop
  max_vartype max_RHS = 1/d_SMALL_NUM;
  double smallest_max_RHS_sofar = max_RHS; 
  int counter = 0;
  bool restart   = false;
  bool recursion = true;
  bool firstIter = true;
  bool modifies_X = true;

  while( counter < d_max_iter_implicit && max_RHS > d_outer_iter_tolerance) {
    //__________________________________
    // schedule the tasks
    for(int L = 0; L<maxLevel; L++){
      const LevelP level = grid->getLevel(L);
      const PatchSet* patch_set = level->eachPatch();
      scheduleSetupMatrix(    subsched, level,  patch_set,  one_matl, 
                                                          all_matls,
                                                          firstIter);
    }

    for(int L = 0; L<maxLevel; L++){
      const LevelP level = grid->getLevel(L);
      schedule_matrixBC_CFI_coarsePatch(subsched, level, one_matl, all_matls);
      scheduleZeroMatrix_UnderFinePatches( subsched,level,one_matl, firstIter);
    }
#if 1
    // Level argument is not really used in this version of scheduleSolve(),
    // so just pass in the coarsest level as it always exists.
    const VarLabel* whichInitialGuess = NULL; 

    solver->scheduleSolve(grid->getLevel(0), subsched, press_matlSet,
                          lb->matrixLabel,   Task::NewDW,
                          lb->imp_delPLabel, modifies_X,
                          lb->rhsLabel,      Task::OldDW,
                          whichInitialGuess, Task::NewDW,
			     solver_parameters);

#else
    const PatchSet* perProcPatches = 
      sched->getLoadBalancer()->createPerProcessorPatchSet(grid);
    const VarLabel* whichInitialGuess = NULL;
    schedule_bogus_imp_delP(subsched,  perProcPatches,        d_press_matl,
                            all_matls);   
#endif

    // add the patchSubset size as part of the criteria if it is an empty subset.
    // ( we need the solver to work for hypre consistency), but don't do these so TG will compile
    for(int L = maxLevel-1; L> 0; L--){
      const LevelP coarseLevel = grid->getLevel(L-1);
      scheduleCoarsen_delP(  subsched,  coarseLevel,  d_press_matl, lb->imp_delPLabel);
    }

    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      const PatchSet* patch_set = level->eachPatch();
      
      scheduleUpdatePressure( subsched,  level, patch_set,  ice_matls,
                                                            mpm_matls,    
                                                            d_press_matl, 
                                                            all_matls);   
      
      scheduleRecomputeVel_FC(subsched,         patch_set,  ice_matls,
                                                            mpm_matls,    
                                                            d_press_matl,  
                                                            all_matls,    
                                                            recursion);   
      
      scheduleSetupRHS(       subsched,         patch_set,  one_matl, 
                                                            all_matls,
                                                            recursion,
                                                            "computes");
    }
    for(int L = 0; L<maxLevel; L++){
      LevelP level = grid->getLevel(L);
      scheduleAddReflux_RHS(  subsched,         level,      one_matl,
                                                            all_matls,
                                                           true);
      scheduleCompute_maxRHS(  subsched,        level,      one_matl,
                                                            all_matls);
    }

    subsched->compile();
    //__________________________________
    //  - move subNewDW to subOldDW
    //  - scrub the subScheduler
    //  - execute the tasks
    subsched->advanceDataWarehouse(grid); 
    subOldDW = subsched->get_dw(2);
    subNewDW = subsched->get_dw(3);
    subOldDW->setScrubbing(DataWarehouse::ScrubComplete);
    subNewDW->setScrubbing(DataWarehouse::ScrubNone);
    
    subsched->execute();
    // Allow for re-scheduling (different) tasks on the next iteration...
    subsched->initialize(3, 1, ParentOldDW, ParentNewDW);
    
    counter ++;
    firstIter = false;
    whichInitialGuess = NULL;
    
    //__________________________________
    // diagnostics
    subNewDW->get(max_RHS,     lb->max_RHSLabel);
    subOldDW->get(max_RHS_old, lb->max_RHSLabel);
    
    if(pg->myrank() == 0) {
      cout << "Outer iteration " << counter
           << " max_rhs before solve "<< max_RHS_old
           << " after solve " << max_RHS<< endl;
    }
    
    //__________________________________
    // restart timestep
                                          //  too many outer iterations
    if (counter > d_iters_before_timestep_restart ){
      restart = true;
      if(pg->myrank() == 0)
        cout <<"\nWARNING: max iterations befor timestep restart reached\n"<<endl;
    }
                                          //  solver has requested a restart
    if (subsched->get_dw(3)->timestepRestarted() ) {
      if(pg->myrank() == 0)
        cout << "\nWARNING: Solver had requested a restart\n" <<endl;
      restart = true;
    }
    
                                           //  solution is diverging
    if(max_RHS < smallest_max_RHS_sofar){
      smallest_max_RHS_sofar = max_RHS;
    }
    if(((max_RHS - smallest_max_RHS_sofar) > 100.0*smallest_max_RHS_sofar) ){
      if(pg->myrank() == 0)
        cout << "\nWARNING: outer interation is diverging now "
             << "restarting the timestep"
             << " Max_RHS " << max_RHS 
             << " smallest_max_RHS_sofar "<< smallest_max_RHS_sofar<< endl;
      restart = true;
    }
    if(restart){
      ParentNewDW->abortTimestep();
      ParentNewDW->restartTimestep();
      //return; - don't return - just break, some operations may require the transfers below to complete
      break;
    }
  }  // outer iteration loop
  
  //__________________________________
  //  BULLET PROOFING
  if ( (counter == d_max_iter_implicit)   && 
       (max_RHS > d_outer_iter_tolerance) &&
       counter > 1) {
    ostringstream s;
    s <<"ERROR ICE::implicitPressureSolve, the maximum number of outer"
      <<" iterations was reached. \n " 
      << "Try either increasing the max_outer_iterations "
      <<" or decrease outer_iteration_tolerance\n ";
    throw ConvergenceFailure(s.str(),counter,max_RHS,d_outer_iter_tolerance,__FILE__,__LINE__);
  }

  //__________________________________
  // Move products of iteration (only) from sub_new_dw -> parent_new_dw
    
  subNewDW  = subsched->get_dw(3);
  bool replace = true;
  const MaterialSubset* all_matls_sub = all_matls->getUnion();

  ParentNewDW->transferFrom(subNewDW,           // press
                    lb->press_CCLabel,         patches,  d_press_matl, replace);
  ParentNewDW->transferFrom(subNewDW,
                    lb->sum_imp_delPLabel,     patches,  d_press_matl, replace); 
  ParentNewDW->transferFrom(subNewDW,           // term2
                    lb->term2Label,            patches,  one_matl,     replace); 
  ParentNewDW->transferFrom(subNewDW,           // term2
                    lb->rhsLabel,              patches,  one_matl,     replace);
                       
  ParentNewDW->transferFrom(subNewDW,           // uvel_FC
                    lb->uvel_FCMELabel,        patches,  all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // vvel_FC
                    lb->vvel_FCMELabel,        patches,  all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // wvel_FC
                    lb->wvel_FCMELabel,        patches,  all_matls_sub,replace);  
  
  ParentNewDW->transferFrom(subNewDW,         // grad_impDelP_XFC
                    lb->grad_dp_XFCLabel,     patches,   d_press_matl, replace); 
  ParentNewDW->transferFrom(subNewDW,         // grad_impDelP_YFC
                    lb->grad_dp_YFCLabel,     patches,   d_press_matl, replace); 
  ParentNewDW->transferFrom(subNewDW,         // grad_impDelP_ZFC
                    lb->grad_dp_ZFCLabel,     patches,   d_press_matl, replace);
                     
  ParentNewDW->transferFrom(subNewDW,          // vol_fracX_FC
                    lb->vol_fracX_FCLabel,     patches, all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // vol_fracY_FC
                    lb->vol_fracY_FCLabel,     patches, all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // vol_fracZ_FC
                    lb->vol_fracZ_FCLabel,     patches, all_matls_sub,replace);
                    
  ParentNewDW->transferFrom(subNewDW,           // vol_fracX_FC_flux
                    lb->vol_frac_X_FC_fluxLabel,patches, all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // vol_fracY_FC_flux
                    lb->vol_frac_Y_FC_fluxLabel,patches, all_matls_sub,replace); 
  ParentNewDW->transferFrom(subNewDW,           // vol_fracZ_FC_flux
                    lb->vol_frac_Z_FC_fluxLabel,patches, all_matls_sub,replace);           
    
  //__________________________________
  //  Turn scrubbing back on
  ParentOldDW->setScrubbing(ParentOldDW_scrubmode);
  ParentNewDW->setScrubbing(ParentNewDW_scrubmode);  

  //__________________________________
  // clean up memory  
  if(press_matlSet->removeReference()){
    delete press_matlSet;
  }
} // end multiLevelPressureSolve()


/*______________________________________________________________________
 Function~  ICE::scheduleAddReflux_RHS--
 Purpose:  Add a refluxing correction to the rhs
 _____________________________________________________________________*/
 void ICE::scheduleAddReflux_RHS(SchedulerP& sched,
                                 const LevelP& coarseLevel,
                                 const MaterialSubset* one_matl,
                                 const MaterialSet* all_matls,
                                 const bool OnOff)
{
  if(OnOff && coarseLevel->hasFinerLevel()){
    
    //__________________________________
    //  Compute reflux_fluxes task
    const Level* fineLevel = coarseLevel->getFinerLevel().get_rep(); 
    cout_doing << d_myworld->myrank() 
               << " ICE::scheduleCompute_refluxFluxes_RHS\t\t\t\tL-" 
               << fineLevel->getIndex() << "->"<< coarseLevel->getIndex()<< endl;

    Task* t1 = scinew Task("ICE::compute_refluxFluxes_RHS",
                     this, &ICE::compute_refluxFluxes_RHS);

    Ghost::GhostType gn  = Ghost::None;
    Ghost::GhostType  gac = Ghost::AroundCells;
    Task::DomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet
    
    // Fluxes from the fine level. These are computed in the advection operator
    t1->requires(Task::NewDW, lb->vol_frac_X_FC_fluxLabel,
                 0,Task::FineLevel, 0, Task::NormalDomain, gn, 0);
    t1->requires(Task::NewDW, lb->vol_frac_Y_FC_fluxLabel,
                 0,Task::FineLevel, 0, Task::NormalDomain, gn, 0);
    t1->requires(Task::NewDW, lb->vol_frac_Z_FC_fluxLabel,
                 0,Task::FineLevel, 0, Task::NormalDomain, gn, 0);             

    t1->modifies(lb->vol_frac_X_FC_fluxLabel);  // these are the correction fluxes
    t1->modifies(lb->vol_frac_Y_FC_fluxLabel);
    t1->modifies(lb->vol_frac_Z_FC_fluxLabel);

    sched->addTask(t1, coarseLevel->eachPatch(), all_matls); 
    
    //__________________________________
    //  Apply reflux corrections to rhs   
    cout_doing << d_myworld->myrank() 
               << " ICE::scheduleApply_refluxFluxes_RHS\t\t\t\tL-" 
               << fineLevel->getIndex() << "->"<< coarseLevel->getIndex()<< endl;

    Task* t2 = scinew Task("ICE::apply_refluxFluxes_RHS",
                     this, &ICE::apply_refluxFluxes_RHS);
                     
    // coarse grid RHS after setupRHS               
    t2->requires(Task::NewDW,lb->rhsLabel, one_matl,oims,gn,0);                        
    
    // Correction fluxes  from the coarse level               
    t2->requires(Task::NewDW, lb->vol_frac_X_FC_fluxLabel, gac, 1);    
    t2->requires(Task::NewDW, lb->vol_frac_Y_FC_fluxLabel, gac, 1);    
    t2->requires(Task::NewDW, lb->vol_frac_Z_FC_fluxLabel, gac, 1);
    t2->modifies(lb->rhsLabel, one_matl,oims);

    sched->addTask(t2, coarseLevel->eachPatch(), all_matls);    
  }
}

/* _____________________________________________________________________
 Function~  ICE::compute_refluxFluxes_RHS
 _____________________________________________________________________  */
void ICE::compute_refluxFluxes_RHS(const ProcessorGroup*,
                                   const PatchSubset* coarsePatches,
                                   const MaterialSubset* matls,
                                   DataWarehouse*,
                                   DataWarehouse* new_dw)
{ 
  const Level* coarseLevel = getLevel(coarsePatches);
  const Level* fineLevel = coarseLevel->getFinerLevel().get_rep();
  
  cout_doing << d_myworld->myrank() 
             << " Doing reflux_computeCorrectionFluxes \t\t\t impAMRICE L-"
             <<fineLevel->getIndex()<< "->"<< coarseLevel->getIndex();
  
  for(int c_p=0;c_p<coarsePatches->size();c_p++){  
    const Patch* coarsePatch = coarsePatches->get(c_p);
    
    cout_doing <<"  patch " << coarsePatch->getID()<< endl;
    CCVariable<double> one;
          
    new_dw->allocateTemporary(one, coarsePatch);
    one.initialize(1.0);
    constCCVariable<double> notUsed = one;

    for(int m = 0;m<matls->size();m++){
      int indx = matls->get(m);

      Level::selectType finePatches;
      coarsePatch->getFineLevelPatches(finePatches);
      //__________________________________
      //   compute the correction
      for(int i=0; i < finePatches.size();i++){  
        const Patch* finePatch = finePatches[i];       

        if(finePatch->hasCoarseFineInterfaceFace() ){
          refluxOperator_computeCorrectionFluxes<double>( notUsed, notUsed, "vol_frac", indx, 
                        coarsePatch, finePatch, coarseLevel, fineLevel,new_dw);
        }
      }
    }  // matl loop
  }  // course patch loop
}
/* _____________________________________________________________________
 Function~  ICE::apply_refluxFluxes_RHS
 _____________________________________________________________________  */
void ICE::apply_refluxFluxes_RHS(const ProcessorGroup*,
                                   const PatchSubset* coarsePatches,
                                   const MaterialSubset* matls,
                                   DataWarehouse*,
                                   DataWarehouse* new_dw)
{ 
  const Level* coarseLevel = getLevel(coarsePatches);
  const Level* fineLevel = coarseLevel->getFinerLevel().get_rep();
  
  cout_doing << d_myworld->myrank() 
             << " Doing apply_refluxFluxes_RHS \t\t\t\t impAMRICE L-"
             <<fineLevel->getIndex()<< "->"<< coarseLevel->getIndex();
  
  for(int c_p=0;c_p<coarsePatches->size();c_p++){  
    const Patch* coarsePatch = coarsePatches->get(c_p);
    cout_doing << "  patch " << coarsePatch->getID()<< endl;

    CCVariable<double> rhs, sumRefluxCorrection;
    new_dw->getModifiable(rhs,lb->rhsLabel, 0, coarsePatch);
    new_dw->allocateTemporary(sumRefluxCorrection, coarsePatch);
    sumRefluxCorrection.initialize(0.0);

    //__________________________________
    // Sum the reflux correction over all materials  
    for(int m = 0;m<matls->size();m++){
      int indx = matls->get(m);

      Level::selectType finePatches;
      coarsePatch->getFineLevelPatches(finePatches); 

      for(int i=0; i < finePatches.size();i++){  
        const Patch* finePatch = finePatches[i];
        
        if(finePatch->hasCoarseFineInterfaceFace() ){

          refluxOperator_applyCorrectionFluxes<double>(
                        sumRefluxCorrection, "vol_frac",  indx, 
                        coarsePatch, finePatch, coarseLevel, fineLevel,new_dw);
          // Note in the equations the rhs is multiplied by vol, which is automatically canceled
          // This cancelation is mystically handled inside of the the applyCorrectionFluxes
          // operator.  
        }  
      }  // finePatch loop 
    }  // matls
    
    //__________________________________
    // apply reflux correction to rhs
    for(CellIterator iter=coarsePatch->getCellIterator(); !iter.done();iter++) {
      IntVector c = *iter;
      rhs[c] += sumRefluxCorrection[c]; 
    }
    
    //__________________________________
    //  Print Data
    if(switchDebug_setupRHS){ 
      ostringstream desc;     
      desc << "apply_refluxFluxes_RHS"<< "_patch_"<< coarsePatch->getID();
      printData(0, coarsePatch,   1, desc.str(), "rhs",rhs);
    }
  }  // course patch loop 
}



/*______________________________________________________________________
 Function~  ICE::scheduleCoarsen_delP--
 Purpose:  After the implicit pressure solve is performed on all levels 
 you need to project/coarsen the fine level solution onto the coarser levels
 _____________________________________________________________________*/
void ICE::scheduleCoarsen_delP(SchedulerP& sched, 
                               const LevelP& coarseLevel,
                               const MaterialSubset* press_matl,
                               const VarLabel* variable)
{                                                                          
  cout_doing << d_myworld->myrank()<< " ICE::scheduleCoarsen_"<< variable->getName()
             <<"\t\t\t\t\tL-" << coarseLevel->getIndex() << endl;

  Task* t = scinew Task("ICE::coarsen_delP",
                  this, &ICE::coarsen_delP, variable);

  Task::DomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.
  Ghost::GhostType  gn = Ghost::None;

  t->requires(Task::NewDW, variable,
              0, Task::FineLevel,  press_matl,oims, gn, 0);

  t->modifies(variable, d_press_matl, oims);        

  sched->addTask(t, coarseLevel->eachPatch(), d_sharedState->allICEMaterials());
}

/* _____________________________________________________________________
 Function~  ICE::Coarsen_delP
 _____________________________________________________________________  */
void ICE::coarsen_delP(const ProcessorGroup*,
                       const PatchSubset* coarsePatches,
                       const MaterialSubset* matls,
                       DataWarehouse*,
                       DataWarehouse* new_dw,
                       const VarLabel* variable)
{ 
  const Level* coarseLevel = getLevel(coarsePatches);
  const Level* fineLevel = coarseLevel->getFinerLevel().get_rep();
  
  for(int p=0;p<coarsePatches->size();p++){
    const Patch* coarsePatch = coarsePatches->get(p);
    
    cout_doing << d_myworld->myrank()<< " Doing Coarsen_" << variable->getName()
               << " on patch " << coarsePatch->getID() 
               << "\t\t\t ICE \tL-" <<coarseLevel->getIndex()<< endl;
    CCVariable<double> delP;                  
    new_dw->getModifiable(delP, variable, 0, coarsePatch);
   
    Level::selectType finePatches;
    coarsePatch->getFineLevelPatches(finePatches);

    Vector dx_c = coarseLevel->dCell();
    Vector dx_f = fineLevel->dCell();
    double coarseCellVol = dx_c.x()*dx_c.y()*dx_c.z();
    double fineCellVol   = dx_f.x()*dx_f.y()*dx_f.z();

    for(int i=0;i<finePatches.size();i++){
      const Patch* finePatch = finePatches[i];

      IntVector cl, ch, fl, fh;
      getFineLevelRange(coarsePatch, finePatch, cl, ch, fl, fh);
      if (fh.x() <= fl.x() || fh.y() <= fl.y() || fh.z() <= fl.z()) {
        continue;
      }

      constCCVariable<double> fine_delP;
      new_dw->getRegion(fine_delP,  variable, 0, fineLevel, fl, fh);

      //cout << " fineToCoarseOperator: finePatch "<< fl << " " << fh 
      //         << " coarsePatch "<< cl << " " << ch << endl;

      IntVector refinementRatio = fineLevel->getRefinementRatio();

      // iterate over coarse level cells
      for(CellIterator iter(cl, ch); !iter.done(); iter++){
        IntVector c = *iter;
        double delP_tmp(0.0);
        IntVector fineStart = coarseLevel->mapCellToFiner(c);

        // for each coarse level cell iterate over the fine level cells   
        for(CellIterator inside(IntVector(0,0,0),refinementRatio );
                                            !inside.done(); inside++){
          IntVector fc = fineStart + *inside;

          delP_tmp += fine_delP[fc] * fineCellVol;
        }
        delP[c] =delP_tmp / coarseCellVol; 
      }
    }

    if (switchDebug_updatePressure) {
      ostringstream desc;
      desc << "BOT_coarsen_delP" << coarsePatch->getID();
      printData( 0, coarsePatch, 0,desc.str(), "delP",delP);
    }  

  } // for patches
}
/*______________________________________________________________________
 Function~  ICE::scheduleZeroMatrix_UnderFinePatches--
 Purpose:  zero out the matrix and rhs on the coarse level, under
  any fine patch.
 _____________________________________________________________________*/
void ICE::scheduleZeroMatrix_UnderFinePatches(SchedulerP& sched, 
                                              const LevelP& coarseLevel,
                                              const MaterialSubset* one_matl,
                                              bool firstIter)
{ 
  cout_doing << d_myworld->myrank()
             << " ICE::scheduleZeroMatrix_RHS_UnderFinePatches\t\t\tL-" 
             << coarseLevel->getIndex() << endl;
  
  Task* t = scinew Task("ICE::zeroMatrix_UnderFinePatches",
                  this, &ICE::zeroMatrix_UnderFinePatches, firstIter);
  
  Task::DomainSpec oims = Task::OutOfDomain;  //outside of ice matlSet.  
  if(coarseLevel->hasFinerLevel()){                                                                      
    t->modifies(lb->matrixLabel, one_matl, oims);
  }   
  sched->addTask(t, coarseLevel->eachPatch(), d_sharedState->allICEMaterials());
}
/* _____________________________________________________________________
 Function~  ICE::zeroMatrix_UnderFinePatches
 _____________________________________________________________________  */
void ICE::zeroMatrix_UnderFinePatches(const ProcessorGroup*,
                                      const PatchSubset* coarsePatches,
                                      const MaterialSubset*,
                                      DataWarehouse* old_dw,
                                      DataWarehouse* new_dw,
                                      bool firstIter)
{ 
  const Level* coarseLevel = getLevel(coarsePatches);
  const Level* fineLevel = 0;

  for(int p=0;p<coarsePatches->size();p++){
    const Patch* coarsePatch = coarsePatches->get(p);
    
    cout_doing << d_myworld->myrank()
               << " Doing zeroMatrix_UnderFinePatches on patch "
               << coarsePatch->getID() << "\t ICE \tL-" <<coarseLevel->getIndex()<< endl;
               
    CCVariable<Stencil7> A; 
    new_dw->getModifiable(A,  lb->matrixLabel, 0, coarsePatch);
    
    Level::selectType finePatches;
    if(coarseLevel->hasFinerLevel()){
      // if there are no finer levels, do nothing.  We have accomplished what we
      // need to by transferring the RHS above.
      fineLevel = coarseLevel->getFinerLevel().get_rep();
      coarsePatch->getFineLevelPatches(finePatches);
    }

    for(int i=0;i<finePatches.size();i++){
      const Patch* finePatch = finePatches[i];

      IntVector cl, ch, fl, fh;
      getFineLevelRange(coarsePatch, finePatch, cl, ch, fl, fh);
      
      if (fh.x() <= fl.x() || fh.y() <= fl.y() || fh.z() <= fl.z()) {
        continue;
      }

      IntVector refinementRatio = fineLevel->getRefinementRatio();

      // iterate over coarse level cells
      for(CellIterator iter(cl, ch); !iter.done(); iter++){
        IntVector c = *iter;
        A[c].e= 0;
        A[c].w= 0; 
        A[c].n= 0; 
        A[c].s= 0; 
        A[c].t= 0; 
        A[c].b= 0;
        A[c].p= 1;
      }
    }
    //__________________________________
    //  Print Data
#if 1
    if (switchDebug_setupMatrix) {    
      ostringstream desc;
      desc << "BOT_zeroMatrix_RHS_UnderFinePatches_coarse_patch_" << coarsePatch->getID()
           <<  " L-" <<coarseLevel->getIndex()<< endl;
      printStencil( 0, coarsePatch, 1, desc.str(), "A", A);
    }
#endif

  } // for patches
}

/*___________________________________________________________________
 Function~  ICE::matrixCoarseLevelIterator--  
 Purpose:  returns the iterator  THIS IS COMPILCATED AND CONFUSING
_____________________________________________________________________*/
void ICE::matrixCoarseLevelIterator(Patch::FaceType patchFace,
                                       const Patch* coarsePatch,
                                       const Patch* finePatch,
                                       const Level* fineLevel,
                                       CellIterator& iter,
                                       bool& isRight_CP_FP_pair)
{
  CellIterator f_iter=finePatch->getFaceCellIterator(patchFace, "alongInteriorFaceCells");

  // find the intersection of the fine patch face iterator and underlying coarse patch
  IntVector f_lo_face = f_iter.begin();                 // fineLevel face indices   
  IntVector f_hi_face = f_iter.end();

  f_lo_face = fineLevel->mapCellToCoarser(f_lo_face);     
  f_hi_face = fineLevel->mapCellToCoarser(f_hi_face);

  IntVector c_lo_patch = coarsePatch->getLowIndex(); 
  IntVector c_hi_patch = coarsePatch->getHighIndex();

  IntVector l = Max(f_lo_face, c_lo_patch);             // intersection
  IntVector h = Min(f_hi_face, c_hi_patch);

  //__________________________________
  // Offset for the coarse level iterator
  // shift l & h,   1 cell for x+, y+, z+ finePatchfaces
  // shift l only, -1 cell for x-, y-, z- finePatchfaces

  string name = finePatch->getFaceName(patchFace);
  IntVector offset = finePatch->faceDirection(patchFace);

  if(name == "xminus" || name == "yminus" || name == "zminus"){
    l += offset;
  }
  if(name == "xplus" || name == "yplus" || name == "zplus"){
    l += offset;
    h += offset;
  }

  l = Max(l, coarsePatch->getLowIndex());
  h = Min(h, coarsePatch->getHighIndex());
  
  iter=CellIterator(l,h);
  isRight_CP_FP_pair = false;
  if ( coarsePatch->containsCell(l) ){
    isRight_CP_FP_pair = true;
  }
  
  if (cout_dbg.active()) {
    cout_dbg << "refluxCoarseLevelIterator: face "<< patchFace
             << " finePatch " << finePatch->getID()
             << " coarsePatch " << coarsePatch->getID()
             << " [CellIterator at " << iter.begin() << " of " << iter.end() << "] "
             << " does this coarse patch own the face centered variable "
             << isRight_CP_FP_pair << endl; 
  }
}

/*___________________________________________________________________
 Function~  ICE::schedule_matrixBC_CFI_coarsePatch--  
_____________________________________________________________________*/
void ICE::schedule_matrixBC_CFI_coarsePatch(SchedulerP& sched, 
                                            const LevelP& coarseLevel,
                                            const MaterialSubset* one_matl,
                                            const MaterialSet* all_matls)

{
  if(coarseLevel->hasFinerLevel()){
    cout_doing << d_myworld->myrank() 
               << " ICE::matrixBC_CFI_coarsePatch\t\t\t\t\tL-" 
               << coarseLevel->getIndex() <<endl;

    Task* task = scinew Task("schedule_matrixBC_CFI_coarsePatch",
                  this, &ICE::matrixBC_CFI_coarsePatch);

    Ghost::GhostType  gn  = Ghost::None;
    //    task->requires(Task::NewDW, lb->matrix_CFI_weightsLabel,
    //                0, Task::FineLevel, one_matl,Task::NormalDomain, gn, 0);

    task->requires(Task::NewDW, lb->matrixLabel, 0, Task::FineLevel, 0, Task::NormalDomain, gn, 0);

    task->modifies(lb->matrixLabel);

    sched->addTask(task, coarseLevel->eachPatch(), all_matls); 
  }
}
/*___________________________________________________________________
 Function~  ICE::matrixBC_CFI_coarsePatch--
 Purpose~   Along each coarseFine interface (on finePatches)
 set the stencil weight
 
   A.p = beta[c] -
          (A.n + A.s + A.e + A.w + A.t + A.b);
   LHS       
   A.p*delP - (A.e*delP_e + A.w*delP_w + A.n*delP_n + A.s*delP_s 
             + A.t*delP_t + A.b*delP_b )
             
Implementation:  For each coarse patch, loop over the overlapping fine level
patches.  If a fine patch has a CFI then set the stencil weights on the coarse level  
_____________________________________________________________________*/
void ICE::matrixBC_CFI_coarsePatch(const ProcessorGroup*,
                                   const PatchSubset* coarsePatches,
                                   const MaterialSubset* matls,
                                   DataWarehouse*,
                                   DataWarehouse* new_dw)
{
  const Level* coarseLevel = getLevel(coarsePatches);
  const Level* fineLevel = coarseLevel->getFinerLevel().get_rep();
  
  cout_doing << d_myworld->myrank() 
             << " Doing matrixBC_CFI_coarsePatch \t\t\t ICE \tL-"
             <<coarseLevel->getIndex();
  //__________________________________
  // over all coarse patches
  for(int c_p=0;c_p<coarsePatches->size();c_p++){  
    const Patch* coarsePatch = coarsePatches->get(c_p);
    cout_doing << "  patch " << coarsePatch->getID()<< endl;
         
    CCVariable<Stencil7> A_coarse;
    new_dw->getModifiable(A_coarse, lb->matrixLabel, 0, coarsePatch);


    Level::selectType finePatches;
    coarsePatch->getFineLevelPatches(finePatches); 
    //__________________________________
    // over all fine patches contained 
    // in the coarse patch
    for(int i=0; i < finePatches.size();i++){  
      const Patch* finePatch = finePatches[i];        

      if(finePatch->hasCoarseFineInterfaceFace() ){
      
        // A_CFI_weights_fine equals A for now. Higher order
        // ghost node interpolation can be developed at
        // a later stage.
        constCCVariable<Stencil7>A_CFI_weights_fine;

        IntVector cl, ch, fl, fh;
        getFineLevelRange(coarsePatch, finePatch, cl, ch, fl, fh);
        if (fh.x() <= fl.x() || fh.y() <= fl.y() || fh.z() <= fl.z()) {
          continue;
        }

        new_dw->getRegion(A_CFI_weights_fine, lb->matrixLabel, 0,fineLevel,fl, fh);

        //__________________________________
        // Iterate over coarsefine interface faces
        vector<Patch::FaceType>::const_iterator iter;  
        for (iter  = finePatch->getCoarseFineInterfaceFaces()->begin(); 
             iter != finePatch->getCoarseFineInterfaceFaces()->end(); ++iter){
          Patch::FaceType patchFace = *iter;

          // determine the iterator on the coarse level.
          CellIterator c_iter(IntVector(-8,-8,-8),IntVector(-9,-9,-9));
          bool isRight_CP_FP_pair;
          matrixCoarseLevelIterator( patchFace,coarsePatch, finePatch, fineLevel,
                                     c_iter ,isRight_CP_FP_pair);

          // eject if this is not the right coarse/fine patch pair
          if (isRight_CP_FP_pair == false ){
            return;
          };   

          // Offset from cell c to nbhr cell underlying the fine patch
          IntVector offset = coarsePatch->faceDirection(patchFace);

          // The matix element is opposite
          // of the patch face
          int element = patchFace;
          if(patchFace == Patch::xminus || 
             patchFace == Patch::yminus || 
             patchFace == Patch::zminus){
            element += 1;  // e, n, t 
          }
          if(patchFace == Patch::xplus || 
             patchFace == Patch::yplus || 
             patchFace == Patch::zplus){
            element -= 1;   // w, s, b
          }
          
          for(; !c_iter.done(); c_iter++){
            IntVector c = *c_iter;
            A_coarse[c].p += A_coarse[c][element];
            A_coarse[c][element] = 0.0;
            
            IntVector fineStart = coarseLevel->mapCellToFiner(c);
    
            // for each coarse level cell iterate over the fine level cells
            // and sum the CFI connection weights and add it to A.p
            double sum_weights = 0.0; 
            IntVector refinementRatio = fineLevel->getRefinementRatio();
              
            for(CellIterator inside(IntVector(0,0,0),refinementRatio );
                !inside.done(); inside++){
              IntVector fc = fineStart + *inside - offset;
              if ((fl.x() <= fc.x()) && (fl.y() <= fc.y()) && (fl.z() <= fc.z()) &&
                  (fc.x() <  fh.x()) && (fc.y() <  fh.y()) && (fc.z() <  fh.z())) {
                sum_weights += A_CFI_weights_fine[fc][patchFace];
              }
            }
            
            //            cerr << "impAMRICE " << c << " A.p " << A_coarse[c].p
            //                 << " -= sum_weights" << sum_weights  
            //                 << " result " << A_coarse[c].p - sum_weights << "\n\n";
            A_coarse[c].p -= sum_weights;
          }  // coarse cell interator
        }  // coarseFineInterface faces
      }  // patch has a coarseFineInterface
    }  // finePatch loop 

    //__________________________________
    //  Print Data
#if 1
    if (switchDebug_setupMatrix) {    
      ostringstream desc;
      desc << "BOT_matrixBC_CFI_coarse_patch_" << coarsePatch->getID()
          <<  " L-" <<coarseLevel->getIndex()<< endl;
      printStencil( 0, coarsePatch, 1, desc.str(), "A_coarse", A_coarse);
    } 
#endif
  }  // course patch loop 
}
  
/*___________________________________________________________________
 Function~  ICE::schedule_bogus_imp_DelP--  
_____________________________________________________________________*/
void ICE::schedule_bogus_imp_delP(SchedulerP& sched,
                                 const PatchSet* perProcPatches,
                                 const MaterialSubset* press_matl,
                                 const MaterialSet* all_matls)
{
  cout_doing << d_myworld->myrank() 
             << " ICE::schedule_bogus_impDelP"<<endl;

  Task* t = scinew Task("bogus_imp_delP",this, &ICE::bogus_imp_delP);
    
  GridP grid = perProcPatches->getUnion()->get(0)->getLevel()->getGrid();

  t->modifies(lb->imp_delPLabel,  press_matl,Task::OutOfDomain);
  sched->addTask(t, perProcPatches, all_matls); 
}
/*___________________________________________________________________ 
 Function~  bogus_imp_delP--  sets imp_delP = 0.0 used for testing
___________________________________________________________________*/
void ICE::bogus_imp_delP(const ProcessorGroup*,
                         const PatchSubset* patches,
                         const MaterialSubset*,
                         DataWarehouse*,
                         DataWarehouse* new_dw)       
{ 
  cout_doing<< d_myworld->myrank()<<" Doing bogus_imp_delP "
            <<"\t\t\t\t\t ICE \tALL LEVELS" << endl;
  for(int p=0;p<patches->size();p++){  
    const Patch* patch = patches->get(p);
    CCVariable<double> imp_delP;
    new_dw->getModifiable(imp_delP,lb->imp_delPLabel, 0,patch);
    imp_delP.initialize(0.0);
  } 
}
