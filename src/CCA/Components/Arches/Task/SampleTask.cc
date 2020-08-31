#include <CCA/Components/Arches/Task/SampleTask.h>

using namespace Uintah;

//--------------------------------------------------------------------------------------------------
SampleTask::SampleTask( std::string task_name, int matl_index )
  : TaskInterface( task_name, matl_index )
{
}

//--------------------------------------------------------------------------------------------------
SampleTask::~SampleTask()
{
}

//--------------------------------------------------------------------------------------------------
void
SampleTask::problemSetup( ProblemSpecP& db )
{
  _value = 1.0;
  //db->findBlock("sample_task")->getAttribute("value",_value);
}

//--------------------------------------------------------------------------------------------------
void
SampleTask::register_initialize( std::vector<ArchesFieldContainer::VariableInformation>& variable_registry, const bool packed_tasks )
{
  // Register all data warehouse variables used in task SampleTask::initialize
  register_variable( "a_sample_field", ArchesFieldContainer::COMPUTES, variable_registry, m_task_name );
  register_variable( "a_result_field", ArchesFieldContainer::COMPUTES, variable_registry, m_task_name );

  // NOTES:
  // * Pass underlying strings into register_variable where possible to improve searchability (e.g., "a_sample_field")
  // * Uintah infrastructure uses underlying strings for debugging output and exceptions
  // * Supported parameter lists can be found in src/CCA/Components/Arches/Task/TaskVariableTools.cc
}

//--------------------------------------------------------------------------------------------------
void
<<<<<<< HEAD
SampleTask::timestep_init( const Patch* patch, ArchesTaskInfoManager* tsk_info ){}

//--------------------------------------------------------------------------------------------------
void
SampleTask::register_initialize(
  std::vector<ArchesFieldContainer::VariableInformation>& variable_registry,
  const bool packed_tasks ){

  typedef ArchesFieldContainer AFC;

  //FUNCITON CALL     STRING NAME(VL)     TYPE       DEPENDENCY    GHOST DW     VR
  register_variable( "a_sample_field", AFC::MODIFIES, variable_registry, m_task_name );
  register_variable( "a_result_field", AFC::COMPUTES, variable_registry, m_task_name );

}

//--------------------------------------------------------------------------------------------------
void
SampleTask::initialize( const Patch* patch, ArchesTaskInfoManager* tsk_info ){

  //CCVariable<double>& field  = *(tsk_info->get_uintah_field<CCVariable<double> >( "a_sample_field" ));
  //CCVariable<double>& result = *(tsk_info->get_uintah_field<CCVariable<double> >( "a_result_field" ));

  //constCCVariable<double>& field = tsk_info->get_const_uintah_field_add<constCCVariable<double> >("a_sample_field");
  CCVariable<double>& field  = tsk_info->get_uintah_field_add<CCVariable<double> >("a_sample_field");
  CCVariable<double>& result = tsk_info->get_uintah_field_add<CCVariable<double> >("a_result_field");

  //traditional functor:
  struct mySpecialOper{
    //constructor
    mySpecialOper( CCVariable<double>& var ) : m_var(var){}
    //operator
    void
    operator()(int i, int j, int k) const {

      m_var(i,j,k) = 2.0;

    }
    private:
    CCVariable<double>& m_var;
  };

  mySpecialOper actual_oper(result);

  Uintah::BlockRange range(patch->getExtraCellLowIndex(), patch->getExtraCellHighIndex() );

  Uintah::parallel_for( range, actual_oper );

  // lambda style
  Uintah::parallel_for( range, [&](int i, int j, int k){
    field(i,j,k) = 1.1;
    result(i,j,k) = 2.1;
  });

=======
SampleTask::initialize( const Patch* patch, ArchesTaskInfoManager* tsk_info )
{
  // Get all data warehouse variables used in SampleTask::initialize
  CCVariable<double>& field  = tsk_info->get_field<CCVariable<double> >( "a_sample_field" );
  CCVariable<double>& result = tsk_info->get_field<CCVariable<double> >( "a_result_field" );

  // Initialize data warehouse variables
  field.initialize( 1.1 );
  result.initialize( 2.1 );

  // NOTES:
  // * Non-portable get_field calls require 1 template parameter: (1) legacy Uintah type
  // * Pass underlying strings into get_field where possible to improve searchability (e.g., "a_sample_field")
  // * Uintah infrastructure uses underlying strings for debugging output and exceptions
>>>>>>> origin/master
}

//--------------------------------------------------------------------------------------------------
void
<<<<<<< HEAD
SampleTask::register_timestep_eval(
  std::vector<ArchesFieldContainer::VariableInformation>& variable_registry,
  const int time_substep, const bool packed_tasks ){

  typedef ArchesFieldContainer AFC;

  //FUNCITON CALL     STRING NAME(VL)     TYPE       DEPENDENCY    GHOST DW     VR
  register_variable( "a_sample_field", AFC::COMPUTES, variable_registry, time_substep, m_task_name );
  register_variable( "a_result_field", AFC::COMPUTES, variable_registry, time_substep, m_task_name );
  register_variable( "density",        AFC::REQUIRES, 1, AFC::LATEST, variable_registry, time_substep, m_task_name );

  register_variable( "A", AFC::COMPUTES, variable_registry, time_substep, m_task_name );

=======
SampleTask::register_timestep_eval( std::vector<ArchesFieldContainer::VariableInformation>& variable_registry, const int time_substep, const bool packed_tasks )
{
  // Register all data warehouse variables used in SampleTask::eval
  register_variable( "a_sample_field", ArchesFieldContainer::COMPUTES, /* Ghost Cell Quantity, Data Warehouse, */            variable_registry, time_substep, m_task_name );
  register_variable( "a_result_field", ArchesFieldContainer::COMPUTES, /* Ghost Cell Quantity, Data Warehouse, */            variable_registry, time_substep, m_task_name );
  register_variable( "density",        ArchesFieldContainer::REQUIRES, 1,                      ArchesFieldContainer::LATEST, variable_registry, time_substep, m_task_name );

  // NOTES:
  // * Pass underlying strings into register_variable where possible to improve searchability (e.g., "a_sample_field")
  // * Uintah infrastructure uses underlying strings for debugging output and exceptions
  // * Supported parameter lists can be found in src/CCA/Components/Arches/Task/TaskVariableTools.cc
>>>>>>> origin/master
}

//--------------------------------------------------------------------------------------------------
void
SampleTask::eval( const Patch* patch, ArchesTaskInfoManager* tsk_info )
{
  // Get all data warehouse variables used in SampleTask::eval
  CCVariable<double>& field   = tsk_info->get_field<CCVariable<double> >( "a_sample_field" );
  CCVariable<double>& result  = tsk_info->get_field<CCVariable<double> >( "a_result_field" );
  CCVariable<double>& density = tsk_info->get_field<CCVariable<double> >( "density" );

  // Setup the range of cells to iterate over
  Uintah::BlockRange range( patch->getCellLowIndex(), patch->getCellHighIndex() );

  // Setup the loop that iterates over cells
  Uintah::parallel_for( range, [&]( int i, int j, int k ){
    field(i,j,k)  = _value * density(i,j,k);
    result(i,j,k) = field(i,j,k) * field(i,j,k);
  });

  // NOTES:
  // * Non-portable get_field calls require 1 template parameter: (1) legacy Uintah type
  // * Pass underlying strings into get_field where possible to improve searchability (e.g., "a_sample_field")
  // * Uintah infrastructure uses underlying strings for debugging output and exceptions
  // * Non-portable Uintah::parallel_for calls do not pass execObj and are executed serially
}
