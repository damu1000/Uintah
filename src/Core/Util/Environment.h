/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in compliance
  with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.
  
  The Original Source Code is SCIRun, released March 12, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1994 
  University of Utah. All Rights Reserved.

  Environment.h: Interface to setting environemnt variables and parsing .rc files

  Written by:
    McKay Davis
    Scientific Computing and Imaging Institute 
    University of Utah
    January 2004
    Copyright (C) 2004 SCI Institute

*/


#ifndef Core_Util_Environemnt_h
#define Core_Util_Environment_h 1

#include <string>

namespace SCIRun {
  using std::string;

  bool find_and_parse_scirunrc();
  bool parse_scirunrc(const string filename);

  // sci_putenv will check the existing_env set string (filled in
  // store_existing_env) If the environment key existed when the
  // program was run, then it will not be overwritten.  This follows
  // the Unix convention of using environment variables to supercede
  // default program settings and .rc files.  Note we actually use tcl
  // to store the environment so that it remains consistent on both
  // the tcl and the C sides.  Therefore it is HIGHLY RECOMMENDED that
  // you use the sci versions of putenv and getenv to keep things
  // consistent.
  void sci_putenv(const string &key, const string &val, bool force=false);

  // sci_getenv_p will lookup the value of the environment variable
  // 'key' and returns false if the variable is equal to 'false',
  // 'no', 'off', or '0', it returns true otherwise.  (Case
  // insensitive)
  bool sci_getenv_p(const string &key);

  // Use the following function to get environment variables.  If you
  // don't, there is a chance that the Tcl and C side environments
  // will not be aligned and you will get incorrect values.  Also, you
  // must free the char* returned, or you will have a memory leak.
  char *sci_getenv( const string & key );
}

#endif // #ifndef Core_Util_Environment_h
