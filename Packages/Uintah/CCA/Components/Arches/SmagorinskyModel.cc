//----- SmagorinskyModel.cc --------------------------------------------------

#include <Packages/Uintah/CCA/Components/Arches/SmagorinskyModel.h>
#include <Packages/Uintah/CCA/Components/Arches/PhysicalConstants.h>
#include <Packages/Uintah/CCA/Components/Arches/BoundaryCondition.h>
#include <Packages/Uintah/CCA/Components/Arches/CellInformation.h>
#include <Packages/Uintah/CCA/Components/Arches/ArchesLabel.h>
#include <Packages/Uintah/CCA/Components/Arches/ArchesMaterial.h>
#include <Packages/Uintah/CCA/Components/Arches/TimeIntegratorLabel.h>
#include <Packages/Uintah/Core/Grid/Variables/Stencil.h>
#include <Packages/Uintah/Core/Grid/Level.h>
#include <Packages/Uintah/CCA/Ports/Scheduler.h>
#include <Packages/Uintah/CCA/Ports/DataWarehouse.h>
#include <Packages/Uintah/Core/Grid/Task.h>
#include <Packages/Uintah/Core/Grid/Variables/CCVariable.h>
#include <Packages/Uintah/Core/Grid/Variables/SFCXVariable.h>
#include <Packages/Uintah/Core/Grid/Variables/SFCYVariable.h>
#include <Packages/Uintah/Core/Grid/Variables/SFCZVariable.h>
#include <Packages/Uintah/Core/Grid/Variables/PerPatch.h>
#include <Packages/Uintah/Core/Grid/Variables/SoleVariable.h>
#include <Packages/Uintah/Core/ProblemSpec/ProblemSpec.h>
#include <Core/Geometry/Vector.h>
#include <Packages/Uintah/Core/Grid/SimulationState.h>
#include <Packages/Uintah/Core/Exceptions/InvalidValue.h>
#include <Packages/Uintah/Core/Grid/Variables/Array3.h>
#include <iostream>

using namespace std;

using namespace Uintah;
using namespace SCIRun;

#include <Packages/Uintah/CCA/Components/Arches/fortran/smagmodel_fort.h>
#include <Packages/Uintah/CCA/Components/Arches/fortran/scalarvarmodel_fort.h>

//****************************************************************************
// Default constructor for SmagorinkyModel
//****************************************************************************
SmagorinskyModel::SmagorinskyModel(const ArchesLabel* label, 
				   const MPMArchesLabel* MAlb,
				   PhysicalConstants* phyConsts,
				   BoundaryCondition* bndry_cond):
                                    TurbulenceModel(label, MAlb),
				    d_physicalConsts(phyConsts),
				    d_boundaryCondition(bndry_cond)
{
}

//****************************************************************************
// Destructor
//****************************************************************************
SmagorinskyModel::~SmagorinskyModel()
{
}

//****************************************************************************
//  Get the molecular viscosity from the Physical Constants object 
//****************************************************************************
double 
SmagorinskyModel::getMolecularViscosity() const {
  return d_physicalConsts->getMolecularViscosity();
}

//****************************************************************************
// Problem Setup 
//****************************************************************************
void 
SmagorinskyModel::problemSetup(const ProblemSpecP& params)
{
  ProblemSpecP db = params->findBlock("Turbulence");
  db->require("cf", d_CF);
  db->require("fac_mesh", d_factorMesh);
  db->require("filterl", d_filterl);
  if (d_calcVariance) {
    cout << "Smagorinsky type model will be used to model variance" << endl;
    db->require("variance_coefficient",d_CFVar); // const reqd by variance eqn
    
    cout << "WARNING! Scalar filtering for variance limit is not supported" << endl;
    cout << "by this model. Possibly high variance values would be generated" << endl;
  }

  // actually, Shmidt number, not Prandtl number
  d_turbPrNo = 0.0;
  if (db->findBlock("turbulentPrandtlNumber"))
    db->getWithDefault("turbulentPrandtlNumber",d_turbPrNo,0.4);

}

//****************************************************************************
// Schedule recomputation of the turbulence sub model 
//****************************************************************************
void 
SmagorinskyModel::sched_reComputeTurbSubmodel(SchedulerP& sched, 
					      const PatchSet* patches,
					      const MaterialSet* matls,
				          const TimeIntegratorLabel* timelabels)
{
  string taskname =  "SmagorinskyModel::ReTurbSubmodel" +
		     timelabels->integrator_step_name;
  Task* tsk = scinew Task(taskname, this,
			  &SmagorinskyModel::reComputeTurbSubmodel,
			  timelabels);

  // Requires
  tsk->requires(Task::NewDW, d_lab->d_densityCPLabel,
		Ghost::None, Arches::ZEROGHOSTCELLS);
  tsk->requires(Task::NewDW, d_lab->d_uVelocitySPBCLabel,
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_vVelocitySPBCLabel, 
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_wVelocitySPBCLabel, 
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_newCCUVelocityLabel,
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_newCCVVelocityLabel,
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_newCCWVelocityLabel,
		Ghost::AroundCells, Arches::ONEGHOSTCELL);

  tsk->requires(Task::NewDW, d_lab->d_cellTypeLabel, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
  // for multimaterial
  if (d_MAlab)
    tsk->requires(Task::NewDW, d_lab->d_mmgasVolFracLabel, 
		  Ghost::None, Arches::ZEROGHOSTCELLS);

      // Computes
  tsk->modifies(d_lab->d_viscosityCTSLabel);

  sched->addTask(tsk, patches, matls);
}


//****************************************************************************
// Actual recompute 
//****************************************************************************
void 
SmagorinskyModel::reComputeTurbSubmodel(const ProcessorGroup*,
					const PatchSubset* patches,
					const MaterialSubset*,
					DataWarehouse*,
					DataWarehouse* new_dw,
				        const TimeIntegratorLabel* timelabels)
{
//  double time = d_lab->d_sharedState->getElapsedTime();
  for (int p = 0; p < patches->size(); p++) {
    const Patch* patch = patches->get(p);
    int archIndex = 0; // only one arches material
    int matlIndex = d_lab->d_sharedState->getArchesMaterial(archIndex)->getDWIndex(); 
    // Variables
    constSFCXVariable<double> uVelocity;
    constSFCYVariable<double> vVelocity;
    constSFCZVariable<double> wVelocity;
    constCCVariable<double> uVelocityCC;
    constCCVariable<double> vVelocityCC;
    constCCVariable<double> wVelocityCC;
    constCCVariable<double> density;
    CCVariable<double> viscosity;
    constCCVariable<double> voidFraction;
    constCCVariable<int> cellType;
    // Get the velocity, density and viscosity from the old data warehouse

    new_dw->getModifiable(viscosity, d_lab->d_viscosityCTSLabel,
			   matlIndex, patch);
    
    new_dw->get(uVelocity, d_lab->d_uVelocitySPBCLabel, matlIndex, patch, 
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
    new_dw->get(vVelocity, d_lab->d_vVelocitySPBCLabel, matlIndex, patch,
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
    new_dw->get(wVelocity, d_lab->d_wVelocitySPBCLabel, matlIndex, patch, 
		Ghost::AroundFaces, Arches::ONEGHOSTCELL);
    new_dw->get(density, d_lab->d_densityCPLabel, matlIndex, patch,
		Ghost::None, Arches::ZEROGHOSTCELLS);
    new_dw->get(uVelocityCC, d_lab->d_newCCUVelocityLabel, matlIndex, patch, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
    new_dw->get(vVelocityCC, d_lab->d_newCCVVelocityLabel, matlIndex, patch, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
    new_dw->get(wVelocityCC, d_lab->d_newCCWVelocityLabel, matlIndex, patch, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);
    
    if (d_MAlab)
      new_dw->get(voidFraction, d_lab->d_mmgasVolFracLabel, matlIndex, patch,
		  Ghost::None, Arches::ZEROGHOSTCELLS);

    new_dw->get(cellType, d_lab->d_cellTypeLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);

    // Get the PerPatch CellInformation data

    PerPatch<CellInformationP> cellInfoP;
    if (new_dw->exists(d_lab->d_cellInfoLabel, matlIndex, patch)) 
      new_dw->get(cellInfoP, d_lab->d_cellInfoLabel, matlIndex, patch);
    else {
      cellInfoP.setData(scinew CellInformation(patch));
      new_dw->put(cellInfoP, d_lab->d_cellInfoLabel, matlIndex, patch);
    }
    CellInformation* cellinfo = cellInfoP.get().get_rep();
    
    // get physical constants
    double mol_viscos; // molecular viscosity
    mol_viscos = d_physicalConsts->getMolecularViscosity();
    
    // Get the patch and variable details
    // compatible with fortran index
    IntVector idxLo = patch->getCellFORTLowIndex();
    IntVector idxHi = patch->getCellFORTHighIndex();
    double CF = d_CF;
#if 0
    if (time < 2.0 ) 
      CF *= (time+ 0.0001)*0.5;
#endif      
    fort_smagmodel(uVelocity, vVelocity, wVelocity,
		   uVelocityCC, vVelocityCC, wVelocityCC,
		   density, viscosity, idxLo, idxHi,
		   cellinfo->sew, cellinfo->sns, cellinfo->stb,
		   mol_viscos, CF, d_factorMesh, d_filterl);

    // boundary conditions
    bool xminus = patch->getBCType(Patch::xminus) != Patch::Neighbor;
    bool xplus =  patch->getBCType(Patch::xplus) != Patch::Neighbor;
    bool yminus = patch->getBCType(Patch::yminus) != Patch::Neighbor;
    bool yplus =  patch->getBCType(Patch::yplus) != Patch::Neighbor;
    bool zminus = patch->getBCType(Patch::zminus) != Patch::Neighbor;
    bool zplus =  patch->getBCType(Patch::zplus) != Patch::Neighbor;
    int wall_celltypeval = d_boundaryCondition->wallCellType();
    if (xminus) {
      int colX = idxLo.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX-1, colY, colZ);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (xplus) {
      int colX = idxHi.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX+1, colY, colZ);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (yminus) {
      int colY = idxLo.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY-1, colZ);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (yplus) {
      int colY = idxHi.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY+1, colZ);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (zminus) {
      int colZ = idxLo.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ-1);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (zplus) {
      int colZ = idxHi.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ+1);
	  if (cellType[currCell] != wall_celltypeval)
	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)];
//	    viscosity[currCell] = viscosity[IntVector(colX,colY,colZ)]
//		    *density[currCell]/density[IntVector(colX,colY,colZ)];
	}
      }
    }

    if (d_MAlab) {
      IntVector indexLow = patch->getCellLowIndex();
      IntVector indexHigh = patch->getCellHighIndex();
      for (int colZ = indexLow.z(); colZ < indexHigh.z(); colZ ++) {
	for (int colY = indexLow.y(); colY < indexHigh.y(); colY ++) {
	  for (int colX = indexLow.x(); colX < indexHigh.x(); colX ++) {
	    // Store current cell
	    IntVector currCell(colX, colY, colZ);
	    viscosity[currCell] *=  voidFraction[currCell];
	  }
	}
      }
    }

  }
}


//****************************************************************************
// Schedule recomputation of the turbulence sub model 
//****************************************************************************
void 
SmagorinskyModel::sched_computeScalarVariance(SchedulerP& sched, 
					      const PatchSet* patches,
					      const MaterialSet* matls,
			    		 const TimeIntegratorLabel* timelabels,
                                              bool d_EKTCorrection,
                                              bool doing_EKT_now)
{
  string taskname =  "SmagorinskyModel::computeScalarVaraince" +
		     timelabels->integrator_step_name;
  Task* tsk = scinew Task(taskname, this,
			  &SmagorinskyModel::computeScalarVariance,
			  timelabels, d_EKTCorrection, doing_EKT_now);

  
  // Requires, only the scalar corresponding to matlindex = 0 is
  //           required. For multiple scalars this will be put in a loop
  if (doing_EKT_now)
    tsk->requires(Task::NewDW, d_lab->d_scalarEKTLabel, 
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
  else
    tsk->requires(Task::NewDW, d_lab->d_scalarSPLabel, 
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);

  tsk->requires(Task::NewDW, d_lab->d_cellTypeLabel, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);

  // Computes
  if ((timelabels->integrator_step_number == TimeIntegratorStepNumber::First) 
      &&((!(d_EKTCorrection))||((d_EKTCorrection)&&(doing_EKT_now)))) {
     tsk->computes(d_lab->d_scalarVarSPLabel);
     tsk->computes(d_lab->d_normalizedScalarVarLabel);
  }
  else {
     tsk->modifies(d_lab->d_scalarVarSPLabel);
     tsk->modifies(d_lab->d_normalizedScalarVarLabel);
  }

  sched->addTask(tsk, patches, matls);
}


void 
SmagorinskyModel::computeScalarVariance(const ProcessorGroup*,
					const PatchSubset* patches,
					const MaterialSubset*,
					DataWarehouse*,
					DataWarehouse* new_dw,
			    		const TimeIntegratorLabel* timelabels,
                                        bool d_EKTCorrection,
                                        bool doing_EKT_now)
{
//  double time = d_lab->d_sharedState->getElapsedTime();
  for (int p = 0; p < patches->size(); p++) {
    const Patch* patch = patches->get(p);
    int archIndex = 0; // only one arches material
    int matlIndex = d_lab->d_sharedState->getArchesMaterial(archIndex)->getDWIndex(); 
    // Variables
    constCCVariable<double> scalar;
    CCVariable<double> scalarVar;
    CCVariable<double> normalizedScalarVar;
    // Get the velocity, density and viscosity from the old data warehouse
    if (doing_EKT_now)
      new_dw->get(scalar, d_lab->d_scalarEKTLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
    else
      new_dw->get(scalar, d_lab->d_scalarSPLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);

    if ((timelabels->integrator_step_number == TimeIntegratorStepNumber::First) 
      &&((!(d_EKTCorrection))||((d_EKTCorrection)&&(doing_EKT_now)))) {
    	new_dw->allocateAndPut(scalarVar, d_lab->d_scalarVarSPLabel, matlIndex,
			       patch);
    	new_dw->allocateAndPut(normalizedScalarVar, d_lab->d_normalizedScalarVarLabel, matlIndex,
			       patch);
    }
    else {
    	new_dw->getModifiable(scalarVar, d_lab->d_scalarVarSPLabel, matlIndex,
			       patch);
    	new_dw->getModifiable(normalizedScalarVar, d_lab->d_normalizedScalarVarLabel, matlIndex,
			       patch);
    }
    scalarVar.initialize(0.0);
    normalizedScalarVar.initialize(0.0);

    constCCVariable<int> cellType;
    new_dw->get(cellType, d_lab->d_cellTypeLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
    
    // Get the PerPatch CellInformation data
    PerPatch<CellInformationP> cellInfoP;
    //  old_dw->get(cellInfoP, d_lab->d_cellInfoLabel, matlIndex, patch);
    new_dw->get(cellInfoP, d_lab->d_cellInfoLabel, matlIndex, patch);
    //  if (old_dw->exists(d_cellInfoLabel, patch)) 
    //  old_dw->get(cellInfoP, d_cellInfoLabel, matlIndex, patch);
    //else {
    //  cellInfoP.setData(scinew CellInformation(patch));
    //  old_dw->put(cellInfoP, d_cellInfoLabel, matlIndex, patch);
    //}
    CellInformation* cellinfo = cellInfoP.get().get_rep();
    
    // compatible with fortran index
    IntVector idxLo = patch->getCellFORTLowIndex();
    IntVector idxHi = patch->getCellFORTHighIndex();
    double CFVar = d_CFVar;
#if 0
    if (time < 2.0 ) 
      CFVar *= (time+ 0.0001)*0.5;
#endif
    fort_scalarvarmodel(scalar, idxLo, idxHi, scalarVar, cellinfo->dxpw,
			cellinfo->dyps, cellinfo->dzpb, cellinfo->sew,
			cellinfo->sns, cellinfo->stb, CFVar, d_factorMesh,
			d_filterl);

    double small = 1.0e-10;
    double var_limit = 0.0;
    for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ);

	  // check variance bounds and normalize
	  var_limit = scalar[currCell] * (1.0 - scalar[currCell]);

          if(scalarVar[currCell] < small)
            scalarVar[currCell] = 0.0;
          if(scalarVar[currCell] > var_limit)
            scalarVar[currCell] = var_limit;

          normalizedScalarVar[currCell] = scalarVar[currCell]/(var_limit+small);
	}
      }
    }

    
    // boundary conditions
    bool xminus = patch->getBCType(Patch::xminus) != Patch::Neighbor;
    bool xplus =  patch->getBCType(Patch::xplus) != Patch::Neighbor;
    bool yminus = patch->getBCType(Patch::yminus) != Patch::Neighbor;
    bool yplus =  patch->getBCType(Patch::yplus) != Patch::Neighbor;
    bool zminus = patch->getBCType(Patch::zminus) != Patch::Neighbor;
    bool zplus =  patch->getBCType(Patch::zplus) != Patch::Neighbor;
    int outlet_celltypeval = d_boundaryCondition->outletCellType();
    int pressure_celltypeval = d_boundaryCondition->pressureCellType();
    if (xminus) {
      int colX = idxLo.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX-1, colY, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    if (xplus) {
      int colX = idxHi.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX+1, colY, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    if (yminus) {
      int colY = idxLo.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY-1, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    if (yplus) {
      int colY = idxHi.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY+1, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    if (zminus) {
      int colZ = idxLo.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ-1);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    if (zplus) {
      int colZ = idxHi.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ+1);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)]) {
	      scalarVar[currCell] = scalarVar[IntVector(colX,colY,colZ)];
	      normalizedScalarVar[currCell] = 
		          normalizedScalarVar[IntVector(colX,colY,colZ)];
	    }
	}
      }
    }
    
  }
}

//****************************************************************************
// Schedule recomputation of the turbulence sub model 
//****************************************************************************
void 
SmagorinskyModel::sched_computeScalarDissipation(SchedulerP& sched, 
						 const PatchSet* patches,
						 const MaterialSet* matls,
			    		 const TimeIntegratorLabel* timelabels,
                                                 bool d_EKTCorrection,
                                                 bool doing_EKT_now)
{
  string taskname =  "SmagorinskyModel::computeScalarDissipation" +
		     timelabels->integrator_step_name;
  Task* tsk = scinew Task(taskname, this,
			  &SmagorinskyModel::computeScalarDissipation,
			  timelabels, d_EKTCorrection, doing_EKT_now);

  
  // Requires, only the scalar corresponding to matlindex = 0 is
  //           required. For multiple scalars this will be put in a loop
  // assuming scalar dissipation is computed before turbulent viscosity calculation 
  if (doing_EKT_now)
    tsk->requires(Task::NewDW, d_lab->d_scalarEKTLabel,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
  else
    tsk->requires(Task::NewDW, d_lab->d_scalarSPLabel,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
  tsk->requires(Task::NewDW, d_lab->d_viscosityCTSLabel,
		Ghost::AroundCells, Arches::ONEGHOSTCELL);

  tsk->requires(Task::NewDW, d_lab->d_cellTypeLabel, 
		Ghost::AroundCells, Arches::ONEGHOSTCELL);

  // Computes
  if ((timelabels->integrator_step_number == TimeIntegratorStepNumber::First) 
      &&((!(d_EKTCorrection))||((d_EKTCorrection)&&(doing_EKT_now))))
     tsk->computes(d_lab->d_scalarDissSPLabel);
  else
     tsk->modifies(d_lab->d_scalarDissSPLabel);

  sched->addTask(tsk, patches, matls);
}


void 
SmagorinskyModel::computeScalarDissipation(const ProcessorGroup*,
					const PatchSubset* patches,
					const MaterialSubset*,
					DataWarehouse*,
					DataWarehouse* new_dw,
			    		const TimeIntegratorLabel* timelabels,
                                        bool d_EKTCorrection,
                                        bool doing_EKT_now)
{
  for (int p = 0; p < patches->size(); p++) {
    const Patch* patch = patches->get(p);
    int archIndex = 0; // only one arches material
    int matlIndex = d_lab->d_sharedState->getArchesMaterial(archIndex)->getDWIndex(); 
    // Variables
    constCCVariable<double> viscosity;
    constCCVariable<double> scalar;
    CCVariable<double> scalarDiss;  // dissipation..chi

    if (doing_EKT_now)
      new_dw->get(scalar, d_lab->d_scalarEKTLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
    else
      new_dw->get(scalar, d_lab->d_scalarSPLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
    new_dw->get(viscosity, d_lab->d_viscosityCTSLabel, matlIndex, patch,
		Ghost::AroundCells, Arches::ONEGHOSTCELL);

    if ((timelabels->integrator_step_number == TimeIntegratorStepNumber::First) 
      &&((!(d_EKTCorrection))||((d_EKTCorrection)&&(doing_EKT_now))))
       new_dw->allocateAndPut(scalarDiss, d_lab->d_scalarDissSPLabel,
			      matlIndex, patch);
    else
       new_dw->getModifiable(scalarDiss, d_lab->d_scalarDissSPLabel,
			      matlIndex, patch);
    scalarDiss.initialize(0.0);

    constCCVariable<int> cellType;
    new_dw->get(cellType, d_lab->d_cellTypeLabel, matlIndex, patch,
		  Ghost::AroundCells, Arches::ONEGHOSTCELL);
    
    // Get the PerPatch CellInformation data
    PerPatch<CellInformationP> cellInfoP;
    new_dw->get(cellInfoP, d_lab->d_cellInfoLabel, matlIndex, patch);
    CellInformation* cellinfo = cellInfoP.get().get_rep();
    
    // compatible with fortran index
    IntVector idxLo = patch->getCellFORTLowIndex();
    IntVector idxHi = patch->getCellFORTHighIndex();
    for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ);
	  double scale = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX+1,colY,colZ)]);
	  double scalw = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX-1,colY,colZ)]);
	  double scaln = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX,colY+1,colZ)]);
	  double scals = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX,colY-1,colZ)]);
	  double scalt = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX,colY,colZ+1)]);
	  double scalb = 0.5*(scalar[currCell]+
			      scalar[IntVector(colX,colY,colZ-1)]);
	  double dfdx = (scale-scalw)/cellinfo->sew[colX];
	  double dfdy = (scaln-scals)/cellinfo->sns[colY];
	  double dfdz = (scalt-scalb)/cellinfo->stb[colZ];
	  scalarDiss[currCell] = viscosity[currCell]/d_turbPrNo*
	                        (dfdx*dfdx + dfdy*dfdy + dfdz*dfdz); 
	}
      }
    }

    
    // boundary conditions
    bool xminus = patch->getBCType(Patch::xminus) != Patch::Neighbor;
    bool xplus =  patch->getBCType(Patch::xplus) != Patch::Neighbor;
    bool yminus = patch->getBCType(Patch::yminus) != Patch::Neighbor;
    bool yplus =  patch->getBCType(Patch::yplus) != Patch::Neighbor;
    bool zminus = patch->getBCType(Patch::zminus) != Patch::Neighbor;
    bool zplus =  patch->getBCType(Patch::zplus) != Patch::Neighbor;
    int outlet_celltypeval = d_boundaryCondition->outletCellType();
    int pressure_celltypeval = d_boundaryCondition->pressureCellType();
    if (xminus) {
      int colX = idxLo.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX-1, colY, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (xplus) {
      int colX = idxHi.x();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	  IntVector currCell(colX+1, colY, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (yminus) {
      int colY = idxLo.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY-1, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (yplus) {
      int colY = idxHi.y();
      for (int colZ = idxLo.z(); colZ <= idxHi.z(); colZ ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY+1, colZ);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (zminus) {
      int colZ = idxLo.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ-1);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    if (zplus) {
      int colZ = idxHi.z();
      for (int colY = idxLo.y(); colY <= idxHi.y(); colY ++) {
	for (int colX = idxLo.x(); colX <= idxHi.x(); colX ++) {
	  IntVector currCell(colX, colY, colZ+1);
          if ((cellType[currCell] == outlet_celltypeval)||
            (cellType[currCell] == pressure_celltypeval))
	    if (scalar[currCell] == scalar[IntVector(colX,colY,colZ)])
	      scalarDiss[currCell] = scalarDiss[IntVector(colX,colY,colZ)];
	}
      }
    }
    
  }
}
