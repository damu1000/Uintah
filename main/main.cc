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


/*
 *  main.cc: 
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   Feb. 1994
 *
 *  Copyright (C) 1999 U of U
 */

#include <main/sci_version.h>
#include <Dataflow/Network/Network.h>
#include <Dataflow/Network/NetworkEditor.h>
#include <Dataflow/Network/PackageDB.h>
#include <Dataflow/Network/Scheduler.h>
#include <Core/Containers/StringUtil.h>
#include <Core/TCLThread/TCLThread.h>
#include <Core/GuiInterface/TCLInterface.h>
#include <Core/Util/Environment.h>
#include <Core/Util/sci_system.h>
#include <Core/Comm/StringSocket.h>
#include <Core/Thread/Thread.h>
#include <Core/Thread/Time.h>
#include <Core/Geom/ShaderProgramARB.h>


#include <Core/Services/ServiceLog.h>
#include <Core/Services/ServiceDB.h>
#include <Core/Services/ServiceManager.h>
#include <Core/SystemCall/SystemCallManager.h>

#include <sys/stat.h>
#include <fcntl.h>

#if defined(__APPLE__)
#  include <Core/Datatypes/MacForceLoad.h>
   namespace SCIRun {
      extern void macImportExportForceLoad();
   }
#endif

#include <string>
#include <iostream>
using std::cout;

#ifdef _WIN32
#  include <windows.h>
#endif

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#pragma set woff 1424
#endif


using namespace SCIRun;

void
usage()
{
  cout << "Usage: scirun [args] [net_file] [session_file]\n";
  cout << "    [-]-r[egression]    : regression test a network\n";
  cout << "    [-]-s[erver] [PORT] : start a TCL server on port number PORT\n";
  cout << "    [-]-e[xecute]       : executes the given network on startup\n";
  cout << "    [-]-v[ersion]       : prints out version information\n";
  cout << "    [-]-h[elp]          : prints usage information\n";
  cout << "    [-]-p[ort] [PORT]   : start remote services port on port number PORT\n";
  //  cout << "    [-]-eai             : enable external applications interface\n";
  cout << "    [--nosplash]        : disable the splash screen\n";
  cout << "    net_file            : SCIRun Network Input File\n";
  cout << "    session_file        : PowerApp Session File\n";
  exit( 0 );
}


// Parse the supported command-line arugments.
// Returns the argument # of the .net file
int
parse_args( int argc, char *argv[] )
{
  int found = 0;
  bool powerapp = false;
  int cnt = 1;
  while (cnt < argc)
  {
    string arg( argv[ cnt ] );
    if( ( arg == "--version" ) || ( arg == "-version" )
	|| ( arg == "-v" ) || ( arg == "--v" ) )
    {
      cout << "Version: " << SCIRUN_VERSION << "\n";
      exit( 0 );
    }
    else if ( ( arg == "--help" ) || ( arg == "-help" ) ||
	      ( arg == "-h" ) ||  ( arg == "--h" ) )
    {
      usage();
    }
    else if ( ( arg == "--execute" ) || ( arg == "-execute" ) ||
	      ( arg == "-e" ) ||  ( arg == "--e" ) )
    {
      sci_putenv("SCIRUN_EXECUTE_ON_STARTUP","1");
    }
    else if ( ( arg == "--eai" ) || ( arg == "-eai" ))
    {
      sci_putenv("SCIRUN_EXTERNAL_APPLICATION_INTERFACE","1");
    }
    else if ( ( arg == "--regression" ) || ( arg == "-regression" ) ||
	      ( arg == "-r" ) ||  ( arg == "--r" ) )
    {
      sci_putenv("SCI_REGRESSION_TESTING","1");
    }
    else if ( arg == "--nosplash" )
    {
      sci_putenv("SCIRUN_NOSPLASH", "1");
    }
    else if ( ( arg == "--server" ) || ( arg == "-server" ) ||
	      ( arg == "-s" ) ||  ( arg == "--s" ) )
    {
      int port;
      if ((cnt+1 < argc) && string_to_int(argv[cnt+1], port)) {
	if (port < 1024 || port > 65535) {
	  cerr << "Server port must be in range 1024-65535\n";
	  exit(0);
	}
	cnt++;
      } else {
	port = 0;
      }
		sci_putenv("SCIRUN_SERVER_PORT",to_string(port));
    }    
	else if ( ( arg == "--port" ) || ( arg == "-port" ) ||
	      ( arg == "-p" ) ||  ( arg == "--p" ) )
    {
      int port;
      if ((cnt+1 < argc) && string_to_int(argv[cnt+1], port)) {
	if (port < 1024 || port > 65535) {
	  cerr << "Server port must be in range 1024-65535\n";
	  exit(0);
	}
	cnt++;
      } else {
	port = 0;
      }
      sci_putenv("SCIRUN_SERVICE_PORT",to_string(port));
      sci_putenv("SCIRUN_EXTERNAL_APPLICATION_INTERFACE","1");
    }   
	else
    {
      struct stat buf;
      if (stat(arg.c_str(),&buf) < 0)
      {
	std::cerr << "Couldn't find net file " << arg
		  << ".\nNo such file or directory.  Exiting." << std::endl;
	//exit(0);
      }

      if (found && !powerapp)
      {
	usage();
      }

      // determine if it is a PowerApp
      if(strstr(arg.c_str(),".app")) {
	powerapp = true;
	found = cnt;
      }
      else if(!powerapp) {
	found = cnt;
      }
    }
    cnt++;
  }
  return found;
}


class RegressionKiller : public Runnable
{
public:
  void run()
  {
    int tmp, seconds = 300;
    const char *timeout = sci_getenv("SCIRUN_REGRESSION_TESTING_TIMEOUT");
    if (timeout && string_to_int(timeout, tmp)) {
      seconds = tmp;
    }
    Time::waitFor((double)seconds);
    cout << "\n";
    cout << "main.cc: RegressionKiller: Regression test timed out\n";
    cout << "         after " << seconds << " seconds.  Killing SCIRun.\n\n";
    Thread::exitAll(1);
  }
};



class TCLSocketRunner : public Runnable
{
private:
  TCLInterface *gui_;
  StringSocket *transmitter_;
public:
  TCLSocketRunner(TCLInterface *gui, StringSocket *dt) : 
    gui_(gui), transmitter_(dt) {}
  void run()
  {
    string buffer;
    while (1) {
      buffer.append(transmitter_->getMessage());
      if (gui_->complete_command(buffer)) {
	buffer = gui_->eval(buffer);
	if (!buffer.empty()) buffer.append("\n");
	transmitter_->putMessage(buffer+"scirun> ");
	buffer.clear();
      } else {
	transmitter_->putMessage("scirun>> ");
      }
    }
  }
};


int
main(int argc, char *argv[], char **environment) {

  // Setup the SCIRun key/value environment
  create_sci_environment(environment, 0);
  sci_putenv("SCIRUN_VERSION", SCIRUN_VERSION);
  sci_putenv("SCIRUN_RCFILE_SUBVERSION", SCIRUN_RCFILE_SUBVERSION);

  // Parse the command line arguments to find a network to execute
  const int startnetno = parse_args( argc, argv );

  // Always switch on this option
  // It is needed for running external applications
  bool use_eai = true;
  // if (sci_getenv("SCIRUN_EXTERNAL_APPLICATION_INTERFACE")) use_eai = true;

  // The environment has been setup
  // Now split of a process for running external processes
  
  if (use_eai)
  {
    systemcallmanager_ = scinew SystemCallManager();
    systemcallmanager_->create();
  }

#if defined(__APPLE__)  
  macImportExportForceLoad(); // Attempting to force load (and thus
                              // instantiation of static constructors) 
  macForceLoad();             // of Core/Datatypes and Core/ImportExport.
#endif


  if (use_eai)
  {
      // Services start up... 

      // Create a database of all available services. The next piece of code
      // Scans both the SCIRun as well as the Packages directories to find
      // Services that need to be started. Services allow communication with
      // thirdparty software and are Threads that run asychronicly with
      // with the rest of SCIRun. Since the thirdparty software may be running
      // on a different platform it allows for connecting to remote machines
      // and running the service on a different machine 
     

      ServiceDBHandle servicedb = scinew ServiceDB;	

      servicedb->loadpackages();	// load all services and find all makers
      servicedb->activateall();		// activate all services

      
      // Services are started and created by the ServiceManager, which will be launched here
      // Two competing managers will be started, one for purely internal usage and one that
      // communicates over a socket. The latter will only be created if a port is set.
      // If the current instance of SCIRun should not provide any services to other instances
      // of SCIRun over the internet, the second manager will not be launched
      
      // A log file is not necessary but handy for debugging purposes
      ServiceLogHandle internallogfile = scinew ServiceLog("scirun_internal_servicemanager.log");
      
      IComAddress internaladdress("internal","servicemanager");
      ServiceManager* internal_service_manager = scinew ServiceManager(servicedb,internaladdress,internallogfile); 
      Thread* t_int = scinew Thread(internal_service_manager,"internal service manager",0,Thread::NotActivated);
      t_int->setStackSize(1024*20);
      t_int->activate(false);
      t_int->detach();

      // Start an external service as well
      const char *serviceport_str = sci_getenv("SCIRUN_SERVICE_PORT");

      // Use the following environment setting is used to switch on IPv6 support
      // Most machines should be running a dual-host stack for the internet
      // connections, so it should not hurt to run in IPv6 mode. In most case
      // ipv4 address will work as well.
      //
      // It might be useful
      const char *serviceport_protocol = sci_getenv("SCIRUN_SERVICE_IPV6");
      std::string ipstr("");
      if (serviceport_protocol)
      {
        std::string protocol(serviceport_protocol);
        if ((protocol=="YES")||(protocol== "Y")||(protocol=="yes")||(protocol=="y")||(protocol=="1")||(protocol=="true")) ipstr = "ipv6";
      }
      
      if (serviceport_str)
      {
        // The protocol for conencting has been called "scirun"
        // In the near future this should be replaced with "sciruns" for
        // a secure version which will run over ssl. 
        
        // A log file is not necessary but handy for debugging purposes
        ServiceLogHandle externallogfile = scinew ServiceLog("scirun_external_servicemanager.log"); 
        
        IComAddress externaladdress("scirun","",serviceport_str,ipstr);
        ServiceManager* external_service_manager = scinew ServiceManager(servicedb,externaladdress,externallogfile); 
        Thread* t_ext = scinew Thread(external_service_manager,"external service manager",0,Thread::NotActivated);
        t_ext->setStackSize(1024*20);
        t_ext->activate(false);
        t_ext->detach();
      }
  }
  
  // Start up TCL...
  Network* net=new Network();
  TCLThread* tcl_task = new TCLThread(argc,argv, net, startnetno);// Only passes program name to TCL
  // We need to start the thread in the NotActivated state, so we can
  // change the stack size.  The 0 is a pointer to a ThreadGroup which
  // will default to the global thread group.
  Thread* t=new Thread(tcl_task,"TCL main event loop",0, Thread::NotActivated);
  t->setStackSize(1024*1024);
  // False here is stating that the tread was stopped or not.  Since
  // we have never started it the parameter should be false.
  t->activate(false);
  t->detach();
  tcl_task->mainloop_waitstart();

  // Create user interface link
  TCLInterface *gui = tcl_task->getTclInterface();

  // TCL Socket
  int port;
  const char *port_str = sci_getenv("SCIRUN_SERVER_PORT");
  if (port_str && string_to_int(port_str, port)) {
    StringSocket *transmitter = scinew StringSocket(port);
    cerr << "URL: " << transmitter->getUrl() << std::endl;
    transmitter->run();
    TCLSocketRunner *socket_runner = scinew TCLSocketRunner(gui, transmitter);
    (new Thread(socket_runner, "TCL Socket"))->detach();
  }

  // Create initial network
  Scheduler* sched_task=new Scheduler(net);

  // Activate the scheduler.  Arguments and return values are meaningless
  Thread* t2=new Thread(sched_task, "Scheduler");
  t2->setDaemon(true);
  t2->detach();
  
  // Determine if SCIRun is in regression testing mode
  const bool doing_regressions = sci_getenv_p("SCI_REGRESSION_TESTING");

  // Test for shaders.
  SCIRun::ShaderProgramARB::init_shaders_supported();

  // When doing regressions, make thread to kill ourselves after timeout
  if (doing_regressions) {
    RegressionKiller *kill = scinew RegressionKiller();
    Thread *tkill = scinew Thread(kill, "Kill a hung SCIRun");
    tkill->detach();
  }

  // Now activate the TCL event loop
  tcl_task->release_mainloop();

#ifdef _WIN32
  // windows has a semantic problem with atexit(), so we wait here instead.
  HANDLE forever = CreateSemaphore(0,0,1,"forever");
  WaitForSingleObject(forever,INFINITE);
#endif

#if !defined(__sgi)
  Semaphore wait("main wait", 0);
  wait.down();
#endif
	
  return 0;
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1209
#pragma reset woff 1424
#endif

