
#ifndef Cylinder_H
#define Cylinder_H 1

#include <Core/Geometry/Point.h>
#include <Core/Geometry/Vector.h>
#include <Packages/rtrt/Core/Object.h>
#include <Packages/rtrt/Core/UVMapping.h>
#include <Core/Geometry/Transform.h>
#include <stdlib.h>

namespace rtrt {

using SCIRun::Vector;
using SCIRun::Point;
using SCIRun::Transform;

class Cylinder : public Object {
protected:
    Point top;
    Point bottom;
    double radius;
    Transform xform;
    Transform ixform;
public:
    Cylinder(Material* matl, const Point& bottom, const Point& top, double radius);
    virtual ~Cylinder();
    virtual void intersect(const Ray& ray, HitInfo& hit, DepthStats* st,
			   PerProcessorContext*);
    virtual void preprocess(double maxradius, int& pp_offset, int& scratchsize);
    virtual Vector normal(const Point&, const HitInfo& hit);
    virtual void compute_bounds(BBox&, double offset);
    virtual void print(ostream& out);
};

} // end namespace rtrt

#endif
