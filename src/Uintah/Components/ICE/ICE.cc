
#include <Uintah/Components/ICE/ICE.h>
#include <Uintah/Interface/CFDInterface.h>
#include <Uintah/Grid/VarLabel.h>
#include <Uintah/Grid/CCVariable.h>
#include <SCICore/Geometry/Vector.h>
#include <Uintah/Parallel/ProcessorGroup.h>
#include <Uintah/Grid/Array3Index.h>
#include <Uintah/Grid/Grid.h>
#include <Uintah/Grid/Level.h>
#include <Uintah/Grid/CCVariable.h>
#include <Uintah/Grid/NCVariable.h>
#include <Uintah/Grid/ParticleSet.h>
#include <Uintah/Grid/ParticleVariable.h>
#include <Uintah/Interface/ProblemSpec.h>
#include <Uintah/Grid/NodeIterator.h>
#include <Uintah/Grid/Patch.h>
#include <Uintah/Grid/PerPatch.h>
#include <Uintah/Grid/ReductionVariable.h>
#include <Uintah/Grid/SimulationState.h>
#include <Uintah/Grid/SoleVariable.h>
#include <Uintah/Grid/Task.h>
#include <Uintah/Interface/DataWarehouse.h>
#include <Uintah/Interface/Scheduler.h>
#include <Uintah/Exceptions/ParameterNotFound.h>
#include <Uintah/Parallel/ProcessorGroup.h>
#include <Uintah/Components/ICE/ICEMaterial.h>
#include <Uintah/Interface/ProblemSpecP.h>
#include <Uintah/Grid/VarTypes.h>

using SCICore::Geometry::Vector;

using namespace Uintah;
using namespace Uintah::ICESpace;


ICE::ICE(const ProcessorGroup* myworld) 
  : UintahParallelComponent(myworld)
{
  lb = new ICELabel();

}

ICE::~ICE()
{
}

void ICE::problemSetup(const ProblemSpecP& prob_spec, GridP& grid,
		       SimulationStateP& sharedState)
{

  d_sharedState = sharedState;

    cerr << "In the preprocessor . . ." << endl;
    
  // Search for the MaterialProperties block and then get the MPM section
  
  ProblemSpecP mat_ps =  prob_spec->findBlock("MaterialProperties");
 
  ProblemSpecP ice_mat_ps = mat_ps->findBlock("ICE");  

  for (ProblemSpecP ps = ice_mat_ps->findBlock("material"); ps != 0;
       ps = ps->findNextBlock("material") ) {
    // Extract out the type of EOS and the 
    // associated parameters
     ICEMaterial *mat = scinew ICEMaterial(ps);
     sharedState->registerMaterial(mat);
  }     

  cout << "Number of materials: " << d_sharedState->getNumMatls() << endl;

}

void ICE::scheduleInitialize(const LevelP& level, SchedulerP& sched, 
			     DataWarehouseP& dw)
{

  Level::const_patchIterator iter;

  for(iter=level->patchesBegin(); iter != level->patchesEnd(); iter++){
    const Patch* patch=*iter;
    Task* t = scinew Task("ICE::actuallyInitialize", patch, dw, dw,this,
			    &ICE::actuallyInitialize);
    t->computes(dw, d_sharedState->get_delt_label());
    for (int m = 0; m < d_sharedState->getNumMatls(); m++ ) {
      Material* matl = d_sharedState->getMaterial(m);
      ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
      if(ice_matl){
	t->computes(dw, lb->temp_CCLabel,ice_matl->getDWIndex(),patch);
	t->computes(dw, lb->rho_micro_CCLabel,ice_matl->getDWIndex(),patch);
	t->computes(dw, lb->cv_CCLabel,ice_matl->getDWIndex(),patch);
      }
    }
    sched->addTask(t);
  }

}

void ICE::scheduleComputeStableTimestep(const LevelP& level,
					SchedulerP& sched,
					DataWarehouseP& dw)
{

}


void ICE::scheduleTimeAdvance(double t, double dt,
			      const LevelP& level,
			      SchedulerP& sched,
			      DataWarehouseP& old_dw,
			      DataWarehouseP& new_dw)
{
  int numMatls = d_sharedState->getNumMatls();

  for(Level::const_patchIterator iter=level->patchesBegin();
       iter != level->patchesEnd(); iter++){

      const Patch* patch=*iter;

      // Step 1
      {
	Task* t = scinew Task("ICE::step1",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep1);
	for (int m = 0; m < numMatls; m++) {
            Material* matl = d_sharedState->getMaterial(m);
            ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
            if(ice_matl){
               EquationOfState* eos = ice_matl->getEOS();
#if 1
               eos->addComputesAndRequiresSS( t,ice_matl,patch, old_dw, new_dw);
               eos->addComputesAndRequiresCEB(t,ice_matl,patch, old_dw, new_dw);
#endif
            }
	}
	sched->addTask(t);
      }

      // Step 2
      {
	Task* t = scinew Task("ICE::step2",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep2);
	for (int m = 0; m < numMatls; m++) {
	}
	sched->addTask(t);
      }

      // Step 3
      {
	Task* t = scinew Task("ICE::step3",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep3);
	for (int m = 0; m < numMatls; m++) {
	}
	sched->addTask(t);
      }

      // Step 4
      {
	Task* t = scinew Task("ICE::step4",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep4);
	for (int m = 0; m < numMatls; m++) {
	}
	sched->addTask(t);
      }

      // Step 5
      {
	Task* t = scinew Task("ICE::step5",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep5);
	for (int m = 0; m < numMatls; m++) {
	}
	sched->addTask(t);
      }

      // Step 6and7
      {
	Task* t = scinew Task("ICE::step6and7",patch, old_dw, new_dw,this,
			       &ICE::actuallyStep6and7);
	for (int m = 0; m < d_sharedState->getNumMatls(); m++ ) {
	  Material* matl = d_sharedState->getMaterial(m);
	  ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
	  if(ice_matl){
	    t->computes(new_dw, lb->temp_CCLabel,ice_matl->getDWIndex(),patch);
	    t->computes(new_dw, lb->rho_micro_CCLabel,ice_matl->getDWIndex(),patch);
	    t->computes(new_dw, lb->cv_CCLabel,ice_matl->getDWIndex(),patch);
	  }
	}
	t->computes(new_dw, d_sharedState->get_delt_label());
	sched->addTask(t);
      }
  }

}

void ICE::actuallyInitialize(const ProcessorGroup*,
			     const Patch* patch,
			     DataWarehouseP& old_dw,
			     DataWarehouseP& new_dw)
  

{

  cout << "Doing actually Initialize" << endl;
  double dT = 0.0001;
  new_dw->put(delt_vartype(dT), lb->delTLabel);

  CCVariable<double> rho_micro, temp, cv;
  for (int m = 0; m < d_sharedState->getNumMatls(); m++ ) {
    Material* matl = d_sharedState->getMaterial(m);
    ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
    if(ice_matl){
      int vfindex = ice_matl->getDWIndex();
      new_dw->allocate(rho_micro,lb->rho_micro_CCLabel,vfindex,patch);
      new_dw->allocate(temp,lb->temp_CCLabel,vfindex,patch);
      new_dw->allocate(cv,lb->cv_CCLabel,vfindex,patch);

      new_dw->put(rho_micro,lb->rho_micro_CCLabel,vfindex,patch);
      new_dw->put(temp,lb->temp_CCLabel,vfindex,patch);
      new_dw->put(cv,lb->cv_CCLabel,vfindex,patch);
    }
  }
  
}

void ICE::actuallyComputeStableTimestep(const ProcessorGroup*,
					const Patch* patch,
					DataWarehouseP& old_dw,
					DataWarehouseP& new_dw)
{
  cout << "Doing acutally Compute Stable Timestep " << endl;
}


void ICE::actuallyStep1(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{

  cout << "Doing actually step1" << endl;

  int numMatls = d_sharedState->getNumMatls();

  // Compute the speed of sound

  for (int m = 0; m < numMatls; m++) {
    Material* matl = d_sharedState->getMaterial(m);
    ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
    if (ice_matl) {
      EquationOfState* eos = ice_matl->getEOS();
      eos->computeSpeedSound(patch,ice_matl,old_dw,new_dw);
      eos->computeEquilibrationPressure(patch,ice_matl,old_dw,new_dw);
    }
  }

}



void ICE::actuallyStep2(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{
  cout << "Doing actually step2" << endl;
}



void ICE::actuallyStep3(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{
  cout << "Doing actually step3" << endl;

}



void ICE::actuallyStep4(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{
  cout << "Doing actually step4" << endl;
}


void ICE::actuallyStep5(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{
  cout << "Doing actually step5" << endl;

}


void ICE::actuallyStep6and7(const ProcessorGroup*,
		   const Patch* patch,
		   DataWarehouseP& old_dw,
		   DataWarehouseP& new_dw)
{

  cout << "Doing actually step6 and 7" << endl;
  double dT = 0.0001;
  new_dw->put(delt_vartype(dT), lb->delTLabel);

    CCVariable<double> rho_micro, temp, cv;
  for (int m = 0; m < d_sharedState->getNumMatls(); m++ ) {
    Material* matl = d_sharedState->getMaterial(m);
    ICEMaterial* ice_matl = dynamic_cast<ICEMaterial*>(matl);
    if(ice_matl){
      int vfindex = ice_matl->getDWIndex();
      new_dw->allocate(rho_micro,lb->rho_micro_CCLabel,vfindex,patch);
      new_dw->allocate(temp,lb->temp_CCLabel,vfindex,patch);
      new_dw->allocate(cv,lb->cv_CCLabel,vfindex,patch);

      new_dw->put(rho_micro,lb->rho_micro_CCLabel,vfindex,patch);
      new_dw->put(temp,lb->temp_CCLabel,vfindex,patch);
      new_dw->put(cv,lb->cv_CCLabel,vfindex,patch);
    }
  }
}




					

//
// $Log$
// Revision 1.26  2000/10/06 03:47:26  jas
// Added computes for the initialization so that step 1 works.  Added a couple
// of CC labels for step 1. Can now go thru multiple timesteps doing work
// only in step 1.
//
// Revision 1.25  2000/10/05 04:26:48  guilkey
// Added code for part of the EOS evaluation.
//
// Revision 1.24  2000/10/05 00:16:33  jas
// Starting to work on the speed of sound stuff.
//
// Revision 1.23  2000/10/04 23:38:21  jas
// All of the steps are in place with just dummy functions.  delT is
// hardwired in for the moment so that we can actually do multiple
// time steps with empty functions.
//
// Revision 1.22  2000/10/04 20:15:27  jas
// Start to bring ICE into UCF.
//
