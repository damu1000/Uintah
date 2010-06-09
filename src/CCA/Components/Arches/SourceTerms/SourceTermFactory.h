#ifndef UT_SourceTermFactory_h
#define UT_SourceTermFactory_h
#include <CCA/Components/Arches/ArchesLabel.h>
#include <CCA/Components/Arches/ArchesVariables.h>
#include <CCA/Ports/DataWarehouseP.h>
#include <Core/Grid/Variables/VarLabel.h>
#include <Core/Grid/Variables/VarTypes.h>
#include <Core/Grid/SimulationStateP.h>
//#include <map>
//#include <vector>
//#include <string>

//====================================================================

/**
 *  @class  SourceTermBuilder
 *  @author James C. Sutherland and Jeremy Thornock
 *  @date   November, 2006
 *
 *  @brief Abstract base class to support source term
 *  additions. Should be used in conjunction with the
 *  SourceTermFactory.
 *
 *  An arbitrary number of source terms may be added to a transport
 *  equation via the SourceTermFactory.  The SourceTermBuilder object
 *  is passed to the factory to provide a mechanism to instantiate the
 *  SourceTerm object.
 */
namespace Uintah {
//---------------------------------------------------------------------------
// Builder
class SourceTermBase; 
class SourceTermBuilder
{
public:
  SourceTermBuilder(std::string src_name, vector<std::string> reqLabelNames, 
                    SimulationStateP& sharedState) : 
                    d_srcName(src_name), d_requiredLabels(reqLabelNames), 
                    d_sharedState(sharedState){};
  virtual ~SourceTermBuilder(){};

  /**
   *  build the SourceTerm.  Should be implemented using the
   *  "scinew" operator.  Ownership is transfered.
   */
  virtual SourceTermBase* build() = 0;

protected: 
  std::string d_srcName;
  vector<string> d_requiredLabels; 
  SimulationStateP& d_sharedState; 

private: 
};
// End Builder
//---------------------------------------------------------------------------

/**
 *  @class  SourceTermFactory
 *  @author James C. Sutherland and Jeremy Thornock
 *  @date   November, 2006
 *  @brief  Factory for source term generation.
 *
 *  Allows easy addition of source terms to a transport equation.
 *  Simply register the builder object for your SourceTerm with
 *  the factory and it will automatically be added to the requested
 *  TransportEquation.  Multiple source terms may be registered with a
 *  single transport equation.
 *
 *  Implemented as a singleton.
 */
class SourceTermFactory
{
public:
  
  typedef std::map< std::string, SourceTermBuilder* > BuildMap;
  typedef std::map< std::string, SourceTermBase*    > SourceMap;

  /** @brief obtain a reference to the SourceTermFactory. */
  static SourceTermFactory& self();

  //////////////////////////////////////////////////
  // Initialization/setup methods

  /** @brief  Grab input file parameters from the UPS file */
  void problemSetup( const ProblemSpecP & params );

  /** @brief  Schedule/do initialization of source terms */
  void sched_sourceInit( const LevelP& level, 
                         SchedulerP& sched );

  void sourceInit( const ProcessorGroup* ,
                   const PatchSubset* patches,
                   const MaterialSubset*,
                   DataWarehouse* old_dw,
                   DataWarehouse* new_dw );

  /////////////////////////////////////////////////////
  // Source term access methods

  /** @brief Register a source term on the specified transport equation.
   *
   *  @param eqnName The name of the transport equation to which we are adding the source term.
   *  @param builder The SourceTermBuilder object to build the SourceTerm object.
   */
  void register_source_term( const std::string name,
                             SourceTermBuilder* builder );


  /**
   *  @brief Retrieve a vector of pointers to all SourceTerm
   *  objects that have been assigned to the transport equation with
   *  the specified name.
   *
   *  @param eqnName The transport equation whose SourceTerm object we are retrieving
   */
  SourceTermBase& retrieve_source_term( const std::string name );

  /** @brief  Get a pointer to to the source term corresponding to
    *         particle-gas momentum coupling (see CoalModels/ParticleVelocity.{cc,h})
    *
    * If there is no particle momentum source, this method won't be called in the first place.
    */
  SourceTermBase* getParticleMomentumSource() {
    return d_particleGasMomentumSource;
  };

  /////////////////////////////////////////////
  // Get/set methods

  /* @brief Get all SourceTerm objects in a map */
  SourceMap& retrieve_all_sources(){
    return sources_; }; 

  /* @brief Set the ArchesLabel class so that SourceTermFactory can get a shared state */
  void setArchesLabel( ArchesLabel* fieldLabels ) {
    d_fieldLabels = fieldLabels;
    d_labelSet = true;
  }

private:

  BuildMap builders_;
  SourceMap sources_;

  ArchesLabel* d_fieldLabels;

  bool d_labelSet; ///< Boolean: has the ArchesLabel been set using setArchesLabel()?
  bool d_useParticleGasMomentumSource; ///< Bool: is there a source term for particle-gas momentum coupling?

  SourceTermBase* d_particleGasMomentumSource;

  SourceTermFactory();
  ~SourceTermFactory();
}; // class SourceTermFactory
}  //Namespace Uintah
#endif

