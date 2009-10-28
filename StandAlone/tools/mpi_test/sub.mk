# 
# 
# The MIT License
# 
# Copyright (c) 1997-2009 Center for the Simulation of Accidental Fires and 
# Explosions (CSAFE), and  Scientific Computing and Imaging Institute (SCI), 
# University of Utah.
# 
# License for the specific language governing rights and limitations under
# Permission is hereby granted, free of charge, to any person obtaining a 
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation 
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included 
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.
# 
# 
# 
# 
# Makefile fragment for this subdirectory

SRCDIR := StandAlone/tools/mpi_test

##############################################
# async_mpi_test.cc

SRCS    := $(SRCDIR)/async_mpi_test.cc
PROGRAM := $(SRCDIR)/async_mpi_test

ifeq ($(IS_STATIC_BUILD),yes)

  PSELIBS := $(CORE_STATIC_PSELIBS)

else # Non-static build

  PSELIBS := \
        Core/Thread \
        Core/Parallel
endif

ifeq ($(IS_STATIC_BUILD),yes)
  LIBS := \
        $(TEEM_LIBRARY) \
        $(XML2_LIBRARY) \
        $(Z_LIBRARY) \
        $(THREAD_LIBRARY) \
        $(F_LIBRARY) \
        $(PETSC_LIBRARY) \
        $(HYPRE_LIBRARY) \
        $(BLAS_LIBRARY) \
        $(LAPACK_LIBRARY) \
        $(MPI_LIBRARY) \
        $(X_LIBRARY) \
        $(M_LIBRARY)
else
  LIBS    := $(XML2_LIBRARY) $(M_LIBRARY) $(MPI_LIBRARY) $(F_LIBRARY)
endif

include $(SCIRUN_SCRIPTS)/program.mk

##############################################
# mpi_test

PROGRAM := $(SRCDIR)/mpi_test
SRCS    := $(SRCDIR)/mpi_test.cc

ifeq ($(IS_STATIC_BUILD),yes)
  LIBS := \
        $(TEEM_LIBRARY) \
        $(XML2_LIBRARY) \
        $(Z_LIBRARY) \
        $(THREAD_LIBRARY) \
        $(F_LIBRARY) \
        $(PETSC_LIBRARY) \
        $(HYPRE_LIBRARY) \
        $(BLAS_LIBRARY) \
        $(LAPACK_LIBRARY) \
        $(MPI_LIBRARY) \
        $(X_LIBRARY) \
        $(M_LIBRARY)
else
  LIBS    := $(MPI_LIBRARY) $(M_LIBRARY) 
endif

ifeq ($(IS_STATIC_BUILD),yes)

  PSELIBS := $(CORE_STATIC_PSELIBS)

else # Non-static build

  PSELIBS := \
	Core/Containers \
	Core/Exceptions \
	Core/Util       \
	Core/Thread
endif

include $(SCIRUN_SCRIPTS)/program.mk

