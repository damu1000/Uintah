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
 *  FieldBoundary.cc:  Build a surface field from a volume field
 *
 *  Written by:
 *   Peter-Pike Sloan and David Weinstein
 *   Department of Computer Science
 *   University of Utah
 *   July 1998
 *
 *  Copyright (C) 1998 SCI Group
 */

#include <Dataflow/Ports/GeometryPort.h>
#include <Dataflow/Ports/FieldPort.h>
#include <Core/Datatypes/TetVolMesh.h>
#include <Core/Datatypes/LatVolMesh.h>
#include <Core/Datatypes/TriSurfMesh.h>
#include <Core/Datatypes/ContourMesh.h>
#include <Core/Datatypes/TriSurf.h>
#include <Core/Geom/GeomTriangles.h>
#include <Core/Malloc/Allocator.h>

#include <Core/Datatypes/DispatchMesh1.h>
#include <iostream>
using std::cerr;

namespace SCIRun {

//! Module to build a surface field from a volume field.
class FieldBoundary : public Module {
public:
  FieldBoundary(const clString& id);
  virtual ~FieldBoundary();
  virtual void execute();

private:
  
  //! Iterates over mesh, and build TriSurf at the boundary
  template <class Msh> void boundary(const Msh *mesh);

  void add_face(const Point &p0, const Point &p1, const Point &p2, 
		MaterialHandle m0, MaterialHandle m1, 
		MaterialHandle m2, GeomTriangles *g);

  //! Input should be a volume field.
  FieldIPort*              infield_;
  //! Scene graph output.
  GeometryOPort*           viewer_;
  //! TriSurf field output.
  FieldOPort*              osurf_;
  
  //! Handle on the generated surface.
  FieldHandle              tri_fh_;
};

extern "C" Module* make_FieldBoundary(const clString& id)
{
  return scinew FieldBoundary(id);
}

FieldBoundary::FieldBoundary(const clString& id) : 
  Module("FieldBoundary", id, Filter, "Fields", "SCIRun"),
  tri_fh_(scinew TriSurf<double>)
{
  // Create the input port
  infield_ = scinew FieldIPort(this, "Field", FieldIPort::Atomic);
  add_iport(infield_);
  viewer_ = scinew GeometryOPort(this, "Geometry", GeometryIPort::Atomic);
  add_oport(viewer_);
  osurf_ = scinew FieldOPort(this, "TriSurf", FieldIPort::Atomic);
  add_oport(osurf_);
}
void 
FieldBoundary::add_face(const Point &p0, const Point &p1, const Point &p2, 
			MaterialHandle m0, MaterialHandle m1, 
			MaterialHandle m2, GeomTriangles *g) 
{
  g->add(p0, m0, 
	 p1, m2, 
	 p2, m1);
}

FieldBoundary::~FieldBoundary()
{
}


template <> void FieldBoundary::boundary(const ContourMesh *mesh) {
  error("FieldBoundary::boundary can't extract a surface from a ContourMesh");
}

template <> void FieldBoundary::boundary(const TriSurfMesh *mesh) {
  // Casting away const.  We need const correct handles.
  tri_fh_ = scinew TriSurf<double>(TriSurfMeshHandle((TriSurfMesh *)mesh), 
				   Field::NODE);
}

template <class Msh>
void 
FieldBoundary::boundary(const Msh *mesh)
{
  GeomTriangles *tris= scinew GeomTriangles;  
  // Walk all the cells in the mesh.
  typename Msh::cell_iterator citer = mesh->cell_begin();
  while (citer != mesh->cell_end()) {
    typename Msh::cell_index ci = *citer;
    ++citer;
    // Get all the faces in the cell.
    typename Msh::face_array faces;
    mesh->get_faces(faces, ci);
    // Check each face for neighbors
    typename Msh::face_array::iterator fiter = faces.begin();
    while (fiter != faces.end()) {
      typename Msh::cell_index nci;
      typename Msh::face_index fi = *fiter;
      ++fiter;
      if (! mesh->get_neighbor(nci , ci, fi)) {
	// Faces with no neighbors are on the boundary, build a tri.
	typename Msh::node_array nodes;
	mesh->get_nodes(nodes, fi);
	// Creating triangles, so fan if more than 3 nodes.
	Point p1, p2, p3;
	typename Msh::node_array::iterator niter = nodes.begin();
	mesh->get_point(p1, *niter);
	++niter;
	mesh->get_point(p2, *niter);
	++niter;
	mesh->get_point(p3, *niter);
	++niter;

	tris->add(p1, p2, p3);
	// FIX_ME add to TriSurf
	//osurf->add_tri(p1,p2,p3);
	while (niter != nodes.end()) {
	  p2 = p3;
	  mesh->get_point(p3, *niter);
	  ++niter;
	  
	  tris->add(p1, p2, p3);
	  // FIX_ME add to TriSurf
	  //osurf->add_tri(p1,p2,p3);
	}
      }
    }
  }
  // FIX_ME remove duplicates and build neighbors
  // osurf->resolve_surf();
  viewer_->addObj(tris, "Boundary Surface");
  viewer_->flushViews();
}



void 
FieldBoundary::execute()
{
  FieldHandle input;
  if (!infield_->get(input)) return;
  if (!input.get_rep()) {
    error("FieldBoundary Error: No input data.");
    return;
  }
  MeshBaseHandle mesh = input->mesh();
  mesh->finish_mesh();
  dispatch_mesh1(input->mesh(), boundary);
}



} // End namespace SCIRun


