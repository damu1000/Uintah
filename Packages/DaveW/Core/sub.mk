#Makefile fragment for the Packages/DaveW/Core directory

include $(OBJTOP_ABS)/scripts/largeso_prologue.mk

SRCDIR := Packages/DaveW/Core
SUBDIRS := \
	$(SRCDIR)/Datatypes \
	$(SRCDIR)/convert \

include $(OBJTOP_ABS)/scripts/recurse.mk

PSELIBS := 
LIBS := $(TK_LIBRARY) $(GL_LIBS) -lm

include $(OBJTOP_ABS)/scripts/largeso_epilogue.mk
