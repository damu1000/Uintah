//  ViscoScram.h 
//  
//  class ConstitutiveModel ConstitutiveModel data type -- 3D - 
//  holds ConstitutiveModel
//  information for the FLIP technique:
//    This is for Compressible NeoHookean materials
//     
//
//    Features:
//     
//      
//      Usage:



#ifndef __VISCROSCRAM_CONSTITUTIVE_MODEL_H__
#define __VISCOSCRAM_CONSTITUTIVE_MODEL_H__


#include <math.h>
#include "ConstitutiveModel.h"	
#include <Uintah/Components/MPM/Util/Matrix3.h>
#include <vector>

namespace Uintah {
   namespace MPM {
      
      class ViscoScram : public ConstitutiveModel {
      private:
         // Create datatype for storing model parameters
      public:
         struct CMData {
            double Bulk;
            double Shear;
         };
      private:
         friend const TypeDescription* fun_getTypeDescription(CMData*);

         CMData d_initialData;

         // Prevent copying of this class
         // copy constructor
         ViscoScram(const ViscoScram &cm);
         ViscoScram& operator=(const ViscoScram &cm);

      public:
         // constructors
         ViscoScram(ProblemSpecP& ps);
       
         // destructor
         virtual ~ViscoScram();
         // compute stable timestep for this patch
         virtual void computeStableTimestep(const Patch* patch,
                                            const MPMMaterial* matl,
                                            DataWarehouseP& new_dw);

         // compute stress at each particle in the patch
         virtual void computeStressTensor(const Patch* patch,
                                          const MPMMaterial* matl,
                                          DataWarehouseP& old_dw,
                                          DataWarehouseP& new_dw);

         // compute total strain energy for all particles in the patch
         virtual double computeStrainEnergy(const Patch* patch,
                                            const MPMMaterial* matl,
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

         const VarLabel* p_cmdata_label;
         const VarLabel* bElBarLabel;
         const VarLabel* p_cmdata_label_preReloc;
         const VarLabel* bElBarLabel_preReloc;

      };
      
   } // end namespace Components
} // end namespace Uintah


#endif  // __VISCOSCRAM_CONSTITUTIVE_MODEL_H__ 

// $Log$
// Revision 1.1  2000/08/21 18:37:41  guilkey
// Initial commit of ViscoScram stuff.  Don't get too excited yet,
// currently these are just cosmetically modified copies of CompNeoHook.
//
