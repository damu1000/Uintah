#ifndef UINTAH_COMPONENTS_SCHEDULERS_ONDEMANDDATAWAREHOUSE_H
#define UINTAH_COMPONENTS_SCHEDULERS_ONDEMANDDATAWAREHOUSE_H

#include <SCICore/Thread/Runnable.h>
#include <SCICore/Thread/CrowdMonitor.h>

#include <Uintah/Interface/DataWarehouse.h>
#include <Uintah/Grid/Grid.h>
#include <Uintah/Components/Schedulers/DWDatabase.h>

#include <map>

namespace Uintah {

class DataItem;
class TypeDescription;
class Region;

using SCICore::Thread::Thread;
using SCICore::Thread::Runnable;

class DataWarehouseMpiHandler : public Runnable {

public:
  DataWarehouseMpiHandler( DataWarehouse * dw );

  void run();

private:

  enum DataType { ReductionVar, GridVar, Quit };

  struct MpiDataRequest {
    int fromMpiRank; // Rank of process that wants to send info to this DW.
    int toMpiRank;   // Rank of the process being sent the data.
    int tag;         // Tag to use in the actual send.
    DataType type;// Type of data that will be sent
    char     varName[ 50 ];
    int      region;
  };

  static const int  MAX_BUFFER_SIZE;
  static const int  MPI_DATA_REQUEST_TAG;

  DataWarehouse * d_dw; // The DataWarehouse that this MpiHandler 
                        // is associated with.
};

/**************************************

  CLASS
        OnDemandDataWarehouse
   
	Short description...

  GENERAL INFORMATION

        OnDemandDataWarehouse.h

	Steven G. Parker
	Department of Computer Science
	University of Utah

	Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
  
	Copyright (C) 2000 SCI Group

  KEYWORDS
        On_Demand_Data_Warehouse

  DESCRIPTION
        Long description...
  
  WARNING
  
****************************************/

class OnDemandDataWarehouse : public DataWarehouse {
public:
   OnDemandDataWarehouse( int MpiRank, int MpiProcesses );
   virtual ~OnDemandDataWarehouse();
   
   virtual void setGrid(const GridP&);
   
   virtual void allocate(ReductionVariableBase&, const VarLabel*);
   virtual void get(ReductionVariableBase&, const VarLabel*) const;
   virtual void put(const ReductionVariableBase&, const VarLabel*);
   virtual void allocate(int numParticles, ParticleVariableBase&,
			 const VarLabel*, int matlIndex, const Region*);
   virtual void allocate(ParticleVariableBase&, const VarLabel*,
			 int matlIndex, const Region*);
   virtual void get(ParticleVariableBase&, const VarLabel*,
		    int matlIndex, const Region*, int numGhostCells) const;
   virtual void put(const ParticleVariableBase&, const VarLabel*,
		    int matlIndex, const Region*);
   virtual void allocate(NCVariableBase&, const VarLabel*,
			 int matlIndex, const Region*);
   virtual void get(NCVariableBase&, const VarLabel*,
		    int matlIndex, const Region*, int numGhostCells) const;
   virtual void put(const NCVariableBase&, const VarLabel*,
		    int matlIndex, const Region*);

   //////////
   // Insert Documentation Here:
   virtual void carryForward(const DataWarehouseP& from);

   //////////
   // When the Scheduler determines that another MPI node will be
   // creating a piece of data (ie: a sibling DataWarehouse will have
   // this data), it uses this procedure to let this DataWarehouse
   // know which mpiNode has the data so that if this DataWarehouse
   // needs the data, it will know who to ask for it.
   virtual void registerOwnership( const VarLabel * label,
				   const Region   * region,
				         int        mpiNode );
   //////////
   // Searches through the list containing which DataWarehouse's
   // have which data to find the mpiNode that the requested
   // variable (with materialIndex, and in the given region) is on.
   virtual int findMpiNode( const VarLabel * label,
			    const Region   * region );
private:

   struct dataLocation {
      const Region   * region;
            int        mpiNode;
   };

   struct ReductionRecord {
      ReductionVariableBase* var;
      ReductionRecord(ReductionVariableBase*);
   };

   typedef std::vector<dataLocation*> variableListType;
   typedef std::map<const VarLabel*, ReductionRecord*, VarLabel::Compare> reductionDBtype;
   typedef std::map<const VarLabel*, variableListType*, VarLabel::Compare> dataLocationDBtype;

   DWDatabase<NCVariableBase>       d_ncDB;
   DWDatabase<ParticleVariableBase> d_particleDB;
   reductionDBtype                  d_reductionDB;
   dataLocationDBtype               d_dataLocation;

   //////////
   // Insert Documentation Here:
   mutable SCICore::Thread::CrowdMonitor  d_lock;
   bool                                   d_allowCreation;
   GridP                                  d_grid;
   
   const VarLabel           * d_positionLabel;

   Thread                   * d_thread;
   DataWarehouseMpiHandler  * d_worker;
};

} // end namespace Uintah

//
// $Log$
// Revision 1.15  2000/05/05 06:42:43  dav
// Added some _hopefully_ good code mods as I work to get the MPI stuff to work.
//
// Revision 1.14  2000/05/02 17:54:29  sparker
// Implemented more of SerialMPM
//
// Revision 1.13  2000/05/02 06:07:16  sparker
// Implemented more of DataWarehouse and SerialMPM
//
// Revision 1.12  2000/04/28 07:35:34  sparker
// Started implementation of DataWarehouse
// MPM particle initialization now works
//
// Revision 1.11  2000/04/27 23:18:48  sparker
// Added problem initialization for MPM
//
// Revision 1.10  2000/04/26 06:48:33  sparker
// Streamlined namespaces
//
// Revision 1.9  2000/04/24 15:17:01  sparker
// Fixed unresolved symbols
//
// Revision 1.8  2000/04/20 18:56:26  sparker
// Updates to MPM
//
// Revision 1.7  2000/04/19 21:20:03  dav
// more MPI stuff
//
// Revision 1.6  2000/04/19 05:26:11  sparker
// Implemented new problemSetup/initialization phases
// Simplified DataWarehouse interface (not finished yet)
// Made MPM get through problemSetup, but still not finished
//
// Revision 1.5  2000/04/13 06:50:57  sparker
// More implementation to get this to work
//
// Revision 1.4  2000/03/22 00:36:37  sparker
// Added new version of getRegionData
//
// Revision 1.3  2000/03/17 01:03:17  dav
// Added some cocoon stuff, fixed some namespace stuff, etc
//
//

#endif
