#ifndef UINTAH_HOMEBREW_DataArchiver_H
#define UINTAH_HOMEBREW_DataArchiver_H

#include <Packages/Uintah/CCA/Ports/Output.h>
#include <Packages/Uintah/Core/Parallel/UintahParallelComponent.h>
#include <Core/OS/Dir.h>
#include <Core/Containers/ConsecutiveRangeSet.h>

namespace Uintah {
   
   /**************************************
     
     CLASS
       DataArchiver
      
       Short Description...
      
     GENERAL INFORMATION
      
       DataArchiver.h
      
       Steven G. Parker
       Department of Computer Science
       University of Utah
      
       Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
      
       Copyright (C) 2000 SCI Group
      
     KEYWORDS
       DataArchiver
      
     DESCRIPTION
       Long description...
      
     WARNING
      
     ****************************************/
    
   class DataArchiver : public Output, public UintahParallelComponent {
   public:
      DataArchiver(const ProcessorGroup* myworld);
      virtual ~DataArchiver();

      //////////
      // Insert Documentation Here:
      virtual void problemSetup(const ProblemSpecP& params);

      //////////
      // Call this when restarting from a checkpoint after calling
      // problemSetup.  This will copy timestep directories and dat
      // files up to the specified timestep from restartFromDir and
      // will set time and timestep variables appropriately to
      // continue smoothly from that timestep.
      virtual void restartSetup(Dir& restartFromDir, int timestep,
				double time, bool removeOldDir);
      //////////
      // Insert Documentation Here:
      virtual void finalizeTimestep(double t, double delt, const LevelP&,
				    SchedulerP&,
				    DataWarehouseP&);
      //////////
      // Insert Documentation Here:
      virtual const std::string getOutputLocation() const;

      //////////
      // Insert Documentation Here:
      void output(const ProcessorGroup*,
		  const Patch* patch,
		  DataWarehouseP& old_dw,
		  DataWarehouseP& new_dw,
		  Dir* p_dir, int timestep,
		  std::pair<const VarLabel*, int>
		  /* combined as a pair because Tasks take up to 3 extra
		     args and I'm too lazy to overload for a 4 arg Task */);

      // Method to output reduction variables to a single file
      void outputReduction(const ProcessorGroup*,
			   DataWarehouseP& old_dw,
			   DataWarehouseP& new_dw,
			   double time);

      // This calls output for all of the checkpoint reduction variables
      // which will end up in globals.xml / globals.data -- in this way,
      // all this data will be output by one process avoiding conflicts.
      void outputCheckpointReduction(const ProcessorGroup* world,
				     DataWarehouseP& old_dw,
				     DataWarehouseP& new_dw,
				     int timestep);
      //////////
      // Get the current time step
      virtual int getCurrentTimestep()
      { return d_currentTimestep; }

      //////////
      // Returns true if the last timestep was one
      // in which data was output.
      virtual bool wasOutputTimestep()
      { return d_wasOutputTimestep; }

      //////////
      // Get the directory of the current time step for outputting info.
      virtual const std::string& getLastTimestepOutputLocation() const
      { return d_lastTimestepLocation; }
   private:
      struct SaveNameItem {
	 std::string labelName;
         ConsecutiveRangeSet matls;
      };
      struct SaveItem {
	 const VarLabel* label;
         ConsecutiveRangeSet matls;
      };

      void initSaveLabels(SchedulerP& sched);
      void initCheckpoints(SchedulerP& sched);
     
      // helper for finalizeTimestep
      void outputTimestep(Dir& dir, std::vector<SaveItem>& saveLabels,
			  double time, double delt,
			  const LevelP& level, SchedulerP& sched,
			  DataWarehouseP& new_dw,
			  std::string* pTimestepDir /* passed back */,
			  bool hasGlobals = false);

      // helper for problemSetup
      void createIndexXML(Dir& dir);

      // helpers for restartSetup
      void copyTimesteps(Dir& fromDir, Dir& toDir, int maxTimestep,
			 bool removeOld, bool areCheckpoints = false);
      void copyDatFiles(Dir& fromDir, Dir& toDir, int maxTimestep,
			bool removeOld);
   
      // add saved global (reduction) variables to index.xml
      void indexAddGlobals();

      std::string d_filebase;
      double d_outputInterval;
      double d_nextOutputTime;
      int d_currentTimestep;
      Dir d_dir;
      bool d_writeMeta;
      std::string d_lastTimestepLocation;
      bool d_wasOutputTimestep;

      bool d_wereSavesAndCheckpointsInitialized;      
      // d_saveLabelNames is a temporary list containing VarLabel
      // names to be saved and the materials to save them for.  The
      // information will be basically transferred to d_saveLabels or
      // d_saveReductionLabels after mapping VarLabel names to their
      // actual VarLabel*'s.
      std::list< SaveNameItem > d_saveLabelNames;
      std::vector< SaveItem > d_saveLabels;
      std::vector< SaveItem > d_saveReductionLabels;

      // d_checkpointLabelNames is a temporary list containing
      // the names of labels to save when checkpointing
      std::vector< SaveItem > d_checkpointLabels;
      std::vector< SaveItem > d_checkpointReductionLabels;
      double d_checkpointInterval;
      int d_checkpointCycle;

      Dir d_checkpointsDir;
      list<std::string> d_checkpointTimestepDirs;
      double d_nextCheckpointTime;

      DataArchiver(const DataArchiver&);
      DataArchiver& operator=(const DataArchiver&);
      
   };

} // End namespace Uintah

#endif
