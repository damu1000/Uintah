/*

The MIT License

Copyright (c) 1997-2010 Center for the Simulation of Accidental Fires and 
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


//----- CO2RateSrc.h -----------------------------------------------

#ifndef Uintah_Component_Arches_CO2RateSrc_h
#define Uintah_Component_Arches_CO2RateSrc_h

#include <CCA/Components/Arches/ExtraScalarSrc.h>

/**************************************
CLASS
   CO2RateSrc
   
   Class CO2RateSrc is 

GENERAL INFORMATION
   CO2RateSrc.h - declaration of the class
   
   Author: Stanislav Borodai(borodai@crsim.utah.edu)
      
   Creation Date:   July 30th , 2007
   
   C-SAFE 
   
   Copyright U of U 2007

KEYWORDS


DESCRIPTION
   Class CO2RateSrc is 

WARNING
   none
****************************************/

namespace Uintah {
class BoundaryCondition; 
class CO2RateSrc: public ExtraScalarSrc{

public:

  ////////////////////////////////////////////////////////////////////////
  // Constructor for CO2RateSrc.
  CO2RateSrc(const ArchesLabel* label, 
             const MPMArchesLabel* MAlb,
             const VarLabel* d_src_label);

  // GROUP: Destructors:
  ////////////////////////////////////////////////////////////////////////
  // Virtual destructor for CO2RateSrc.
  ~CO2RateSrc();


  // GROUP: Problem Setup :
  ///////////////////////////////////////////////////////////////////////
  // Interface for Set up of the problem specification database
  void problemSetup(const ProblemSpecP& db);


  void sched_addExtraScalarSrc(SchedulerP& sched, 
                               const PatchSet* patches,
                               const MaterialSet* matls,
                               const TimeIntegratorLabel* timelabels);

  inline const string getTableName(){
    return d_tableName;
  }
/*
  inline const int getTableIndex(){
          return d_tableIndex;
  }
*/
  inline void setTableIndex(int tableIndex){
    d_tableIndex = tableIndex;
  }


protected:

private:
  CO2RateSrc();


  void addExtraScalarSrc(const ProcessorGroup*,
                         const PatchSubset* patches,
                         const MaterialSubset* matls,
                         DataWarehouse* old_dw,
                         DataWarehouse* new_dw,
                         const TimeIntegratorLabel* timelabels);

  string d_tableName;
  int d_tableIndex;
  double d_scaleFactor; 

}; // End class CO2RateSrc
} // End namespace Uintah

#endif


