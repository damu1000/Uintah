#
# Makefile fragment for this subdirectory
# $Id$
#

# *** NOTE ***
# 
# Do not remove or modify the comment line:
#
# #[INSERT NEW ?????? HERE]
#
# It is required by the module maker to properly edit this file.
# if you want to edit this file by hand, see the "Create A New Module"
# documentation on how to do it correctly.

include $(SRCTOP)/scripts/smallso_prologue.mk

SRCDIR   := PSECommon/Modules/Matrix

SRCS     += \
	$(SRCDIR)/BldTransform.cc\
	$(SRCDIR)/EditMatrix.cc\
	$(SRCDIR)/ExtractSubmatrix.cc\
	$(SRCDIR)/MatMat.cc\
	$(SRCDIR)/MatSelectVec.cc\
	$(SRCDIR)/MatVec.cc\
	$(SRCDIR)/SolveMatrix.cc\
	$(SRCDIR)/VisualizeMatrix.cc\
	$(SRCDIR)/cConjGrad.cc\
	$(SRCDIR)/cPhase.cc\
#[INSERT NEW MODULE HERE]

PSELIBS := PSECore/Dataflow PSECore/Datatypes SCICore/Persistent \
	SCICore/Exceptions SCICore/Thread SCICore/Containers \
	SCICore/TclInterface SCICore/Geometry SCICore/Datatypes \
	SCICore/Util SCICore/Geom SCICore/TkExtensions
LIBS := $(TK_LIBRARY) $(GL_LIBS) -lm

include $(SRCTOP)/scripts/smallso_epilogue.mk

#
# $Log$
# Revision 1.2.2.1  2000/09/28 03:16:03  mcole
# merge trunk into FIELD_REDESIGN branch
#
# Revision 1.4  2000/06/08 22:46:27  moulding
# Added a comment note about not messing with the module maker comment lines,
# and how to edit this file by hand.
#
# Revision 1.3  2000/06/07 00:11:38  moulding
# made some modifications that will allow the module make to edit and add
# to this file
#
# Revision 1.2  2000/03/20 19:37:00  sparker
# Added VPATH support
#
# Revision 1.1  2000/03/17 09:27:08  sparker
# New makefile scheme: sub.mk instead of Makefile.in
# Use XML-based files for module repository
# Plus many other changes to make these two things work
#
#
