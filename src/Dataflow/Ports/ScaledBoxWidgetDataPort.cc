//static char *id="@(#) $Id$";

/*
 *  ScaledBoxWidgetDataPort.cc
 *
 *  Written by:
 *   David Weinstein
 *   Department of Computer Science
 *   University of Utah
 *   July 1994
 *
 *  Copyright (C) 1994 SCI Group
 */

#include <CommonDatatypes/ScaledBoxWidgetDataPort.h>

namespace PSECommon {
namespace CommonDatatypes {

clString SimpleIPort<ScaledBoxWidgetDataHandle>::port_type("Data");
clString SimpleIPort<ScaledBoxWidgetDataHandle>::port_color("black");

} // End namespace CommonDatatypes
} // End namespace PSECommon

//
// $Log$
// Revision 1.1  1999/07/27 16:55:49  mcq
// Initial commit
//
// Revision 1.1.1.1  1999/04/24 23:12:50  dav
// Import sources
//
//
