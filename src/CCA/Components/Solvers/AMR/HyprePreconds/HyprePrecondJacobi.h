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

#ifndef Packages_Uintah_CCA_Components_Solvers_HyprePrecondJacobi_h
#define Packages_Uintah_CCA_Components_Solvers_HyprePrecondJacobi_h

/*--------------------------------------------------------------------------
CLASS
   HyprePrecondJacobi
   
   A Hypre Jacobi (Jacobi relaxation) preconditioner.

GENERAL INFORMATION

   File: HyprePrecondJacobi.h

   Oren E. Livne
   Department of Computer Science
   University of Utah

   Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
  

KEYWORDS
   HypreSolverBase, Precond, HypreSolverBase, HypreSolverParams.

DESCRIPTION 
   Class HyprePrecondJacobi sets up and destroys the Hypre
   Jacobi preconditioner to be used with Hypre solvers. Jacobi runs a few
   Jacobi relaxation sweeps (I - D^{-1} A for a matrix A with diagonal D).
   is the distributed sparse linear solver. Jacobi preconditioner
   is often used with CG (I think it works only for symmetric matrices).
  
WARNING
   Works with Hypre Struct interface only and probably good only for symmetric
   matrices.
   --------------------------------------------------------------------------*/

#include <CCA/Components/Solvers/AMR/HyprePreconds/HyprePrecondBase.h>

namespace Uintah {
  
  //---------- Types ----------
  
  class HyprePrecondJacobi : public HyprePrecondBase {

    //========================== PUBLIC SECTION ==========================
  public:
  
    HyprePrecondJacobi(void) : HyprePrecondBase(initPriority()) {}
    virtual ~HyprePrecondJacobi(void);

    virtual void setup(void);
    
    //========================== PROTECTED SECTION ==========================
  protected:
    static Priorities initPriority(void);

  }; // end class HyprePrecondJacobi

} // end namespace Uintah

#endif // Packages_Uintah_CCA_Components_Solvers_HyprePrecondJacobi_h
