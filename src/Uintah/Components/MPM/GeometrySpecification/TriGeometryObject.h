#ifndef __TRI_GEOMETRY_OBJECT_H__
#define __TRI_GEOMETRY_OBJECT_H__

#include "GeometryObject.h"
#include <SCICore/Geometry/Point.h>
#include <Uintah/Grid/Box.h>
#include <string>

using SCICore::Geometry::Point;
using Uintah::Grid::Box;

namespace Uintah {
namespace Components {

/**************************************
	
CLASS
   GeometryObject
	
   Short description...
	
GENERAL INFORMATION
	
   GeometryObject.h
	
   John A. Schmidt
   Department of Mechanical Engineering
   University of Utah
	
   Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
	
 
	
KEYWORDS
   GeometryObject
	
DESCRIPTION
   Long description...
	
WARNING
	
****************************************/

class TriGeometryObject : public GeometryObject {
 public:
  //////////
  // Insert Documentation Here:
  TriGeometryObject(ProblemSpecP &);
  //////////
  // Insert Documentation Here:
  virtual ~TriGeometryObject();

  //////////
  // Insert Documentation Here:
  virtual bool inside(const Point &p) const;

  //////////
  // Insert Documentation Here:
  virtual Box getBoundingBox() const;

 private:
  

};

} // end namespace Components
} // end namespace Uintah

#endif // __TRI_GEOMETRY_OBJECT_H__

// $Log$
// Revision 1.4  2000/04/22 16:51:04  jas
// Put in a skeleton framework for documentation (coccoon comment form).
// Comments still need to be filled in.
//
// Revision 1.3  2000/04/20 22:37:14  jas
// Fixed up the GeometryObjectFactory.  Added findBlock() and findNextBlock()
// to ProblemSpec stuff.  This will iterate through all of the nodes (hopefully).
//
// Revision 1.2  2000/04/20 15:09:26  jas
// Added factory methods for GeometryObjects.
//
// Revision 1.1  2000/04/19 21:31:09  jas
// Revamping of the way objects are defined.  The different geometry object
// subtypes only do a few simple things such as testing whether a point
// falls inside the object and also gets the bounding box for the object.
// The constructive solid geometry objects:union,difference, and intersection
// have the same simple operations.
//
