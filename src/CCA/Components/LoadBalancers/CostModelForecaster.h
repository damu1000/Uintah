/*

The MIT License

Copyright (c) 1997-2009 Center for the Simulation of Accidental Fires and 
Explosions (CSAFE), and  Scientific Computing and Imaging Institute (SCI), 
University of Utah.

License for the specific language governing rights and limitations under
Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.

*/


#ifndef UINTAH_HOMEBREW_CostModelForecaster_H
#define UINTAH_HOMEBREW_CostModelForecaster_H

#include <map>
#include <vector>
#include <fstream>
using namespace std;

#include <Core/Grid/Grid.h>
#include <Core/Grid/Region.h>
#include <Core/Grid/Patch.h>
#include <Core/Grid/Level.h>
#include <Core/Grid/Variables/CellIterator.h>
#include <Core/Grid/Variables/ComputeSet.h>
#include <Core/Parallel/ProcessorGroup.h>
#include <CCA/Components/LoadBalancers/CostForecasterBase.h>
#include <CCA/Components/LoadBalancers/CostModeler.h>
#include <CCA/Components/LoadBalancers/DynamicLoadBalancer.h>
#include <CCA/Ports/Scheduler.h>

namespace Uintah {
   /**************************************
     
     CLASS
       CostModelForecaster 
      
       Profiles the execution costs of regions and then uses the profiling
       data to compute the coefficents of the cost model.  
       
     GENERAL INFORMATION
      
       CostModelForecaster.h
      
       Justin Luitjens
       Department of Computer Science
       University of Utah
      
       Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
      
       Copyright (C) 2000 SCI Group
      
     KEYWORDS
       CostModelForecaster
       DynamicLoadBalancer
      
     DESCRIPTION
       Profiles the execution costs of regions and then uses the profiling
       data to compute the coefficents of the cost model.  

       The DLB uses the getWeights function to assign weights to regions which 
       can then be used to load balance the calculation.
      
     WARNING
      
     ****************************************/

  class CostModelForecaster : public CostModeler {
    public:
      CostModelForecaster(const ProcessorGroup* myworld, DynamicLoadBalancer *lb, double patchCost, double cellCost, double particleCost ) : CostModeler(patchCost,cellCost,particleCost), d_lb(lb), d_myworld(myworld){setTimestepWindow(20);};
      void addContribution(DetailedTask *task, double cost);
      //finalize the contributions for this timestep
      void finalizeContributions(const GridP currentGrid);
      //get the contributions for each patch, particles are ignored
      void getWeights(const Grid* grid, vector<vector<int> > num_particles, vector<vector<double> >&costs);
      //sets the decay rate for the exponential average
      void setTimestepWindow(int window) { d_alpha=2.0/(window+1);}
      
      struct PatchInfo
      {
        PatchInfo(){};
        PatchInfo(int np,int nc, double et) : num_particles(np), num_cells(nc), execTime(et)
        {}
        int num_particles;
        int num_cells;
        double execTime;
      };
    private:
      DynamicLoadBalancer *d_lb;
      const ProcessorGroup* d_myworld;
      double d_alpha;

      map<int,double> execTimes;



      void collectPatchInfo(const GridP currentGrid, vector<PatchInfo> &patch_info);
  };
      
  ostream& operator<<(ostream& out, const CostModelForecaster::PatchInfo &pi);

} // End namespace Uintah


#endif

