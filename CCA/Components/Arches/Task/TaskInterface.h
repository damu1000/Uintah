#ifndef Uintah_Component_Arches_TaskInterface_h
#define Uintah_Component_Arches_TaskInterface_h

#include <spatialops/structured/FVStaggeredFieldTypes.h>
#include <spatialops/structured/MemoryWindow.h>

#include <Core/Grid/Variables/VarLabel.h>
#include <Core/Grid/LevelP.h>
#include <Core/Grid/Variables/CCVariable.h>
#include <Core/Grid/Variables/SFCXVariable.h>
#include <Core/Grid/Variables/SFCYVariable.h>
#include <Core/Grid/Variables/SFCZVariable.h>
#include <Core/Grid/Variables/VarTypes.h>
#include <Core/Exceptions/InvalidValue.h>
#include <CCA/Ports/Scheduler.h>
#include <Core/Grid/Task.h>
#include <string>
#include <vector>
#include <boost/foreach.hpp>

//===============================================================

/** 
* @class  Task Interface for Arches
* @author Jeremy Thornock
* @date   2014
* 
* @brief Serves as the interface to a standard uintah task. 
* 
**/ 

//===============================================================

namespace Uintah{ 

  class Task; 
  class VarLabel; 
  class Level;  

  class TaskInterface{ 

public: 

    enum VAR_DEPEND { COMPUTES, MODIFIES, REQUIRES, LOCAL_COMPUTES, LOCAL_MODIFIES, LOCAL_REQUIRES };
    enum WHICH_DW { OLDDW, NEWDW, LATEST };
    enum VAR_TYPE { CC_INT, CC_DOUBLE, CC_VEC, FACEX, FACEY, FACEZ, SUM, MAX, MIN };

    struct VariableInformation { 

      std::string name;
      VAR_TYPE    type; 
      VAR_DEPEND  depend; 
      WHICH_DW    dw;
      int         nGhost;
      bool        dw_inquire; 
      const VarLabel* label; 
      Task::WhichDW uintah_task_dw; 
      Ghost::GhostType ghost_type; 
    
    };

    /** @brief Default constructor **/ 
    TaskInterface( std::string take_name, int matl_index ); 

    /** @brief Default destructor **/ 
    virtual ~TaskInterface();

    /** @brief Print task name. **/ 
    void print_task_name(){ 
      std::cout << "Task: " << _task_name << std::endl; 
    }

    /** @brief Registers all variables with pertinent information for the 
     *         uintah dw interface **/ 
    virtual void register_all_variables( std::vector<VariableInformation>& variable_registry ) = 0; 

    /** @brief Matches labels to variables in the registry **/ 
    void resolve_labels( std::vector<VariableInformation>& variable_registry ); 


    /** @brief Add this task to the Uintah task scheduler. (Overloaded)
     * Start with a predefined variable registry **/ 
    void schedule_task( const LevelP& level, 
                        SchedulerP& sched, 
                        const MaterialSet* matls,
                        std::vector<VariableInformation>& variable_registry,
                        int time_substep );

    /** @brief Add this task to the Uintah task scheduler **/ 
    void schedule_task( const LevelP& level, 
                        SchedulerP& sched, 
                        const MaterialSet* matls,
                        int time_substep );

    /** @brief The actual task interface function that references the 
     *         derived class implementation **/ 
    void do_task( const ProcessorGroup* pc, 
                  const PatchSubset* patches, 
                  const MaterialSubset* matls, 
                  DataWarehouse* old_dw, 
                  DataWarehouse* new_dw, 
                  std::vector<VariableInformation> variable_registry, 
                  int time_substep );

    /**
     * \fn void get_bc_logicals( const Uintah::Patch* const, SpatialOps::structured::IntVec&, SpatialOps::structured::IntVec& );
     * \brief Given the patch, populate information about whether a physical
     *        boundary exists on each side of the patch.
     * \param patch   - the patch of interest
     * \param bcMinus - assigned to 0 if no BC present on (-) faces, 1 if present
     * \param bcPlus  - assigned to 0 if no BC present on (+) faces, 1 if present
     */
    void get_bc_logicals( const Uintah::Patch* const patch,
                          SpatialOps::structured::IntVec& bcMinus,
                          SpatialOps::structured::IntVec& bcPlus );

    template< typename FieldT, typename UFT >
    inline FieldT* wrap_uintah_field_as_spatialops( UFT& uintahVar,
                                                    const Uintah::Patch* const patch,
                                                    const int nGhost, 
                                                    const SpatialOps::MemoryType mtype=SpatialOps::LOCAL_RAM,
                                                    const unsigned short int deviceIndex=0,
                                                    double* uintahDeviceVar = NULL )
    {

      namespace SS = SpatialOps::structured;

      using SCIRun::IntVector;

      const SCIRun::IntVector lowIx       = uintahVar->getLowIndex();
      const SCIRun::IntVector highIx      = uintahVar->getHighIndex();
      const SCIRun::IntVector fieldSize   = uintahVar->getWindow()->getData()->size();
      const SCIRun::IntVector fieldOffset = uintahVar->getWindow()->getOffset();
      const SCIRun::IntVector fieldExtent = highIx - lowIx;

      const SS::IntVec   size( fieldSize[0],   fieldSize[1],   fieldSize[2]   );
      const SS::IntVec extent( fieldExtent[0], fieldExtent[1], fieldExtent[2] );
      const SS::IntVec offset( lowIx[0]-fieldOffset[0], lowIx[1]-fieldOffset[1], lowIx[2]-fieldOffset[2] );

      SS::IntVec bcMinus, bcPlus;
      get_bc_logicals( patch, bcMinus, bcPlus );

      double* fieldValues_ = NULL;
//      if( mtype == SpatialOps::EXTERNAL_CUDA_GPU ){
//#       ifdef HAVE_CUDA
//        fieldValues_ = const_cast<double*>( uintahDeviceVar );
//#       endif
//      }
//      else{
        fieldValues_ = const_cast<typename FieldT::value_type*>( uintahVar->getPointer() );
//      }

      return new FieldT( SS::MemoryWindow( size, offset, extent ),
                         SS::BoundaryCellInfo::build<FieldT>(bcPlus),
                         nGhost,
                         fieldValues_,
                         SS::ExternalStorage,
                         mtype,
                         deviceIndex );
    }


protected: 

    /** @brief Inteface to register_variable_work -- this function is overloaded. **/ 
    void register_variable( std::string name, 
                            VAR_TYPE type, 
                            VAR_DEPEND dep, 
                            int nGhost, 
                            WHICH_DW dw, 
                            std::vector<VariableInformation>& var_reg );

    /** @brief Builds a struct for each variable containing all pertinent uintah
     * DW information **/ 
    void register_variable_work( std::string name, 
                                 VAR_TYPE type, 
                                 VAR_DEPEND dep, 
                                 int nGhost, 
                                 WHICH_DW dw, 
                                 std::vector<VariableInformation>& var_reg );

    /** ()()()() Task grid variable storage ()()()() **/ 
    template<class T>
    struct VarContainer{ 
      T* variable; 
    };

    typedef std::map<std::string, VarContainer<CCVariable<double> > > CCDoubleVarMap; 
    typedef std::map<std::string, GridVariableBase* > UintahVarMap; 
    typedef std::map<std::string, constVariableBase<GridVariableBase>* > ConstUintahVarMap; 

    /** @brief Resolves the DW fields with the dependency **/ 
    void resolve_fields( DataWarehouse* old_dw, 
                         DataWarehouse* new_dw, 
                         const Patch* patch, 
                         std::vector<VariableInformation>& variable_registry, 
                         UintahVarMap& var_map, 
                         ConstUintahVarMap& const_var_map,
                         const int time_substep );

    /** @brief The actual work done within the derived class **/ 
    virtual void eval( const Patch* patch, UintahVarMap& var_map, ConstUintahVarMap& const_var_map ) = 0; 

    /** @brief Return the UINTAH grid variable by string name. **/ 
    template<class T, class M>
    T* get_uintah_grid_var(std::string name, M& var_map); 

    template<class SOST, class M>
    inline SOST* get_sos_grid_var(std::string name, M& var_map, 
                                  const Patch* patch, const int nghost );
    template<class M>
    inline SpatialOps::structured::SVolField* get_sos_grid_var(std::string name, M& var_map, 
                                  const Patch* patch, const int n_ghost );
    template<class M>
    inline SpatialOps::structured::SSurfXField* get_sos_grid_var(std::string name, M& var_map, 
                                  const Patch* patch, const int n_ghost );
    template<class M>
    inline SpatialOps::structured::SSurfYField* get_sos_grid_var(std::string name, M& var_map, 
                                  const Patch* patch, const int n_ghost );
    template<class M>
    inline SpatialOps::structured::SSurfZField* get_sos_grid_var(std::string name, M& var_map, 
                                  const Patch* patch, const int n_ghost );

    /** @brief Builder class containing instructions on how to build the task **/ 
    class TaskBuilder { 

      public: 

        virtual ~TaskBuilder() {}

        virtual TaskInterface* build() = 0; 

      protected: 

    }; 

    std::string _task_name; 
    const int _matl_index; 
   
private: 

    /** @brief Performs all DW get*,allocateAndPut, etc.. for all variables for this 
     *         task. **/
    template<class T>
    void resolve_field_modifycompute( DataWarehouse* old_dw, DataWarehouse* new_dw, T* field, VariableInformation& info, const Patch* patch, const int time_substep );

    /** @brief Performs all DW get*,allocateAndPut, etc.. for all variables for this 
     *         task. **/
    template<class T>
    void resolve_field_requires( DataWarehouse* old_dw, DataWarehouse* new_dw, T& field, VariableInformation& info, const Patch* patch, const int time_substep );

  
  };

  //====================================================================================
  // GRID VARIABLE ACCESS
  //====================================================================================
  //UINTAH
  template<class T, class M>
  inline T* TaskInterface::get_uintah_grid_var(std::string name, M& var_map ){

    if ( var_map.find(name) != var_map.end() ) return dynamic_cast<T* >(var_map.find(name)->second); 

    throw InvalidValue("Arches Task Error: (UINTAH) Cannot resolve grid variable "+name, __FILE__, __LINE__); 
  
  };

  //SPATIAL OPS
  template<class SOST, class M>
  inline SOST* TaskInterface::get_sos_grid_var(std::string name, M& var_map, const Patch* patch, const int n_ghost ){

    throw InvalidValue("Arches Task Error: (SPATIAL OPS) No known SpatialOps conversion for variable named: "+name, __FILE__, __LINE__); 

  };

  template<class M>
  inline SpatialOps::structured::SVolField* TaskInterface::get_sos_grid_var(std::string name, M& var_map, const Patch* patch, const int n_ghost ){

    if ( var_map.find(name) != var_map.end() ) {

      CCVariable<double>* var = dynamic_cast<CCVariable<double>* >(var_map.find(name)->second); 
      return wrap_uintah_field_as_spatialops<SpatialOps::structured::SVolField>( var, patch, n_ghost );

    }

    throw InvalidValue("Arches Task Error: (SPATIAL OPS) Cannot resolve grid variable "+name, __FILE__, __LINE__); 

  };

  template<class M>
  inline SpatialOps::structured::SSurfXField* TaskInterface::get_sos_grid_var(std::string name, M& var_map, const Patch* patch, const int n_ghost ){

    if ( var_map.find(name) != var_map.end() ) {

      SFCXVariable<double>* var = dynamic_cast<SFCXVariable<double>* >(var_map.find(name)->second); 
      return wrap_uintah_field_as_spatialops<SpatialOps::structured::SSurfXField>( var, patch, n_ghost );

    }

    throw InvalidValue("Arches Task Error: (SPATIAL OPS) Cannot resolve grid variable "+name, __FILE__, __LINE__); 

  };

  template<class M>
  inline SpatialOps::structured::SSurfYField* TaskInterface::get_sos_grid_var(std::string name, M& var_map, const Patch* patch, const int n_ghost ){

    if ( var_map.find(name) != var_map.end() ) {

      SFCYVariable<double>* var = dynamic_cast<SFCYVariable<double>* >(var_map.find(name)->second); 
      return wrap_uintah_field_as_spatialops<SpatialOps::structured::SSurfYField>( var, patch, n_ghost );

    }

    throw InvalidValue("Arches Task Error: (SPATIAL OPS) Cannot resolve grid variable "+name, __FILE__, __LINE__); 

  };

  template<class M>
  inline SpatialOps::structured::SSurfZField* TaskInterface::get_sos_grid_var(std::string name, M& var_map, const Patch* patch, const int n_ghost ){

    if ( var_map.find(name) != var_map.end() ) {

      SFCZVariable<double>* var = dynamic_cast<SFCZVariable<double>* >(var_map.find(name)->second); 
      return wrap_uintah_field_as_spatialops<SpatialOps::structured::SSurfZField>( var, patch, n_ghost );

    }

    throw InvalidValue("Arches Task Error: (SPATIAL OPS) Cannot resolve grid variable "+name, __FILE__, __LINE__); 

  };

}

#endif 
