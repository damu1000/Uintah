#ifndef UINTAH_HOMEBREW_CFDINTERFACE_H
#define UINTAH_HOMEBREW_CFDINTERFACE_H

#include <Uintah/Parallel/UintahParallelPort.h>
#include <Uintah/Interface/CFDInterfaceP.h>
#include <Uintah/Interface/DataWarehouseP.h>
#include <Uintah/Grid/GridP.h>
#include <Uintah/Grid/LevelP.h>
#include <Uintah/Interface/SchedulerP.h>
#include <Uintah/Interface/ProblemSpecP.h>

namespace Uintah {
namespace Interface {

using Uintah::Parallel::UintahParallelPort;
using Uintah::Interface::ProblemSpecP;
using Uintah::Grid::LevelP;
using Uintah::Grid::GridP;

/**************************************

CLASS
   CFDInterface
   
   Short description...

GENERAL INFORMATION

   CFDInterface.h

   Steven G. Parker
   Department of Computer Science
   University of Utah

   Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
  
   Copyright (C) 2000 SCI Group

KEYWORDS
   CFD_Interface

DESCRIPTION
   Long description...
  
WARNING
  
****************************************/

class CFDInterface : public UintahParallelPort {
public:
    CFDInterface();
    virtual ~CFDInterface();

    //////////
    // Insert Documentation Here:
    virtual void problemSetup(const ProblemSpecP& params, GridP& grid,
			      DataWarehouseP&)=0;

    //////////
    // Insert Documentation Here:
    virtual void computeStableTimestep(const LevelP& level,
				       SchedulerP&, DataWarehouseP&) = 0;

    //////////
    // Insert Documentation Here:
    virtual void timeStep(double t, double dt,
			  const LevelP& level, SchedulerP&,
			  const DataWarehouseP&, DataWarehouseP&) = 0;
private:
    CFDInterface(const CFDInterface&);
    CFDInterface& operator=(const CFDInterface&);
};

} // end namespace Interface
} // end namespace Uintah

//
// $Log$
// Revision 1.5  2000/03/23 20:00:16  jas
// Changed the include files, namespace, and using statements to reflect the
// move of ProblemSpec from Grid/ to Interface/.
//
// Revision 1.4  2000/03/17 09:30:02  sparker
// New makefile scheme: sub.mk instead of Makefile.in
// Use XML-based files for module repository
// Plus many other changes to make these two things work
//
// Revision 1.3  2000/03/16 22:08:22  dav
// Added the beginnings of cocoon docs.  Added namespaces.  Did a few other coding standards updates too
//
//

#endif

