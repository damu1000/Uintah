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
 *  MDSPlusReader.cc:
 *
 *  Written by:
 *   Allen Sanderson
 *   School of Computing
 *   University of Utah
 *   March 2002
 *
 *  Copyright (C) 2002 SCI Group
 */

/*
  This is a C++ interface for fetching data from a MDSPlus Server.

  Unfortunately, because MDSPlus has several short commings this interface
  is need is to provide a link between SCIRun and MDSPlus. The two items
  addressed are making MDSPlus thread safe and allowing seemless access to
  multiple connections (e.g. MDSPlus sockets).

  For more information on the calls see mdsPlusAPI.c
*/

#include <include/sci_defs/mdsplus_defs.h>

#include <Packages/DataIO/Core/ThirdParty/mdsPlusReader.h>

#include "mdsPlusAPI.h"

namespace DataIO {

using namespace SCIRun;

Mutex MDSPlusReader::mdsPlusLock_( "MDS Plus Mutex Lock" );

MDSPlusReader::MDSPlusReader() : socket_(-1)
{
}

// Simple connection interface that insures thread safe calls and the correct socket.
int MDSPlusReader::connect( std::string server )
{
#ifdef HAVE_MDSPLUS
  int retVal;

  mdsPlusLock_.lock();

  // Insure that there will not be a disconnect if another connection is present.
  MDS_SetSocket( -1 );

  /* Connect to MDSplus */
  socket_ = MDS_Connect((char*)server.c_str());

  if( socket_ > 0 )
    retVal = 0;
  else
    retVal = -1;

  mdsPlusLock_.unlock();

  return retVal;
#else
  return -1;
#endif
}

// Simple open interface that insures thread safe calls and the correct socket.
int MDSPlusReader::open( const std::string tree,
			 const int shot )
{
#ifdef HAVE_MDSPLUS
  int retVal;

  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  /* Open tree */
  retVal = MDS_Open((char*)tree.c_str(), shot);

  mdsPlusLock_.unlock();

  return retVal;
#else
  return -1;
#endif
}

// Simple disconnect interface that insures thread safe calls and
// the correct socket.
void MDSPlusReader::disconnect()
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  /* Disconnect to MDSplus */
  MDS_Disconnect();

  socket_ = -1;

  mdsPlusLock_.unlock();
#endif
}

// Simple valid interface that insures thread safe calls
// and the correct socket.
bool MDSPlusReader::valid( const std::string signal )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  bool valid = is_valid( signal.c_str() );

  mdsPlusLock_.unlock();

  return valid;
#else
  return false;
#endif
}

// Simple rank (number of dimensions) interface that insures thread safe calls
// and the correct socket.
int MDSPlusReader::rank( const std::string signal )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  int rank = get_rank( signal.c_str() );

  mdsPlusLock_.unlock();

  return rank;
#else
  return 0;
#endif
}

// Simple type (data type) interface that insures thread safe calls
// and the correct socket.
int MDSPlusReader::type( const std::string signal )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  int rank = get_type( signal.c_str() );

  mdsPlusLock_.unlock();

  return rank;
#else
  return 0;
#endif
}

// Simple dims (number of dimension) interface that insures thread safe calls
// and the correct socket.
int MDSPlusReader::dims( const std::string signal, int** dims )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  int rank = get_dims( signal.c_str(), dims );

  mdsPlusLock_.unlock();

  return rank;
#else
  return 0;
#endif
}

// Simple data (double data) interface that insures thread safe calls
//  and the correct socket.
void* MDSPlusReader::values( const std::string signal, int dtype )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  void* values = get_values( signal.c_str(), dtype );

  mdsPlusLock_.unlock();

  return values;
#else
  return 0;
#endif
}

// Simple search interface that insures thread safe calls and
// the correct socket.
int MDSPlusReader::search( const std::string signal,
			   const int regexp,
			   std::vector<std::string> &signals ) 
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  // Search for the signals.
  signals.push_back("\\NIMROD::TOP.OUTPUTS.CODE.GRID:R");
  signals.push_back("\\NIMROD::TOP.OUTPUTS.CODE.GRID:Z");
  signals.push_back("\\NIMROD::TOP.OUTPUTS.CODE.GRID:PHI");
  signals.push_back("\\NIMROD::TOP.OUTPUTS.CODE.GRID:K");

  mdsPlusLock_.unlock();

  return signals.size();
#else
  return 0;
#endif
}

// Simple grid (elements) interface that insures thread safe calls and
// the correct socket.
double* MDSPlusReader::grid( const std::string axis, int **dims )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  double* data = get_grid( axis.c_str(), dims );

  mdsPlusLock_.unlock();

  return data;
#else
  return NULL;
#endif
}

// Simple slice nids (nids number are ids) interface that insures thread safe calls and
// the correct socket.
int MDSPlusReader::slice_ids( int **nids ) 
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  int nSlices = get_slice_ids( nids );

  mdsPlusLock_.unlock();

  return nSlices;
#else
  return 0;
#endif
}

// Simple slice name interface that insures thread safe calls and the correct socket.
std::string MDSPlusReader::slice_name( const int nid )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  std::string name( get_name(nid) );

  mdsPlusLock_.unlock();

  return name;
#else
  return "";
#endif
}

// Simple slice time interface that insures thread safe calls and the correct socket.
double MDSPlusReader::slice_time( const std::string name )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  double time = get_slice_time( name.c_str() );

  mdsPlusLock_.unlock();

  return time;
#else
  return 0;
#endif
}


// Simple slice data interface that insures thread safe calls and the correct socket.
double *MDSPlusReader::slice_data( const std::string name,
				   const std::string space,
				   const std::string node,
				   int **dims )
{
#ifdef HAVE_MDSPLUS
  mdsPlusLock_.lock();

  MDS_SetSocket( socket_ );

  double* data =
    get_slice_data( name.c_str(), space.c_str(), node.c_str(), dims );
  mdsPlusLock_.unlock();

  return data;
#else
  return NULL;
#endif
}
}
