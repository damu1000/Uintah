/*
 *  FieldBoundary.cc:  Unfinished modules
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
#include <Dataflow/Ports/MeshPort.h>
#include <Dataflow/Ports/SurfacePort.h>
#include <Core/Datatypes/TriSurface.h>
#include <Core/Geom/GeomTriangles.h>
#include <Core/Malloc/Allocator.h>
#include <iostream>
using std::cerr;

namespace SCIRun {


class FieldBoundary : public Module {
    MeshIPort* inport;
    GeometryOPort* outport;
    SurfaceOPort* osurf;
public:
    FieldBoundary(const clString& id);
    virtual ~FieldBoundary();
    virtual void execute();
};

extern "C" Module* make_FieldBoundary(const clString& id)
{
    return scinew FieldBoundary(id);
}

FieldBoundary::FieldBoundary(const clString& id)
: Module("FieldBoundary", id, Filter)
{
   // Create the input port
    inport=scinew MeshIPort(this, "Mesh", MeshIPort::Atomic);
    add_iport(inport);
    outport=scinew GeometryOPort(this, "Geometry", GeometryIPort::Atomic);
    add_oport(outport);
    osurf=scinew SurfaceOPort(this, "Surface", SurfaceIPort::Atomic);
    add_oport(osurf);
}

FieldBoundary::~FieldBoundary()
{
}

void FieldBoundary::execute()
{
    MeshHandle mesh;
    if (!inport->get(mesh))
	return;


    // now actualy look at this stuff...

    GeomTrianglesP *tris= scinew GeomTrianglesP;
    mesh->compute_face_neighbors();

    int facei[8] = {0,1,2,3,0,1,2,3};
    Array1<int> bdryNodes(mesh->nodesize());
    bdryNodes.initialize(0);

    int i;
    for(i=0;i<mesh->elemsize();i++) {
      Element *teste = mesh->element(i);
      if (teste) {
	for(int j=0;j<4;j++) {
	  if (teste->faces[j] == -1) {
	      bdryNodes[teste->n[facei[j+1]]] = 1;
	      bdryNodes[teste->n[facei[j+2]]] = 1;
	      bdryNodes[teste->n[facei[j+3]]] = 1;
	      tris->add(mesh->point(teste->n[facei[j+1]]),
			mesh->point(teste->n[facei[j+2]]),
			mesh->point(teste->n[facei[j+3]]));
	  }
	}
      }
    }
    TriSurface *ts = new TriSurface;
    Array1<int> nodeMap(bdryNodes.size());
    nodeMap.initialize(-1);
    int count=0;
    for (i=0; i<mesh->nodesize(); i++) {
	if (bdryNodes[i]) {
	    nodeMap[i] = count;
	    count++;
	    ts->points_.add(mesh->point(i));
	}
    }
    for (i=0; i<mesh->elemsize(); i++) {
	Element *teste = mesh->element(i);
	if (teste) {
	    for(int j=0;j<4;j++) {
		if (teste->faces[j] == -1) {
		    int n1=nodeMap[teste->n[facei[j+1]]];
		    int n2=nodeMap[teste->n[facei[j+2]]];
		    int n3=nodeMap[teste->n[facei[j+3]]];
		    if (n1 == -1 || n2 == -1 || n3 == -1) {
			cerr << "ERROR in FieldBoundary!\n";
		    }
		    ts->faces_.add(TSElement(n1,n2,n3));
		}
	    }
	}
    }
    outport->delAll();
    outport->addObj(tris,"Boundary Triangles");
    SurfaceHandle tsHandle(ts);
    osurf->send(tsHandle);
}

} // End namespace SCIRun


