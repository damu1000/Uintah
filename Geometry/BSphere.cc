
/*
 *  BSphere.cc: Bounding Sphere's
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   December 1994
 *
 *  Copyright (C) 1994 SCI Group
 */

#include <Geometry/BSphere.h>
#include <Classlib/Assert.h>
#include <Geometry/Ray.h>

BSphere::BSphere()
: have_some(0)
{
}

BSphere::~BSphere()
{
}

BSphere::BSphere(const BSphere& copy)
: have_some(copy.have_some), cen(copy.cen), rad(copy.rad), rad2(copy.rad2)
{
}

void BSphere::reset()
{
    have_some=0;
}

double BSphere::volume()
{
    return 4./3.*M_PI*rad*rad*rad;
}

void BSphere::extend(const BSphere& s)
{
    ASSERT(s.have_some);
    extend(cen, rad);
}

void BSphere::extend(const Point& ncen, double nrad)
{
    Vector dv=ncen-cen;
    double d=dv.length();
    if(!have_some){
	cen=ncen;
	rad=nrad;
	rad2=rad*rad;
    } else if(rad > d+nrad){
	// This one is big enough...
    } else if(nrad > d+rad){
	// Use s's BV
	cen=ncen;
	rad=nrad;
	rad2=rad*rad;
    } else {
	// Union
	cen=cen+dv*0.5;
	rad=(rad+nrad)*0.5;
	rad2=rad*rad;
    }
}

int BSphere::intersect(const Ray& ray)
{
    Vector OC(cen-ray.origin());
    double tca=Dot(OC, ray.direction());
    double l2oc=OC.length2();
    double radius_sq=rad*rad;
    if(l2oc > radius_sq){
	if(tca >= 0.0){
	    double t2hc=radius_sq-l2oc+tca*tca;
	    if(t2hc > 0.0){
		return 1;
	    }
	}
    }
    return 0;
}
