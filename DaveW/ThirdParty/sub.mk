#
# Makefile fragment for this subdirectory
# $Id$
#

SRCDIR := DaveW/ThirdParty

SUBDIRS := $(SRCDIR)/NumRec $(SRCDIR)/Nrrd $(SRCDIR)/OldLinAlg

include $(OBJTOP_ABS)/scripts/recurse.mk

#
# $Log$
# Revision 1.1  2000/03/17 09:26:08  sparker
# New makefile scheme: sub.mk instead of Makefile.in
# Use XML-based files for module repository
# Plus many other changes to make these two things work
#
#
