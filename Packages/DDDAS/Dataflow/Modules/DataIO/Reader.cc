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
*/

 
/*
 *  Reader.cc:  Continuously reads a series of .out mesh files and produces a 
 *              LatVol representation of each set of solution points generated 
 *              by the AggieFEM/ccfv code. There are 64000 cell-centered 
 *              solution points in the image, represented as a structured grid.
 *              Each set of solution points represents one time step.
 *   
 *
 *  Written by:
 *  Jenny Simpson
 *  (Modified from SampleLattice module written by Michael Callahan)
 *
 *  Copyright (C) 2003 SCI Group
 */

#include <Dataflow/Network/Module.h>
#include <Dataflow/Ports/FieldPort.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Datatypes/LatVolField.h>
#include <Core/Geometry/BBox.h>
#include <Core/Geometry/Point.h>
#include <Core/GuiInterface/GuiVar.h>
#include <Core/Containers/StringUtil.h>
#include <Packages/DDDAS/share/share.h>

#include <iostream>
#include <fstream>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>

namespace DDDAS {

using namespace SCIRun;
  
class DDDASSHARE Reader : public Module {

public:
  Reader(GuiContext* ctx);
  virtual ~Reader();

  virtual void execute();

  virtual void tcl_command(GuiArgs&, void*);

  double * read_ccfv( string filename, int& size );

  void fill_mesh( string filename, FieldOPort * ofp );
  
};
 
 
DECLARE_MAKER(Reader)

Reader::Reader(GuiContext* ctx)
  : Module("Reader", ctx, Source, "DataIO", "DDDAS")
{ 
}



Reader::~Reader()
{
}


void
Reader::execute()
{
  // Declare input field
  
  // We don't use the input field right now, but we still check to make sure
  // it can be initialized in case it needs to be used later.
  FieldIPort *ifp = (FieldIPort *)get_iport("Input Field");
  FieldHandle ifieldhandle;
  if (!ifp) {
    error("(Reader::execute) Unable to initialize iport 'Input Field'.");
    return;
  }

  // Declare output field
  FieldOPort *ofp = (FieldOPort *)get_oport("Output Sample Field");
  if (!ofp) {
    error("(Reader::execute) Unable to initialize oport 'Output Sample Field'.");
    return;
  }

  // Fill the mesh with solution points found in data files and output mesh
  DIR *dirp;
  struct dirent *dp;
  char *filename;
  char *dir = "/home/sci/simpson/data_stream/current/data/data_64K/";
  string dir_str = dir;
  
  dirp = opendir(dir);
  if (!dirp)
  {
    error("(Reader::execute) Directory Read Error");
    return;
  }

  
  while (1)
  {
    errno = 0;
    dp = readdir(dirp);
    if (!dp) break;

    filename = dp->d_name;
    string fn = filename;

    if(filename[0] != '.') // Exclude hidden files
    {
      string fn_str = filename;
      string filepath = dir_str + fn_str;
      cerr << "(Reader::execute) filename: " << filepath << "\n";

      // Fill mesh
      fill_mesh( filepath, ofp );
    }
    
  }

  if(errno) error("(Reader::execute) Directory Read error");
  closedir(dirp);

  // TODO: Need to change so that last send is a send, not send_intermediate  
  ofp->reset();
}


void
Reader::tcl_command(GuiArgs& args, void* userdata)
{
  Module::tcl_command(args, userdata);
}

// Reads solution points from a file, populates the mesh with them, and
// outputs the mesh.
void
Reader::fill_mesh( string filename, FieldOPort * ofp )
{
  cerr << "(Reader::fill_mesh) Inside\n";

  cerr << "(Reader::fill_mesh) filename = " << filename << "\n";

  Point minb, maxb;
  minb = Point(-1.0, -1.0, -1.0);
  maxb = Point(1.0, 1.0, 1.0);

  Vector diag((maxb.asVector() - minb.asVector()) * (0.0/100.0));
  minb -= diag;
  maxb += diag;
  
  // Read in the scalar values (solution points) from the file, store them in
  // an array
  double * sol_pts;
  int num_sols;
  
  sol_pts = read_ccfv( filename, num_sols );

  // Error checking -- returns if data file did not exist
  if( sol_pts == 0 )
  {
    return;
  }

  cerr << "(Reader::fill_mesh) Creating blank mesh\n";
  
  // Create blank mesh.
  int cube_root = (int) cbrt( num_sols );
  unsigned int sizex;
  unsigned int sizey;
  unsigned int sizez;
  sizex = sizey = sizez = Max(2, cube_root) + 1;
  LatVolMeshHandle mesh = scinew LatVolMesh(sizex, sizey, sizez, minb, maxb);

  cerr << "(Reader::fill_mesh) Assign data to cell centers\n";
  
  // Assign data to cell centers
  Field::data_location data_at = Field::CELL;

  cerr << "(Reader::fill_mesh) Create Image Field\n";
  // Create Image Field.
  FieldHandle ofh;

  LatVolField<double> *lvf = scinew LatVolField<double>(mesh, data_at);
  if (data_at != Field::NONE)
  {
    LatVolField<double>::fdata_type::iterator itr = lvf->fdata().begin();

    // Iterator for solution points array
    int i = 0; 
    while (itr != lvf->fdata().end())
    {
      assert( i < num_sols );
      *itr = sol_pts[i];
      ++itr;
      i++;
    }
    cerr << "(Reader::execute) number of field data points = " << i << "\n";
  } 
  ofh = lvf;

  cerr << "(Reader::fill_mesh) Deallocate memory for sol_pts array\n";
  
  // Deallocate memory for sol_pts array
  delete[] sol_pts;

  cerr << "(Reader::fill_mesh) Sending data to output field\n";
  
  ofp->send_intermediate(ofh);

  cerr << "(Reader::fill_mesh) Leaving\n";
}

// Read in a file generated by ccfv and store its solution points
// as an array of doubles.  Returns a pointer to the array of
// solution points. Assumes that if the file exists, it is of the
// correct format.
double *
Reader::read_ccfv( string filename, int& size )
{
  cerr << "(Reader::read_ccfv) Inside\n";
  /*
    Here's an sample of what the file should look like:

    Data File Format
    ----------------

    [char *s (file name)]
    solution u -size=[int Ncells (number of solution pts)] -components=1 -type=nodal
    [double solution(0) (Solution point 0.  See notes below)]
    [double solution(1)]
    [double solution(2)]
    [double solution(3)]
    [double solution(4)]
    [double solution(5)]
    ...
    [double solution(Ncells)]

    Sample File
    -----------

    sat.out
    solution u  -size=64000 -components=1 -type=nodal
    0.584279
    0.249236
    0.0711161
    0.0134137
    0.00190625
    0.000223068
    2.70226e-05
    ...
    
  */
  
  // Open up an input file stream
  ifstream input( filename.c_str() );
  if ( !input )
  {
    error( "(Reader::read_ccfv)  File not found." );
    return 0;
  }

  // Set up variables corresponding to the file values
  string filename_h;
  string solution;
  int components;
  string type;
  string str;
  char ch;
  double sol;
  
  // Parse the header, assigning variable values

  input >> filename_h
        >> str
        >> solution
        >> ch >> ch >> ch >> ch >> ch >> ch
        >> size
        >> ch >> ch >> ch >> ch >> ch >> ch 
        >> ch >> ch >> ch >> ch >> ch >> ch
        >> components
        >> ch >> ch >> ch >> ch >> ch >> ch
        >> type;

  // Allocate memory for solution points
  double * sol_pts = new double[size]; // Replace new with scinew?
  
  // Populate the field values array with solution points
  for( int i = 0; i < size; i++ )
  {
    input >> sol;
    sol_pts[i] = sol;
  }

  // Close file
  input.close();

  cerr << "(Reader::read_ccfv) Leaving\n";
  return sol_pts;
}
 

} // End namespace DDDAS




