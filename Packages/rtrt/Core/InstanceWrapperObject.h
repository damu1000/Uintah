#ifndef INSTANCE_WRAPPER_OBJECT_H
#define INSTANCE_WRAPPER_OBJECT_H

#include <Packages/rtrt/Core/Object.h>
#include <Packages/rtrt/Core/Ray.h>
#include <Packages/rtrt/Core/Light.h>
#include <Core/Geometry/Vector.h>
#include <Packages/rtrt/Core/Material.h>
#include <Packages/rtrt/Core/PerProcessorContext.h>
#include <Packages/rtrt/Core/HitInfo.h>
#include <Packages/rtrt/Core/InstanceWrapperObject.h>

using namespace rtrt;

class InstanceWrapperObject {
  
 public:

  Object* obj;
  BBox bb;
  bool was_processed;
  bool computed_bbox;

  InstanceWrapperObject(Object* obj) :
    obj(obj) 
    {
      was_processed = false;
      computed_bbox = false;
    }

  void preprocess(double maxradius, int& pp_offset, int& scratchsize)
    {
      if (!was_processed) {
	obj->preprocess(maxradius,pp_offset,scratchsize);
	was_processed = true;
	if (!computed_bbox) {
	  obj->compute_bounds(bb,1E-5);
	  computed_bbox = true;
	}
      }
    }
   
  inline void intersect(const Ray& ray, HitInfo& hit, DepthStats* st,
			PerProcessorContext* ppc)
    {
      obj->intersect(ray, hit, st,ppc);
    }

  inline void light_intersect(Light* light, const Ray& ray,
			      HitInfo& hit, double dist, Color& atten,
			      DepthStats* st, PerProcessorContext* ppc)
    {
      obj->light_intersect(light,ray,hit,dist,atten,st,ppc);
    }

  void compute_bounds(BBox& bbox, double offset)
    {
      if (!computed_bbox) {
	obj->compute_bounds(bb,offset);
	computed_bbox = true;
      }
      bbox.extend(bb);
    }
};
#endif
