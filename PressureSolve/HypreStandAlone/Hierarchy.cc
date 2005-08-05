#include "Hierarchy.h"
#include "util.h"
#include "Level.h"
#include "Patch.h"

#include <mpi.h>


void
Hierarchy::make()
  /*_____________________________________________________________________
    Function Hierarchy::make()
    Create a static refinement hierarchy hier into our data structures of
    Hierarchy, Levels and Patches. We define all patches owned by ALL
    processors, not only the ones owned by this proc. 
    We also initialize the plevel and refinementRatio arrays
    needed by Hypre FAC.
    _param = strcture of input parameters.
    _____________________________________________________________________*/
{
  /* Hardcoded for a rectangular domain consisting of one box.
     Dirichlet B.C. on the boundaries of the box 2 levels, level 1 is
     twice finer and extends over the central half of the domain.

     TODO: restore domain as a union of boxes and add code to generally
     parse it and assign them to processors bla bla bla
     assert(_param->domain->patchList.size() == 1);
  */

  serializeProcsBegin();
  const Counter numDims   = _param->numDims;
  const Counter numLevels = _param->numLevels;
  const Counter n         = _param->baseResolution;
  const int numProcs      = _param->numProcs;
  //_param->domain->_resolution[0]; // Assumed uniform in all dimensions

  /* Initialize the patches that THIS proc owns at all levels */
  for (Counter level = 0; level < numLevels; level++) {
    Vector<Counter> k(0,numDims,0,"k",2);
    IntMatrix procMap = grayCode(numDims,k);
    const Counter& numPatches = procMap.numRows();

    /* Print procMap */
    if (MYID == 0) {
      Proc0Print("procMap at level %d\n",level);
      for (Counter i = 0; i < numPatches; i++){
        for(Counter d = 0; d < numDims; d++){
          if (d == 0) {
            Proc0Print("\t%d",procMap(i,d));
          } else {
            PrintNP("\t%d",procMap(i,d));
          }
        }
        PrintNP("\n");
      }
    }

    /* Set Refinement ratio w.r.t. parent level. Hard-coded to be
       constant (1:2) in all dimensions and all levels for now. */
    Vector<Counter> refRat(0,numDims);
    if (level == 0) {        /* Dummy ref. ratio value */
      for (Counter dim = 0; dim < numDims; dim++) {
        refRat[dim] = 1;
      }
    } else {
      for (Counter dim = 0; dim < numDims; dim++) {
        refRat[dim] = 2;
      }
    }

    double h;
    /* Compute meshsize, assumed the same in all directions at each level */
    if (level == 0) {
      h = 1.0/n;   // Size of domain divided by # of gridpoints
    } else {
      h = _levels[level-1]->_meshSize[0] /  refRat[0]; /* ref. ratio
                                                          assumed constant for
                                                          all dims */
    }
    
    _levels.push_back(new Level(numDims,h));
    Level* lev = _levels[level];
    lev->_refRat = refRat;
    lev->_patchList.resize(numProcs);
    int offset = level; // For tests where the fine patch is owned by
    // one proc, and its parent patch is owned by another proc

    /* Mesh box extents (lower left corner, upper right corner) */
    /* This is where the specific geometry of the hierarchy is
       hard-coded. */
    Print("numPatches = %d\n",numPatches);
    for (Counter i = 0; i < numPatches; i++) {
      int proc = (i+offset) % numProcs; // Owner of patch i, hard-coded
      Print("Creating Patch i = %2d   owned by proc %2d\n",i,proc);
      Vector<int> lower(0,numDims);
      Vector<int> upper(0,numDims);
      for (Counter dim = 0; dim < numDims; dim++) {
        switch (level) {
        case 0:
          {
            lower[dim] = procMap(i,dim) * n/2;
            upper[dim] = lower[dim] + n/2 - 1;
            break;
          }
        case 1:
          {
            lower[dim] = n/2 + procMap(i,dim) * n/2;
            upper[dim] = lower[dim] + n/2 - 1;
            break;
          }
        default:
          {
            PrintNP("\n\nError, unsupported level=%d in "
                    "Hierarchy::make()\n",level);
            clean();
            exit(1);
          }
        } // end switch (level)
      } // for dim
      
      // Create patch object from the geometry parameters above and
      // add it to the level's appropriate list of patches.
      Patch* patch = new Patch(proc,level,Box(lower,upper));
      //        Print("Doing setDomainBoundaries\n");
      patch->setDomainBoundaries(*lev);
      //        Print("Done setDomainBoundaries\n");
      lev->_patchList[proc].push_back(patch);
    } // end for i (patches)
  } // end for level

  serializeProcsEnd();

  getPatchesFromOtherProcs();  // Compute global patch IDs
}


void
Hierarchy::getPatchesFromOtherProcs()
{
  Print("getPatchesFromOtherProcs() begin\n");
  /* Types for arrays that are sent/received through MPI are int;
     convert to Counter later on, but don't risk having MPI_INT and
     Counter mixed up in the same call. */
  int sendbuf[_param->numProcs];  /* Suppose this proc is #0 has 5 patches,
                                     don't know what everyone else has.
                                     So this array will look like : 5 0 0 0 0. */
  int numPatches[_param->numProcs];  // After assembling sendbuf from all procs
  Counter globalPatchID = 0;

  for (Counter level = 0; level < _levels.size(); level++) {
    Level* lev = _levels[level];
    //    const Vector<Counter>& resolution = lev->_resolution;
    // clear sendbuf
    for( int index = 0; index < _param->numProcs; index++ ) {
      if (index == MYID) {
        sendbuf[index] = lev->_patchList[MYID].size();
      } else {
        sendbuf[index] = 0;
      }
    }

    // Talk to all procs to find out how many patches they have on
    // this level.
    MPI_Allreduce(sendbuf, numPatches, _param->numProcs,
                  MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    int totalPatches = 0;
    int startPosition = 0;
    for(int index = 0; index < _param->numProcs; index++ ) {
      //      Proc0Print("has %d patches on level %d\n",
      // numPatches[index], level);
      totalPatches += numPatches[index];
      if( index < MYID )
        startPosition += numPatches[index];
    }

    Proc0Print("got totalPatches of %d\n", totalPatches);

    // Put our patch information into a big Vector, share it with
    // all other procs
    int recordSize = 2*_param->numDims+1;
    int * sendPatchInfo = new int[ recordSize * totalPatches ];
    int * patchInfo     = new int[ recordSize * totalPatches ];
    for( int index = 0; index < recordSize*totalPatches; index++ ) {
      sendPatchInfo[index] = 0;
      patchInfo[index] = -1;
    }
    int count = startPosition * recordSize;
    for(Counter index = 0; index < lev->_patchList[MYID].size(); index++ ) {
      Patch* patch = lev->_patchList[MYID][index];
      sendPatchInfo[count++] = patch->_procID;
      for (Counter d = 0; d < _param->numDims; d++)
        sendPatchInfo[count++] = patch->_box.get(Left)[d];
      for (Counter d = 0; d < _param->numDims; d++)
        sendPatchInfo[count++] = patch->_box.get(Right)[d];
    }

    MPI_Allreduce(sendPatchInfo, patchInfo, totalPatches*recordSize,
                  MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    //    for(int index = 0; index < recordSize*totalPatches; index++ ) {
    //Proc0Print("%3d: %3d %3d\n",index,sendPatchInfo[index],patchInfo[index]);
    //}
    //    Print("Looping over patches and setting their boundary types\n");
    Vector<int> ilower(0,_param->numDims);
    Vector<int> iupper(0,_param->numDims);
    Vector<int> otherilower(0,_param->numDims);
    Vector<int> otheriupper(0,_param->numDims);
    int patchIndex = 0;

    for (Counter index = 0; index < totalPatches; index++ ) {
      int owner = patchInfo[recordSize*index];
      if (MYID != owner) {   // This patch is processed only on its owning proc
        globalPatchID++;
        continue;
      }
      Print("Processing global ID for patch %d\n",globalPatchID);
      Patch* patch = lev->_patchList[MYID][patchIndex];
      patch->_patchID = globalPatchID; // Save the global patch index in _patchID
      patchIndex++;
      globalPatchID++;
      for (Counter d = 0; d < _param->numDims; d++) {
        ilower[d] = patchInfo[recordSize*index + d + 1];
        iupper[d] = patchInfo[recordSize*index + _param->numDims + d + 1];

        /* Defaults: boundary is a C/F boundary; boundary condition is
           not applicable. */
        for (Side s = Left; s <= Right; ++s) {
          if (patch->getBoundaryType(d,s) != Patch::Domain) {
            patch->setBoundaryType(d,s,Patch::CoarseFine);
            patch->setBC(d,s,Patch::NA);
          }
        }

        /* Check if patch is near a domain boundary */
        // Hardcoded to one domain box of size [0,0] to [resolution].
        // TODO: L-shaped domain with its own makeHierarchy() function of
        // patches. Right now hard-coded to two levels and 2^d processors.
      }

      for (Counter other = 0; other < totalPatches; other++) {
        if (other == index)
          continue;
        for (Counter d = 0; d < _param->numDims; d++) {
          otherilower[d] = patchInfo[recordSize*other + d + 1];
          otheriupper[d] = patchInfo[recordSize*other + _param->numDims + d + 1];
        }
        /*
          Print("Comparing patch index=%d: from ",index);
          printIndex(ilower);
          PrintNP(" to ");
          printIndex(iupper);
          PrintNP("  owned by proc %d",patchInfo[recordSize*index]);
          PrintNP("\n");
          Print("To patch other=%d: from ",other);
          printIndex(otherilower);
          PrintNP(" to ");
          printIndex(otheriupper);
          PrintNP("  owned by proc %d",patchInfo[recordSize*other]);
          PrintNP("\n");
        */
        for (Counter d = 0; d < _param->numDims; d++) {
          /* Check if patch has a nbhring patch on its left */
          if (ilower[d] == otheriupper[d]+1) {
            for (Counter d2 = 0; d2 < _param->numDims; d2++) {
              if (d2 == d) continue;
              // TODO: put that in Box::intersect()
              if (MAX(ilower[d2],otherilower[d2]) <=
                  MIN(iupper[d2],otheriupper[d2])) {
                patch->setBoundaryType(d,Left,Patch::Neighbor);
              }
            }
          }

          /* Check if patch has a nbhring patch on its right */
          if (iupper[d] == otherilower[d]-1) {
            for (Counter d2 = 0; d2 < _param->numDims; d2++) {
              if (d2 == d) continue;
              if (MAX(ilower[d2],otherilower[d2]) <=
                  MIN(iupper[d2],otheriupper[d2])) {
                patch->setBoundaryType(d,Right,Patch::Neighbor);
              }
            }
          }
        }
      } // end for other
    } // end for index
    delete [] sendPatchInfo;
    delete [] patchInfo;
  } // end for level
  Print("getPatchesFromOtherProcs() end\n");
} // getPatchesFromOtherProcs()

void
Hierarchy::printPatchBoundaries()
{
  serializeProcsBegin();
  /* Print boundary types */
  for(Counter level = 0; level < _levels.size(); level++ ) {
    Print("---- Patch boundaries at level %d ----\n",level);
    Level* lev = _levels[level];
    for (Counter index = 0; index < lev->_patchList[MYID].size(); index++ ) {
      Patch* patch = lev->_patchList[MYID][index];
      Print("Patch # %d, owned by proc %d: ",
            patch->_patchID,patch->_procID);
      cout << patch->_box;
      PrintNP("\n");
      for (Counter d = 0; d < _param->numDims; d++) {
        for (Side s = Left; s <= Right; ++s) {
          Print("  boundary( d = %d , s = %+d ) = %s\n",
                d,s,
                Patch::boundaryTypeString[patch->getBoundaryType(d,s)].c_str());
        }
      }
    }
  } // end for level
  Print("\n");
  serializeProcsEnd();
} // end getPatchesFromOtherProcs()
