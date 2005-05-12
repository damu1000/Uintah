#include "ElasticPlastic.h"
#include "PlasticityModels/YieldConditionFactory.h"
#include "PlasticityModels/StabilityCheckFactory.h"
#include "PlasticityModels/PlasticityModelFactory.h"
#include "PlasticityModels/DamageModelFactory.h"
#include "PlasticityModels/MPMEquationOfStateFactory.h"
#include "PlasticityModels/ShearModulusModelFactory.h"
#include "PlasticityModels/MeltingTempModelFactory.h"
#include "PlasticityModels/PlasticityState.h"
#include <Packages/Uintah/CCA/Components/MPM/ConstitutiveModel/MPMMaterial.h>
#include <Packages/Uintah/Core/Grid/Patch.h>
#include <Packages/Uintah/CCA/Ports/DataWarehouse.h>
#include <Packages/Uintah/Core/Grid/Variables/NCVariable.h>
#include <Packages/Uintah/Core/Grid/Variables/ParticleSet.h>
#include <Packages/Uintah/Core/Grid/Variables/ParticleVariable.h>
#include <Packages/Uintah/Core/Grid/Task.h>
#include <Packages/Uintah/Core/Grid/Level.h>
#include <Packages/Uintah/Core/Grid/Variables/VarLabel.h>
#include <Packages/Uintah/Core/Labels/MPMLabel.h>
#include <Core/Math/MinMax.h>
#include <Core/Math/Gaussian.h>
#include <Packages/Uintah/Core/Math/Matrix3.h>
#include <Packages/Uintah/Core/Math/FastMatrix.h>
#include <Packages/Uintah/Core/Math/TangentModulusTensor.h>
#include <Packages/Uintah/Core/Math/Short27.h> //for Fracture
#include <Packages/Uintah/Core/Grid/Variables/VarTypes.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Util/DebugStream.h>
#include <math.h>
#include <sgi_stl_warnings_off.h>
#include <iostream>
#include <sgi_stl_warnings_on.h>

#include <Packages/Uintah/Core/ProblemSpec/ProblemSpec.h>
#include <Packages/Uintah/Core/Exceptions/ParameterNotFound.h>

using std::cerr;
using namespace Uintah;
using namespace SCIRun;

static DebugStream cout_EP("EP",false);
static DebugStream cout_EP1("EP1",false);

ElasticPlastic::ElasticPlastic(ProblemSpecP& ps,MPMLabel* Mlb,MPMFlags* Mflag)
  : ConstitutiveModel(Mlb,Mflag)
{
  ps->require("bulk_modulus",d_initialData.Bulk);
  ps->require("shear_modulus",d_initialData.Shear);
  d_initialData.alpha = 1.0e-5; // default is per K
  ps->get("coeff_thermal_expansion", d_initialData.alpha);
  d_initialData.Chi = 0.9;
  ps->get("taylor_quinney_coeff",d_initialData.Chi);

  d_tol = 1.0e-10;
  ps->get("tolerance",d_tol);

  d_useModifiedEOS = false;
  ps->get("useModifiedEOS",d_useModifiedEOS);

  d_initialMaterialTemperature = 294.0;
  ps->get("initial_material_temperature",d_initialMaterialTemperature);

  d_checkTeplaFailureCriterion = true;
  ps->get("check_TEPLA_failure_criterion",d_checkTeplaFailureCriterion);

  d_doMelting = true;
  ps->get("do_melting",d_doMelting);

  d_yield = YieldConditionFactory::create(ps);
  if(!d_yield){
    ostringstream desc;
    desc << "An error occured in the YieldConditionFactory that has \n"
         << " slipped through the existing bullet proofing. Please tell \n"
         << " Biswajit.  "<< endl;
    throw ParameterNotFound(desc.str());
  }

  d_stable = StabilityCheckFactory::create(ps);
  if(!d_stable) cerr << "Stability check disabled\n";

  d_plastic = PlasticityModelFactory::create(ps);
  if(!d_plastic){
    ostringstream desc;
    desc << "An error occured in the PlasticityModelFactory that has \n"
         << " slipped through the existing bullet proofing. Please tell \n"
         << " Biswajit.  "<< endl;
    throw ParameterNotFound(desc.str());
  }

  d_damage = DamageModelFactory::create(ps);
  if(!d_damage){
    ostringstream desc;
    desc << "An error occured in the DamageModelFactory that has \n"
         << " slipped through the existing bullet proofing. Please tell \n"
         << " Biswajit.  "<< endl;
    throw ParameterNotFound(desc.str());
  }
  
  d_eos = MPMEquationOfStateFactory::create(ps);
  if(!d_eos){
    ostringstream desc;
    desc << "An error occured in the EquationOfStateFactory that has \n"
         << " slipped through the existing bullet proofing. Please tell \n"
         << " Biswajit.  "<< endl;
    throw ParameterNotFound(desc.str());
  }

  d_shear = ShearModulusModelFactory::create(ps);
  if (!d_shear) {
    ostringstream desc;
    desc << "ElasticPlastic::Error in shear modulus model factory" << endl;
    throw ParameterNotFound(desc.str());
  }
  
  d_melt = MeltingTempModelFactory::create(ps);
  if (!d_melt) {
    ostringstream desc;
    desc << "ElasticPlastic::Error in melting temp model factory" << endl;
    throw ParameterNotFound(desc.str());
  }
  
  setErosionAlgorithm();
  getInitialPorosityData(ps);
  getInitialDamageData(ps);
  getSpecificHeatData(ps);
  initializeLocalMPMLabels();

}

ElasticPlastic::ElasticPlastic(const ElasticPlastic* cm)
{
  lb = cm->lb;
  flag = cm->flag;
  NGN = cm->NGN ;
  d_initialData.Bulk = cm->d_initialData.Bulk;
  d_initialData.Shear = cm->d_initialData.Shear;
  d_initialData.alpha = cm->d_initialData.alpha;
  d_initialData.Chi = cm->d_initialData.Chi;

  d_tol = cm->d_tol ;
  d_useModifiedEOS = cm->d_useModifiedEOS;

  d_initialMaterialTemperature = cm->d_initialMaterialTemperature ;
  d_checkTeplaFailureCriterion = cm->d_checkTeplaFailureCriterion;

  d_setStressToZero = cm->d_setStressToZero;
  d_allowNoTension = cm->d_allowNoTension;
  d_removeMass = cm->d_removeMass;

  d_evolvePorosity = cm->d_evolvePorosity;
  d_porosity.f0 = cm->d_porosity.f0 ;
  d_porosity.f0_std = cm->d_porosity.f0_std ;
  d_porosity.fc = cm->d_porosity.fc ;
  d_porosity.fn = cm->d_porosity.fn ;
  d_porosity.en = cm->d_porosity.en ;
  d_porosity.sn = cm->d_porosity.sn ;
  d_porosity.porosityDist = cm->d_porosity.porosityDist ;

  d_evolveDamage = cm->d_evolveDamage;
  d_scalarDam.D0 = cm->d_scalarDam.D0 ;
  d_scalarDam.D0_std = cm->d_scalarDam.D0_std ;
  d_scalarDam.Dc = cm->d_scalarDam.Dc ;
  d_scalarDam.scalarDamageDist = cm->d_scalarDam.scalarDamageDist ;

  d_computeSpecificHeat = cm->d_computeSpecificHeat;
  d_Cp.A = cm->d_Cp.A;
  d_Cp.B = cm->d_Cp.B;
  d_Cp.C = cm->d_Cp.C;

  d_yield = YieldConditionFactory::createCopy(cm->d_yield);
  d_stable = StabilityCheckFactory::createCopy(cm->d_stable);
  d_plastic = PlasticityModelFactory::createCopy(cm->d_plastic);
  d_damage = DamageModelFactory::createCopy(cm->d_damage);
  d_eos = MPMEquationOfStateFactory::createCopy(cm->d_eos);
  d_shear = ShearModulusModelFactory::createCopy(cm->d_shear);
  d_melt = MeltingTempModelFactory::createCopy(cm->d_melt);
  
  initializeLocalMPMLabels();
}

ElasticPlastic::~ElasticPlastic()
{
  // Destructor 
  VarLabel::destroy(pRotationLabel);
  VarLabel::destroy(pStrainRateLabel);
  VarLabel::destroy(pPlasticStrainLabel);
  VarLabel::destroy(pPlasticStrainRateLabel);
  VarLabel::destroy(pDamageLabel);
  VarLabel::destroy(pPorosityLabel);
  VarLabel::destroy(pLocalizedLabel);

  VarLabel::destroy(pRotationLabel_preReloc);
  VarLabel::destroy(pStrainRateLabel_preReloc);
  VarLabel::destroy(pPlasticStrainLabel_preReloc);
  VarLabel::destroy(pPlasticStrainRateLabel_preReloc);
  VarLabel::destroy(pDamageLabel_preReloc);
  VarLabel::destroy(pPorosityLabel_preReloc);
  VarLabel::destroy(pLocalizedLabel_preReloc);

  delete d_plastic;
  delete d_yield;
  delete d_stable;
  delete d_damage;
  delete d_eos;
  delete d_shear;
  delete d_melt;
}

void
ElasticPlastic::initializeLocalMPMLabels()
{
  pRotationLabel = VarLabel::create("p.rotation",
     ParticleVariable<Matrix3>::getTypeDescription());
  pStrainRateLabel = VarLabel::create("p.strainRate",
     ParticleVariable<double>::getTypeDescription());
  pPlasticStrainLabel = VarLabel::create("p.plasticStrain",
     ParticleVariable<double>::getTypeDescription());
  pPlasticStrainRateLabel = VarLabel::create("p.plasticStrainRate",
     ParticleVariable<double>::getTypeDescription());
  pDamageLabel = VarLabel::create("p.damage",
     ParticleVariable<double>::getTypeDescription());
  pPorosityLabel = VarLabel::create("p.porosity",
     ParticleVariable<double>::getTypeDescription());
  pLocalizedLabel = VarLabel::create("p.localized",
     ParticleVariable<int>::getTypeDescription());

  pRotationLabel_preReloc = VarLabel::create("p.rotation+",
     ParticleVariable<Matrix3>::getTypeDescription());
  pStrainRateLabel_preReloc = VarLabel::create("p.strainRate+",
     ParticleVariable<double>::getTypeDescription());
  pPlasticStrainLabel_preReloc = VarLabel::create("p.plasticStrain+",
     ParticleVariable<double>::getTypeDescription());
  pPlasticStrainRateLabel_preReloc = VarLabel::create("p.plasticStrainRate+",
     ParticleVariable<double>::getTypeDescription());
  pDamageLabel_preReloc = VarLabel::create("p.damage+",
     ParticleVariable<double>::getTypeDescription());
  pPorosityLabel_preReloc = VarLabel::create("p.porosity+",
     ParticleVariable<double>::getTypeDescription());
  pLocalizedLabel_preReloc = VarLabel::create("p.localized+",
     ParticleVariable<int>::getTypeDescription());
}

void 
ElasticPlastic::getInitialPorosityData(ProblemSpecP& ps)
{
  d_evolvePorosity = true;
  ps->get("evolve_porosity",d_evolvePorosity);
  d_porosity.f0 = 0.002;  // Initial porosity
  d_porosity.f0_std = 0.002;  // Initial STD porosity
  d_porosity.fc = 0.5;    // Critical porosity
  d_porosity.fn = 0.1;    // Volume fraction of void nucleating particles
  d_porosity.en = 0.3;    // Mean strain for nucleation
  d_porosity.sn = 0.1;    // Standard deviation strain for nucleation
  d_porosity.porosityDist = "constant";
  ps->get("initial_mean_porosity",         d_porosity.f0);
  ps->get("initial_std_porosity",          d_porosity.f0_std);
  ps->get("critical_porosity",             d_porosity.fc);
  ps->get("frac_nucleation",               d_porosity.fn);
  ps->get("meanstrain_nucleation",         d_porosity.en);
  ps->get("stddevstrain_nucleation",       d_porosity.sn);
  ps->get("initial_porosity_distrib",      d_porosity.porosityDist);
}

void 
ElasticPlastic::getInitialDamageData(ProblemSpecP& ps)
{
  d_evolveDamage = true;
  ps->get("evolve_damage",d_evolveDamage);
  d_scalarDam.D0 = 0.0; // Initial scalar damage
  d_scalarDam.D0_std = 0.0; // Initial STD scalar damage
  d_scalarDam.Dc = 1.0; // Critical scalar damage
  d_scalarDam.scalarDamageDist = "constant";
  ps->get("initial_mean_scalar_damage",        d_scalarDam.D0);
  ps->get("initial_std_scalar_damage",         d_scalarDam.D0_std);
  ps->get("critical_scalar_damage",            d_scalarDam.Dc);
  ps->get("initial_scalar_damage_distrib",     d_scalarDam.scalarDamageDist);
}

/*! Compute specific heat
   double T = temperature;
   C_p = 1.0e3*(A + B*T + C/T^2)
   ** For steel **
   C_p = 1.0e3*(0.09278 + 7.454e-4*T + 12404.0/(T*T));
*/
void 
ElasticPlastic::getSpecificHeatData(ProblemSpecP& ps)
{
  d_computeSpecificHeat = false;
  ps->get("compute_specific_heat",d_computeSpecificHeat);
  d_Cp.A = 0.09278;  // Constant A
  d_Cp.B = 7.454e-4; // Constant B
  d_Cp.C = 12404.0;  // Constant C
  ps->get("Cp_constA", d_Cp.A);
  ps->get("Cp_constB", d_Cp.B);
  ps->get("Cp_constC", d_Cp.C);
}

void 
ElasticPlastic::setErosionAlgorithm()
{
  d_setStressToZero = false;
  d_allowNoTension = false;
  d_removeMass = false;
  if (flag->d_doErosion) {
    if (flag->d_erosionAlgorithm == "RemoveMass") 
      d_removeMass = true;
    else if (flag->d_erosionAlgorithm == "AllowNoTension") 
      d_allowNoTension = true;
    else if (flag->d_erosionAlgorithm == "ZeroStress") 
      d_setStressToZero = true;
  }
}

void 
ElasticPlastic::addParticleState(std::vector<const VarLabel*>& from,
                                 std::vector<const VarLabel*>& to)
{
  // Add the local particle state data for this constitutive model.
  from.push_back(pRotationLabel);
  from.push_back(pStrainRateLabel);
  from.push_back(pPlasticStrainLabel);
  from.push_back(pPlasticStrainRateLabel);
  from.push_back(pDamageLabel);
  from.push_back(pPorosityLabel);
  from.push_back(pLocalizedLabel);

  to.push_back(pRotationLabel_preReloc);
  to.push_back(pStrainRateLabel_preReloc);
  to.push_back(pPlasticStrainLabel_preReloc);
  to.push_back(pPlasticStrainRateLabel_preReloc);
  to.push_back(pDamageLabel_preReloc);
  to.push_back(pPorosityLabel_preReloc);
  to.push_back(pLocalizedLabel_preReloc);

  // Add the particle state for the plasticity model
  d_plastic->addParticleState(from, to);
}

void 
ElasticPlastic::addInitialComputesAndRequires(Task* task,
                                              const MPMMaterial* matl,
                                              const PatchSet* patch) const
{
  const MaterialSubset* matlset = matl->thisMaterial();

  task->computes(pRotationLabel,      matlset);
  task->computes(pStrainRateLabel,    matlset);
  task->computes(pPlasticStrainLabel, matlset);
  task->computes(pPlasticStrainRateLabel, matlset);
  task->computes(pDamageLabel,        matlset);
  task->computes(pPorosityLabel,      matlset);
  task->computes(pLocalizedLabel,     matlset);
 
  // Add internal evolution variables computed by plasticity model
  d_plastic->addInitialComputesAndRequires(task, matl, patch);
}

void 
ElasticPlastic::initializeCMData(const Patch* patch,
                                 const MPMMaterial* matl,
                                 DataWarehouse* new_dw)
{
  // Initialize the variables shared by all constitutive models
  // This method is defined in the ConstitutiveModel base class.
  initSharedDataForExplicit(patch, matl, new_dw);

  // Put stuff in here to initialize each particle's
  // constitutive model parameters and deformationMeasure
  //cout << "Initialize CM Data in ElasticPlastic" << endl;
  Matrix3 one, zero(0.); one.Identity();

  ParticleSubset* pset = new_dw->getParticleSubset(matl->getDWIndex(), patch);

  ParticleVariable<Matrix3> pRotation;
  ParticleVariable<double> pPlasticStrain, pDamage, pPorosity, 
                           pPlasticStrainRate, pStrainRate;
  ParticleVariable<int> pLocalized;

  new_dw->allocateAndPut(pRotation, pRotationLabel, pset);
  new_dw->allocateAndPut(pStrainRate, pStrainRateLabel, pset);
  new_dw->allocateAndPut(pPlasticStrain, pPlasticStrainLabel, pset);
  new_dw->allocateAndPut(pPlasticStrainRate, pPlasticStrainRateLabel, pset);
  new_dw->allocateAndPut(pDamage, pDamageLabel, pset);
  new_dw->allocateAndPut(pLocalized, pLocalizedLabel, pset);
  new_dw->allocateAndPut(pPorosity, pPorosityLabel, pset);

  for(ParticleSubset::iterator iter = pset->begin();iter != pset->end();iter++){

    pRotation[*iter] = one;
    pStrainRate[*iter] = 0.0;
    pPlasticStrain[*iter] = 0.0;
    pPlasticStrainRate[*iter] = 0.0;
    pDamage[*iter] = d_damage->initialize();
    pPorosity[*iter] = d_porosity.f0;
    pLocalized[*iter] = 0;
  }

  // Do some extra things if the porosity or the damage distribution
  // is not uniform.  
  // ** WARNING ** Weibull distribution needs to be implemented.
  //               At present only Gaussian available.
  if (d_porosity.porosityDist != "constant") {

    SCIRun::Gaussian gaussGen(d_porosity.f0, d_porosity.f0_std, 0);
    ParticleSubset::iterator iter = pset->begin();
    for(;iter != pset->end();iter++){

      // Generate a Gaussian distributed random number given the mean
      // porosity and the std.
      pPorosity[*iter] = fabs(gaussGen.rand());
    }
  }

  if (d_scalarDam.scalarDamageDist != "constant") {

    SCIRun::Gaussian gaussGen(d_scalarDam.D0, d_scalarDam.D0_std, 0);
    ParticleSubset::iterator iter = pset->begin();
    for(;iter != pset->end();iter++){

      // Generate a Gaussian distributed random number given the mean
      // damage and the std.
      pDamage[*iter] = fabs(gaussGen.rand());
    }
  }

  // Initialize the data for the plasticity model
  d_plastic->initializeInternalVars(pset, new_dw);

  computeStableTimestep(patch, matl, new_dw);
}

void 
ElasticPlastic::computeStableTimestep(const Patch* patch,
                                      const MPMMaterial* matl,
                                      DataWarehouse* new_dw)
{
  // This is only called for the initial timestep - all other timesteps
  // are computed as a side-effect of computeStressTensor
  Vector dx = patch->dCell();
  int matlindex = matl->getDWIndex();

  // Retrieve the array of constitutive parameters
  ParticleSubset* pset = new_dw->getParticleSubset(matlindex, patch);

  constParticleVariable<double> pMass, pVolume;
  constParticleVariable<Vector> pVelocity;

  new_dw->get(pMass,     lb->pMassLabel,     pset);
  new_dw->get(pVolume,   lb->pVolumeLabel,   pset);
  new_dw->get(pVelocity, lb->pVelocityLabel, pset);

  double c_dil = 0.0;
  Vector WaveSpeed(1.e-12,1.e-12,1.e-12);

  double shear = d_initialData.Shear;
  double bulk = d_initialData.Bulk;

  ParticleSubset::iterator iter = pset->begin(); 
  for(; iter != pset->end(); iter++){
    particleIndex idx = *iter;

    // Compute wave speed at each particle, store the maximum
    Vector pvelocity_idx = pVelocity[idx];
    if(pMass[idx] > 0){
      c_dil = sqrt((bulk + 4.0*shear/3.0)*pVolume[idx]/pMass[idx]);
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
  new_dw->put(delt_vartype(patch->getLevel()->adjustDelt(delT_new)), 
              lb->delTLabel);
}

void 
ElasticPlastic::addComputesAndRequires(Task* task,
                                       const MPMMaterial* matl,
                                       const PatchSet* patch) const
{
  // Add the computes and requires that are common to all explicit 
  // constitutive models.  The method is defined in the ConstitutiveModel
  // base class.
  const MaterialSubset* matlset = matl->thisMaterial();
  addSharedCRForExplicit(task, matlset, patch);

  // Other constitutive model and input dependent computes and requires
  Ghost::GhostType  gnone = Ghost::None;

  task->requires(Task::OldDW, lb->pTempPreviousLabel, matlset, gnone); 
  task->requires(Task::NewDW, lb->pTempCurrentLabel,  matlset, gnone); 
  task->requires(Task::OldDW, lb->pErosionLabel,      matlset, gnone);

  task->requires(Task::OldDW, pRotationLabel,         matlset, gnone);
  task->requires(Task::OldDW, pStrainRateLabel,       matlset, gnone);
  task->requires(Task::OldDW, pPlasticStrainLabel,    matlset, gnone);
  task->requires(Task::OldDW, pPlasticStrainRateLabel,matlset, gnone);
  task->requires(Task::OldDW, pDamageLabel,           matlset, gnone);
  task->requires(Task::OldDW, pPorosityLabel,         matlset, gnone);
  task->requires(Task::OldDW, pLocalizedLabel,        matlset, gnone);

  task->computes(pRotationLabel_preReloc,       matlset);
  task->computes(pStrainRateLabel_preReloc,     matlset);
  task->computes(pPlasticStrainLabel_preReloc,  matlset);
  task->computes(pPlasticStrainRateLabel_preReloc,  matlset);
  task->computes(pDamageLabel_preReloc,         matlset);
  task->computes(pPorosityLabel_preReloc,       matlset);
  task->computes(pLocalizedLabel_preReloc,      matlset);

  // Add internal evolution variables computed by plasticity model
  d_plastic->addComputesAndRequires(task, matl, patch);
}

void 
ElasticPlastic::computeStressTensor(const PatchSubset* patches,
                                    const MPMMaterial* matl,
                                    DataWarehouse* old_dw,
                                    DataWarehouse* new_dw)
{
  /*
  if (cout_EP.active()) {
    cout_EP << getpid() << "ComputeStressTensor: In : Matl = " << matl 
            << " id = " << matl->getDWIndex() <<  " patch = " 
            << (patches->get(0))->getID();
  }
  */

  // General stuff
  Matrix3 one; one.Identity(); Matrix3 zero(0.0);
  Matrix3 tensorD(0.0); // Rate of deformation
  Matrix3 tensorW(0.0); // Spin 
  Matrix3 tensorF; tensorF.Identity(); // Deformation gradient
  Matrix3 tensorU; tensorU.Identity(); // Right Cauchy-Green stretch
  Matrix3 tensorR; tensorR.Identity(); // Rotation 
  Matrix3 tensorSig(0.0); // The Cauchy stress
  Matrix3 tensorEta(0.0); // Deviatoric part of tensor D
  Matrix3 tensorS(0.0); // Devaitoric part of tensor Sig
  Matrix3 tensorF_new; tensorF_new.Identity(); // Deformation gradient

  Vector WaveSpeed(1.e-12,1.e-12,1.e-12);

  double bulk  = d_initialData.Bulk;
  double shear = d_initialData.Shear;
  double alpha = d_initialData.alpha;
  double rho_0 = matl->getInitialDensity();
  double Tm = matl->getMeltTemperature();
  double sqrtTwo = sqrt(2.0);
  double sqrtThreeTwo = sqrt(1.5);
  double sqrtTwoThird = 1.0/sqrtThreeTwo;
  double totalStrainEnergy = 0.0;

  // Loop thru patches
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);

    ParticleInterpolator* interpolator = flag->d_interpolator->clone(patch);
    vector<IntVector> ni;
    ni.reserve(interpolator->size());
    vector<Vector> d_S;
    d_S.reserve(interpolator->size());
    

    //cerr << getpid() << " patch = " << patch->getID() << endl;
    // Get grid size
    Vector dx = patch->dCell();
    double oodx[3] = {1./dx.x(), 1./dx.y(), 1./dx.z()};
    //double dx_ave = (dx.x() + dx.y() + dx.z())/3.0;

    // Get the set of particles
    int dwi = matl->getDWIndex();
    ParticleSubset* pset = old_dw->getParticleSubset(dwi, patch);

    // GET GLOBAL DATA 

    // Get the deformation gradient (F)
    // Note : The deformation gradient from the old datawarehouse is no
    // longer used, but it is updated for possible use elsewhere
    constParticleVariable<Matrix3>  pDeformGrad;
    old_dw->get(pDeformGrad, lb->pDeformationMeasureLabel, pset);

    // Get the particle location, particle size, particle mass, particle volume
    constParticleVariable<Point> px;
    constParticleVariable<Vector> psize;
    constParticleVariable<double> pMass, pVolume;
    old_dw->get(px, lb->pXLabel, pset);
    old_dw->get(psize, lb->pSizeLabel, pset);
    old_dw->get(pMass, lb->pMassLabel, pset);
    old_dw->get(pVolume, lb->pVolumeLabel, pset);

    // Get the velocity from the grid and particle velocity
    constParticleVariable<Vector> pVelocity;
    constNCVariable<Vector> gVelocity;
    old_dw->get(pVelocity, lb->pVelocityLabel, pset);
    Ghost::GhostType  gac = Ghost::AroundCells;
    new_dw->get(gVelocity, lb->gVelocityLabel, dwi, patch, gac, NGN);

    // Get the particle stress and temperature
    constParticleVariable<Matrix3> pStress;
    constParticleVariable<double> pTemperature;
    old_dw->get(pStress, lb->pStressLabel, pset);
    old_dw->get(pTemperature, lb->pTemperatureLabel, pset);

    constParticleVariable<double> pErosion;
    old_dw->get(pErosion, lb->pErosionLabel, pset);

    // Get the time increment (delT)
    delt_vartype delT;
    old_dw->get(delT, lb->delTLabel, getLevel(patches));

    constParticleVariable<Short27> pgCode;
    constNCVariable<Vector> GVelocity;
    if (flag->d_fracture) {
      new_dw->get(pgCode, lb->pgCodeLabel, pset);
      new_dw->get(GVelocity,lb->GVelocityLabel, dwi, patch, gac, NGN);
    }

    constParticleVariable<double> pTempPrev, pTempCur;
    old_dw->get(pTempPrev, lb->pTempPreviousLabel, pset); 
    new_dw->get(pTempCur,  lb->pTempCurrentLabel,  pset); 

    // GET LOCAL DATA 

    // Get the left stretch (V) and rotation (R)
    constParticleVariable<Matrix3> pRotation;
    old_dw->get(pRotation, pRotationLabel, pset);

    // Get the particle damage state
    constParticleVariable<double> pPlasticStrain, pDamage, pPorosity, 
      pStrainRate, pPlasticStrainRate;
    old_dw->get(pPlasticStrain, pPlasticStrainLabel, pset);
    old_dw->get(pDamage, pDamageLabel, pset);
    old_dw->get(pStrainRate, pStrainRateLabel, pset);
    old_dw->get(pPlasticStrainRate, pPlasticStrainRateLabel, pset);
    old_dw->get(pPorosity, pPorosityLabel, pset);

    // Get the particle localization state
    constParticleVariable<int> pLocalized;
    old_dw->get(pLocalized, pLocalizedLabel, pset);

    // Create and allocate arrays for storing the updated information
    // GLOBAL
    ParticleVariable<Matrix3> pDeformGrad_new, pStress_new;
    ParticleVariable<double> pVolume_deformed;
    new_dw->allocateAndPut(pDeformGrad_new,  
                           lb->pDeformationMeasureLabel_preReloc, pset);
    new_dw->allocateAndPut(pStress_new,      
                           lb->pStressLabel_preReloc,             pset);
    new_dw->allocateAndPut(pVolume_deformed, 
                           lb->pVolumeDeformedLabel,              pset);

    // LOCAL
    ParticleVariable<Matrix3> pRotation_new;
    ParticleVariable<double>  pPlasticStrain_new, pDamage_new, pPorosity_new, 
      pStrainRate_new, pPlasticStrainRate_new;
    ParticleVariable<int>     pLocalized_new;
    new_dw->allocateAndPut(pRotation_new,    
                           pRotationLabel_preReloc,               pset);
    new_dw->allocateAndPut(pStrainRate_new,      
                           pStrainRateLabel_preReloc,             pset);
    new_dw->allocateAndPut(pPlasticStrain_new,      
                           pPlasticStrainLabel_preReloc,          pset);
    new_dw->allocateAndPut(pPlasticStrainRate_new,      
                           pPlasticStrainRateLabel_preReloc,          pset);
    new_dw->allocateAndPut(pDamage_new,      
                           pDamageLabel_preReloc,                 pset);
    new_dw->allocateAndPut(pPorosity_new,      
                           pPorosityLabel_preReloc,               pset);
    new_dw->allocateAndPut(pLocalized_new,      
                           pLocalizedLabel_preReloc,              pset);

    // Allocate variable to store internal heating rate
    ParticleVariable<double> pIntHeatRate;
    new_dw->allocateAndPut(pIntHeatRate, lb->pInternalHeatRateLabel_preReloc, 
                           pset);

    // Get the plastic strain
    d_plastic->getInternalVars(pset, old_dw);
    d_plastic->allocateAndPutInternalVars(pset, new_dw);

    // Loop thru particles
    ParticleSubset::iterator iter = pset->begin(); 
    for( ; iter != pset->end(); iter++){
      particleIndex idx = *iter;

      // Assign zero internal heating by default - modify if necessary.
      pIntHeatRate[idx] = 0.0;

      /*
      if (cout_EP.active()) {
        cout_EP << getpid() << " idx = " << idx 
                << " vel = " << pVelocity[idx] << endl;
      }
      */

      //-----------------------------------------------------------------------
      // Stage 1:
      //-----------------------------------------------------------------------
      // Calculate the velocity gradient (L) from the grid velocity

      interpolator->findCellAndShapeDerivatives(px[idx],ni,d_S,psize[idx]);

      Matrix3 tensorL(0.0);
      short pgFld[27];
      if (flag->d_fracture) {
        for(int k=0; k<27; k++) 
          pgFld[k]=pgCode[idx][k];
        
        computeVelocityGradient(tensorL,ni,d_S,oodx,pgFld,gVelocity,GVelocity);
      } else {
        computeVelocityGradient(tensorL,ni,d_S,oodx,gVelocity,pErosion[idx]);
      }

      /*
      for (int ii = 0; ii < 3; ++ii) {
        for (int jj = 0; jj < 3; ++jj) {
          tensorL(ii,jj)=(fabs(tensorL(ii,jj)) < d_tol) ? 0.0 : tensorL(ii,jj);
        }
      }
      */

      // Compute the deformation gradient increment using the time_step
      // velocity gradient F_n^np1 = dudx * dt + Identity
      // Update the deformation gradient tensor to its time n+1 value.
      Matrix3 tensorFinc = tensorL*delT + one;
      tensorF_new = tensorFinc*pDeformGrad[idx];
      pDeformGrad_new[idx] = tensorF_new;
      double J = tensorF_new.Determinant();

      // Check 1: Look at Jacobian
      if (!(J > 0.0)) {
        cerr << getpid() 
             << "**ERROR** Negative Jacobian of deformation gradient" << endl;
        throw ParameterNotFound("**ERROR**:ElasticPlastic");
      }

      // Calculate the current density and deformed volume
      double rho_cur = rho_0/J;
      pVolume_deformed[idx]=pMass[idx]/rho_cur;

      // Compute polar decomposition of F (F = RU)
      tensorF_new.polarDecomposition(tensorU, tensorR, d_tol, true);

      // Calculate rate of deformation tensor (D)
      tensorD = (tensorL + tensorL.Transpose())*0.5;

      // Update the kinematic variables
      pRotation_new[idx] = tensorR;

      // If the particle is just sitting there, do nothing
      /*
      double defRateSq = tensorD.NormSquared();
      if (!(defRateSq > 0)) {
        pStress_new[idx] = pStress[idx];
        pStrainRate_new[idx] = 0.0;
        pPlasticStrain_new[idx] = pPlasticStrain[idx];
        pPlasticStrainRate_new[idx] = 0.0;
        pDamage_new[idx] = pDamage[idx];
        pPorosity_new[idx] = pPorosity[idx];
        pLocalized_new[idx] = pLocalized[idx];
        d_plastic->updateElastic(idx);
        continue;
      }
      */

      // Rotate the total rate of deformation tensor back to the 
      // material configuration
      tensorD = (tensorR.Transpose())*(tensorD*tensorR);

      // Subtract the thermal expansion to get D_e + D_p
      double dT_dt = (pTempCur[idx] - pTempPrev[idx])/delT;
      tensorD -= one*(alpha*dT_dt);
      
      // Calculate the deviatoric part of the non-thermal part
      // of the rate of deformation tensor
      tensorEta = tensorD - one*(tensorD.Trace()/3.0);
      pStrainRate_new[idx] = sqrtTwoThird*tensorD.Norm();

      // Rotate the Cauchy stress back to the 
      // material configuration and calculate the deviatoric part
      tensorSig = pStress[idx];
      tensorSig = (tensorR.Transpose())*(tensorSig*tensorR);
      double pressure = tensorSig.Trace()/3.0;
      Matrix3 tensorP = one*pressure;
      tensorS = tensorSig - tensorP;

      // Calculate the temperature at the start of the time step
      double temperature = pTemperature[idx];

      // Set up the PlasticityState
      PlasticityState* state = scinew PlasticityState();
      state->plasticStrainRate = sqrtTwoThird*tensorD.Norm();
      //state->plasticStrainRate = sqrtTwoThird*tensorEta.Norm();
      //state->plasticStrainRate = pPlasticStrainRate[idx];
      state->plasticStrain = pPlasticStrain[idx];
      state->pressure = pressure;
      state->temperature = temperature;
      state->density = rho_cur;
      state->initialDensity = rho_0;
      state->bulkModulus = bulk ;
      state->initialBulkModulus = bulk;
      state->shearModulus = shear ;
      state->initialShearModulus = shear;
      state->meltingTemp = Tm ;
      state->initialMeltTemp = Tm;
    
      // Calculate the shear modulus and the melting temperature at the
      // start of the time step
      double mu_cur = d_shear->computeShearModulus(state);
      double Tm_cur = d_melt->computeMeltingTemp(state);

      // Update the plasticity state
      state->shearModulus = mu_cur ;
      state->meltingTemp = Tm_cur ;

      // compute the local sound wave speed
      double c_dil = sqrt((bulk + 4.0*mu_cur/3.0)/rho_cur);

      //-----------------------------------------------------------------------
      // Stage 2:
      //-----------------------------------------------------------------------
      // Integrate the stress rate equation to get a trial deviatoric stress
      Matrix3 trialS = tensorS + tensorEta*(2.0*mu_cur*delT);

      // Calculate the equivalent stress
      double equivStress = sqrtThreeTwo*trialS.Norm();

      // Calculate flow stress (strain driven problem)
      double flowStress = d_plastic->computeFlowStress(state, delT, d_tol, 
                                                       matl, idx);
      state->yieldStress = flowStress;

      // Material has melted if flowStress <= 0.0
      bool melted = false;
      bool elastic = false;
      bool plastic = false;
      if (temperature > Tm_cur || flowStress <= 0.0) {

        melted = true;

        // Set the deviatoric stress to zero
        if (d_doMelting) tensorS = 0.0;

        d_plastic->updateElastic(idx);

      } else {

        // Get the current porosity 
        double porosity = pPorosity[idx];

        // Evaluate yield condition
        double traceOfTrialStress = 3.0*pressure + 
          tensorD.Trace()*(2.0*mu_cur*delT);
        double Phi = d_yield->evalYieldCondition(equivStress, flowStress,
                                                 traceOfTrialStress, 
                                                 porosity, state->yieldStress);
        // Compute the deviatoric stress
        //cerr << "Phi = " << Phi << " s_eq = " << equivStress
        //     << " s_flow = " << flowStress << endl;
        if (Phi <= 0.0) {

          elastic = true;

          // Set the elastic stress to the trial stress
          tensorS = trialS;

          // Update the internal variables
          d_plastic->updateElastic(idx);

        } else {

          plastic = true;

          double delGamma = 0.0;
          double sqrtSxS = tensorS.Norm(); 

          // If the material goes plastic in the first step, or
          // gammadotplus < 0 or delGamma < 0 use the Simo algorithm
          // with Newton iterations.
          bool doNewtonIterations = true;
          Matrix3 Stilde(0.0);
          if (sqrtSxS > 0.0 ) {

            // Using the algorithm from Zocher, Maudlin, Chen, Flower-Maudlin
            // European Congress on Computational Methods in Applied Sciences 
            // and Engineering,  September 11-14, 2000.
            // Basic assumption is that all strain rate is plastic strain rate

            // Calculate the derivative of the yield function (using the 
            // previous time step (n) values)
            Matrix3 q(0.0);
            d_yield->evalDevDerivOfYieldFunction(tensorSig, flowStress, 
                                                 porosity, q);

            // Calculate the tensor u (at start of time interval)
            double sqrtqs = sqrt(q.Contract(tensorS));
            Matrix3 u = q/sqrtqs;

            // Calculate u_q and u_eta
            double etaeta = sqrt(tensorEta.NormSquared());
            Matrix3 u_eta = tensorEta/etaeta;
            double sqrtqq = sqrt(q.NormSquared());
            Matrix3 u_q = q/sqrtqq;

            // Calculate c and d at the beginning of time step
            double cplus = u.NormSquared();
            double dplus = u.Contract(tensorEta);
            double gammadotplus = dplus/cplus;

            // Alternative calculation of gammadotplus
            //double gammadotplus = 
            // sqrtThreeTwo*sqrtqs/sqrtqq*state->plasticStrainRate;
            //gammadotplus = (gammadotplus < 0.0) ? 0.0 : gammadotplus;

            if (gammadotplus > 0.0) {
         
              // Calculate dStar/cstar 
              double u_eta_eta = u_eta.Contract(tensorEta);
              double u_q_eta = u_q.Contract(tensorEta);
              double AA = 2.0/sqrt(cplus);
              double BB = - (u_eta_eta + u_q_eta);
              double CC = - gammadotplus*cplus*(u_eta_eta - u_q_eta);
              double term1 = BB*BB - 4.0*AA*CC;
              term1 = (term1 < 0.0) ? 0.0 : term1;
            
              double dStar = (-BB + sqrt(term1))/(2.0*AA);

              // Calculate delGammaEr
              state->plasticStrainRate = 
                (sqrtTwoThird*sqrtqq*gammadotplus)/sqrtqs;
              state->yieldStress = d_plastic->computeFlowStress(state, delT, 
                                                                d_tol, matl, 
                                                                idx);
              double delGammaEr =  (sqrtTwo*state->yieldStress - sqrtqs)/
                (2.0*mu_cur*cplus);

              // Calculate delGamma
              delGamma = dStar/cplus*delT - delGammaEr;
              if (delGamma > 0.0) {

                // Compute the actual epdot, ep
                state->plasticStrainRate = 
                  (sqrtTwoThird*sqrtqq*delGamma)/(sqrtqs*delT);
                state->plasticStrain = pPlasticStrain[idx] + 
                  state->plasticStrainRate*delT;

                // Calculate Stilde

                // The exact form of denom will be different for 
                // different yield conditions ** WARNING ***
                ASSERT(state->yieldStress != 0.0);
                double denom = 1.0 + (3.0*sqrtTwo*mu_cur*delGamma)/
                  state->yieldStress; 
                ASSERT(denom != 0.0);
                Stilde = trialS/denom;

		/*
                if (idx == 1) {
                  double delLambda = sqrtqq*delGamma/sqrtqs;
                  cout << "idx = " << idx << " delGamma = " << delLambda 
                     << " sigy = " << state->yieldStress 
                     << " epdot = " << state->plasticStrainRate 
                     << " ep = " << state->plasticStrain << endl;
                }
		*/

                // We have found Stilde. Turn off Newton Iterations.
                doNewtonIterations = false;
        
              } // end of delGamma > 0 if

            } // end of gammdotplus > 0 if

          } // end of sqrtSxS == 0 if

          if (doNewtonIterations) {

            // Compute Stilde using Newton iterations a la Simo
            computeStilde(trialS, delT, matl, idx, Stilde, state, delGamma);

            /*
              cout << "sqrtSxS = 0 || gammadotplus <= 0 || delGamma <= 0.0" 
                   << endl;
              cout << "idx = " << idx
                   << " delGamma = " << delGamma  
                   << " Tau_n+1 = " << state->yieldStress
                   << " Tau_n = " << sqrtThreeTwo*sqrtSxS
                   << " ep_n = " << pPlasticStrain[idx]
                   << " ep_n+1 = " << state->plasticStrain
                   << " epdot_n = " << pPlasticStrainRate[idx]
                   << " epdot_n+1 = " << state->plasticStrainRate
                   << endl;
             */

          }

          // Do radial return adjustment
          double stst = sqrtThreeTwo*Stilde.Norm();
          ASSERT(stst != 0.0);
          tensorS = Stilde*(state->yieldStress/stst);

          //equivStress = sqrtThreeTwo*tensorS.Norm();
          //cout << "idx = " << " sig_eq = " << equivStress
          //                 << " sig_y = " << state->yieldStress << endl;

          // Update internal variables
          d_plastic->updatePlastic(idx, delGamma);

        } // end of Phi if

      } // end of temperature if

      // Calculate the updated hydrostatic stress
      double p = d_eos->computePressure(matl, state, tensorF_new, tensorD, 
                                        delT);

      /*
      if (flag->d_artificial_viscosity) {
        double Dkk = tensorD.Trace();
        double c_bulk = sqrt(bulk/rho_cur);
        double q = artificialBulkViscosity(Dkk, c_bulk, rho_cur, dx_ave);
        p -= q;
      }
      */

      Matrix3 tensorHy = one*p;
   
      // Calculate the total stress
      tensorSig = tensorS + tensorHy;

      // If the particle has already failed, apply various erosion algorithms
      if (flag->d_doErosion) {
        if (pLocalized[idx]) {
          if (d_allowNoTension) {
            if (p > 0.0) tensorSig = zero;
            else tensorSig = tensorHy;
          }
          else if (d_setStressToZero) tensorSig = zero;
        }
      }

      //-----------------------------------------------------------------------
      // Stage 3:
      //-----------------------------------------------------------------------
      // Compute porosity/damage/temperature change
      if (!plastic) {

        // Save the updated data
        pPlasticStrain_new[idx] = pPlasticStrain[idx];
        pPlasticStrainRate_new[idx] = 0.0;
        pDamage_new[idx] = pDamage[idx];
        pPorosity_new[idx] = pPorosity[idx];
        
      } else {

        // Update the plastic strain
        pPlasticStrain_new[idx] = state->plasticStrain;
        pPlasticStrainRate_new[idx] = state->plasticStrainRate;

        // Update the porosity
        if (d_evolvePorosity) 
          pPorosity_new[idx] = updatePorosity(tensorD, delT, pPorosity[idx], 
                                              state->plasticStrain);
        else
          pPorosity_new[idx] = pPorosity[idx];

        // Calculate the updated scalar damage parameter
        if (d_evolveDamage) 
          pDamage_new[idx] = 
           d_damage->computeScalarDamage(state->plasticStrainRate, tensorSig, 
                                         temperature, delT, matl, d_tol, 
                                         pDamage[idx]);
        else
          pDamage_new[idx] = pDamage[idx];

        // Calculate rate of temperature increase due to plastic strain
        double taylorQuinney = d_initialData.Chi;
        double C_p = matl->getSpecificHeat();
        if (d_computeSpecificHeat) C_p = computeSpecificHeat(temperature);
        double fac = taylorQuinney/(rho_cur*C_p);

        // Calculate Tdot (internal plastic heating rate)
        pIntHeatRate[idx] = tensorS.Contract(tensorEta)*fac;
      }

      //-----------------------------------------------------------------------
      // Stage 4:
      //-----------------------------------------------------------------------
      // Find if the particle has failed/localized
      pLocalized_new[idx] = pLocalized[idx];
      bool isLocalized = false;
      double tepla = 0.0;

      if (flag->d_doErosion) {

        // Check 1: Look at the temperature
        if (melted) isLocalized = true;

        // Check 2 and 3: Look at TEPLA and stability
        else if (plastic) {

          // Check 2: Modified Tepla rule
          if (d_checkTeplaFailureCriterion) {
            tepla = pow(pPorosity_new[idx]/d_porosity.fc,2.0) + 
                    pow(pDamage_new[idx],2.0);
            if (tepla > 1.0) isLocalized = true;
          } 

          // Check 3: Stability criterion (only if material is plastic)
          if (d_stable && !isLocalized) {

            // Calculate values needed for tangent modulus calculation
            state->temperature = temperature;
            mu_cur = d_shear->computeShearModulus(state);
            Tm_cur = d_melt->computeMeltingTemp(state);
            double sigY = d_plastic->computeFlowStress(state, delT, d_tol, 
                                                       matl, idx);
            if (!(sigY > 0.0)) isLocalized = true;
            else {
              double dsigYdep = 
                d_plastic->evalDerivativeWRTPlasticStrain(state, idx);
              double A = voidNucleationFactor(state->plasticStrain);

              // Calculate the elastic tangent modulus
              TangentModulusTensor Ce;
              computeElasticTangentModulus(bulk, mu_cur, Ce);
  
              // Calculate the elastic-plastic tangent modulus
              TangentModulusTensor Cep;
              d_yield->computeElasPlasTangentModulus(Ce, tensorSig, sigY, 
                                                     dsigYdep, 
                                                     pPorosity_new[idx],
                                                     A, Cep);
          
              // Initialize localization direction
              Vector direction(0.0,0.0,0.0);
              isLocalized = d_stable->checkStability(tensorSig, tensorD, Cep, 
                                                     direction);
            }
          }
        }

        // Use erosion algorithms to treat localized particles
        if (isLocalized) {

          // If the localized particles fail again then set their stress to zero
          if (pLocalized[idx]) {
            pDamage_new[idx] = 0.0;
            pPorosity_new[idx] = 0.0;
            tensorSig = zero;
          } else {

            // set the particle localization flag to true  
            pLocalized_new[idx] = 1;
            pDamage_new[idx] = 0.0;
            pPorosity_new[idx] = 0.0;

            // Apply various erosion algorithms
            if (d_allowNoTension) {
              if (p > 0.0) tensorSig = zero;
              else tensorSig = tensorHy;
            }
            else if (d_setStressToZero) tensorSig = zero;
          }

        }
      }

      //-----------------------------------------------------------------------
      // Stage 5:
      //-----------------------------------------------------------------------
      // Rotate the stress back to the laboratory coordinates
      tensorSig = (tensorR*tensorSig)*(tensorR.Transpose());

      // Save the new data
      pStress_new[idx] = tensorSig;
        
      // Rotate the deformation rate back to the laboratory coordinates
      tensorD = (tensorR*tensorD)*(tensorR.Transpose());

      // Compute the strain energy for the particles
      Matrix3 avgStress = (pStress_new[idx] + pStress[idx])*0.5;
      double pStrainEnergy = (tensorD(0,0)*avgStress(0,0) +
                              tensorD(1,1)*avgStress(1,1) +
                              tensorD(2,2)*avgStress(2,2) +
                              2.0*(tensorD(0,1)*avgStress(0,1) + 
                                   tensorD(0,2)*avgStress(0,2) +
                                   tensorD(1,2)*avgStress(1,2)))*
        pVolume_deformed[idx]*delT;
      totalStrainEnergy += pStrainEnergy;                  

      // Compute wave speed at each particle, store the maximum
      Vector pVel = pVelocity[idx];
      WaveSpeed=Vector(Max(c_dil+fabs(pVel.x()),WaveSpeed.x()),
                       Max(c_dil+fabs(pVel.y()),WaveSpeed.y()),
                       Max(c_dil+fabs(pVel.z()),WaveSpeed.z()));

      delete state;
    }
    WaveSpeed = dx/WaveSpeed;
    double delT_new = WaveSpeed.minComponent();
    new_dw->put(delt_vartype(patch->getLevel()->adjustDelt(delT_new)), 
                lb->delTLabel);
    new_dw->put(sum_vartype(totalStrainEnergy), lb->StrainEnergyLabel);
    delete interpolator;
  }

  if (cout_EP.active()) 
    cout_EP << getpid() << "... End." << endl;

}

void 
ElasticPlastic::carryForward(const PatchSubset* patches,
                             const MPMMaterial* matl,
                             DataWarehouse* old_dw,
                             DataWarehouse* new_dw)
{
  for(int p=0;p<patches->size();p++){
    const Patch* patch = patches->get(p);
    int dwi = matl->getDWIndex();
    ParticleSubset* pset = old_dw->getParticleSubset(dwi, patch);

    // Carry forward the data common to all constitutive models 
    // when using RigidMPM.
    // This method is defined in the ConstitutiveModel base class.
    carryForwardSharedData(pset, old_dw, new_dw, matl);

    // Carry forward the data local to this constitutive model 
    constParticleVariable<Matrix3> pRotation;
    constParticleVariable<double>  pPlasticStrain, pDamage, pPorosity, 
      pStrainRate, pPlasticStrainRate;
    constParticleVariable<int>     pLocalized;

    old_dw->get(pRotation,       pRotationLabel,       pset);
    old_dw->get(pStrainRate,     pStrainRateLabel,     pset);
    old_dw->get(pPlasticStrain,  pPlasticStrainLabel,  pset);
    old_dw->get(pPlasticStrainRate,  pPlasticStrainRateLabel,  pset);
    old_dw->get(pDamage,         pDamageLabel,         pset);
    old_dw->get(pPorosity,       pPorosityLabel,       pset);
    old_dw->get(pLocalized,      pLocalizedLabel,      pset);

    ParticleVariable<Matrix3>      pRotation_new;
    ParticleVariable<double>       pPlasticStrain_new, pDamage_new, 
      pPorosity_new, pStrainRate_new, pPlasticStrainRate_new;
    ParticleVariable<int>          pLocalized_new;

    new_dw->allocateAndPut(pRotation_new,    
                           pRotationLabel_preReloc,               pset);
    new_dw->allocateAndPut(pStrainRate_new,      
                           pStrainRateLabel_preReloc,             pset);
    new_dw->allocateAndPut(pPlasticStrain_new,      
                           pPlasticStrainLabel_preReloc,          pset);
    new_dw->allocateAndPut(pPlasticStrainRate_new,      
                           pPlasticStrainRateLabel_preReloc,          pset);
    new_dw->allocateAndPut(pDamage_new,      
                           pDamageLabel_preReloc,                 pset);
    new_dw->allocateAndPut(pPorosity_new,      
                           pPorosityLabel_preReloc,               pset);
    new_dw->allocateAndPut(pLocalized_new,      
                           pLocalizedLabel_preReloc,              pset);

    // Get the plastic strain
    d_plastic->getInternalVars(pset, old_dw);
    d_plastic->allocateAndPutRigid(pset, new_dw);

    for(ParticleSubset::iterator iter = pset->begin();
        iter != pset->end(); iter++){
      particleIndex idx = *iter;
      pRotation_new[idx] = pRotation[idx];
      pStrainRate_new[idx] = pStrainRate[idx];
      pPlasticStrain_new[idx] = pPlasticStrain[idx];
      pPlasticStrainRate_new[idx] = pPlasticStrainRate[idx];
      pDamage_new[idx] = pDamage[idx];
      pPorosity_new[idx] = pPorosity[idx];
      pLocalized_new[idx] = pLocalized[idx];
    }

    new_dw->put(delt_vartype(patch->getLevel()->adjustDelt(1.e10)), 
                lb->delTLabel);
    new_dw->put(sum_vartype(0.),     lb->StrainEnergyLabel);
  }
}

void 
ElasticPlastic::allocateCMDataAddRequires(Task* task,
                                          const MPMMaterial* matl,
                                          const PatchSet* patch,
                                          MPMLabel* lb) const
{
  const MaterialSubset* matlset = matl->thisMaterial();

  // Allocate the variables shared by all constitutive models
  // for the particle convert operation
  // This method is defined in the ConstitutiveModel base class.
  addSharedRForConvertExplicit(task, matlset, patch);

  // Add requires local to this model
  Ghost::GhostType  gnone = Ghost::None;
  task->requires(Task::NewDW, pRotationLabel_preReloc,       matlset, gnone);
  task->requires(Task::NewDW, pStrainRateLabel_preReloc,     matlset, gnone);
  task->requires(Task::NewDW, pPlasticStrainLabel_preReloc,  matlset, gnone);
  task->requires(Task::NewDW, pPlasticStrainRateLabel_preReloc, matlset, gnone);
  task->requires(Task::NewDW, pDamageLabel_preReloc,         matlset, gnone);
  task->requires(Task::NewDW, pLocalizedLabel_preReloc,      matlset, gnone);
  task->requires(Task::NewDW, pPorosityLabel_preReloc,       matlset, gnone);
  d_plastic->allocateCMDataAddRequires(task,matl,patch,lb);
}

void 
ElasticPlastic::allocateCMDataAdd(DataWarehouse* new_dw,
                                  ParticleSubset* addset,
                                  map<const VarLabel*, 
                                  ParticleVariableBase*>* newState,
                                  ParticleSubset* delset,
                                  DataWarehouse* old_dw)
{
  // Copy the data common to all constitutive models from the particle to be 
  // deleted to the particle to be added. 
  // This method is defined in the ConstitutiveModel base class.
  copyDelToAddSetForConvertExplicit(new_dw, delset, addset, newState);
  
  // Copy the data local to this constitutive model from the particles to 
  // be deleted to the particles to be added
  ParticleSubset::iterator n,o;

  ParticleVariable<Matrix3> pRotation;
  ParticleVariable<double> pPlasticStrain, pDamage,pPorosity, pStrainRate,
                           pPlasticStrainRate;
  ParticleVariable<int> pLocalized;

  constParticleVariable<Matrix3> o_Rotation;
  constParticleVariable<double> o_PlasticStrain, o_Damage,o_Porosity, 
    o_StrainRate, o_PlasticStrainRate;
  constParticleVariable<int> o_Localized;

  new_dw->allocateTemporary(pRotation,addset);
  new_dw->allocateTemporary(pPlasticStrain,addset);
  new_dw->allocateTemporary(pPlasticStrainRate,addset);
  new_dw->allocateTemporary(pDamage,addset);
  new_dw->allocateTemporary(pStrainRate,addset);
  new_dw->allocateTemporary(pLocalized,addset);
  new_dw->allocateTemporary(pPorosity,addset);

  new_dw->get(o_Rotation,pRotationLabel_preReloc,delset);
  new_dw->get(o_StrainRate,pStrainRateLabel_preReloc,delset);
  new_dw->get(o_PlasticStrain,pPlasticStrainLabel_preReloc,delset);
  new_dw->get(o_PlasticStrainRate,pPlasticStrainRateLabel_preReloc,delset);
  new_dw->get(o_Damage,pDamageLabel_preReloc,delset);
  new_dw->get(o_Localized,pLocalizedLabel_preReloc,delset);
  new_dw->get(o_Porosity,pPorosityLabel_preReloc,delset);

  n = addset->begin();
  for (o=delset->begin(); o != delset->end(); o++, n++) {
    pRotation[*n] = o_Rotation[*o];
    pStrainRate[*n] = o_StrainRate[*o];
    pPlasticStrain[*n] = o_PlasticStrain[*o];
    pPlasticStrainRate[*n] = o_PlasticStrainRate[*o];
    pDamage[*n] = o_Damage[*o];
    pLocalized[*n] = o_Localized[*o];
    pPorosity[*n] = o_Porosity[*o];
  }

  (*newState)[pRotationLabel]=pRotation.clone();
  (*newState)[pStrainRateLabel]=pStrainRate.clone();
  (*newState)[pPlasticStrainLabel]=pPlasticStrain.clone();
  (*newState)[pPlasticStrainRateLabel]=pPlasticStrainRate.clone();
  (*newState)[pDamageLabel]=pDamage.clone();
  (*newState)[pLocalizedLabel]=pLocalized.clone();
  (*newState)[pPorosityLabel]=pPorosity.clone();
  
  // Initialize the data for the plasticity model
  d_plastic->allocateCMDataAdd(new_dw,addset, newState, delset, old_dw);
}


void 
ElasticPlastic::addRequiresDamageParameter(Task* task,
                                           const MPMMaterial* matl,
                                           const PatchSet* ) const
{
  const MaterialSubset* matlset = matl->thisMaterial();
  task->requires(Task::NewDW, pLocalizedLabel_preReloc,matlset,Ghost::None);
}

void 
ElasticPlastic::getDamageParameter(const Patch* patch,
                                   ParticleVariable<int>& damage,
                                   int dwi,
                                   DataWarehouse* old_dw,
                                   DataWarehouse* new_dw)
{
  ParticleSubset* pset = old_dw->getParticleSubset(dwi,patch);
  constParticleVariable<int> pLocalized;
  new_dw->get(pLocalized, pLocalizedLabel_preReloc, pset);

  ParticleSubset::iterator iter;
  for (iter = pset->begin(); iter != pset->end(); iter++) {
    damage[*iter] = pLocalized[*iter];
  }
   
}
         
// Compute the elastic tangent modulus tensor for isotropic
// materials (**NOTE** can get rid of one copy operation if needed)
void 
ElasticPlastic::computeElasticTangentModulus(double bulk,
                                             double shear,
                                             TangentModulusTensor& Ce)
{
  // Form the elastic tangent modulus tensor
  double E = 9.0*bulk*shear/(3.0*bulk+shear);
  double nu = E/(2.0*shear) - 1.0;
  double fac = E/((1.0+nu)*(1.0-2.0*nu));
  double C11 = fac*(1.0-nu);
  double C12 = fac*nu;
  FastMatrix C_6x6(6,6);
  for (int ii = 0; ii < 6; ++ii) 
    for (int jj = 0; jj < 6; ++jj) C_6x6(ii,jj) = 0.0;
  C_6x6(0,0) = C11; C_6x6(1,1) = C11; C_6x6(2,2) = C11;
  C_6x6(0,1) = C12; C_6x6(0,2) = C12; 
  C_6x6(1,0) = C12; C_6x6(1,2) = C12; 
  C_6x6(2,0) = C12; C_6x6(2,1) = C12; 
  C_6x6(3,3) = shear; C_6x6(4,4) = shear; C_6x6(5,5) = shear;
  
  Ce.convertToTensorForm(C_6x6);
}

// Update the porosity of the material
double 
ElasticPlastic::updatePorosity(const Matrix3& D,
                               double delT, 
                               double f,
                               double ep)
{
  // Growth
  // Calculate trace of D
  double Dkk = D.Trace();
  Matrix3 one; one.Identity();
  Matrix3 eta = D - one*(Dkk/3.0);

  // Calculate rate of growth
  double fdot_grow = 0.0;
  if (Dkk > 0.0) fdot_grow = (1.0-f)*Dkk;

  // Nucleation 
  // Calculate A
  double A = voidNucleationFactor(ep);

  // Calculate plastic strain rate
  double epdot = sqrt(eta.NormSquared()/1.5);

  // Calculate rate of nucleation
  double fdot_nucl = A*epdot;

  // Update void volume fraction using forward euler
  double f_new = f + delT*(fdot_nucl + fdot_grow);
  //cout << "Porosity: D = " << D << endl;
  //cout << "Porosity: eta = " << eta << endl;
  //cout << "Porosity: Dkk = " << Dkk << endl;
  //cout << "Porosity::fdot_gr = " << fdot_grow 
  //     << " fdot_nucl = " << fdot_nucl << " f = " << f 
  //     << " f_new = " << f_new << endl;
  return f_new;
}

// Calculate the void nucleation factor
inline double 
ElasticPlastic::voidNucleationFactor(double ep)
{
  double temp = (ep - d_porosity.en)/d_porosity.sn;
  double A = d_porosity.fn/(d_porosity.sn*sqrt(2.0*M_PI))*
    exp(-0.5*temp*temp);
  return A;
}

double 
ElasticPlastic::computeSpecificHeat(double T)
{
  return 1.0e3*(d_Cp.A + d_Cp.B*T + d_Cp.C/(T*T));
}

double ElasticPlastic::computeRhoMicroCM(double pressure,
                                         const double p_ref,
                                         const MPMMaterial* matl)
{
  double rho_orig = matl->getInitialDensity();
  double bulk = d_initialData.Bulk;

  double p_gauge = pressure - p_ref;
  double rho_cur;

  if(d_useModifiedEOS && p_gauge < 0.0) {
    double A = p_ref;  // modified EOS
    double n = p_ref/bulk;
    rho_cur  = rho_orig*pow(pressure/A,n);
  } else {             // Standard EOS
    double p_g_over_bulk = p_gauge/bulk;
    rho_cur=rho_orig*(p_g_over_bulk + sqrt(p_g_over_bulk*p_g_over_bulk +1.));
  }
  return rho_cur;
}

void ElasticPlastic::computePressEOSCM(double rho_cur,double& pressure,
                                       double p_ref,  
                                       double& dp_drho, double& tmp,
                                       const MPMMaterial* matl)
{
  double bulk = d_initialData.Bulk;
  double rho_orig = matl->getInitialDensity();
  double inv_rho_orig = 1./rho_orig;

  if(d_useModifiedEOS && rho_cur < rho_orig){
    double A = p_ref;           // MODIFIED EOS
    double n = bulk/p_ref;
    double rho_rat_to_the_n = pow(rho_cur/rho_orig,n);
    pressure = A*rho_rat_to_the_n;
    dp_drho  = (bulk/rho_cur)*rho_rat_to_the_n;
    tmp      = dp_drho;         // speed of sound squared
  } else {                      // STANDARD EOS
    double p_g = .5*bulk*(rho_cur*inv_rho_orig - rho_orig/rho_cur);
    pressure   = p_ref + p_g;
    dp_drho    = .5*bulk*(rho_orig/(rho_cur*rho_cur) + inv_rho_orig);
    tmp        = bulk/rho_cur;  // speed of sound squared
  }
}

double ElasticPlastic::getCompressibility()
{
  return 1.0/d_initialData.Bulk;
}

void
ElasticPlastic::scheduleCheckNeedAddMPMMaterial(Task* task,
                                                const MPMMaterial* matl,
                                                const PatchSet* patch) const
{
  task->computes(lb->NeedAddMPMMaterialLabel);
}

void ElasticPlastic::checkNeedAddMPMMaterial(const PatchSubset* patches,
                                             const MPMMaterial* matl,
                                             DataWarehouse* old_dw,
                                             DataWarehouse* new_dw)
{
  if (cout_EP.active()) {
    cout_EP << getpid() << "checkNeedAddMPMMaterial: In : Matl = " << matl
            << " id = " << matl->getDWIndex() <<  " patch = "
            << (patches->get(0))->getID();
  }

  double need_add=0.;

  new_dw->put(sum_vartype(need_add),     lb->NeedAddMPMMaterialLabel);
}

////////////////////////////////////////////////////////////////////////
// Compute the quantity 
//             \f$d(\gamma)/dt * \Delta T = \Delta \gamma \f$ 
//             using Newton iterative root finder */
////////////////////////////////////////////////////////////////////////
double 
ElasticPlastic::computeDeltaGamma(const double& delT,
                                  const double& tolerance,
                                  const double& normTrialS,
                                  const MPMMaterial* matl,
                                  const particleIndex idx,
                                  PlasticityState* state)
{
  // Initialize constants
  double twothird = 2.0/3.0;
  double stwothird = sqrt(twothird);
  double sthreetwo = 1.0/stwothird;
  double twomu = 2.0*state->shearModulus;
  double ep = state->plasticStrain;

  // Initialize variables
  //localState->plasticStrainRate = 0.0;
  double deltaGamma = state->plasticStrainRate*delT*sthreetwo;
  double deltaGammaOld = deltaGamma;
  double g = 0.0;
  double Dg = 1.0;

  // iterate
  int count = 0;
  do {

    ++count;

    // Compute sigma_y
    double sigma_y = d_plastic->computeFlowStress(state, delT, tolerance, 
                                                  matl, idx);
    /*
    if (isnan(sigma_y)) {
      cout << "idx = " << idx << " iter = " << count 
           << " epdot = " << state->plasticStrainRate
           << " ep = " << state->plasticStrain
           << " T = " << state->temperature 
           << " Tm = " << state->meltingTemp << endl;
    }
    */

    // Update the yield stress in the local state
    state->yieldStress = sigma_y;

    // Compute g
    g = normTrialS - stwothird*sigma_y - twomu*deltaGamma;

    // Compute d(sigma_y)/d(epdot)
    double dsigy_depdot = d_plastic->evalDerivativeWRTStrainRate(state,
                                                                 idx);

    // Compute d(sigma_y)/d(ep)
    double dsigy_dep = d_plastic->evalDerivativeWRTPlasticStrain(state,
                                                                 idx);

    // Compute d(g)/d(deltaGamma)
    Dg = -twothird*(dsigy_depdot/delT + dsigy_dep) - twomu;

    // Update deltaGamma
    deltaGammaOld = deltaGamma;
    deltaGamma -= g/Dg;

    // Update local plastic strain rate
    double stt_deltaGamma = max(stwothird*deltaGamma, 0.0);
    state->plasticStrainRate = stt_deltaGamma/delT;
    
    // Update local plastic strain 
    state->plasticStrain = ep + stt_deltaGamma;

    if (isnan(g) || isnan(deltaGamma)) {
      cout << "idx = " << idx << " iter = " << count 
         << " g = " << g << " Dg = " << Dg << " deltaGamma = " << deltaGamma 
         << " sigy = " << sigma_y 
         << " dsigy/depdot = " << dsigy_depdot << " dsigy/dep = " << dsigy_dep 
         << " epdot = " << state->plasticStrainRate 
         << " ep = " << state->plasticStrain << endl;
    }

    if (fabs(deltaGamma-deltaGammaOld) < tolerance || count > 100) break;

  } while (fabs(g) > tolerance);

  /*
  //if (idx == 1) {
    cout << "idx = " << idx << " deltaGamma = " << deltaGamma 
         << " sigy = " << state->yieldStress 
         << " epdot = " << state->plasticStrainRate 
         << " ep = " << state->plasticStrain << endl;
  //}
  */

  return deltaGamma;
}

////////////////////////////////////////////////////////////////////////
/*! \brief Compute Stilde, epdot, ep, and delGamma using 
           Simo's approach */
////////////////////////////////////////////////////////////////////////
void 
ElasticPlastic::computeStilde(const Matrix3& trialS,
                              const double& delT,
                              const MPMMaterial* matl,
                              const particleIndex idx,
                              Matrix3& Stilde,
                              PlasticityState* state,
                              double& delGamma)
{
  double normTrialS = trialS.Norm();
  Matrix3 nn = trialS/normTrialS;

  // Do Newton iteration to compute delGamma and updated 
  // plastic strain, plastic strain rate, and yield stress
  double tolerance = min(delT, 1.0e-6);
  delGamma = computeDeltaGamma(delT, tolerance, normTrialS, matl,
                               idx, state);
  Stilde = trialS - nn*(2.0*state->shearModulus*delGamma);

}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

