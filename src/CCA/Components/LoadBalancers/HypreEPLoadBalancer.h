/*
 * The MIT License
 *
 * Copyright (c) 1997-2020 The University of Utah
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

#ifndef CCA_COMPONENTS_LOADBALANCERS_HypreEPLoadBalancer_H
#define CCA_COMPONENTS_LOADBALANCERS_HypreEPLoadBalancer_H

#include <CCA/Components/LoadBalancers/LoadBalancerCommon.h>

namespace Uintah {

/**************************************

 CLASS
   HypreEPLoadBalancer


 GENERAL INFORMATION

 HypreEPLoadBalancer.h

 Steven G. Parker
 Department of Computer Science
 University of Utah

 Center for the Simulation of Accidental Fires and Explosions (C-SAFE)


 KEYWORDS
   HypreEPLoadBalancer

 DESCRIPTION

 ****************************************/

class Patch;
class ProcessorGroup;

class HypreEPLoadBalancer : public LoadBalancerCommon {

public:

  HypreEPLoadBalancer(const ProcessorGroup* myworld);

  virtual ~HypreEPLoadBalancer() {};

  virtual int getPatchwiseProcessorAssignment(const Patch * patch);

  virtual void problemSetup(ProblemSpecP& pspec, GridP& grid, const MaterialManagerP& materialManager);

  //! The old processor is the same as the current for this load balancer.
  virtual int getOldProcessorAssignment(const Patch * patch)
  {
    return getPatchwiseProcessorAssignment(patch);
  }

  virtual bool needRecompile( const GridP& ) { return false; };

private:

  // eliminate copy, assignment and move
  HypreEPLoadBalancer( const HypreEPLoadBalancer & )            = delete;
  HypreEPLoadBalancer& operator=( const HypreEPLoadBalancer & ) = delete;
  HypreEPLoadBalancer( HypreEPLoadBalancer && )                 = delete;
  HypreEPLoadBalancer& operator=( HypreEPLoadBalancer && )      = delete;

#ifdef USE_MPI_EP
  std::vector<int> m_patch_to_proc; //mapping between patch id to proc. index into the vector is patch id.
#endif
};

}  // End namespace Uintah

#endif // CCA_COMPONENTS_LOADBALANCERS_HypreEPLoadBalancer_H

