#
# Makefile fragment for this subdirectory
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

SRCDIR   := PSECommon/Modules/Fields

SRCS     += \
	$(SRCDIR)/Downsample.cc\
	$(SRCDIR)/ExtractSurfs.cc\
        $(SRCDIR)/FieldFilter.cc\
	$(SRCDIR)/FieldGainCorrect.cc\
	$(SRCDIR)/FieldMedianFilter.cc\
	$(SRCDIR)/FieldRGAug.cc\
	$(SRCDIR)/FieldSeed.cc\
	$(SRCDIR)/GenField.cc\
	$(SRCDIR)/Gradient.cc\
	$(SRCDIR)/GradientMagnitude.cc\
	$(SRCDIR)/LocalMinMax.cc\
	$(SRCDIR)/MergeTensor.cc\
	$(SRCDIR)/OpenGL_Ex.cc\
	$(SRCDIR)/SFRGfile.cc\
	$(SRCDIR)/ShowGeometry.cc\
	$(SRCDIR)/TracePath.cc\
	$(SRCDIR)/TrainSeg2.cc\
	$(SRCDIR)/TrainSegment.cc\
	$(SRCDIR)/TransformField.cc\
	$(SRCDIR)/ScalarFieldProbe.cc\
	$(SRCDIR)/GenVectorField.cc\
	$(SRCDIR)/GenScalarField.cc\
#[INSERT NEW CODE FILE HERE]

#	$(SRCDIR)/ClipField.cc\


PSELIBS := PSECore/Dataflow PSECore/Datatypes PSECore/Widgets \
	SCICore/Persistent SCICore/Exceptions SCICore/Thread \
	SCICore/Containers SCICore/TclInterface SCICore/Geom \
	SCICore/Datatypes SCICore/Geometry SCICore/TkExtensions \
	SCICore/Math
LIBS := $(TK_LIBRARY) $(GL_LIBS) $(FLEX_LIBS) -lm

include $(SRCTOP)/scripts/smallso_epilogue.mk

