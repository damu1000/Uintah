
/*
 *  PIDL.h: Include a bunch of PIDL files for external clients
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

#include <Component/PIDL/PIDL.h>
#include <Component/PIDL/GlobusError.h>
#include <Component/PIDL/Object_proxy.h>
#include <Component/PIDL/Wharehouse.h>
#include <Component/Exceptions/InternalError.h>
#include <globus_nexus.h>
#include <iostream>
#include <sstream>

using Component::Exceptions::InternalError;

static unsigned short port;
static char* host;

Wharehouse* PIDL::wharehouse;

static int approval_fn(void*, char* urlstring, globus_nexus_startpoint_t* sp)
{
    using namespace Component::PIDL;
    try {
	Wharehouse* wh=PIDL::getWharehouse();
	return wh->approval(urlstring, sp);
    } catch(const Component::Exceptions::Exception& e) {
	std::cerr << "Caught exception (" << e.message() << "): " << urlstring
		  << ", rejecting client (code=1005)\n";
	return 1005;
    } catch(...) {
	std::cerr << "Caught unknown exception: " << urlstring
		  << ", rejecting client (code=1006)\n";
	return 1006;
    }
}

void PIDL::initialize(int, char*[])
{
    if(!wharehouse){
	wharehouse=new Wharehouse;
	if(int gerr=globus_module_activate(GLOBUS_NEXUS_MODULE))
	    throw GlobusError("Unable to initialize nexus", gerr);
 	if(int gerr=globus_nexus_allow_attach(&port, &host, approval_fn, 0))
	    throw GlobusError("globus_nexus_allow_attach failed", gerr);
    }
}

Wharehouse* PIDL::getWharehouse()
{
    if(!wharehouse)
	throw InternalError("Wharehouse not initialized!\n");
    return wharehouse;
}


Object
PIDL::objectFrom(const URL& url)
{
    Reference ref(url);
    return new Object_proxy(ref);
}

Object
PIDL::objectFrom(const Reference& url)
{
    Reference ref(url);
    return objectFrom(ref);
}

void PIDL::serveObjects()
{
    if(!wharehouse)
	throw InternalError("Wharehouse not initialized!\n");
    wharehouse->run();
}

std::string PIDL::getBaseURL()
{
    if(!wharehouse)
	throw InternalError("Wharehouse not initialized!\n");
    std::ostringstream o;
    o << "x-nexus://" << host << ":" << port << "/";
    return o.str();
}
//
// $Log$
// Revision 1.1  1999/08/30 17:39:46  sparker
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
