#ifndef __NONE_CHECK_H__
#define __NONE_CHECK_H__

#include "StabilityCheck.h"	
#include <Packages/Uintah/Core/ProblemSpec/ProblemSpecP.h>
#include <Packages/Uintah/Core/Math/FastMatrix.h>
#include <Packages/Uintah/Core/Math/TangentModulusTensor.h>

namespace Uintah {

  /*! \class NoneCheck
   *  \brief Do not check for loss of ellipticity/hyperbolicity.
   *  \author  Biswajit Banerjee, \n
   *           C-SAFE and Department of Mechanical Engineering,\n
   *           University of Utah.\n
   *           Copyright (C) 2003 Container Dynamics Group\n

  */
  class NoneCheck : public StabilityCheck {

  public:
	 
    //! Construct an object that can be used to check stability
    NoneCheck();
    NoneCheck(ProblemSpecP& ps);
    NoneCheck(const NoneCheck* cm);

    //! Destructor of stability check
    ~NoneCheck();

    virtual void outputProblemSpec(ProblemSpecP& ps);

    virtual bool doIt() {
      return false;
    };
	 
    /*! Check the stability.

      \return true if unstable
      \return false if stable
    */
    bool checkStability(const Matrix3& stress,
                        const Matrix3& deformRate,
                        const TangentModulusTensor& tangentModulus,
                        Vector& direction);

  private:


    // Prevent copying of this class and copy constructor
    //NoneCheck(const NoneCheck &);
    NoneCheck& operator=(const NoneCheck &);
  };
} // End namespace Uintah
      
#endif  // __NONE_CHECK_H__

