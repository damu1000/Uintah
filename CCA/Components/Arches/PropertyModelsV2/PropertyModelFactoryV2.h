#ifndef UT_PropertyModelFactoryV2_h
#define UT_PropertyModelFactoryV2_h

#include <CCA/Components/Arches/Task/TaskFactoryBase.h>
#include <string>

namespace Uintah{ 

  class PropertyModelFactoryV2 : public TaskFactoryBase { 

  public: 

    PropertyModelFactoryV2(); 
    ~PropertyModelFactoryV2(); 

    void register_all_tasks( ProblemSpecP& db ); 

    void build_all_tasks( ProblemSpecP& db ); 

    std::vector<std::string> retrieve_task_subset( const std::string subset ) { 
      throw InvalidValue("Error: No task subsets here yet. ", __FILE__,__LINE__);
    }

  protected: 

  private: 

  };
}
#endif 
