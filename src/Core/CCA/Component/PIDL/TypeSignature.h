
/*
 *  TypeSignature.h: compact representation for a type.
 *                   This is the type name for now.  Someday it
 * 		     should become a uuid or something
 *  $Id$
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   July 1999
 *
 *  Copyright (C) 1999 SCI Group
 */

#ifndef Component_PIDL_TypeSignature_h
#define Component_PIDL_TypeSignature_h

#include <string>

namespace Component {
    namespace PIDL {
	typedef std::string TypeSignature;
    }
}

#endif
//
// $Log$
// Revision 1.1  1999/08/30 17:39:49  sparker
// Updates to configure script:
//  rebuild configure if configure.in changes (Bug #35)
//  Fixed rule for rebuilding Makefile from Makefile.in (Bug #36)
//  Rerun configure if configure changes (Bug #37)
//  Don't build Makefiles for modules that aren't --enabled (Bug #49)
//  Updated Makfiles to build sidl and Component if --enable-parallel
// Updates to sidl code to compile on linux
// Imported PIDL code
// Created top-level Component directory
// Added ProcessManager class - a simpler interface to fork/exec (not finished)
//
//
