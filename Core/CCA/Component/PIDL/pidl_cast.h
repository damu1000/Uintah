
/*
 *  pidl_cast.h: The equivalent of dynamic_cast for network based objects
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

#ifndef Component_PIDL_pidl_cast_h
#define Component_PIDL_pidl_cast_h

// In global namespace for now...
#include <Component/PIDL/Object.h>

template<class T>
T pidl_cast(const Component::PIDL::Object& obj)
{
    // Try the direct cast before we go remote
    T::proxytype* proxy=dynamic_cast<T::proxytype*>(obj);
    if(proxy)
	return proxy;

    Component::PIDL::ProxyBase* p=dynamic_cast<Component::PIDL::ProxyBase*>(obj);
    if(!p)
	return 0;

    // This is a remote procedure call.
    if(p->isa(T::type_signature()))
	return new T::proxytype(p->getReference());
    else
	return 0;
}

#endif
//
// $Log$
// Revision 1.1  1999/08/30 17:39:50  sparker
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
