/*
 * The MIT License
 *
 * Copyright (c) 1997-2013 The University of Utah
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

#include <CCA/Components/MD/AnalyticNonBonded.h>
#include <CCA/Components/MD/MDSystem.h>
#include <CCA/Components/MD/MDLabel.h>
#include <Core/Grid/Patch.h>
#include <Core/Grid/Variables/ParticleVariable.h>
#include <Core/Grid/Variables/ParticleSubset.h>
#include <Core/Grid/Variables/VarTypes.h>
#include <Core/Grid/Box.h>
#include <Core/Geometry/IntVector.h>
#include <Core/Geometry/Point.h>
#include <Core/Math/MiscMath.h>
#include <Core/Util/DebugStream.h>

#include <iostream>
#include <iomanip>

#ifdef DEBUG
#include <Core/Util/FancyAssert.h>
#endif

using namespace Uintah;

extern SCIRun::Mutex cerrLock;

static DebugStream analytic_dbg("AnalyticNonbondedDbg", false);

AnalyticNonBonded::AnalyticNonBonded()
{

}

AnalyticNonBonded::~AnalyticNonBonded()
{

}

AnalyticNonBonded::AnalyticNonBonded(MDSystem* system,
                                     const double r12,
                                     const double r6,
                                     const double cutoffRadius) :
    d_system(system), d_r12(r12), d_r6(r6), d_cutoffRadius(cutoffRadius)
{
  d_nonBondedInteractionType = NonBonded::LJ12_6;
}

//-----------------------------------------------------------------------------
// Interface implementations
void AnalyticNonBonded::initialize(const ProcessorGroup* pg,
                                   const PatchSubset* patches,
                                   const MaterialSubset* materials,
                                   DataWarehouse* old_dw,
                                   DataWarehouse* new_dw)
{
  int numPatches = patches->size();
  // need a neighbor list for each patch
  d_neighborList.reserve(numPatches);
  for (int i = 0; i < numPatches; i++) {
    d_neighborList.push_back(vector<vector<int> >());
  }

  for (int p = 0; p < numPatches; p++) {
    const Patch* patch = patches->get(p);

    // do this for each material; currently only using material "0"
    int numMatls = materials->size();
    for (int m = 0; m < numMatls; m++) {
      int matl = materials->get(m);

      // get interior (purely local) bounds of current patch to correctly initialize particles (atoms)
      IntVector lowInterior = patch->getCellLowIndex();
      IntVector highInterior = patch->getCellHighIndex();
      ParticleSubset* localpset = new_dw->getParticleSubset(matl, patch, lowInterior, highInterior);

      // for neighbor indices; one list for each atom
      int numAtoms = localpset->numParticles();
      for (int i = 0; i < numAtoms; i++) {
        d_neighborList[p].push_back(vector<int>());
      }
    }
  }
  new_dw->put(sum_vartype(0.0), d_lb->vdwEnergyLabel);
}

void AnalyticNonBonded::setup(const ProcessorGroup* pg,
                              const PatchSubset* patches,
                              const MaterialSubset* materials,
                              DataWarehouse* old_dw,
                              DataWarehouse* new_dw)
{
  // create neighbor list for each atom in the system
  unsigned int numPatches = patches->size();
  for (unsigned int p = 0; p < numPatches; p++) {
    const Patch* patch = patches->get(p);
    unsigned int numMatls = materials->size();
    for (unsigned int m = 0; m < numMatls; m++) {
      int matl = materials->get(m);

      // get interior (purely local) bounds of current patch to correctly initialize particles (atoms)
      IntVector lowInterior = patch->getCellLowIndex();
      IntVector highInterior = patch->getCellHighIndex();
      ParticleSubset* local_pset = old_dw->getParticleSubset(matl, patch, lowInterior, highInterior);
      ParticleSubset* neighbor_pset = old_dw->getParticleSubset(matl, patch, Ghost::AroundNodes, SHRT_MAX, d_lb->pXLabel);

      constParticleVariable<Point> px_local;
      constParticleVariable<Point> px_neighbors;
      old_dw->get(px_local, d_lb->pXLabel, local_pset);
      old_dw->get(px_neighbors, d_lb->pXLabel, neighbor_pset);

      generateNeighborList(local_pset, neighbor_pset, px_local, px_neighbors, p);
    }
  }
}

void AnalyticNonBonded::calculate(const ProcessorGroup* pg,
                                  const PatchSubset* patches,
                                  const MaterialSubset* materials,
                                  DataWarehouse* old_dw,
                                  DataWarehouse* new_dw)
{
  if (d_system->newBox()) {
    setup(pg, patches, materials, old_dw, new_dw);
  }

  Vector box = d_system->getBox();
  double vdwEnergy = 0;

  // loop through all patches
  unsigned int numPatches = patches->size();
  for (unsigned int p = 0; p < numPatches; p++) {
    const Patch* patch = patches->get(p);
    unsigned int numMatls = materials->size();
    for (unsigned int m = 0; m < numMatls; m++) {
      int matl = materials->get(m);

      // get interior (purely local) bounds of current patch to correctly initialize particles (atoms)
      IntVector lowInterior = patch->getCellHighIndex();
      IntVector highInterior = patch->getCellLowIndex();
      ParticleSubset* local_pset = old_dw->getParticleSubset(matl, patch, lowInterior, highInterior);

      // requires variables
      constParticleVariable<Point> px;
      constParticleVariable<Vector> pforce;
      constParticleVariable<double> penergy;
      old_dw->get(px, d_lb->pXLabel, local_pset);
      old_dw->get(penergy, d_lb->pEnergyLabel, local_pset);
      old_dw->get(pforce, d_lb->pForceLabel, local_pset);

      // computes variables
      ParticleVariable<Vector> pforcenew;
      ParticleVariable<double> penergynew;
      new_dw->allocateAndPut(pforcenew, d_lb->pForceLabel_preReloc, local_pset);
      new_dw->allocateAndPut(penergynew, d_lb->pEnergyLabel_preReloc, local_pset);

      // loop over all atoms in system, calculate the forces
      double r2, ir2, ir6, ir12, T6, T12;
      double forceTerm;
      Vector totalForce, atomForce;
      Vector reducedCoordinates;
      unsigned int localAtoms = local_pset->numParticles();

      for (unsigned int i = 0; i < localAtoms; i++) {
        atomForce = Vector(0.0, 0.0, 0.0);

        // loop over the neighbors of atom "i"
        unsigned int idx;
        unsigned int numNeighbors = d_neighborList[p][i].size();
        for (unsigned int j = 0; j < numNeighbors; j++) {
          idx = d_neighborList[p][i][j];

          // the vector distance between atom i and j
          reducedCoordinates = px[i] - px[idx];

          // this is required for periodic boundary conditions
          reducedCoordinates -= (reducedCoordinates / box).vec_rint() * box;
          double reducedX = reducedCoordinates[0] * reducedCoordinates[0];
          double reducedY = reducedCoordinates[1] * reducedCoordinates[1];
          double reducedZ = reducedCoordinates[2] * reducedCoordinates[2];

          r2 = reducedX + reducedY + reducedZ;
          ir2 = 1.0 / r2;         // 1/r^2
          ir6 = ir2 * ir2 * ir2;  // 1/r^6
          ir12 = ir6 * ir6;       // 1/r^12
          T12 = d_r12 * ir12;
          T6 = d_r6 * ir6;
          penergynew[idx] = T12 - T6;  // energy
          vdwEnergy += penergynew[idx];  // count the energy
          forceTerm = (12.0 * T12 - 6.0 * T6) * ir2;  // the force term
          totalForce = forceTerm * reducedCoordinates;

          // the contribution of force on atom i
          atomForce += totalForce;
        }  // end neighbor loop for atom "i"

        // sum up contributions to force for atom i
        pforcenew[i] += atomForce;
      }  // end atom loop

      // this accounts for double energy from Aij <--> Aji
      vdwEnergy *= 0.50;

      if (analytic_dbg.active()) {
        cerrLock.lock();
        Vector forces(0.0, 0.0, 0.0);
        unsigned int numParticles = local_pset->numParticles();
        for (unsigned int i = 0; i < numParticles; i++) {
          forces += pforcenew[i];
        }
        std::cout.setf(std::ios_base::scientific);
        std::cout << "Total Local Energy: " << std::setprecision(16) << vdwEnergy << std::endl;
        std::cout << "Local Force: [";
        std::cout << std::setw(16) << std::setprecision(8) << forces.x();
        std::cout << std::setw(16) << std::setprecision(8) << forces.y();
        std::cout << std::setprecision(8) << forces.z() << std::setw(4) << "]";
        std::cout << std::endl;
        std::cout.unsetf(std::ios_base::scientific);
        cerrLock.unlock();
      }

    }  // end materials loop

    // global reduction on vdwEnergy
    new_dw->put(sum_vartype(vdwEnergy), d_lb->vdwEnergyLabel);

  }  // end patch loop
}

void AnalyticNonBonded::finalize(const ProcessorGroup* pg,
                                 const PatchSubset* patches,
                                 const MaterialSubset* materials,
                                 DataWarehouse* old_dw,
                                 DataWarehouse* new_dw)
{

}

void AnalyticNonBonded::generateNeighborList(ParticleSubset* local_pset,
                                             ParticleSubset* neighbor_pset,
                                             constParticleVariable<Point> px_local,
                                             constParticleVariable<Point> px_neighbors,
                                             int patchID)
{
  // loop over all local atoms, determine neighbors from the particle set with ghost cells
  unsigned int localAtoms = local_pset->numParticles();
  unsigned int totalAtoms = neighbor_pset->numParticles();

  double r2;
  Vector box = d_system->getBox();
  SCIRun::Vector reducedCoordinates;
  double cut_sq = d_cutoffRadius * d_cutoffRadius;

  for (unsigned int i = 0; i < localAtoms; i++) {
    for (unsigned int j = 0; j < totalAtoms; j++) {
      if (i != j) {
        // the vector distance between atom i and j
        reducedCoordinates = px_local[i] - px_neighbors[j];

        // this is required for periodic boundary conditions
        reducedCoordinates -= (reducedCoordinates / box).vec_rint() * box;

        // eliminate atoms outside of cutoff radius, add those within as neighbors
        if ((fabs(reducedCoordinates[0]) < d_cutoffRadius) && (fabs(reducedCoordinates[1]) < d_cutoffRadius)
            && (fabs(reducedCoordinates[2]) < d_cutoffRadius)) {
          double reducedX = reducedCoordinates[0] * reducedCoordinates[0];
          double reducedY = reducedCoordinates[1] * reducedCoordinates[1];
          double reducedZ = reducedCoordinates[2] * reducedCoordinates[2];
          r2 = sqrt(reducedX + reducedY + reducedZ);
          // only add neighbor atoms within spherical cut-off around atom "i"
          if (r2 < cut_sq) {
            d_neighborList[patchID][i].push_back(j);
          }
        }
      }
    }
  }
}

bool AnalyticNonBonded::isNeighbor(const Point* atom1,
                                   const Point* atom2)
{
  // get the simulation box size
  Vector box = d_system->getBox();

  double r2;
  Vector reducedCoordinates;
  double cut_sq = d_cutoffRadius * d_cutoffRadius;

  // the vector distance between atom 1 and 2
  reducedCoordinates = *atom1 - *atom2;

  // this is required for periodic boundary conditions
  reducedCoordinates -= (reducedCoordinates / box).vec_rint() * box;

  // check if outside of cutoff radius
  if ((fabs(reducedCoordinates[0]) < d_cutoffRadius) && (fabs(reducedCoordinates[1]) < d_cutoffRadius)
      && (fabs(reducedCoordinates[2]) < d_cutoffRadius)) {
    r2 = sqrt(pow(reducedCoordinates[0], 2.0) + pow(reducedCoordinates[1], 2.0) + pow(reducedCoordinates[2], 2.0));
    return r2 < cut_sq;
  }
  return false;
}

