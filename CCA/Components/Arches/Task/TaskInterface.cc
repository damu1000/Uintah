#include <CCA/Components/Arches/Task/TaskInterface.h>
#include <CCA/Components/Arches/Operators/Operators.h>

//Uintah Includes:  

//3P Includes: 
//#include <boost/foreach.hpp>

using namespace Uintah; 
namespace so = SpatialOps;

TaskInterface::TaskInterface( std::string task_name, int matl_index ) : 
  _task_name(task_name),
  _matl_index(matl_index)
{
}

TaskInterface::~TaskInterface()
{ 
  //destroy local labels
  BOOST_FOREACH( const VarLabel* &ilab, _local_labels ){ 
    VarLabel::destroy(ilab); 
  }
}

//====================================================================================
//
//====================================================================================
void 
TaskInterface::register_variable( std::string name, 
                                  VAR_TYPE type, 
                                  VAR_DEPEND dep, 
                                  int nGhost, 
                                  WHICH_DW dw, 
                                  std::vector<VariableInformation>& variable_registry, 
                                  const int time_substep ){

  register_variable_work( name, type, dep, nGhost, dw, variable_registry, time_substep );

}

void 
TaskInterface::register_variable( std::string name, 
                                  VAR_TYPE type, 
                                  VAR_DEPEND dep, 
                                  int nGhost, 
                                  WHICH_DW dw, 
                                  std::vector<VariableInformation>& variable_registry ){

  register_variable_work( name, type, dep, nGhost, dw, variable_registry, 0 );

}

//====================================================================================
//
//====================================================================================
void 
TaskInterface::register_variable_work( std::string name, 
                                       VAR_TYPE type, 
                                       VAR_DEPEND dep, 
                                       int nGhost, 
                                       WHICH_DW dw, 
                                       std::vector<VariableInformation>& variable_registry, 
                                       const int time_substep ){
  
  VariableInformation info; 

  info.name   = name; 
  info.depend = dep;
  info.dw     = dw; 
  info.type   = type; 
  info.nGhost = nGhost; 
  info.dw_inquire = false; 
  info.local = false; 

  switch (dw){

    case OLDDW: 

      info.uintah_task_dw = Task::OldDW; 
      break; 

    case NEWDW: 

      info.uintah_task_dw = Task::NewDW; 
      break; 

    case LATEST: 

      info.dw_inquire = true; 
      break; 

    default:

      throw InvalidValue("Arches Task Error: Cannot determine the DW needed for variable: "+name, __FILE__, __LINE__); 
      break;

  }

  //check for conflicts: 
  if ( (dep == COMPUTES && dw == OLDDW) ||
       (dep == LOCAL_COMPUTES && dw == OLDDW) ){ 
    throw InvalidValue("Arches Task Error: Cannot COMPUTE (COMPUTES) a variable from OldDW for variable: "+name, __FILE__, __LINE__); 
  }

  if ( (dep == MODIFIES && dw == OLDDW) ){ 
    throw InvalidValue("Arches Task Error: Cannot MODIFY a variable from OldDW for variable: "+name, __FILE__, __LINE__); 
  }

  if ( dep == COMPUTES || 
       dep == LOCAL_COMPUTES ){

    if ( nGhost > 0 ){ 

      std::cout << "Arches Task Warning: Variable COMPUTE (COMPUTES) found that is requesting ghosts for: "+name+" Nghosts set to zero!" << std::endl; 
      info.nGhost = 0;
    
    }
  }

  //create new varlabels if needed
  //NOTE: We aren't going to check here 
  //to make sure that other variables are
  //created somewhere else.  That check 
  //will be done later. 
  if ( dep == LOCAL_COMPUTES ) {  
       

    const VarLabel* test = NULL; 
    test = VarLabel::find( name );

    if ( test == NULL && time_substep == 0 ){ 

      if ( type == CC_INT ){
        info.label = VarLabel::create( name, CCVariable<int>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == CC_DOUBLE ){ 
        info.label = VarLabel::create( name, CCVariable<double>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == CC_VEC ){ 
        info.label = VarLabel::create( name, CCVariable<Vector>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == FACEX ){ 
        info.label = VarLabel::create( name, SFCXVariable<double>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == FACEY ){ 
        info.label = VarLabel::create( name, SFCYVariable<double>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == FACEZ ){
        info.label = VarLabel::create( name, SFCZVariable<double>::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == SUM ){
        info.label = VarLabel::create( name, sum_vartype::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == MAX ){
        info.label = VarLabel::create( name, max_vartype::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      } else if ( type == MIN ){
        info.label = VarLabel::create( name, min_vartype::getTypeDescription() );
        info.local = true; 
        _local_labels.push_back(info.label); 
      }

      info.depend = COMPUTES; 
      dep = COMPUTES; 

    } else if ( test != NULL && time_substep > 0 ){ 

      //because computing happens on time_substep = 0 
      //checking for duplicate labels occurred upstream
      info.depend = MODIFIES; 
      dep = MODIFIES; 
    
    } else { 

      throw InvalidValue("Arches Task Error: Trying to create a local variable, "+name+", that already exists for Arches Task: "+_task_name, __FILE__, __LINE__); 

    }
    
  }

  if ( dep == REQUIRES ){ 

    if ( type == CC_INT ){
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundCells; 
      }
    } else if ( type == CC_DOUBLE ){ 
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundCells; 
      }
    } else if ( type == CC_VEC ){ 
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundCells; 
      }
    } else if ( type == FACEX ){ 
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundFaces; 
      }
    } else if ( type == FACEY ){ 
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundFaces; 
      }
    } else if ( type == FACEZ ){
      if ( nGhost == 0 ){ 
        info.ghost_type = Ghost::None; 
      } else { 
        info.ghost_type = Ghost::AroundFaces; 
      }
    }  
  }

  //label will be matched later. 
  //info.label = NULL; 

  //load the variable on the registry: 
  variable_registry.push_back( info ); 

}

//====================================================================================
//
//====================================================================================
void 
TaskInterface::resolve_labels( std::vector<VariableInformation>& variable_registry ){ 

  BOOST_FOREACH( VariableInformation &ivar, variable_registry ){ 

    ivar.label = VarLabel::find( ivar.name ); 

    if ( ivar.label == NULL ){ 
      throw InvalidValue("Arches Task Error: Cannot resolve variable label for task execution: "+ivar.name, __FILE__, __LINE__); 
    }
  }

}

//====================================================================================
//
//====================================================================================
template <class T>
void TaskInterface::resolve_field_requires( DataWarehouse* old_dw, 
                                            DataWarehouse* new_dw, 
                                            T* field, 
                                            VariableInformation& info, 
                                            const Patch* patch, 
                                            const int time_substep ){ 

  if ( info.dw_inquire ){ 
    if ( time_substep > 0 ){ 
      new_dw->get( *field , info.label, _matl_index, patch, info.ghost_type, info.nGhost );
    } else { 
      old_dw->get( *field , info.label, _matl_index, patch, info.ghost_type, info.nGhost );
    }
  } else {
    if ( info.dw == OLDDW ){ 
      old_dw->get( *field , info.label, _matl_index, patch, info.ghost_type, info.nGhost );
    } else { 
      new_dw->get( *field , info.label, _matl_index, patch, info.ghost_type, info.nGhost );
    }
  }


}

//====================================================================================
//
//====================================================================================
template <class T>
void TaskInterface::resolve_field_modifycompute( DataWarehouse* old_dw, DataWarehouse* new_dw, T* field, VariableInformation& info, const Patch* patch, const int time_substep ){ 

  switch(info.depend){ 

    case COMPUTES: 

      new_dw->allocateAndPut( field, info.label, _matl_index, patch );
      break; 

    case MODIFIES: 

      new_dw->getModifiable( field, info.label, _matl_index, patch );
      break; 

    default: 

      throw InvalidValue("Arches Task Error: Cannot resolve DW dependency for variable: "+info.name, __FILE__, __LINE__); 

  }
}

//====================================================================================
//
//====================================================================================
void TaskInterface::resolve_fields( DataWarehouse* old_dw, 
                                    DataWarehouse* new_dw, 
                                    const Patch* patch, 
                                    UintahVarMap& var_map, 
                                    ConstUintahVarMap& const_var_map, 
                                    FieldCollector* f_collector ){ 


  std::vector<VariableInformation>& variable_registry = f_collector->get_variable_reg(); 

  int time_substep = f_collector->get_time_substep();

  //loop through all the fields and do the allocates, modifies, and gets
  //stuff the resultant fields into a map for later reference. 
  BOOST_FOREACH( VariableInformation &ivar, variable_registry ){ 

    switch ( ivar.type ){

      case CC_INT:
        if ( ivar.depend == REQUIRES ){ 

          constCCVariable<int>* var = scinew constCCVariable<int>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          CCVariable<int>* var = scinew CCVariable<int>;
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 

        } else { 

          CCVariable<int>* var = scinew CCVariable<int>;
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      case CC_DOUBLE: 

        if ( ivar.depend == REQUIRES ){ 

          constCCVariable<double>* var = scinew constCCVariable<double>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          CCVariable<double>* var = scinew CCVariable<double>;
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
            
        } else { 

          CCVariable<double>* var = scinew CCVariable<double>;
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      case CC_VEC: 

        if ( ivar.depend == REQUIRES ){ 

          constCCVariable<Vector>* var = scinew constCCVariable<Vector>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          CCVariable<Vector>* var = scinew CCVariable<Vector>;
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 

        } else { 

          CCVariable<Vector>* var = scinew CCVariable<Vector>;
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      case FACEX: 

        if ( ivar.depend == REQUIRES ){ 

          constSFCXVariable<double>* var = scinew constSFCXVariable<double>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          SFCXVariable<double>* var = scinew SFCXVariable<double>; 
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 

        } else { 

          SFCXVariable<double>* var = scinew SFCXVariable<double>; 
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      case FACEY: 

        if ( ivar.depend == REQUIRES ){ 

          constSFCYVariable<double>* var = scinew constSFCYVariable<double>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          SFCYVariable<double>* var = scinew SFCYVariable<double>; 
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 

        } else { 

          SFCYVariable<double>* var = scinew SFCYVariable<double>; 
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      case FACEZ: 

        if ( ivar.depend == REQUIRES ){ 

          constSFCZVariable<double>* var = scinew constSFCZVariable<double>; 
          resolve_field_requires( old_dw, new_dw, var, ivar, patch, time_substep ); 
          const_var_map.insert(std::make_pair(ivar.name, var)); 

        } else if ( ivar.depend == MODIFIES ){ 

          SFCZVariable<double>* var = scinew SFCZVariable<double>; 
          new_dw->getModifiable( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 

        } else { 

          SFCZVariable<double>* var = scinew SFCZVariable<double>; 
          new_dw->allocateAndPut( *var, ivar.label, _matl_index, patch );
          var_map.insert(std::make_pair(ivar.name, var)); 
          
        }
        break; 

      default: 
        throw InvalidValue("Arches Task Error: Cannot resolve DW dependency for variable: "+ivar.name, __FILE__, __LINE__); 
        break; 

    }
  }
}

//====================================================================================
//
//====================================================================================
void TaskInterface::schedule_task( const LevelP& level, 
                                   SchedulerP& sched, 
                                   const MaterialSet* matls,
                                   int time_substep ){ 

  std::vector<VariableInformation> variable_registry; 

  register_timestep_eval( variable_registry, time_substep ); 

  resolve_labels( variable_registry ); 

  Task* tsk = scinew Task( _task_name, this, &TaskInterface::do_task, variable_registry, time_substep ); 

  BOOST_FOREACH( VariableInformation &ivar, variable_registry ){ 

    switch(ivar.depend){

      case COMPUTES: 
        if ( time_substep == 0 ){ 
          tsk->computes( ivar.label ); //only compute on the zero time substep
        } else { 
          tsk->modifies( ivar.label );
          ivar.dw = NEWDW; 
          ivar.uintah_task_dw = Task::NewDW; 
          ivar.depend = MODIFIES; 
        }
        break; 
      case MODIFIES: 
        tsk->modifies( ivar.label );
        break; 
      case REQUIRES: 
        if ( ivar.dw_inquire ){
          if ( time_substep > 0 ){ 
            ivar.dw = NEWDW;
            ivar.uintah_task_dw = Task::NewDW; 
          } else { 
            ivar.dw = OLDDW; 
            ivar.uintah_task_dw = Task::OldDW; 
          }
        } else { 
          if ( ivar.dw == OLDDW ){
            ivar.uintah_task_dw = Task::OldDW; 
          } else { 
            ivar.uintah_task_dw = Task::NewDW; 
          }
        }
        tsk->requires( ivar.uintah_task_dw, ivar.label, ivar.ghost_type, ivar.nGhost );
        break; 
      default: 
        throw InvalidValue("Arches Task Error: Cannot schedule task becuase of incomplete variable dependency: "+_task_name, __FILE__, __LINE__); 
        break; 

    }
  }

  //other variables: 
  tsk->requires(Task::OldDW, VarLabel::find("delT")); 

  sched->addTask( tsk, level->eachPatch(), matls );

}

//====================================================================================
//
//====================================================================================
void TaskInterface::schedule_init( const LevelP& level, 
                                   SchedulerP& sched, 
                                   const MaterialSet* matls ){ 

  std::vector<VariableInformation> variable_registry; 

  register_initialize( variable_registry ); 

  resolve_labels( variable_registry ); 

  Task* tsk = scinew Task( _task_name+"_initialize", this, &TaskInterface::do_init, variable_registry ); 

  BOOST_FOREACH( VariableInformation &ivar, variable_registry ){ 

    if ( ivar.dw == OLDDW ){ 
      throw InvalidValue("Arches Task Error: Cannot use OLDDW for initialization task: "+_task_name, __FILE__, __LINE__); 
    }

    switch(ivar.depend){

      case COMPUTES: 
        tsk->computes( ivar.label ); 
        break; 
      case MODIFIES: 
        tsk->modifies( ivar.label );
        break; 
      case REQUIRES: 
        ivar.dw = NEWDW; 
        ivar.uintah_task_dw = Task::NewDW; 
        tsk->requires( ivar.uintah_task_dw, ivar.label, ivar.ghost_type, ivar.nGhost );
        break; 
      default: 
        throw InvalidValue("Arches Task Error: Cannot schedule task becuase of incomplete variable dependency: "+_task_name, __FILE__, __LINE__); 
        break; 

    }
  }

  sched->addTask( tsk, level->eachPatch(), matls );

}

//void TaskInterface::schedule_task( const LevelP& level, 
//                                   SchedulerP& sched, 
//                                   const MaterialSet* matls,
//                                   std::vector<VariableInformation>& variable_registry, 
//                                   int time_substep ){ 
//
//  register_all_variables( variable_registry, time_substep ); 
//
//  resolve_labels( variable_registry ); 
//
//  Task* tsk = scinew Task( _task_name+"_eval", this, &TaskInterface::do_task, variable_registry, time_substep ); 
//
//  BOOST_FOREACH( VariableInformation &ivar, variable_registry ){ 
//
//    switch(ivar.depend){
//
//      case COMPUTES: 
//        if ( time_substep == 0 ){ 
//          tsk->computes( ivar.label ); //only compute on the zero time substep
//        } else { 
//          tsk->modifies( ivar.label );
//          ivar.dw = NEWDW; 
//          ivar.uintah_task_dw = Task::NewDW; 
//          ivar.depend= MODIFIES; 
//        }
//        break; 
//      case MODIFIES: 
//        tsk->modifies( ivar.label );
//        break; 
//      case REQUIRES: 
//        if ( ivar.dw_inquire ){
//          if ( time_substep > 0 ){ 
//            ivar.dw = NEWDW;
//            ivar.uintah_task_dw = Task::NewDW; 
//          } else { 
//            ivar.dw = OLDDW; 
//            ivar.uintah_task_dw = Task::OldDW; 
//          }
//        } else { 
//          if ( ivar.dw == OLDDW ){
//            ivar.uintah_task_dw = Task::OldDW; 
//          } else { 
//            ivar.uintah_task_dw = Task::NewDW; 
//          }
//        }
//        tsk->requires( ivar.uintah_task_dw, ivar.label, ivar.ghost_type, ivar.nGhost );
//        break; 
//      default: 
//        throw InvalidValue("Arches Task Error: Cannot schedule task becuase of incomplete variable dependency: "+_task_name, __FILE__, __LINE__); 
//        break; 
//
//    }
//  }
//
//  //other variables: 
//  tsk->requires(Task::OldDW, VarLabel::find("delT")); 
//
//  sched->addTask( tsk, level->eachPatch(), matls );
//
//}

void TaskInterface::do_task( const ProcessorGroup* pc, 
                             const PatchSubset* patches, 
                             const MaterialSubset* matls, 
                             DataWarehouse* old_dw, 
                             DataWarehouse* new_dw, 
                             std::vector<VariableInformation> variable_registry, 
                             int time_substep ){

  for (int p = 0; p < patches->size(); p++) {
    
    const Patch* patch = patches->get(p);
  
    UintahVarMap variable_map;
    ConstUintahVarMap const_variable_map; 

    SchedToTaskInfo info; 

    //get the current dt
    delt_vartype DT;
    old_dw->get(DT, VarLabel::find("delT")); 
    info.dt = DT; 
    info.time_substep = time_substep; 

    FieldCollector* field_collector = scinew FieldCollector(variable_registry, patch, info); 

    //doing DW gets...
    resolve_fields( old_dw, new_dw, patch, variable_map, const_variable_map, field_collector ); 

    //this makes the "getting" of the grid variables easier from the user side (ie, only need a string name )
    field_collector->set_var_maps(variable_map, const_variable_map); 

    //get the operator DB for this patch
    Operators& opr = Operators::self(); 
    Operators::PatchInfoMap::iterator i_opr = opr.patch_info_map.find(patch->getID()); 

    eval( patch, field_collector, i_opr->second._sodb ); 

    //clean up 
    delete field_collector; 
    
    for ( UintahVarMap::iterator i = variable_map.begin(); i != variable_map.end(); i++ ){
      delete i->second; 
    }
    for ( ConstUintahVarMap::iterator i = const_variable_map.begin(); i != const_variable_map.end(); i++ ){
      delete i->second; 
    }
  }
}

void TaskInterface::do_init( const ProcessorGroup* pc, 
                             const PatchSubset* patches, 
                             const MaterialSubset* matls, 
                             DataWarehouse* old_dw, 
                             DataWarehouse* new_dw, 
                             std::vector<VariableInformation> variable_registry ){

  for (int p = 0; p < patches->size(); p++) {
    
    const Patch* patch = patches->get(p);

    UintahVarMap variable_map;
    ConstUintahVarMap const_variable_map; 

    SchedToTaskInfo info; 

    //get the current dt
    info.dt = 0; 
    info.time_substep = 0; 

    FieldCollector* field_collector = scinew FieldCollector(variable_registry, patch, info); 

    //doing DW gets...
    resolve_fields( old_dw, new_dw, patch, variable_map, const_variable_map, field_collector ); 

    //this makes the "getting" of the grid variables easier from the user side (ie, only need a string name )
    field_collector->set_var_maps(variable_map, const_variable_map); 

    //get the operator DB for this patch
    Operators& opr = Operators::self(); 
    Operators::PatchInfoMap::iterator i_opr = opr.patch_info_map.find(patch->getID()); 

    initialize( patch, field_collector, i_opr->second._sodb ); 

    //clean up 
    delete field_collector; 

    for ( UintahVarMap::iterator i = variable_map.begin(); i != variable_map.end(); i++ ){
      delete i->second; 
    }
    for ( ConstUintahVarMap::iterator i = const_variable_map.begin(); i != const_variable_map.end(); i++ ){
      delete i->second; 
    }
  }
}
