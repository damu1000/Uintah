
#include "CompNeoHookPlas.h"
#include <Uintah/Grid/Patch.h>
#include <Uintah/Interface/DataWarehouse.h>
#include <Uintah/Grid/NCVariable.h>
#include <Uintah/Grid/ParticleSet.h>
#include <Uintah/Grid/ParticleVariable.h>
#include <Uintah/Grid/ReductionVariable.h>
#include <Uintah/Grid/Task.h>
#include <Uintah/Grid/VarLabel.h>
#include <SCICore/Math/MinMax.h>
#include <Uintah/Components/MPM/Util/Matrix3.h>
#include <Uintah/Components/MPM/ConstitutiveModel/MPMMaterial.h>
#include <Uintah/Grid/VarTypes.h>
#include <SCICore/Malloc/Allocator.h>
#include <iostream>
#include <Uintah/Components/MPM/MPMLabel.h>
using std::cerr;
using namespace Uintah::MPM;
using SCICore::Math::Min;
using SCICore::Math::Max;
using SCICore::Geometry::Vector;

CompNeoHookPlas::CompNeoHookPlas(ProblemSpecP& ps)
{
  ps->require("bulk_modulus",d_initialData.Bulk);
  ps->require("shear_modulus",d_initialData.Shear);
  ps->require("yield_stress",d_initialData.FlowStress);
  ps->require("hardening_modulus",d_initialData.K);
  ps->require("alpha",d_initialData.Alpha);

  p_cmdata_label = scinew VarLabel("p.cmdata",
                                ParticleVariable<CMData>::getTypeDescription());
  p_cmdata_label_preReloc = scinew VarLabel("p.cmdata+",
                                ParticleVariable<CMData>::getTypeDescription());
 
  bElBarLabel = scinew VarLabel("p.bElBar",
		ParticleVariable<Matrix3>::getTypeDescription());
  bElBarLabel_preReloc = scinew VarLabel("p.bElBar+",
		ParticleVariable<Matrix3>::getTypeDescription());
}

void CompNeoHookPlas::addParticleState(std::vector<const VarLabel*>& from,
				       std::vector<const VarLabel*>& to)
{
   from.push_back(p_cmdata_label);
   from.push_back(bElBarLabel);
   to.push_back(p_cmdata_label_preReloc);
   to.push_back(bElBarLabel_preReloc);
}

CompNeoHookPlas::~CompNeoHookPlas()
{
  // Destructor 
}

void CompNeoHookPlas::initializeCMData(const Patch* patch,
                                        const MPMMaterial* matl,
                                        DataWarehouseP& new_dw)
{
   // Put stuff in here to initialize each particle's
   // constitutive model parameters and deformationMeasure
   Matrix3 Identity, zero(0.);
   Identity.Identity();
   const MPMLabel* lb = MPMLabel::getLabels();

   ParticleSubset* pset = new_dw->getParticleSubset(matl->getDWIndex(), patch);
   ParticleVariable<CMData> cmdata;
   new_dw->allocate(cmdata, p_cmdata_label, pset);
   ParticleVariable<Matrix3> deformationGradient;
   new_dw->allocate(deformationGradient, lb->pDeformationMeasureLabel, pset);
   ParticleVariable<Matrix3> pstress;
   new_dw->allocate(pstress, lb->pStressLabel, pset);
   ParticleVariable<Matrix3> bElBar;
   new_dw->allocate(bElBar,  bElBarLabel, pset);

   for(ParticleSubset::iterator iter = pset->begin();
          iter != pset->end(); iter++) {
	    cmdata[*iter] = d_initialData;
          deformationGradient[*iter] = Identity;
          bElBar[*iter] = Identity;
          pstress[*iter] = zero;
   }
   new_dw->put(cmdata, p_cmdata_label);
   new_dw->put(deformationGradient, lb->pDeformationMeasureLabel);
   new_dw->put(pstress, lb->pStressLabel);
   new_dw->put(bElBar, bElBarLabel);

   computeStableTimestep(patch, matl, new_dw);

}

void CompNeoHookPlas::computeStableTimestep(const Patch* patch,
					     const MPMMaterial* matl,
					     DataWarehouseP& new_dw)
{
   // This is only called for the initial timestep - all other timesteps
   // are computed as a side-effect of computeStressTensor
  Vector dx = patch->dCell();
  int matlindex = matl->getDWIndex();
  const MPMLabel* lb = MPMLabel::getLabels();
  // Retrieve the array of constitutive parameters
  ParticleSubset* pset = new_dw->getParticleSubset(matlindex, patch);
  ParticleVariable<CMData> cmdata;
  new_dw->get(cmdata, p_cmdata_label, pset);
  ParticleVariable<double> pmass;
  new_dw->get(pmass, lb->pMassLabel, pset);
  ParticleVariable<double> pvolume;
  new_dw->get(pvolume, lb->pVolumeLabel, pset);
  ParticleVariable<Vector> pvelocity;
  new_dw->get(pvelocity, lb->pVelocityLabel, pset);

  double c_dil = 0.0;
  Vector WaveSpeed(1.e-12,1.e-12,1.e-12);

  for(ParticleSubset::iterator iter = pset->begin();
      iter != pset->end(); iter++){
     particleIndex idx = *iter;

     // Compute wave speed at each particle, store the maximum
     double mu = cmdata[idx].Shear;
     double bulk = cmdata[idx].Bulk;
     if(pmass[idx] > 0){
       c_dil = sqrt((bulk + 4.*mu/3.)*pvolume[idx]/pmass[idx]);
     }
     else{
       c_dil = 0.0;
       pvelocity[idx] = Vector(0.0,0.0,0.0);
     }
     WaveSpeed=Vector(Max(c_dil+fabs(pvelocity[idx].x()),WaveSpeed.x()),
		      Max(c_dil+fabs(pvelocity[idx].y()),WaveSpeed.y()),
		      Max(c_dil+fabs(pvelocity[idx].z()),WaveSpeed.z()));
    }
    WaveSpeed = dx/WaveSpeed;
    double delT_new = WaveSpeed.minComponent();
    new_dw->put(delt_vartype(delT_new), lb->delTLabel);
}

void CompNeoHookPlas::computeStressTensor(const Patch* patch,
					  const MPMMaterial* matl,
					  DataWarehouseP& old_dw,
					  DataWarehouseP& new_dw)
{

  Matrix3 bElBarTrial,deformationGradientInc;
  Matrix3 shearTrial,Shear,normal;
  Matrix3 fbar,velGrad;
  double J,p,fTrial,IEl,muBar,delgamma,sTnorm;
  double onethird = (1.0/3.0);
  double sqtwthds = sqrt(2.0/3.0);
  Matrix3 Identity;
  double c_dil = 0.0,se = 0.;
  Vector WaveSpeed(1.e-12,1.e-12,1.e-12);
  double U,W;

  Identity.Identity();

  Vector dx = patch->dCell();
  double oodx[3] = {1./dx.x(), 1./dx.y(), 1./dx.z()};

  int matlindex = matl->getDWIndex();
  const MPMLabel* lb = MPMLabel::getLabels();
  // Create array for the particle position
  ParticleSubset* pset = old_dw->getParticleSubset(matlindex, patch);
  ParticleVariable<Point> px;
  old_dw->get(px, lb->pXLabel, pset);
  // Create array for the particle deformation
  ParticleVariable<Matrix3> deformationGradient;
  old_dw->get(deformationGradient, lb->pDeformationMeasureLabel, pset);
  ParticleVariable<Matrix3> bElBar;
  old_dw->get(bElBar, bElBarLabel, pset);

  // Create array for the particle stress
  ParticleVariable<Matrix3> pstress;
  old_dw->get(pstress, lb->pStressLabel, pset);

  // Retrieve the array of constitutive parameters
  ParticleVariable<CMData> cmdata;
  old_dw->get(cmdata, p_cmdata_label, pset);
  ParticleVariable<double> pmass;
  old_dw->get(pmass, lb->pMassLabel, pset);
  ParticleVariable<double> pvolume;
  old_dw->get(pvolume, lb->pVolumeLabel, pset);
  ParticleVariable<Vector> pvelocity;
  old_dw->get(pvelocity, lb->pVelocityLabel, pset);
  ParticleVariable<double> pvolumedef;
  new_dw->allocate(pvolumedef, lb->pVolumeDeformedLabel, pset);

  NCVariable<Vector> gvelocity;

  new_dw->get(gvelocity, lb->gMomExedVelocityLabel, matlindex,patch,
	      Ghost::None, 0);
  delt_vartype delT;
  old_dw->get(delT, lb->delTLabel);

  for(ParticleSubset::iterator iter = pset->begin();
     iter != pset->end(); iter++){
     particleIndex idx = *iter; 

     velGrad.set(0.0);
     // Get the node indices that surround the cell
     IntVector ni[8];
     Vector d_S[8];
     if(!patch->findCellAndShapeDerivatives(px[idx], ni, d_S))
         continue;

      for(int k = 0; k < 8; k++) {
          Vector& gvel = gvelocity[ni[k]];
          for (int j = 0; j<3; j++){
            for (int i = 0; i<3; i++) {
                velGrad(i+1,j+1)+=gvel(i) * d_S[k](j) * oodx[j];
            }
          }
      }

    // Calculate the stress Tensor (symmetric 3 x 3 Matrix) given the
    // time step and the velocity gradient and the material constants
    double shear = cmdata[idx].Shear;
    double bulk  = cmdata[idx].Bulk;
    double flow  = cmdata[idx].FlowStress;
    double K     = cmdata[idx].K;
    double alpha = cmdata[idx].Alpha;

    // Compute the deformation gradient increment using the time_step
    // velocity gradient
    // F_n^np1 = dudx * dt + Identity
    deformationGradientInc = velGrad * delT + Identity;

    // Update the deformation gradient tensor to its time n+1 value.
    deformationGradient[idx] = deformationGradientInc *
                             deformationGradient[idx];

    // get the volume preserving part of the deformation gradient increment
    fbar = deformationGradientInc *
			pow(deformationGradientInc.Determinant(),-onethird);

    // predict the elastic part of the volume preserving part of the left
    // Cauchy-Green deformation tensor
    bElBarTrial = fbar*bElBar[idx]*fbar.Transpose();
    IEl = onethird*bElBarTrial.Trace();

    // shearTrial is equal to the shear modulus times dev(bElBar)
    shearTrial = (bElBarTrial - Identity*IEl)*shear;

    // get the volumetric part of the deformation
    J = deformationGradient[idx].Determinant();

    // get the hydrostatic part of the stress
    p = 0.5*bulk*(J - 1.0/J);

    // Compute ||shearTrial||
    sTnorm = shearTrial.Norm();

    muBar = IEl * shear;

    // Check for plastic loading
    fTrial = sTnorm - sqtwthds*(K*alpha + flow);

    if(fTrial > 0.0){
	// plastic

	delgamma = (fTrial/(2.0*muBar)) / (1.0 + (K/(3.0*muBar)));

	normal = shearTrial/sTnorm;

        // The actual elastic shear stress
	Shear = shearTrial - normal*2.0*muBar*delgamma;

        // Deal with history variables
      cmdata[idx].Alpha = alpha + sqtwthds*delgamma;
      bElBar[idx] = Shear/shear + Identity*IEl;
    }
    else {
	// not plastic

      bElBar[idx] = bElBarTrial;
	Shear = shearTrial;
    }

    // compute the total stress (volumetric + deviatoric)
    pstress[idx] = Identity*p + Shear/J;

    // Compute the strain energy for all the particles
    U = .5*bulk*(.5*(pow(J,2.0) - 1.0) - log(J));
    W = .5*shear*(bElBar[idx].Trace() - 3.0);

    se += (U + W)*pvolume[idx];

    // Compute wave speed at each particle, store the maximum

    if(pmass[idx] > 0){
      c_dil = sqrt((bulk + 4.*shear/3.)*pvolume[idx]/pmass[idx]);
    }
    else{
      c_dil = 0.0;
      pvelocity[idx] = Vector(0.0,0.0,0.0);
    }
    WaveSpeed=Vector(Max(c_dil+fabs(pvelocity[idx].x()),WaveSpeed.x()),
		     Max(c_dil+fabs(pvelocity[idx].y()),WaveSpeed.y()),
		     Max(c_dil+fabs(pvelocity[idx].z()),WaveSpeed.z()));
    pvolumedef[idx]=pvolume[idx];
  }

  WaveSpeed = dx/WaveSpeed;
  double delT_new = WaveSpeed.minComponent();
  new_dw->put(delt_vartype(delT_new), lb->delTLabel);
  new_dw->put(pstress, lb->pStressLabel_preReloc);
  new_dw->put(deformationGradient, lb->pDeformationMeasureLabel_preReloc);
  new_dw->put(bElBar, bElBarLabel_preReloc);

  // Put the strain energy in the data warehouse
  new_dw->put(sum_vartype(se), lb->StrainEnergyLabel);

  // This is just carried forward with the updated alpha
  new_dw->put(cmdata, p_cmdata_label_preReloc);
  // Volume is currently being carried forward, will be updated
  new_dw->put(pvolumedef,lb->pVolumeDeformedLabel);

}

double CompNeoHookPlas::computeStrainEnergy(const Patch* patch,
					    const MPMMaterial* matl,
					    DataWarehouseP& new_dw)
{
  double se=0;
#if 0
  double U,W,J,se=0;
  int matlindex = matl->getDWIndex();
  const MPMLabel* lb = MPMLabel::getLabels();
  // Create array for the particle deformation
  ParticleVariable<Matrix3> deformationGradient;
  new_dw->get(deformationGradient, lb->pDeformationMeasureLabel,
              matlindex, patch, Ghost::None, 0);

  // Get the elastic part of the shear strain
  ParticleVariable<Matrix3> bElBar;
  new_dw->get(bElBar, bElBarLabel, matlindex, patch, Ghost::None, 0);
  // Retrieve the array of constitutive parameters
  ParticleVariable<CMData> cmdata;
  new_dw->get(cmdata, p_cmdata_label, matlindex, patch, Ghost::None, 0);
  ParticleVariable<double> pvolume;
  new_dw->get(pvolume, lb->pVolumeLabel, matlindex, patch, Ghost::None, 0);

  ParticleSubset* pset = deformationGradient.getParticleSubset();
  ASSERT(pset == pvolume.getParticleSubset());

  for(ParticleSubset::iterator iter = pset->begin();
     iter != pset->end(); iter++){
     particleIndex idx = *iter;
 
     double shear = cmdata[idx].Shear;
     double bulk  = cmdata[idx].Bulk;

     J = deformationGradient[idx].Determinant();

     U = .5*bulk*(.5*(pow(J,2.0) - 1.0) - log(J));
     W = .5*shear*(bElBar[idx].Trace() - 3.0);

     se += (U + W)*pvolume[idx];
  }
#endif
  return se;
}

void CompNeoHookPlas::addComputesAndRequires(Task* task,
					     const MPMMaterial* matl,
					     const Patch* patch,
					     DataWarehouseP& old_dw,
					     DataWarehouseP& new_dw) const
{
  const MPMLabel* lb = MPMLabel::getLabels();
   task->requires(old_dw, lb->pXLabel, matl->getDWIndex(), patch,
                  Ghost::None);
   task->requires(old_dw, lb->pDeformationMeasureLabel, matl->getDWIndex(), patch,
                  Ghost::None);
   task->requires(old_dw, p_cmdata_label, matl->getDWIndex(),  patch,
                  Ghost::None);
   task->requires(old_dw, lb->pMassLabel, matl->getDWIndex(),  patch,
                  Ghost::None);
   task->requires(old_dw, lb->pVolumeLabel, matl->getDWIndex(),  patch,
                  Ghost::None);
   task->requires(new_dw, lb->gMomExedVelocityLabel, matl->getDWIndex(), patch,
                  Ghost::AroundCells, 1);
   task->requires(old_dw, bElBarLabel, matl->getDWIndex(), patch,
                  Ghost::None);
   task->requires(old_dw, lb->delTLabel);

   task->computes(new_dw, lb->pStressLabel_preReloc, matl->getDWIndex(),  patch);
   task->computes(new_dw, lb->pDeformationMeasureLabel_preReloc, matl->getDWIndex(), patch);
   task->computes(new_dw, bElBarLabel_preReloc, matl->getDWIndex(),  patch);
   task->computes(new_dw, p_cmdata_label_preReloc, matl->getDWIndex(),  patch);
   task->computes(new_dw, lb->pVolumeDeformedLabel, matl->getDWIndex(), patch);
}

#ifdef __sgi
#define IRIX
#pragma set woff 1209
#endif

namespace Uintah {
   namespace MPM {
const TypeDescription* fun_getTypeDescription(CompNeoHookPlas::CMData*)
{
   static TypeDescription* td = 0;
   if(!td){
      ASSERTEQ(sizeof(CompNeoHookPlas::CMData), sizeof(double)*5);
      td = scinew TypeDescription(TypeDescription::Other, "CompNeoHookPlas::CMData", true);
   }
   return td;   
}
   }
}

// $Log$
// Revision 1.30  2000/06/23 22:11:08  guilkey
// Added hack to the wavespeed to avoid floating point exception in case of no particles
// on a patch.
//
// Revision 1.29  2000/06/19 21:22:33  bard
// Moved computes for reduction variables outside of loops over materials.
//
// Revision 1.28  2000/06/16 23:23:39  guilkey
// Got rid of pVolumeDeformedLabel_preReloc to fix some confusion
// the scheduler was having.
//
// Revision 1.27  2000/06/16 05:03:04  sparker
// Moved timestep multiplier to simulation controller
// Fixed timestep min/max clamping so that it really works now
// Implemented "override" for reduction variables that will
//   allow the value of a reduction variable to be overridden
//
// Revision 1.26  2000/06/15 21:57:04  sparker
// Added multi-patch support (bugzilla #107)
// Changed interface to datawarehouse for particle data
// Particles now move from patch to patch
//
// Revision 1.25  2000/06/09 23:52:37  bard
// Added fudge factors to time step calculations.
//
// Revision 1.24  2000/06/09 21:07:33  jas
// Added code to get the fudge factor directly into the constitutive model
// inititialization.
//
// Revision 1.23  2000/06/08 22:00:29  bard
// Added Time Step control to reflect finite deformations and material velocities.
// Removed fudge factors.
//
// Revision 1.22  2000/06/08 16:50:52  guilkey
// Changed some of the dependencies to account for what goes on in
// the burn models.
//
// Revision 1.21  2000/06/01 23:12:06  guilkey
// Code to store integrated quantities in the DW and save them in
// an archive of sorts.  Also added the "computes" in the right tasks.
//
// Revision 1.20  2000/05/31 22:37:09  guilkey
// Put computation of strain energy inside the computeStressTensor functions,
// and store it in a reduction variable in the datawarehouse.
//
// Revision 1.19  2000/05/30 21:07:02  dav
// delt to delT
//
// Revision 1.18  2000/05/30 20:19:02  sparker
// Changed new to scinew to help track down memory leaks
// Changed region to patch
//
// Revision 1.17  2000/05/30 17:08:26  dav
// Changed delt to delT
//
// Revision 1.16  2000/05/26 21:37:33  jas
// Labels are now created and accessed using Singleton class MPMLabel.
//
// Revision 1.15  2000/05/26 18:15:12  guilkey
// Brought the CompNeoHook constitutive model up to functionality
// with the UCF.  Also, cleaned up all of the working models to
// rid them of the SAMRAI crap.
//
// Revision 1.14  2000/05/20 08:09:06  sparker
// Improved TypeDescription
// Finished I/O
// Use new XML utility libraries
//
// Revision 1.13  2000/05/18 16:06:25  guilkey
// Implemented computeStrainEnergy for CompNeoHookPlas.  In both working
// constitutive models, moved the carry forward of the particle volume to
// computeStressTensor.  This "carry forward" will be replaced by a real
// update eventually.  Removed the carry forward in the SerialMPM and
// then replaced where the particle volume was being required from the old_dw
// with requires from the new_dw.  Don't update these files until I've
// checked in a new SerialMPM.cc, which should be in a few minutes.
//
// Revision 1.12  2000/05/17 21:10:07  guilkey
// Fixed computeStrainEnergy so that it can be used as a diagnostic.
//
// Revision 1.11  2000/05/11 20:10:14  dav
// adding MPI stuff.  The biggest change is that old_dws cannot be const and so a large number of declarations had to change.
//
// Revision 1.10  2000/05/10 23:29:34  guilkey
// Filled in addComputesAndRequires
//
// Revision 1.9  2000/05/10 20:02:46  sparker
// Added support for ghost cells on node variables and particle variables
//  (work for 1 patch but not debugged for multiple)
// Do not schedule fracture tasks if fracture not enabled
// Added fracture directory to MPM sub.mk
// Be more uniform about using IntVector
// Made patches have a single uniform index space - still needs work
//
// Revision 1.8  2000/05/07 06:02:03  sparker
// Added beginnings of multiple patch support and real dependencies
//  for the scheduler
//
// Revision 1.7  2000/05/04 16:37:30  guilkey
// Got the CompNeoHookPlas constitutive model up to speed.  It seems
// to work but hasn't had a rigorous test yet.
//
// Revision 1.6  2000/04/26 06:48:15  sparker
// Streamlined namespaces
//
// Revision 1.5  2000/04/25 18:42:34  jas
// Revised the factory method and constructor to take a ProblemSpec argument
// to create a new constitutive model.
//
// Revision 1.4  2000/04/19 21:15:55  jas
// Changed BoundedArray to vector<double>.  More stuff to compile.  Critical
// functions that need access to data warehouse still have WONT_COMPILE_YET
// around the methods.
//
// Revision 1.3  2000/04/14 17:34:42  jas
// Added ProblemSpecP capabilities.
//
// Revision 1.2  2000/03/20 17:17:07  sparker
// Made it compile.  There are now several #idef WONT_COMPILE_YET statements.
//
// Revision 1.1  2000/03/14 22:11:48  jas
// Initial creation of the constitutive model directory with the legacy
// constitutive model classes.
//
// Revision 1.1  2000/02/24 06:11:54  sparker
// Imported homebrew code
//
// Revision 1.1  2000/01/24 22:48:48  sparker
// Stuff may actually work someday...
//

