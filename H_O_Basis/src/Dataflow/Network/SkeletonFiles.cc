/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2004 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


/* SkeletonFiles.cc */

namespace SCIRun {

char component_skeleton[] = \
"/*\n"
" *  %s.cc:\n" /* component name */
" *\n"
" *  Written by:\n"
" *   %s\n" /* author name */
" *   %s\n"   /* today's date */
" *\n"
" */\n"
"\n"
"#include <Dataflow/Network/Module.h>\n"
"#include <Core/Malloc/Allocator.h>\n"
"\n"
"#include <Dataflow/share/share.h>\n"
"\n"
"namespace %s {\n" /* package name */
"\n"
"using namespace SCIRun;\n"
"\n"
"class PSECORESHARE %s : public Module {\n" /* component name */
"" 
"public:\n"
"  %s(GuiContext*);\n" /* component name */
"\n"
"  virtual ~%s();\n" /* component name */
"\n"
"  virtual void execute();\n"
"\n"
"  virtual void tcl_command(GuiArgs&, void*);\n"
"};\n"
"\n"
"\n"
"DECLARE_MAKER(%s)\n" /* component name */
"%s::%s(GuiContext* ctx)\n" /* component name, component name */
"  : Module(\"%s\", ctx, Source, \"%s\", \"%s\")\n" /* comp, cat, pack */
"{\n"
"}\n"
"\n"
"%s::~%s()" /* component name, component name */
"{\n"
"}\n"
"\n"
"void\n %s::execute()" /* component name */
"{\n"
"}\n"
"\n"
"void\n %s::tcl_command(GuiArgs& args, void* userdata)\n" /* component name */
"{\n"
"  Module::tcl_command(args, userdata);\n"
"}\n"
"\n"
"} // End namespace %s\n" /* package name */
"\n"
"\n";

char gui_skeleton[] = \
"itcl_class %s_%s_%s {\n" /* package name, category name, component name */
"    inherit Module\n"
"    constructor {config} {\n"
"        set name %s\n" /* component name */
"        set_defaults\n"
"    }\n"
"\n"
"    method set_defaults {} {\n"
"    }\n"
"\n"
"    method ui {} {\n"
"        set w .ui[modname]\n"
"        if {[winfo exists $w]} {\n"
"            raise $w\n"
"            return\n"
"        }\n"
"        toplevel $w\n"
"        label $w.row1 -text \"This GUI was"
" auto-generated by the Component Wizard.\"\n"
"        label $w.row2 -text {edit the file \"%s\" to modify it.}\n" /*file*/
"        pack $w.row1 $w.row2 -side top -padx 10 -pady 10\n"
"    }\n"
"}\n"
"\n"
"\n";

char dllentry_skeleton[] = \
"/* DllEntry.cc */\n"
"\n"
"#ifdef _WIN32\n"
"\n"
"#include <afxwin.h>\n"
"#include <stdio.h>\n"
"\n"
"BOOL APIENTRY DllMain(HANDLE hModule, \n"
"                      DWORD  ul_reason_for_call, \n"
"                      LPVOID lpReserved)\n"
"{\n"
"#ifdef DEBUG\n"
"  char reason[100]=\"\\0\";\n"
"  printf(\"\\n*** %%sd.dll is initializing {%%s,%%d} ***\\n\",__FILE__,__LINE__);\n" /* package name */
"  printf(\"*** hModule = %%d ***\\n\",hModule);\n"
"  switch (ul_reason_for_call){\n"
"    case DLL_PROCESS_ATTACH:sprintf(reason,\"DLL_PROCESS_ATTACH\"); break;\n"
"    case DLL_THREAD_ATTACH:sprintf(reason,\"DLL_THREAD_ATTACH\"); break;\n"
"    case DLL_THREAD_DETACH:sprintf(reason,\"DLL_THREAD_DETACH\"); break;\n"
"    case DLL_PROCESS_DETACH:sprintf(reason,\"DLL_PROCESS_DETACH\"); break;\n"
"  }\n"
"  printf(\"*** ul_reason_for_call = %%s ***\\n\",reason);\n"
"#endif\n"
"  return TRUE;\n"
"}\n"
"\n"
"#endif\n"
"\n"
"\n";

char share_skeleton[] = \
"/* share.h */\n"
"\n"
"#undef %sSHARE\n" /* package name */
"\n"
"#ifdef _WIN32\n"
"  #if defined(BUILD_%s)\n" /* package name */
"    #define %sSHARE __declspec(dllexport)\n" /* package name */
"  #else\n"
"    #define %sSHARE __declspec(dllimport)\n" /* package name */
"  #endif\n" 
"#else\n" 
"  #define %sSHARE\n" /* package name */
"#endif\n"
"\n"
"\n";

char package_submk_skeleton[] = \
"\n"
"SRCDIR := Packages/%s\n" /* package name */
"\n"
"SUBDIRS := \\\n"
"        $(SRCDIR)/Core \\\n"
"        $(SRCDIR)/Dataflow \\\n"
"\n"
"include $(SCIRUN_SCRIPTS)/recurse.mk\n"
"\n"
"\n";

char dataflow_submk_skeleton[] = \
"include $(SCIRUN_SCRIPTS)/largeso_prologue.mk\n"
"\n"
"SRCDIR := %sDataflow\n" /* package dir */
"\n"
"SUBDIRS := \\\n"
"        $(SRCDIR)/GUI \\\n"
"        $(SRCDIR)/Modules \\\n"
"\n"
"include $(SCIRUN_SCRIPTS)/recurse.mk\n"
"\n"
"PSELIBS := \n"
"LIBS := $(TK_LIBRARY) $(GL_LIBRARY) $(M_LIBRARY)\n"
"\n"
"include $(SCIRUN_SCRIPTS)/largeso_epilogue.mk\n"
"\n"
"\n";

char core_submk_skeleton[] = \
"include $(SCIRUN_SCRIPTS)/largeso_prologue.mk\n"
"\n"
"SRCDIR := %sCore\n" /* package dir */
"\n"
"SUBDIRS := \\\n"
"        $(SRCDIR)/Datatypes \\\n"
"\n"
"include $(SCIRUN_SCRIPTS)/recurse.mk\n"
"\n"
"PSELIBS := \n"
"LIBS := $(TK_LIBRARY) $(GL_LIBRARY) $(M_LIBRARY)\n"
"\n"
"include $(SCIRUN_SCRIPTS)/largeso_epilogue.mk\n"
"\n"
"\n";

char modules_submk_skeleton[] = \
"# *** NOTE ***\n"
"#\n"
"# Do not remove or modify the comment line:\n"
"#\n"
"# #[INSERT NEW ?????? HERE]\n"
"#\n"
"# It is required by the Component Wizard to properly edit this file.\n"
"# if you want to edit this file by hand, see the \"Create A New Component\"\n"
"# documentation on how to do it correctly.\n"
"\n"
"SRCDIR := %sDataflow/Modules\n" /* package dir */
"\n"
"SUBDIRS := \\\n"
"#[INSERT NEW CATEGORY DIR HERE]\n"
"\n"
"include $(SCIRUN_SCRIPTS)/recurse.mk\n"
"\n"
"\n";

char category_submk_skeleton[] = \
"# *** NOTE ***\n"
"#\n"
"# Do not remove or modify the comment line:\n"
"#\n"
"# #[INSERT NEW ?????? HERE]\n"
"#\n"
"# It is required by the Component Wizard to properly edit this file.\n"
"# if you want to edit this file by hand, see the \"Create A New Component\"\n"
"# documentation on how to do it correctly.\n"
"\n"
"include $(SCIRUN_SCRIPTS)/smallso_prologue.mk\n"
"\n"
"SRCDIR   := %sDataflow/Modules/%s\n" /* package dir, category name */
"\n"
"SRCS     += \\\n"
"#[INSERT NEW CODE FILE HERE]\n"
"\n"
"PSELIBS := Core/Datatypes Dataflow/Network Dataflow/Ports \\\n"
"        Core/Persistent Core/Containers Core/Util \\\n"
"        Core/Exceptions Core/Thread Core/GuiInterface \\\n"
"        Core/Geom Core/Datatypes Core/Geometry \\\n"
"        Core/GeomInterface Core/TkExtensions\n"
"LIBS := $(TK_LIBRARY) $(GL_LIBRARY) $(M_LIBRARY)\n"
"\n"
"include $(SCIRUN_SCRIPTS)/smallso_epilogue.mk\n"
"\n"
"\n";

char datatypes_submk_skeleton[] = \
"# *** NOTE ***\n"
"#\n"
"# Do not remove or modify the comment line:\n"
"#\n"
"# #[INSERT NEW ?????? HERE]\n"
"#\n"
"# It is required by the Component Wizard to properly edit this file.\n"
"# if you want to edit this file by hand, see the \"Create A New Component\"\n"
"# documentation on how to do it correctly.\n"
"\n"
"include $(SCIRUN_SCRIPTS)/smallso_prologue.mk\n"
"\n"
"SRCDIR   := %sCore/Datatypes\n" /* package dir */
"\n"
"SRCS     += \\\n"
"#[INSERT NEW CODE FILE HERE]\n"
"\n"
"PSELIBS :=\n"
"LIBS :=\n"
"\n"
"include $(SCIRUN_SCRIPTS)/smallso_epilogue.mk"
"\n"
"\n";

char gui_submk_skeleton[] = \
"# *** NOTE ***\n"
"#\n"
"# Do not remove or modify the comment line:\n"
"#\n"
"# #[INSERT NEW ?????? HERE]\n"
"#\n"
"# It is required by the Component Wizard to properly edit this file.\n"
"# if you want to edit this file by hand, see the \"Create A New Component\"\n"
"# documentation on how to do it correctly.\n"
"\n"
"SRCDIR := %sDataflow/GUI\n" /* package dir */
"\n"
"ALLTARGETS := $(ALLTARGETS) $(SRCDIR)/tclIndex\n"
"\n"
"$(SRCDIR)/tclIndex: \\\n"
"#[INSERT NEW TCL FILE HERE]\n"
"\t$(OBJTOP)/createTclIndex "
"$(SRCTOP)/%sDataflow/GUI\n" /* package dir */
"\n"
"CLEANPROGS := $(CLEANPROGS) $(SRCDIR)/tclIndex\n"
"\n"
"\n";

} // End namespace SCIRun

