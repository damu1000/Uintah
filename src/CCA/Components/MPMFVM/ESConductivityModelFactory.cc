/*
 * The MIT License
 *
 * Copyright (c) 1997-2018 The University of Utah
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
#include <CCA/Components/MPMFVM/ESConductivityModelFactory.h>
#include <CCA/Components/MPMFVM/ESConductivityModel.h>

#include <Core/Exceptions/ProblemSetupException.h>
#include <Core/ProblemSpec/ProblemSpec.h>
#include <Core/Malloc/Allocator.h>

#include <iostream>

using namespace Uintah;

ESConductivityModel* ESConductivityModelFactory::create(const ProblemSpecP& ps,
                                                        SimulationStateP& shared_state,
                                                        MPMFlags* mpm_flags,
                                                        MPMLabel* mpm_lb,
                                                        FVMLabel* fvm_lb)
{
  ProblemSpecP child = ps->findBlock("ESMPM");
  if(!child)
    throw ProblemSetupException("Cannot find ESMPM tag", __FILE__, __LINE__);

  std::string model_type;

  child->require("conductivity_model", model_type);

  if (model_type == "ivd")
    return scinew ESConductivityModel(shared_state, mpm_flags, mpm_lb, fvm_lb, model_type);

  else
    throw ProblemSetupException("Unknown Conductivity Model: ("+model_type+")", __FILE__, __LINE__);

  return 0;
}
