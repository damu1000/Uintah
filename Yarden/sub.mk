#
# Makefile fragment for this subdirectory
# $Id$
#

include $(OBJTOP_ABS)/scripts/largeso_prologue.mk

SRCDIR := Yarden

SUBDIRS := $(SRCDIR)/GUI $(SRCDIR)/Datatypes \
	$(SRCDIR)/Modules

include $(OBJTOP_ABS)/scripts/recurse.mk

PSELIBS := PSECore SCICore
LIBS := $(TK_LIBRARY) $(GL_LIBS) -lm

include $(OBJTOP_ABS)/scripts/largeso_epilogue.mk

#
# $Log$
# Revision 1.1  2000/03/17 09:30:24  sparker
# New makefile scheme: sub.mk instead of Makefile.in
# Use XML-based files for module repository
# Plus many other changes to make these two things work
#
#
