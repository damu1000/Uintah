//static char *id="@(#) $Id$";

/*
 *  GuiManager.cc: Client side (slave) manager of a pool of remote GUI
 *   connections.
 *
 *  This class keeps a dynamic array of connections for use by TCL variables
 *  needing to get their values from the Master.  These are kept in a pool.
 *
 *  Written by:
 *   Michelle Miller
 *   Department of Computer Science
 *   University of Utah
 *   May 1998
 *
 *  Copyright (C) 1998 SCI Group
 */

#include <TclInterface/GuiManager.h>
#include <Containers/Array1.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>	// needed for atoi

namespace SCICore {
namespace TclInterface {

GuiManager::GuiManager (char* hostname, char* portname) 
{
    base_port = atoi (portname);
    strcpy (host, hostname);
    strcat (host,".cs.utah.edu");
}

GuiManager::~GuiManager()
{ }

int
GuiManager::addConnection()
{
    // open one socket 
    int conn = requestConnect (base_port-1, host);
    if (conn == -1) {
            perror ("outgoing connection setup failed");
            return -1;
    }
    connect_pool.add(conn);
    return conn;
}

/*   Traverse array of sockets looking for an unused socket.  If no unused
 *   sockets, add another socket, lock it, and return to caller.
 */
int
GuiManager::getConnection()
{
    int sock;

#ifdef DEBUG
cerr << "attempting to lock, lock = " << &access << " pid " << getpid() << endl;
#endif

    access.lock();

#ifdef DEBUG
cerr << "GuiManager::getConnection() pid " << getpid() << " is locking"
     << endl;
#endif

    if (connect_pool.size() == 0) {
	sock = addConnection();
    } else {
	sock = connect_pool[connect_pool.size()-1];	// get last elem
    }

    connect_pool.remove(connect_pool.size()-1);	// take out of pool

#ifdef DEBUG
cerr << "GuiManager::getConnection() pid " << getpid() << " is unlocking"
     << endl;
#endif

    access.unlock();
    return sock;
}

/*  Return a connection to the pool.  */
void 
GuiManager::putConnection (int sock)
{
    access.lock();
    connect_pool.add(sock);
    access.unlock();
}

} // End namespace TclInterface
} // End namespace SCICore

//
// $Log$
// Revision 1.1  1999/07/27 16:57:13  mcq
// Initial commit
//
// Revision 1.1.1.1  1999/04/24 23:12:25  dav
// Import sources
//
//
