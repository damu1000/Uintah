#include "ConstitutiveModelFactory.h"
#include "IdealGasMP.h"
#include <Core/Malloc/Allocator.h>
#include <Packages/Uintah/Core/Grid/Patch.h>
#include <Packages/Uintah/CCA/Ports/DataWarehouse.h>
#include <Packages/Uintah/Core/Grid/NCVariable.h>
#include <Packages/Uintah/Core/Grid/ParticleSet.h>
#include <Packages/Uintah/Core/Grid/ParticleVariable.h>
#include <Packages/Uintah/Core/Grid/Task.h>
#include <Packages/Uintah/Core/Grid/VarLabel.h>
#include <Packages/Uintah/Core/Grid/VarTypes.h>
#include <Packages/Uintah/Core/Math/Matrix3.h>
#include <Packages/Uintah/CCA/Components/MPM/ConstitutiveModel/MPMMaterial.h>
#include <Packages/Uintah/CCA/Components/MPM/MPMLabel.h>
#include <Packages/Uintah/Core/ProblemSpec/ProblemSpec.h>
#include <Core/Math/MinMax.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Util/NotFinished.h>
#include <fstream>
#include <iostream>
#include <Core/Datatypes/DenseMatrix.h>

using std::cerr;
using namespace Uintah;
using namespace SCIRun;

IdealGasMP::IdealGasMP(ProblemSpecP& ps,  MPMLabel* Mlb, int n8or27)
{
  lb = Mlb;

  ps->require("gamma", d_initialData.gamma);
  ps->require("specific_heat",d_initialData.cv);

  d_8or27 = n8or27;

}

IdealGasMP::~IdealGasMP()
{
}

void IdealGasMP::initializeCMData(const Patch* patch,
                                        const MPMMaterial* matl,
                                        DataWarehouse* new_dw)
{
   // Put stuff in here to initialize each particle's
   // constitutive model parameters and deformationMeasure
   Matrix3 Identity, zero(0.);
   Identity.Identity();

   ParticleSubset* pset = new_dw->getParticleSubset(matl->getDWIndex(), patch);
   ParticleVariable<Matrix3> deformationGradient, pstress;

   new_dw->allocate(deformationGradient, lb->pDeformationMeasureLabel, pset);
   new_dw->allocate(pstress,             lb->pStressLabel,             pset);

   for(ParticleSubset::iterator iter = pset->begin();
          iter != pset->end(); iter++) {
          deformationGradient[*iter] = Identity;
          pstress[*iter] = zero;
   }
   new_dw->put(deformationGradient, lb->pDeformationMeasureLabel);
   new_dw->put(pstress, lb->pStressLabel);

   computeStableTimestep(patch, matl, new_dw);
}

void IdealGasMP::addParticleState(std::vector<const VarLabel*>& from,
				   std::vector<const VarLabel*>& to)
{
   from.push_back(lb->pDeformationMeasureLabel);
   from.push_back(lb->pStressLabel);

   to.push_back(lb->pDeformationMeasureLabel_preReloc);
   to.push_back(lb->pStressLabel_preReloc);
}

void IdealGasMP::computeStableTimestep(const Patch* patch,
                                           const MPMMaterial* matl,
                                           DataWarehouse* new_dw)
{
  // This is only called for the initial timestep - all other timesteps
  // are computed as a side-effect of computeStressTensor
  Vector dx = patch->dCell();
  int matlindex = matl->getDWIndex();
  // Retrieve the array of constitutive parameters
  ParticleSubset* pset = new_dw->getParticleSubset(matlindex, patch);
  constParticleVariable<double> pmass, pvolume, ptemp;
  constParticleVariable<Vector> pvelocity;

  new_dw->get(pmass,     lb->pMassLabel,        pset);
  new_dw->get(pvolume,   lb->pVolumeLabel,      pset);
  new_dw->get(ptemp,     lb->pTemperatureLabel, pset);
  new_dw->get(pvelocity, lb->pVelocityLabel,    pset);

  double c_dil = 0.0;
  Vector WaveSpeed(1.e-12,1.e-12,1.e-12);

  double gamma = d_initialData.gamma;
  double cv    = d_initialData.cv;

  for(ParticleSubset::iterator iter = pset->begin();iter != pset->end();iter++){
     particleIndex idx = *iter;

     double rhoM = pmass[idx]/pvolume[idx];
     double dp_drho = (gamma - 1.0)*cv*ptemp[idx];
     double dp_de   = (gamma - 1.0)*rhoM;

     double p = (gamma - 1.0)*rhoM*cv*ptemp[idx];

     double tmp = dp_drho + dp_de * p /(rhoM * rhoM);

     // Compute wave speed at each particle, store the maximum
     c_dil = sqrt(tmp);
     WaveSpeed=Vector(Max(c_dil+fabs(pvelocity[idx].x()),WaveSpeed.x()),
		      Max(c_dil+fabs(pvelocity[idx].y()),WaveSpeed.y()),
		      Max(c_dil+fabs(pvelocity[idx].z()),WaveSpeed.z()));
  }
  WaveSpeed = dx/WaveSpeed;
  double delT_new = WaveSpeed.minComponent();
  new_dw->put(delt_vartype(delT_new), lb->delTLabel);
}

void IdealGasMP::computeStressTensor(const PatchSubset* patches,
				      const MPMMaterial* matl,
				      DataWarehouse* old_dw,
				      DataWarehouse* new_dw)
{
  for(int pp=0;pp<patches->size();pp++){
    const Patch* patch = patches->get(pp);
    Matrix3 velGrad,deformationGradientInc;
    double J,p,se=0.;
    double c_dil=0.0;
    Vector WaveSpeed(1.e-12,1.e-12,1.e-12);
    Matrix3 Identity;

    Identity.Identity();

    Vector dx = patch->dCell();
    double oodx[3] = {1./dx.x(), 1./dx.y(), 1./dx.z()};

    int matlindex = matl->getDWIndex();
    ParticleSubset* pset = old_dw->getParticleSubset(matlindex, patch);
    constParticleVariable<Point> px;
    ParticleVariable<Matrix3> deformationGradient_new;
    constParticleVariable<Matrix3> deformationGradient;
    ParticleVariable<Matrix3> pstress;
    constParticleVariable<double> pmass,ptemp;
    ParticleVariable<double> pvolume_deformed;
    constParticleVariable<Vector> pvelocity;
    constNCVariable<Vector> gvelocity;
    delt_vartype delT;

    old_dw->get(px,                  lb->pXLabel,                  pset);
    old_dw->get(pmass,               lb->pMassLabel,               pset);
    old_dw->get(ptemp,               lb->pTemperatureLabel,        pset);
    old_dw->get(pvelocity,           lb->pVelocityLabel,           pset);
    old_dw->get(deformationGradient, lb->pDeformationMeasureLabel, pset);
    new_dw->allocate(pstress,        lb->pStressLabel_preReloc,    pset);
    new_dw->allocate(pvolume_deformed, lb->pVolumeDeformedLabel,   pset);
    new_dw->allocate(deformationGradient_new,
		     lb->pDeformationMeasureLabel_preReloc, pset);

    new_dw->get(gvelocity,           lb->gVelocityLabel, matlindex,patch,
            Ghost::AroundCells, 1);
    old_dw->get(delT, lb->delTLabel);
    constParticleVariable<Vector> psize;
    if(d_8or27==27){
      old_dw->get(psize,             lb->pSizeLabel,                  pset);
    }

    double gamma = d_initialData.gamma;
    double cv    = d_initialData.cv;

    double rho_orig = matl->getInitialDensity();

    for(ParticleSubset::iterator iter = pset->begin();
	iter != pset->end(); iter++){
       particleIndex idx = *iter;

       velGrad.set(0.0);
       // Get the node indices that surround the cell
       IntVector ni[MAX_BASIS];
       Vector d_S[MAX_BASIS];

       if(d_8or27==8){
          patch->findCellAndShapeDerivatives(px[idx], ni, d_S);
        }
        else if(d_8or27==27){
          patch->findCellAndShapeDerivatives27(px[idx], ni, d_S,psize[idx]);
        }

       for(int k = 0; k < d_8or27; k++) {
	    const Vector& gvel = gvelocity[ni[k]];
	    for (int j = 0; j<3; j++){
	       for (int i = 0; i<3; i++) {
	          velGrad(i+1,j+1) += gvel(i) * d_S[k](j) * oodx[j];
	       }
	    }
        }

      // Compute the deformation gradient increment using the time_step
      // velocity gradient
      // F_n^np1 = dudx * dt + Identity
      deformationGradientInc = velGrad * delT + Identity;

      // Update the deformation gradient tensor to its time n+1 value.
      deformationGradient_new[idx] = deformationGradientInc *
				     deformationGradient[idx];

      // get the volumetric part of the deformation
      J    = deformationGradient_new[idx].Determinant();

      pvolume_deformed[idx]=(pmass[idx]/rho_orig)*J;
      double rhoM = pmass[idx]/pvolume_deformed[idx];
      double dp_drho = (gamma - 1.0)*cv*ptemp[idx];
      double dp_de   = (gamma - 1.0)*rhoM;

      p = (gamma - 1.0)*rhoM*cv*ptemp[idx];

      double tmp = dp_drho + dp_de * p /(rhoM * rhoM);

      pstress[idx] = Identity*(-p);

      Vector pvelocity_idx = pvelocity[idx];
      if(pmass[idx] > 0){
        c_dil = sqrt(tmp);
      }
      else{
        c_dil = 0.0;
        pvelocity_idx = Vector(0.0,0.0,0.0);
      }
      WaveSpeed=Vector(Max(c_dil+fabs(pvelocity_idx.x()),WaveSpeed.x()),
  		       Max(c_dil+fabs(pvelocity_idx.y()),WaveSpeed.y()),
		       Max(c_dil+fabs(pvelocity_idx.z()),WaveSpeed.z()));
    }

    WaveSpeed = dx/WaveSpeed;
    double delT_new = WaveSpeed.minComponent();
    new_dw->put(delt_vartype(delT_new), lb->delTLabel);
    new_dw->put(pstress,                lb->pStressLabel_preReloc);
    new_dw->put(deformationGradient_new,lb->pDeformationMeasureLabel_preReloc);
    new_dw->put(pvolume_deformed,       lb->pVolumeDeformedLabel);
    new_dw->put(sum_vartype(se),        lb->StrainEnergyLabel);

  }
}

void IdealGasMP::addComputesAndRequires(Task* task,
					 const MPMMaterial* matl,
					 const PatchSet*) const
{
   const MaterialSubset* matlset = matl->thisMaterial();
   task->requires(Task::OldDW, lb->pXLabel,             matlset, Ghost::None);
   task->requires(Task::OldDW, lb->pMassLabel,          matlset, Ghost::None);
   task->requires(Task::OldDW, lb->pTemperatureLabel,   matlset, Ghost::None);
   task->requires(Task::OldDW, lb->pVelocityLabel,      matlset, Ghost::None);
   task->requires(Task::OldDW, lb->pDeformationMeasureLabel,
						        matlset, Ghost::None);
   task->requires(Task::NewDW,lb->gVelocityLabel,matlset,Ghost::AroundCells,1);

   task->requires(Task::OldDW, lb->delTLabel);

   if(d_8or27==27){
     task->requires(Task::OldDW, lb->pSizeLabel,      matlset, Ghost::None);
   }

   task->computes(lb->pStressLabel_preReloc,             matlset);
   task->computes(lb->pDeformationMeasureLabel_preReloc, matlset);
   task->computes(lb->pVolumeDeformedLabel,              matlset);
}

// The "CM" versions use the pressure-volume relationship of the CNH model
double IdealGasMP::computeRhoMicroCM(double press, 
                                      const double Temp,
                                      const MPMMaterial* matl)
{
  double gamma = d_initialData.gamma;
  double cv    = d_initialData.cv;

  return  press/((gamma - 1.0)*cv*Temp);
}

void IdealGasMP::computePressEOSCM(const double rhoM,double& pressure, 
                                               const double Temp,
                                               double& dp_drho, double& tmp,
                                                const MPMMaterial* matl)
{
  double gamma = d_initialData.gamma;
  double cv    = d_initialData.cv;

  pressure   = (gamma - 1.0)*rhoM*cv*Temp;
  dp_drho = (gamma - 1.0)*cv*Temp;
  double dp_de   = (gamma - 1.0)*rhoM;
  tmp = dp_drho + dp_de * pressure/(rhoM*rhoM);    // C^2
}

double IdealGasMP::getCompressibility()
{
  return 1.0/101325.;
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

namespace Uintah {

#if 0
static MPI_Datatype makeMPI_CMData()
{
   ASSERTEQ(sizeof(IdealGasMP::StateData), sizeof(double)*0);
   MPI_Datatype mpitype;
   MPI_Type_vector(1, 0, 0, MPI_DOUBLE, &mpitype);
   MPI_Type_commit(&mpitype);
   return mpitype;
}

const TypeDescription* fun_getTypeDescription(IdealGasMP::StateData*)
{
   static TypeDescription* td = 0;
   if(!td){
      td = scinew TypeDescription(TypeDescription::Other,
			       "IdealGasMP::StateData", true, &makeMPI_CMData);
   }
   return td;
}
#endif
} // End namespace Uintah
