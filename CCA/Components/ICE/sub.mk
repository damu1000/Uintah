# Makefile fragment for this subdirectory
include $(SCIRUN_SCRIPTS)/smallso_prologue.mk

SRCDIR   := Packages/Uintah/CCA/Components/ICE

SRCS       += \
       $(SRCDIR)/ICE.cc \
       $(SRCDIR)/AMRICE.cc \
       $(SRCDIR)/ICERF.cc \
       $(SRCDIR)/ICEDebug.cc \
       $(SRCDIR)/ICEMaterial.cc \
       $(SRCDIR)/Diffusion.cc \
       $(SRCDIR)/BoundaryCond.cc \
       $(SRCDIR)/GeometryObject2.cc \
       $(SRCDIR)/LODI2.cc \
       $(SRCDIR)/Turbulence.cc \
       $(SRCDIR)/SmagorinskyModel.cc \
       $(SRCDIR)/DynamicModel.cc \
       $(SRCDIR)/TurbulenceFactory.cc \
       $(SRCDIR)/impICE.cc \
       $(SRCDIR)/NG_NozzleBCs.cc \
       $(SRCDIR)/microSlipBCs.cc \
       $(SRCDIR)/customInitialize.cc   
       
SUBDIRS := $(SRCDIR)/EOS $(SRCDIR)/Advection

include $(SCIRUN_SCRIPTS)/recurse.mk          

PSELIBS := \
       Packages/Uintah/CCA/Ports                       \
       Packages/Uintah/Core/Grid                       \
       Packages/Uintah/Core/Math                       \
       Packages/Uintah/Core/Disclosure                 \
       Packages/Uintah/Core/ProblemSpec                \
       Packages/Uintah/Core/Parallel                   \
       Packages/Uintah/Core/Exceptions                 \
       Packages/Uintah/Core/Math                       \
       Packages/Uintah/Core/Labels                     \
       Core/Exceptions Core/Geometry                   \
       Core/Thread Core/Util

LIBS       := $(XML_LIBRARY) $(MPI_LIBRARY) $(M_LIBRARY)

include $(SCIRUN_SCRIPTS)/smallso_epilogue.mk


