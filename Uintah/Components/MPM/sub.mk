#
# Makefile fragment for this subdirectory
# $Id$
#

include $(SRCTOP)/scripts/smallso_prologue.mk

SRCDIR   := Uintah/Components/MPM

SRCS     += $(SRCDIR)/SerialMPM.cc \
	$(SRCDIR)/BoundCond.cc

SUBDIRS := $(SRCDIR)/ConstitutiveModel $(SRCDIR)/Contact \
	$(SRCDIR)/GeometrySpecification $(SRCDIR)/Util

include $(SRCTOP)/scripts/recurse.mk

PSELIBS := Uintah/Interface Uintah/Grid Uintah/Parallel \
	Uintah/Exceptions SCICore/Exceptions SCICore/Thread \
	SCICore/Geometry
LIBS := $(XML_LIBRARY)

include $(SRCTOP)/scripts/smallso_epilogue.mk

#
# $Log$
# Revision 1.4  2000/04/12 22:59:03  sparker
# Working to make it compile
# Added xerces to link line
#
# Revision 1.3  2000/03/20 19:38:23  sparker
# Added VPATH support
#
# Revision 1.2  2000/03/20 17:17:05  sparker
# Made it compile.  There are now several #idef WONT_COMPILE_YET statements.
#
# Revision 1.1  2000/03/17 09:29:32  sparker
# New makefile scheme: sub.mk instead of Makefile.in
# Use XML-based files for module repository
# Plus many other changes to make these two things work
#
