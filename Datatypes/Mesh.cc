/*
 *  Mesh.h: Unstructured meshes
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   July 1994
 *
 *  Copyright (C) 1994 SCI Group
 */

#include <Datatypes/Mesh.h>

#include <Classlib/FastHashTable.h>
#include <Classlib/NotFinished.h>
#include <Classlib/String.h>
#include <Classlib/TrivialAllocator.h>
#include <Datatypes/ColumnMatrix.h>
#include <Geom/Group.h>
#include <Geom/Material.h>
#include <Geom/Sphere.h>
#include <Geom/Polyline.h>
#include <Geom/Tri.h>
#include <Malloc/Allocator.h>
#include <Math/Mat.h>
#include <iostream.h>
#include <fstream.h>

static TrivialAllocator Element_alloc(sizeof(Element));
static TrivialAllocator Node_alloc(sizeof(Node));
static TrivialAllocator Face_alloc(sizeof(Face));
static TrivialAllocator DFace_alloc(sizeof(Face));

struct DFace {
    DFace* next;
    int hash;
    int face_idx;
    int n[3];
    inline DFace(int n0, int n1, int n2) {
	n[0]=n0;
	n[1]=n1;
	n[2]=n2;
	if(n[0] < n[1]){
	    int tmp=n[0]; n[0]=n[1]; n[1]=tmp;
	}
	if(n[0] < n[2]){
	    int tmp=n[0]; n[0]=n[2]; n[2]=tmp;
	}
	if(n[1] < n[2]){
	    int tmp=n[1]; n[1]=n[2]; n[2]=tmp;
	}
	hash=(n[0]*7+5)^(n[1]*5+3)^(n[2]*3+1);
    }
    inline int operator==(const DFace& f) const {
	return n[0]==f.n[0] && n[1]==f.n[1] && n[2]==f.n[2];
    }

    inline void* operator new(size_t) {
	return DFace_alloc.alloc();
    }
    void operator delete(void* rp, size_t) {
	DFace_alloc.free(rp);
    }
};

static Persistent* make_Mesh()
{
    return scinew Mesh;
}

static Persistent* make_Node()
{
    return new Node(Point(0,0,0));
}

PersistentTypeID Mesh::type_id("Mesh", "Datatype", make_Mesh);
PersistentTypeID Node::type_id("Node", "Datatype", make_Node);

Mesh::Mesh()
: have_all_neighbors(0), current_generation(2)
{
    cond_tensors.grow(1);
    cond_tensors[0].grow(6);
    cond_tensors[0][0]=1;
    cond_tensors[0][1]=0;
    cond_tensors[0][2]=0;
    cond_tensors[0][3]=1;
    cond_tensors[0][4]=0;
    cond_tensors[0][5]=1;
}

Mesh::Mesh(int nnodes, int nelems)
: nodes(nnodes), elems(nelems), have_all_neighbors(0), current_generation(2)
{
    cond_tensors.grow(1);
    cond_tensors[0].grow(6);
    cond_tensors[0][0]=1;
    cond_tensors[0][1]=0;
    cond_tensors[0][2]=0;
    cond_tensors[0][3]=1;
    cond_tensors[0][4]=0;
    cond_tensors[0][5]=1;
}

Mesh::Mesh(const Mesh& copy)
: nodes(copy.nodes), elems(copy.elems.size()),
  cond_tensors(copy.cond_tensors), have_all_neighbors(copy.have_all_neighbors),
  current_generation(2)
{
    int nelems=elems.size();
    for(int i=0;i<nelems;i++){
	Element* e=new Element(*copy.elems[i], this);
	elems[i]=e;
    }
}

Mesh::~Mesh()
{
    remove_all_elements();
}

void Mesh::remove_all_elements()
{
    for(int i=0;i<elems.size();i++)
	if(elems[i])
	    delete elems[i];
    elems.remove_all();
}

Mesh* Mesh::clone()
{
    return scinew Mesh(*this);
}

#define MESH_VERSION 4

void Pio(Piostream& stream, NodeVersion1& node)
{
    stream.begin_cheap_delim();
    Pio(stream, node.p);
    stream.end_cheap_delim();
}

void Pio(Piostream& stream, ElementVersion1& elem)
{
    stream.begin_cheap_delim();
    Pio(stream, elem.n0);
    Pio(stream, elem.n1);
    Pio(stream, elem.n2);
    Pio(stream, elem.n3);
    stream.end_cheap_delim();
}

void Mesh::io(Piostream& stream)
{
    int version=stream.begin_class("Mesh", MESH_VERSION);
    if(version == 1){
	Array1<NodeVersion1> tmpnodes;
	Pio(stream, tmpnodes);
	nodes.resize(tmpnodes.size());
	for(int i=0;i<tmpnodes.size();i++)
	    nodes[i]=new Node(tmpnodes[i].p);
    } else {
	Pio(stream, nodes);
    }

    if (version < 3) {	// didn't used to have conductivities...
	Array1<ElementVersion1> tmpElements;
	Pio(stream, tmpElements);
	elems.resize(tmpElements.size());
	for (int i=0; i<tmpElements.size(); i++) {
	    elems[i]=new Element(0, tmpElements[i].n0,
				 tmpElements[i].n1,
				 tmpElements[i].n2,
				 tmpElements[i].n3);
	}
    } else		// ... now we do!
	Pio(stream, elems);

    if (version >= 4){
	Pio(stream, cond_tensors);
    }

    stream.end_class();
    if(stream.reading()){
	for(int i=0;i<elems.size();i++){
	    elems[i]->mesh=this;
	    elems[i]->orient();
	    elems[i]->compute_basis();
	}
	compute_neighbors();
    }
}

void Pio(Piostream& stream, Element*& data)
{
    if(stream.reading())
	data=new Element(0,0,0,0,0);
    stream.begin_cheap_delim();
    Pio(stream, data->n[0]);
    Pio(stream, data->n[1]);
    Pio(stream, data->n[2]);
    Pio(stream, data->n[3]);
    Pio(stream, data->cond);
    stream.end_cheap_delim();
}

#define NODE_VERSION 4

void Node::io(Piostream& stream)
{
    int version=stream.begin_class("Node", NODE_VERSION);
    Pio(stream, p);
    if(version >= 3){
      if (version >= 4) {
	  Pio(stream, fluxBC);
      } else {
	  fluxBC=0;
      }
      int flag;
      if(!stream.reading()){
	flag=bc?1:0;
      }
      Pio(stream, flag);
      if(stream.reading() && flag)
	bc=new DirichletBC(0,0);
      if(flag){
	Pio(stream, bc->fromsurf);
	Pio(stream, bc->value);
      }
    }
    stream.end_class();
}

Node::~Node()
{
    if(bc)
	delete bc;
}

Element::Element(Mesh* mesh, int n1, int n2, int n3, int n4)
: generation(0), cond(0), mesh(mesh)
{
    n[0]=n1; n[1]=n2; n[2]=n3; n[3]=n4;
    faces[0]=faces[1]=faces[2]=faces[3]=-2;

#ifdef STORE_ELEMENT_BASIS
    if(mesh)
	compute_basis();
    else
	vol=-9999;
#endif
}

void Element::compute_basis()
{
#ifdef STORE_ELEMENT_BASIS
    Point p1(mesh->nodes[n[0]]->p);
    Point p2(mesh->nodes[n[1]]->p);
    Point p3(mesh->nodes[n[2]]->p);
    Point p4(mesh->nodes[n[3]]->p);
    double x1=p1.x();
    double y1=p1.y();
    double z1=p1.z();
    double x2=p2.x();
    double y2=p2.y();
    double z2=p2.z();
    double x3=p3.x();
    double y3=p3.y();
    double z3=p3.z();
    double x4=p4.x();
    double y4=p4.y();
    double z4=p4.z();
    double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
    double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
    double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
    double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
    double iV6=1./(a1+a2+a3+a4);

    double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
    double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
    double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
    g[0]=Vector(b1*iV6, c1*iV6, d1*iV6);
    double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
    double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
    double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
    g[1]=Vector(b2*iV6, c2*iV6, d2*iV6);
    double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
    double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
    double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
    g[2]=Vector(b3*iV6, c3*iV6, d3*iV6);
    double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
    double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
    double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
    g[3]=Vector(b4*iV6, c4*iV6, d4*iV6);
    a[0]=a1*iV6;
    a[1]=a2*iV6;
    a[2]=a3*iV6;
    a[3]=a4*iV6;

    vol=(1./iV6)/6.0;
#endif
}

Element::Element(const Element& copy, Mesh* mesh)
: generation(0), cond(copy.cond), mesh(mesh)
{
    faces[0]=copy.faces[0];
    faces[1]=copy.faces[1];
    faces[2]=copy.faces[2];
    faces[3]=copy.faces[3];
#ifdef STORE_ELEMENT_BASIS
    n[0]=copy.n[0];
    n[1]=copy.n[1];
    n[2]=copy.n[2];
    n[3]=copy.n[3];
    g[0]=copy.g[0];
    g[1]=copy.g[1];
    g[2]=copy.g[2];
    g[3]=copy.g[3];
    a[0]=copy.a[0];
    a[1]=copy.a[1];
    a[2]=copy.a[2];
    a[3]=copy.a[3];
    vol=copy.vol;
#endif
}

Node::Node(const Point& p)
: p(p), elems(0, 4), bc(0), fluxBC(0)
{
}

Node::Node(const Node& copy)
: p(copy.p), elems(copy.elems), bc(copy.bc?new DirichletBC(*copy.bc):0),
  fluxBC(0)
{
}

Node* Node::clone()
{
    return new Node(*this);
}

int Mesh::unify(Element* not,
		const Array1<int>& n1, const Array1<int>& n2,
		const Array1<int>& n3)
{
    int s1=n1.size();
    int s2=n2.size();
    int s3=n3.size();
    int i1=0;
    int i2=0;
    int i3=0;
    while(i1<s1 && i2<s2 && i3<s3){
	int d1=n1[i1];
	int d2=n2[i2];
	int d3=n3[i3];
	if(d1==d2){
	    if(d2==d3){
		if(elems[d1] != not){
		    // Found it...
		    return d1;
		} else {
		    i1++;
		    i2++;
		    i3++;
		}
	    } else if(d3<d1){
		i3++;
	    } else {
		i1++;
		i2++;
	    }
	} else if(d1<d2){
	    if(d1<d3){
		i1++;
	    } else {
		i3++;
	    }
	} else {
	    if(d2<d3){
		i2++;
	    } else {
		i3++;
	    }
	}
    }
    return -1;
}

void Element::get_sphere2(Point& cen, double& rad2, double& err)
{
    Point p0(mesh->nodes[n[0]]->p);
    Point p1(mesh->nodes[n[1]]->p);
    Point p2(mesh->nodes[n[2]]->p);
    Point p3(mesh->nodes[n[3]]->p);
    double mat[3][3];
    mat[0][0]=p1.x()-p0.x();
    mat[0][1]=p1.y()-p0.y();
    mat[0][2]=p1.z()-p0.z();
    mat[1][0]=p2.x()-p0.x();
    mat[1][1]=p2.y()-p0.y();
    mat[1][2]=p2.z()-p0.z();
    mat[2][0]=p3.x()-p0.x();
    mat[2][1]=p3.y()-p0.y();
    mat[2][2]=p3.z()-p0.z();
    double rhs[3];
    double c0=p0.x()*p0.x()+p0.y()*p0.y()+p0.z()*p0.z();
    double c1=p1.x()*p1.x()+p1.y()*p1.y()+p1.z()*p1.z();
    double c2=p2.x()*p2.x()+p2.y()*p2.y()+p2.z()*p2.z();
    double c3=p3.x()*p3.x()+p3.y()*p3.y()+p3.z()*p3.z();
    rhs[0]=(c1-c0)*0.5;
    rhs[1]=(c2-c0)*0.5;
    rhs[2]=(c3-c0)*0.5;
    double rcond;
    matsolve3by3_cond(mat, rhs, &rcond);
    if(rcond < 1.e-7){
	cerr << "WARNING - degenerate element, rcond=" << rcond << endl;
    }
    cen=Point(rhs[0], rhs[1], rhs[2]);
    rad2=(p0-cen).length2();
    //err=Max(1.e-6, 1.e-6/rcond);
    err=1.e-4;

}

Point Element::centroid()
{
    Point p0(mesh->nodes[n[0]]->p);
    Point p1(mesh->nodes[n[1]]->p);
    Point p2(mesh->nodes[n[2]]->p);
    Point p3(mesh->nodes[n[3]]->p);
    return AffineCombination(p0, .25, p1, .25, p2, .25, p3, .25);
}

void Mesh::detach_nodes()
{
    for(int i=0;i<nodes.size();i++)
	if(nodes[i].get_rep())
	    nodes[i].detach();
}

void Mesh::compute_neighbors()
{
    // Clear old neighbors...
    int i;
    for(i=0;i<nodes.size();i++)
	if(nodes[i].get_rep())
	    nodes[i]->elems.remove_all();
    // Compute element info for nodes
    for(i=0;i<elems.size();i++){
	Element* elem=elems[i];
	if(elem){
	    if(nodes[elem->n[0]].get_rep())
		nodes[elem->n[0]]->elems.add(i);
	    if(nodes[elem->n[1]].get_rep())
		nodes[elem->n[1]]->elems.add(i);
	    if(nodes[elem->n[2]].get_rep())
		nodes[elem->n[2]]->elems.add(i);
	    if(nodes[elem->n[3]].get_rep())
		nodes[elem->n[3]]->elems.add(i);
	}
    }
    // Reset face neighbors
    for(i=0;i<elems.size();i++){
	if(elems[i]){
	    elems[i]->faces[0]=-2;
	    elems[i]->faces[1]=-2;
	    elems[i]->faces[2]=-2;
	    elems[i]->faces[3]=-2;
	}
    }
}

// Barycentric coordinate computation from 
// Computer-Aided Analysis and Design of
// Electromagnetic Devices
// S. Ranajeeven & H. Hoole

int Mesh::inside(const Point& p, Element* elem)
{
    cerr << "inside called...\n";
#ifndef STORE_ELEMENT_BASIS
    Point p1(nodes[elem->n[0]]->p);
    Point p2(nodes[elem->n[1]]->p);
    Point p3(nodes[elem->n[2]]->p);
    Point p4(nodes[elem->n[3]]->p);
    double x1=p1.x();
    double y1=p1.y();
    double z1=p1.z();
    double x2=p2.x();
    double y2=p2.y();
    double z2=p2.z();
    double x3=p3.x();
    double y3=p3.y();
    double z3=p3.z();
    double x4=p4.x();
    double y4=p4.y();
    double z4=p4.z();
    double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
    double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
    double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
    double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
    double iV6=1./(a1+a2+a3+a4);

    double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
    double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
    double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
    double s1=iV6*(a1+b1*p.x()+c1*p.y()+d1*p.z());
    if(s1<-1.e-6)
	return 0;

    double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
    double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
    double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
    double s2=iV6*(a2+b2*p.x()+c2*p.y()+d2*p.z());
    if(s2<-1.e-6)
	return 0;

    double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
    double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
    double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
    double s3=iV6*(a3+b3*p.x()+c3*p.y()+d3*p.z());
    if(s3<-1.e-6)
	return 0;

    double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
    double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
    double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
    double s4=iV6*(a4+b4*p.x()+c4*p.y()+d4*p.z());
    if(s4<-1.e-6)
	return 0;
#else
    double s0=elem->a[0]+Dot(elem->g[0], p);
    if(s0<-1.e-6)
	return 0;
    double s1=elem->a[1]+Dot(elem->g[1], p);
    if(s1<-1.e-6)
	return 0;
    double s2=elem->a[2]+Dot(elem->g[2], p);
    if(s2<-1.e-6)
	return 0;
    double s3=elem->a[3]+Dot(elem->g[3], p);
    if(s3<-1.e-6)
	return 0;
#endif

    return 1;
}

void Mesh::get_interp(Element* elem, const Point& p,
		      double& s0, double& s1, double& s2, double& s3)
{
#ifndef STORE_ELEMENT_BASIS
    Point p1(nodes[elem->n[0]]->p);
    Point p2(nodes[elem->n[1]]->p);
    Point p3(nodes[elem->n[2]]->p);
    Point p4(nodes[elem->n[3]]->p);
    double x1=p1.x();
    double y1=p1.y();
    double z1=p1.z();
    double x2=p2.x();
    double y2=p2.y();
    double z2=p2.z();
    double x3=p3.x();
    double y3=p3.y();
    double z3=p3.z();
    double x4=p4.x();
    double y4=p4.y();
    double z4=p4.z();
    double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
    double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
    double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
    double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
    double iV6=1./(a1+a2+a3+a4);

    double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
    double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
    double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
    s0=iV6*(a1+b1*p.x()+c1*p.y()+d1*p.z());
    double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
    double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
    double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
    s1=iV6*(a2+b2*p.x()+c2*p.y()+d2*p.z());
    double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
    double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
    double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
    s2=iV6*(a3+b3*p.x()+c3*p.y()+d3*p.z());
    double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
    double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
    double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
    s3=iV6*(a4+b4*p.x()+c4*p.y()+d4*p.z());
#else
    s0=elem->a[0]+Dot(elem->g[0], p);
    s1=elem->a[1]+Dot(elem->g[1], p);
    s2=elem->a[2]+Dot(elem->g[2], p);
    s3=elem->a[3]+Dot(elem->g[3], p);
#endif
}

double Mesh::get_grad(Element* elem, const Point&,
		      Vector& g0, Vector& g1, Vector& g2, Vector& g3)
{
#ifndef STORE_ELEMENT_BASIS
    Point p1(nodes[elem->n[0]]->p);
    Point p2(nodes[elem->n[1]]->p);
    Point p3(nodes[elem->n[2]]->p);
    Point p4(nodes[elem->n[3]]->p);
    double x1=p1.x();
    double y1=p1.y();
    double z1=p1.z();
    double x2=p2.x();
    double y2=p2.y();
    double z2=p2.z();
    double x3=p3.x();
    double y3=p3.y();
    double z3=p3.z();
    double x4=p4.x();
    double y4=p4.y();
    double z4=p4.z();
    double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
    double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
    double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
    double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
    double iV6=1./(a1+a2+a3+a4);

    double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
    double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
    double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
    g0=Vector(b1*iV6, c1*iV6, d1*iV6);
    double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
    double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
    double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
    g1=Vector(b2*iV6, c2*iV6, d2*iV6);
    double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
    double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
    double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
    g2=Vector(b3*iV6, c3*iV6, d3*iV6);
    double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
    double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
    double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
    g3=Vector(b4*iV6, c4*iV6, d4*iV6);

    double vol=(1./iV6)/6.0;
    return(vol);
#else
    g0=elem->g[0];
    g1=elem->g[1];
    g2=elem->g[2];
    g3=elem->g[3];
    return elem->vol;
#endif
}

void print_element(Element* e, Mesh* mesh)
{
    cerr << "Element is composed of nodes: " << e->n[0] << ", " << e->n[1] << ", " << e->n[2] << ", " << e->n[3] << endl;
    for(int i=0;i<4;i++){
	int nn=e->n[i];
	NodeHandle& n=mesh->nodes[nn];
	cerr << nn << ": " << n->p << endl;
    }
}

void dump_mesh(Mesh* mesh)
{
    ofstream out("mesh.dump");
    out << "Nodes:" << endl;
    int i;
    for(i=0;i<mesh->nodes.size();i++){
	Node* n=mesh->nodes[i].get_rep();
	if(!n)continue;
	out << i << ": " << n->p;
	for(int ii=0;ii<n->elems.size();ii++){
	    out << n->elems[ii] << " ";
	}
	out << endl;
    }
    out << "Elements:" << endl;
    for(i=0;i<mesh->elems.size();i++){
	Element* e=mesh->elems[i];
	if(!e)continue;
	out << i << ": " << e->n[0] << " " << e->n[1] << " " << e->n[2] << " " << e->n[3] << "(" << e->faces[0] << " " << e->faces[1] << " " << e->faces[2] << " " << e->faces[3] << ")" << endl;
    }
}

int Mesh::locate(const Point& p, int& ix, double epsilon1, double epsilon2)
{
    // Start with the initial element
    int i=ix;
    if(i<0)
	i=0;
    // Find the next valid element in the list
    while(i<elems.size() && !elems[i])i++;
    if(i>=elems.size()){
	// If we get to the end, start over...
	i=0;
	while(i<ix && i<elems.size() && !elems[i])i++;
    }
    int count=0;
    int nelems=elems.size();
    while(count++<nelems){
	Element* elem=elems[i];
#ifndef STORE_ELEMENT_BASIS
	Point p1(nodes[elem->n[0]]->p);
	Point p2(nodes[elem->n[1]]->p);
	Point p3(nodes[elem->n[2]]->p);
	Point p4(nodes[elem->n[3]]->p);
	double x1=p1.x();
	double y1=p1.y();
	double z1=p1.z();
	double x2=p2.x();
	double y2=p2.y();
	double z2=p2.z();
	double x3=p3.x();
	double y3=p3.y();
	double z3=p3.z();
	double x4=p4.x();
	double y4=p4.y();
	double z4=p4.z();
	double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
	double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
	double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
	double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
	double iV6=1./(a1+a2+a3+a4);

	double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
	double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
	double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
	double s0=iV6*(a1+b1*p.x()+c1*p.y()+d1*p.z());

	double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
	double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
	double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
	double s1=iV6*(a2+b2*p.x()+c2*p.y()+d2*p.z());

	double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
	double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
	double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
	double s2=iV6*(a3+b3*p.x()+c3*p.y()+d3*p.z());

	double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
	double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
	double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
	double s3=iV6*(a4+b4*p.x()+c4*p.y()+d4*p.z());
#else
	double s0=elem->a[0]+Dot(elem->g[0], p);
	double s1=elem->a[1]+Dot(elem->g[1], p);
	double s2=elem->a[2]+Dot(elem->g[2], p);
	double s3=elem->a[3]+Dot(elem->g[3], p);
#endif
	int f=0;
	double min=s0;
	if(s1<min){
	    min=s1;
	    f=1;
	}
	if(s2<min){
	    min=s2;
	    f=2;
	}
	if(s3<min){
	    min=s3;
	    f=3;
	}
	if(min<-epsilon1){
	    int ni=elem->face(f);
#if 0
	    if(i==-1){
		cerr << "Boundary, min=" << min << endl;
		min=s0;
		f=0;
		if(s1<min && elem->face(1)!=-1){
		    min=s1;
		    f=1;
		}
		if(s2<min && elem->face(2)!=-1){
		    min=s2;
		    f=2;
		}
		if(s3<min && elem->face(3)!=-1){
		    min=s3;
		    f=3;
		}
		if(min<-1.e-6){
		    i=elem->face(f);
	            if(i != -1)
		         continue;
	        }
		return 0;
	    }
#endif
	    if(ni==-1){
		ix=i;
#if 0
		if(min < -epsilon2){
		    cerr << "Boundary, min=" << min << endl;
		}
#endif
		return min<-epsilon2?0:1;
	    }
	    i=ni;
	    continue;
	}
	ix=i;
	return 1;
    }
    return 0;
}

int Mesh::locate2(const Point& p, int& ix, double epsilon1)
{
    // Exhaustive search
    int nelems=elems.size();
    for(int i=0;i<nelems;i++){
	Element* elem=elems[i];
	if(!elem)
	  continue;
#ifndef STORE_ELEMENT_BASIS
	Point p1(nodes[elem->n[0]]->p);
	Point p2(nodes[elem->n[1]]->p);
	Point p3(nodes[elem->n[2]]->p);
	Point p4(nodes[elem->n[3]]->p);
	double x1=p1.x();
	double y1=p1.y();
	double z1=p1.z();
	double x2=p2.x();
	double y2=p2.y();
	double z2=p2.z();
	double x3=p3.x();
	double y3=p3.y();
	double z3=p3.z();
	double x4=p4.x();
	double y4=p4.y();
	double z4=p4.z();
	double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
	double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
	double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
	double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
	double iV6=1./(a1+a2+a3+a4);

	double b1=-(y3*z4-y4*z3)-(y4*z2-y2*z4)-(y2*z3-y3*z2);
	double c1=+(x3*z4-x4*z3)+(x4*z2-x2*z4)+(x2*z3-x3*z2);
	double d1=-(x3*y4-x4*y3)-(x4*y2-x2*y4)-(x2*y3-x3*y2);
	double s0=iV6*(a1+b1*p.x()+c1*p.y()+d1*p.z());

	double b2=+(y4*z1-y1*z4)+(y1*z3-y3*z1)+(y3*z4-y4*z3);
	double c2=-(x4*z1-x1*z4)-(x1*z3-x3*z1)-(x3*z4-x4*z3);
	double d2=+(x4*y1-x1*y4)+(x1*y3-x3*y1)+(x3*y4-x4*y3);
	double s1=iV6*(a2+b2*p.x()+c2*p.y()+d2*p.z());

	double b3=-(y1*z2-y2*z1)-(y2*z4-y4*z2)-(y4*z1-y1*z4);
	double c3=+(x1*z2-x2*z1)+(x2*z4-x4*z2)+(x4*z1-x1*z4);
	double d3=-(x1*y2-x2*y1)-(x2*y4-x4*y2)-(x4*y1-x1*y4);
	double s2=iV6*(a3+b3*p.x()+c3*p.y()+d3*p.z());

	double b4=+(y2*z3-y3*z2)+(y3*z1-y1*z3)+(y1*z2-y2*z1);
	double c4=-(x2*z3-x3*z2)-(x3*z1-x1*z3)-(x1*z2-x2*z1);
	double d4=+(x2*y3-x3*y2)+(x3*y1-x1*y3)+(x1*y2-x2*y1);
	double s3=iV6*(a4+b4*p.x()+c4*p.y()+d4*p.z());
#else
	double s0=elem->a[0]+Dot(elem->g[0], p);
	double s1=elem->a[1]+Dot(elem->g[1], p);
	double s2=elem->a[2]+Dot(elem->g[2], p);
	double s3=elem->a[3]+Dot(elem->g[3], p);
#endif
	double min=s0;
	if(s1<min){
	    min=s1;
	}
	if(s2<min){
	    min=s2;
	}
	if(s3<min){
	    min=s3;
	}
	if(min>-epsilon1){
	  ix=i;
	  return 1;
	}
    }
    return 0;
}

void* Element::operator new(size_t)
{
    return Element_alloc.alloc();
}

void Element::operator delete(void* rp, size_t)
{
    Element_alloc.free(rp);
}

void* Node::operator new(size_t)
{
    return Node_alloc.alloc();
}

void Node::operator delete(void* rp, size_t)
{
    Node_alloc.free(rp);
}

int Element::orient()
{
    double sgn=volume();
    if(sgn< 0.0){
	// Switch two of the edges so that the volume is positive
	int tmp=n[0];
	n[0]=n[1];
	n[1]=tmp;
	tmp=faces[0];
	faces[0]=faces[1];
	faces[1]=tmp;
	compute_basis();
	sgn=-sgn;
    }
    if(sgn < 1.e-9){
//	return 0; // Degenerate...
	cerr << "Warning - small element, volume=" << sgn << endl;
    }
    return 1;
}

double Element::volume()
{
    Point p1(mesh->nodes[n[0]]->p);
    Point p2(mesh->nodes[n[1]]->p);
    Point p3(mesh->nodes[n[2]]->p);
    Point p4(mesh->nodes[n[3]]->p);
    double x1=p1.x();
    double y1=p1.y();
    double z1=p1.z();
    double x2=p2.x();
    double y2=p2.y();
    double z2=p2.z();
    double x3=p3.x();
    double y3=p3.y();
    double z3=p3.z();
    double x4=p4.x();
    double y4=p4.y();
    double z4=p4.z();
    double a1=+x2*(y3*z4-y4*z3)+x3*(y4*z2-y2*z4)+x4*(y2*z3-y3*z2);
    double a2=-x3*(y4*z1-y1*z4)-x4*(y1*z3-y3*z1)-x1*(y3*z4-y4*z3);
    double a3=+x4*(y1*z2-y2*z1)+x1*(y2*z4-y4*z2)+x2*(y4*z1-y1*z4);
    double a4=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);
    return (a1+a2+a3+a4)/6.;
}

void Mesh::get_bounds(Point& min, Point& max)
{
    min=nodes[0]->p;
    max=nodes[0]->p;
    for(int i=1;i<nodes.size();i++){
	min=Min(min, nodes[i]->p);
	max=Max(max, nodes[i]->p);
    }
}    

void* Face::operator new(size_t)
{
    return Face_alloc.alloc();
}

void Face::operator delete(void* rp, size_t)
{
    Face_alloc.free(rp);
}

Face::Face(int n0, int n1, int n2)
{
    n[0]=n0;
    n[1]=n1;
    n[2]=n2;
    if(n[0] < n[1]){
	int tmp=n[0]; n[0]=n[1]; n[1]=tmp;
    }
    if(n[0] < n[2]){
	int tmp=n[0]; n[0]=n[2]; n[2]=tmp;
    }
    if(n[1] < n[2]){
	int tmp=n[1]; n[1]=n[2]; n[2]=tmp;
    }
    hash=(n[0]*7+5)^(n[1]*5+3)^(n[2]*3+1);
}

Edge::Edge()
{
}

Edge::Edge(int n0, int n1)
{
    if (n0 < n1)
    {
	n[0] = n0;
	n[1] = n1;
    }
    else
    {
	n[0] = n1;
	n[1] = n0;
    }
}

int Edge::hash(int hash_size) const
{
    return (((n[0]*7+5)^(n[1]*5+3))^(3*hash_size+1))%hash_size;
}

int Edge::operator==(const Edge& e) const
{
    return n[0]==e.n[0] && n[1]==e.n[1];
}

int Mesh::face_idx(int p, int f)
{
    Element* e=elems[p];
    int n=e->faces[f];
    if(n==-1)
	return -1;
    Element* ne=elems[n];
    for(int i=0;i<4;i++){
	if(ne->faces[i]==p){
	    return (n<<2)|i;
	}
    }
    cerr << "face_idx confused!\n";
    cerr << "p=" << p << endl;
    cerr << "f=" << f << endl;
    cerr << "faces: " << e->faces[0] << " " << e->faces[1] << " " << e->faces[2] << " " << e->faces[3] << endl;
    cerr << "nfaces: " << ne->faces[0] << " " << ne->faces[1] << " " << ne->faces[2] << " " << ne->faces[3] << endl;
    return 0;
}

int Mesh::insert_delaunay(const Point& p, GeometryOPort* ogeom)
{
    int idx=nodes.size();
    nodes.add(new Node(p));
    return insert_delaunay(idx, ogeom);
}

MaterialHandle ptmatl(scinew Material(Color(0,0,0), Color(1,1,0), Color(.6,.6,.6), 10));
MaterialHandle inmatl(ptmatl);
MaterialHandle remmatl(scinew Material(Color(0,0,0), Color(1,0,0), Color(.6, .6, .6), 10));
MaterialHandle facematl(scinew Material(Color(0,0,0), Color(1,0,1), Color(.6, .6, .6), 10));
MaterialHandle circummatl(scinew Material(Color(0,0,0), Color(0, 1, 1), Color(.6, .6, .6), 10));
MaterialHandle lamatl(scinew Material(Color(0,0,0), Color(0, 0, 1), Color(.6, .6, .6), 10));


void Mesh::draw_element(Element* e, GeomGroup* group)
{
    Point p1(nodes[e->n[0]]->p);
    Point p2(nodes[e->n[1]]->p);
    Point p3(nodes[e->n[2]]->p);
    Point p4(nodes[e->n[3]]->p);
    GeomPolyline* poly=new GeomPolyline;
    poly->add(p1);
    poly->add(p2);
    poly->add(p3);
    poly->add(p4);
    poly->add(p1);
    poly->add(p3);
    poly->add(p2);
    poly->add(p4);
    group->add(poly);
}

void Mesh::draw_element(int in_element, GeomGroup* group)
{
    Element* e=elems[in_element];
    draw_element(e, group);
}

int Mesh::insert_delaunay(int node, GeometryOPort*)
{
    if(!have_all_neighbors)
	compute_face_neighbors();
    Point p(nodes[node]->p);

    // Start the element search at the last added element...
    int in_element=elems.size()-1;
    while(!elems[in_element] && in_element>0)
	in_element--;
    if(!locate(p, in_element)){
      if(!locate2(p, in_element)){
        cerr << "Error locating point: " << p << endl;
	return 0;
      }
    }


    Array1<int> to_remove;
    to_remove.add(in_element);

    // Find it's neighbors...
    current_generation++;
    elems[in_element]->generation=current_generation;

    FastHashTable<DFace> face_table;
    int i=0;
    while(i<to_remove.size()){
	// See if the neighbor should also be removed...
	int tr=to_remove[i];
	Element* e=elems[tr];
	if(!e){
	    cerr << "Removing a zero element!!!!!!!!!!!!!\n\n\n\n";
	}

	for(int j=0;j<4;j++){
	    // Add these faces to the list of exposed faces...
	    DFace* f=new DFace(e->n[(j+1)%4], e->n[(j+2)%4], e->n[(j+3)%4]);
	
	    // If the face is in the list, remove it.
	    // Otherwise, add it.
	    DFace* dummy;
	    if(face_table.lookup(f, dummy)){
		face_table.remove(f);
		delete f;
	    } else {
		f->face_idx=face_idx(tr, j);
		face_table.insert(f);
	    }

	    int neighbor=e->faces[j];
	    if(neighbor != -1){
		Element* ne=elems[neighbor];
		if(!ne){
		    cerr << endl;
		    cerr << "neighbor=" << neighbor << endl;
		    cerr << "is a neighbor of " << tr << endl;
		    cerr << "node=" << node << endl;
		    cerr << "WHAT!!!!!!!!!!\n";
		}
		if(ne->generation != current_generation){
		    Point cen;
		    double rad2;
		    double err;
		    ne->get_sphere2(cen, rad2, err);
		    double ndist2=(p-cen).length2();
		    if(ndist2 < rad2-err){
			// This one must go...
			to_remove.add(neighbor);
		    }
		}
		ne->generation=current_generation;
	    }
	}
	i++;
    }
    for(i=0;i<to_remove.size();i++){
	int tr=to_remove[i];
//	cerr << "Removing: " << tr << endl;
	delete elems[tr];
	elems[tr]=0;
    }

    //int start_new=elems.size();

    // Add the new elements from the faces...
    FastHashTableIter<DFace> fiter(&face_table);
    
    // Make a copy of the face table.  We use the faces in there
    // To compute the new neighborhood information
    FastHashTable<DFace> new_faces(face_table);

    for(fiter.first();fiter.ok();++fiter){
	DFace* f=fiter.get_key();
	Element* ne=new Element(this, node, f->n[0], f->n[1], f->n[2]);
	
	// If the new element is not degenerate, add it to the mix...
	if(ne->orient()){
	    int nen=elems.size();
	    for(int j=0;j<4;j++){
		// Add these faces to the list of exposed faces...
		DFace* f=new DFace(ne->n[(j+1)%4], ne->n[(j+2)%4], ne->n[(j+3)%4]);

		DFace* ef;
		if(new_faces.lookup(f, ef)){
		    // We have this face...
		    if(ef->face_idx==-1){
			ne->faces[j]=-1; // Boundary
		    } else {
			int which_face=ef->face_idx%4;
			int which_elem=ef->face_idx>>2;
			ne->faces[j]=which_elem;
			elems[which_elem]->faces[which_face]=nen;
		    }
		    new_faces.remove(f);
		    delete f;
		} else {
		    f->face_idx=(nen<<2)|j;
		    new_faces.insert(f);
		    ne->faces[j]=-3;
		}
	    }
	    elems.add(ne);
	} else {
	    cerr << "Degenerate element (node=" << node << ")\n";
	    cerr << "Volume=" << ne->volume() << endl;
	    return 0;
	}
    }

    
    // Go through and look for small elements - try to get rid of
    // them by swapping an edge
#if 0
    for(int degen=start_new;degen<elems.size();degen++){
	Element* e=elems[degen];
	if(e->volume() < 1.e-10){
	    cerr << "Crap, element " << degen << " is degenerate...\n";
	    // Hmmm...
	    int d1a=0;
	    int d1b=-999;
	    double d1l=0;
	    Point p0=nodes[e->n[0]]->p;
	    for(int i=1;i<4;i++){
		double l=(nodes[e->n[i]]->p-p0).length2();
		if(l>d1l){
		    d1l=l;
		    d1b=i;
		}
	    }
	    int d2a=1;
	    while(d2a == d1b)d2a++;
	    int d2b=1;
	    while(d2b == d1b || d2b == d2a)d2b++;
	    double d2l=(nodes[e->n[d2a]]->p - nodes[e->n[d2b]]->p).length2();
	    cerr << "d1l=" << d1l << ", d2l=" << d2l << endl;
	    int a, b;
	    int c, d;
	    if(d1l < d2l){
		a=d1a;
		b=d1b;
		c=d2a;
		d=d2b;
	    } else {
		a=d2a;
		b=d2b;
		c=d1a;
		d=d1b;
	    }

	    cerr << "degen=" << degen << endl;
	    cerr << "a,b,c,d=" << a << " " << b << " " << c << " " << d << endl;
	    cerr << "nodes of degen are: " << e->n[a] << " " << e->n[b] << " " << e->n[c] << " " << e->n[d] << endl;

	    int e1=e->faces[a];
	    if(e1 != -1){
		Element* ee1=elems[e1];
		int j1;
		for(j1=0;j1<4;j1++){
		    if(ee1->faces[j1] == degen)
			break;
		}
		cerr << "ee1=" << ee1->n[0] << " " << ee1->n[1] << " " << ee1->n[2] << " " << ee1->n[3] << endl;
		ASSERT(j1<4);
		int n1=ee1->n[j1];

		int e2=e->faces[b];
		if(e2 != -1){
		    Element* ee2=elems[e2];
		    cerr << "ee2=" << ee2->n[0] << " " << ee2->n[1] << " " << ee2->n[2] << " " << ee2->n[3] << endl;
		    int j2;
		    for(j2=0;j2<4;j2++){
			if(ee2->faces[j2] == degen)
			    break;
		    }
		    ASSERT(j2<4);
		    int n2=ee2->n[j2];

		    if(n1 == n2){
			ee1->n[(j1+1)%4]=a;
			ee1->n[(j1+2)%4]=b;
			ee1->n[(j1+3)%4]=c;
			ee2->n[(j2+1)%4]=a;
			ee2->n[(j2+2)%4]=b;
			ee2->n[(j2+3)%4]=d;

			// Fix up face neighbors...
			ee2->faces[j2]=e1;
			ee1->faces[j1]=e2;
			
			// Let the face code figure out the other ones
			ee1->faces[(j1+1)%4]=-2;
			ee1->faces[(j1+2)%4]=-2;
			ee1->faces[(j1+3)%4]=-2;
			ee2->faces[(j2+1)%4]=-2;
			ee2->faces[(j2+2)%4]=-2;
			ee2->faces[(j2+3)%4]=-2;
			int ii;
			for(ii=0;ii<4;ii++){
			    int nn=elems[degen]->n[ii];
			    Node* n=nodes[nn].get_rep();
			    for(int j=0;j<n->elems.size();j++){
				if(n->elems[j] == degen){
				    n->elems.remove(j);
				    break;
				}
			    }
			}
			for(ii=0;ii<4;ii++){
			    ee1->face(ii);
			    ee2->face(ii);
			}
			ee1->orient();
			ee2->orient();
			cerr << "after, ee1=" << ee1->n[0] << " " << ee1->n[1] << " " << ee1->n[2] << " " << ee1->n[3] << endl;
			cerr << "after, ee2=" << ee2->n[0] << " " << ee2->n[1] << " " << ee2->n[2] << " " << ee2->n[3] << endl;
			delete elems[degen];
			elems[degen]=0;
		    } else {
			cerr << "Sorry, can't fix this one..." << endl;
		    }
		} else {
		    cerr << "Can't fix because it is on the edge, case 2\n";
		}
	    } else {
		cerr << "Can't fix because it is on the edge, case 1\n";
	    }
	}
    }
#endif

#if 0
    {
	int nelems=elems.size();
	for(int i=0;i<nelems;i++){
	    Element* e=elems[i];
	    if(e){
		if(e->volume() < 1.e-5){
		    cerr << "Degenerate... (" << i << ") volume=" << e->volume() << endl;
		}
		Point cen;
		double rad2;
		double err;
		e->get_sphere2(cen, rad2, err);
		for(int ii=0;ii<node;ii++){
		    Point p(nodes[ii]->p);
		    double ndist2=(p-cen).length2();
		    if(ndist2 < rad2-1.e-6){
			cerr << "Invalid tesselation!\n";
			cerr << "ndist2=" << ndist2 << endl;
			cerr << "rad2=" << rad2 << endl;
			cerr << "err=" << err << endl;
		    }
		}
	    }
	}
    }
    if(node > 13800){
	for(int i=0;i<elems.size();i++){
	    Element* e=elems[i];
	    if(e){
		for(int j=0;j<4;j++){
		    int face=e->faces[j];
		    if(face != -1){
			if(elems[face] == 0){
			    cerr << "FACE MESSED UP!!!!!!!!!\n";
			    cerr << "face=" << face << endl;
			    cerr << "element=" << i << endl;
			    cerr << "node=" << node << endl;
			    cerr << endl;
			} else {
			    Element* ne=elems[face];
			    for(int jj=0;jj>4;jj++){
				if(ne->faces[jj] == i)
				    break;
			    }
			    if(jj==4){
				cerr << "Inconsistent neighbor information!!!!!!!\n";
				cerr << "element=" << i << endl;
				cerr << "neighbor=" << face << endl;
				cerr << endl;
			    }
			}

		    }
		}
	    }
	}
    }
#endif

    return 1;
}

void Mesh::pack_elems()
{
    // Pack the elements...
    int nelems=elems.size();
    int idx=0;
    Array1<int> map(nelems);
    int i;
    for(i=0;i<nelems;i++){
	Element* e=elems[i];
	if(e){
	    map[i]=idx;
	    elems[idx++]=e;
	} else {
	    map[i]=-1234;
	}
    }
    elems.resize(idx);
    int nnodes=nodes.size();
    for(i=0;i<nnodes;i++){
	NodeHandle&  n=nodes[i];
	if(n.get_rep()){
	    int ne=n->elems.size();
	    for(int j=0;j<ne;j++){
		int elem=n->elems[j];
		int new_elem=map[elem];
		if(new_elem == -1234)
		    cerr << "Warning: pointing to old element: " << elem << endl;
		n->elems[j]=new_elem;
	    }
	}
    }
    for(i=0;i<elems.size();i++){
	Element* e=elems[i];
	for(int j=0;j<4;j++){
	    int face=e->faces[j];
	    if(face>=0){
		if(map[face] == -1234)
		    cerr << "Warning: pointing to old element: " << e->faces[j] << endl;
		e->faces[j]=map[face];
	    }
	}
    }
}

void Mesh::pack_nodes()
{
    // Pack the elements...
    int nnodes=nodes.size();
    int idx=0;
    Array1<int> map(nnodes);
    int i;
    for(i=0;i<nnodes;i++){
	NodeHandle& n=nodes[i];
	if(n.get_rep()){
	    map[i]=idx;
	    nodes[idx++]=n;
	} else {
	    map[i]=-1234;
	}
    }
    nodes.resize(idx);
    int nelems=elems.size();
    for(i=0;i<nelems;i++){
	Element* e=elems[i];
	if(e){
	    for(int j=0;j<4;j++){
		if(map[e->n[j]]==-1234)
		    cerr << "Warning: pointing to old node: " << e->n[j] << endl;
		e->n[j]=map[e->n[j]];
	    }
	}
    }
}

void Mesh::pack_all()
{
    compute_neighbors();
    pack_nodes();
    pack_elems();
}

void Mesh::remove_delaunay(int node, int fill)
{
    if(!fill){
	NodeHandle& n=nodes[node];
	for(int i=0;i<n->elems.size();i++){
	    if(elems[n->elems[i]]){
		delete elems[n->elems[i]];
		elems[n->elems[i]]=0;
	    }
	}
	nodes[node]=0;
    } else {
	NOT_FINISHED("Mesh::remove_delaunay");
    }
}

void Mesh::compute_face_neighbors()
{
    // This could probably be done much faster...
    for(int i=0;i<elems.size();i++){
	if(elems[i]){
	    for(int j=0;j<4;j++){
		elems[i]->face(j);
	    }
	}
    }
    have_all_neighbors=1;
}

static void heapify(int* data, int n, int i)
{
    int l=2*i+1;
    int r=l+1;
    int largest=i;
    if(l<n && data[l] > data[i])
	largest=l;
    if(r<n && data[r] > data[largest])
	largest=r;
    if(largest != i){
	int tmp=data[i];
	data[i]=data[largest];
	data[largest]=tmp;
	heapify(data, n, largest);
    }
}

void Mesh::add_node_neighbors(int node, Array1<int>& idx, int apBC)
{
    NodeHandle& n=nodes[node];
    int ne=n->elems.size();
    Array1<int> neighbor_nodes(4*ne+1);
    int nodesi=0;
    // Gather all of the nodes
    int i;
    for(i=0;i<ne;i++){
	int ei=n->elems[i];
	Element* e=elems[ei];
	for(int j=0;j<4;j++){
	    int n=e->n[j];
	    if(!nodes[n]->bc || !apBC)
		neighbor_nodes[nodesi++]=n;
	}
    }
    // Sort it...
    // Build the heap...
    int* data=&neighbor_nodes[0];
    for(i=nodesi/2-1;i >= 0;i--){
	heapify(data, nodesi, i);
    }
    // Sort
    for(i=nodesi-1;i>0;i--){
	// Exchange 1 and i
	int tmp=data[i];
	data[i]=data[0];
	data[0]=tmp;
	heapify(data, i, 0);
    }


    // Find the unique set...
    for(i=0;i<nodesi;i++){
	if(i==0 || neighbor_nodes[i] != neighbor_nodes[i-1])
	    idx.add(neighbor_nodes[i]);
    }
}

DirichletBC::DirichletBC(const SurfaceHandle& fromsurf, double value)
: fromsurf(fromsurf), value(value)
{
}

void Mesh::get_boundary_lines(Array1<Point>&)
{
    NOT_FINISHED("Mesh::get_boundary_lines");
}

#ifdef __GNUG__

#include <Classlib/LockingHandle.cc>
template class LockingHandle<Node>;
template class LockingHandle<Mesh>;

#include <Classlib/Array1.cc>
template class Array1<NodeVersion1>;
template class Array1<int>;
template class Array1<NodeHandle>;
template class Array1<Element*>;
template class Array1<Array1<double> >;
template void Pio(Piostream&, MeshHandle&);
template void Pio(Piostream&, Array1<Element*>&);
template void Pio(Piostream&, Array1<NodeVersion1>&);
template void Pio(Piostream&, Array1<NodeHandle>&);
template void Pio(Piostream&, NodeHandle&);

#include <Classlib/FastHashTable.cc>
template class FastHashTable<DFace>;
template class FastHashTableIter<DFace>;

#endif

#ifdef __sgi
#if _MIPS_SZPTR == 64
#include <Classlib/Array1.cc>

static void _dummy_(Piostream& p1, Array1<Element*>& p2)
{
    Pio(p1, p2);
}

static void _dummy_(Piostream& p1, Array1<NodeHandle>& p2)
{
    Pio(p1, p2);
}

static void _dummy_(Piostream& p1, Array1<NodeVersion1>& p2)
{
    Pio(p1, p2);
}

#endif
#endif

