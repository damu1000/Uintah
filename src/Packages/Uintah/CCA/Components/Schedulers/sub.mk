# Makefile fragment for this subdirectory

include $(SCIRUN_SCRIPTS)/smallso_prologue.mk

SRCDIR   := Packages/Uintah/CCA/Components/Schedulers

SRCS += \
	$(SRCDIR)/DetailedTasks.cc \
	$(SRCDIR)/MemoryLog.cc \
	$(SRCDIR)/CommRecMPI.cc \
	$(SRCDIR)/MPIScheduler.cc \
	$(SRCDIR)/MessageLog.cc \
	$(SRCDIR)/MixedScheduler.cc \
	$(SRCDIR)/OnDemandDataWarehouse.cc \
	$(SRCDIR)/Relocate.cc \
	$(SRCDIR)/SchedulerCommon.cc \
	$(SRCDIR)/SchedulerFactory.cc \
	$(SRCDIR)/SendState.cc \
	$(SRCDIR)/SingleProcessorScheduler.cc \
	$(SRCDIR)/TaskGraph.cc \
	$(SRCDIR)/ThreadPool.cc \
	$(SRCDIR)/GhostOffsetVarMap.cc \
	$(SRCDIR)/DependencyException.cc \
	$(SRCDIR)/IncorrectAllocation.cc \
	$(SRCDIR)/Util.cc \
	$(SRCDIR)/templates.cc

PSELIBS := \
	Packages/Uintah/CCA/Components/ProblemSpecification \
	Packages/Uintah/Core/Grid        \
	Packages/Uintah/Core/Util        \
	Packages/Uintah/Core/Disclosure  \
	Packages/Uintah/Core/ProblemSpec \
	Packages/Uintah/CCA/Ports        \
	Packages/Uintah/Core/Parallel    \
	Packages/Uintah/Core/Exceptions  \
	Core/Containers                  \
	Core/Exceptions                  \
	Core/Geometry                    \
	Core/OS                          \
	Core/Thread                      \
	Core/Util

LIBS := $(XML2_LIBRARY) $(TAU_LIBRARY) $(MPI_LIBRARY) $(VAMPIR_LIBRARY) $(PERFEX_LIBRARY)

include $(SCIRUN_SCRIPTS)/smallso_epilogue.mk

