//  HypoElastic.h 
//  class ConstitutiveModel ConstitutiveModel data type -- 3D - 
//  holds ConstitutiveModel
//  information for the FLIP technique:
//    This is for HypoElasticity
//    Features:
//      Usage:



#ifndef __HYPOELASTIC_CONSTITUTIVE_MODEL_H__
#define __HYPOELASTIC_CONSTITUTIVE_MODEL_H__


#include <math.h>
#include "ConstitutiveModel.h"	
#include <Packages/Uintah/CCA/Components/MPM/Util/Matrix3.h>
#include <vector>

namespace Uintah {
      class HypoElastic : public ConstitutiveModel {
      private:
         // Create datatype for storing model parameters
      public:
         struct CMData {
            double G;
	    double K;
         };

	 struct StateData {
	    Matrix3 DevStress[5];
	 };
      private:
         friend const TypeDescription* fun_getTypeDescription(CMData*);

         CMData d_initialData;

         // Prevent copying of this class
         // copy constructor
         HypoElastic(const HypoElastic &cm);
         HypoElastic& operator=(const HypoElastic &cm);

      public:
         // constructors
         HypoElastic(ProblemSpecP& ps);
       
         // destructor
         virtual ~HypoElastic();
         // compute stable timestep for this patch
         virtual void computeStableTimestep(const Patch* patch,
                                            const MPMMaterial* matl,
                                            DataWarehouseP& new_dw);

         // compute stress at each particle in the patch
         virtual void computeStressTensor(const Patch* patch,
                                          const MPMMaterial* matl,
                                          DataWarehouseP& old_dw,
                                          DataWarehouseP& new_dw);

         // initialize  each particle's constitutive model data
         virtual void initializeCMData(const Patch* patch,
                                       const MPMMaterial* matl,
                                       DataWarehouseP& new_dw);

         virtual void addComputesAndRequires(Task* task,
                                             const MPMMaterial* matl,
                                             const Patch* patch,
                                             DataWarehouseP& old_dw,
                                             DataWarehouseP& new_dw) const;

         //for fracture
         virtual void computeCrackSurfaceContactForce(const Patch* patch,
                                           const MPMMaterial* matl,
                                           DataWarehouseP& old_dw,
                                           DataWarehouseP& new_dw);

         virtual void addComputesAndRequiresForCrackSurfaceContact(
	                                     Task* task,
					     const MPMMaterial* matl,
					     const Patch* patch,
					     DataWarehouseP& old_dw,
					     DataWarehouseP& new_dw) const;

         // class function to read correct number of parameters
         // from the input file
         static void readParameters(ProblemSpecP ps, double *p_array);

         // class function to write correct number of parameters
         // from the input file, and create a new object
         static ConstitutiveModel* readParametersAndCreate(ProblemSpecP ps);

         // member function to read correct number of parameters
         // from the input file, and any other particle information
         // need to restart the model for this particle
         // and create a new object
         static ConstitutiveModel* readRestartParametersAndCreate(
                                                        ProblemSpecP ps);

	 virtual void addParticleState(std::vector<const VarLabel*>& from,
				       std::vector<const VarLabel*>& to);
         // class function to create a new object from parameters
         static ConstitutiveModel* create(double *p_array);

         const VarLabel* p_statedata_label;
         const VarLabel* p_statedata_label_preReloc;
//         const VarLabel* bElBarLabel;
//         const VarLabel* bElBarLabel_preReloc;

      };

} // End namespace Uintah

#endif  // __HYPOELASTIC_CONSTITUTIVE_MODEL_H__ 
