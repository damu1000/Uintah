
/*
 *  RingWidget.h
 *
 *  Written by:
 *   James Purciful
 *   Department of Computer Science
 *   University of Utah
 *   Jan. 1995
 *
 *  Copyright (C) 1995 SCI Group
 */


#ifndef SCI_project_Ring_Widget_h
#define SCI_project_Ring_Widget_h 1

#include <Widgets/BaseWidget.h>


class RingWidget : public BaseWidget {
public:
   RingWidget( Module* module, CrowdMonitor* lock, double widget_scale );
   RingWidget( const RingWidget& );
   ~RingWidget();

   virtual void widget_execute();
   virtual void geom_moved(int, double, const Vector&, void*);

   virtual void MoveDelta( const Vector& delta );
   virtual Point ReferencePoint() const;

   void SetPosition( const Point& center, const Vector& normal, const Real radius );
   void GetPosition( Point& center, Vector& normal, Real& radius ) const;
   
   void SetRatio( const Real ratio );
   Real GetRatio() const;

   void SetRadius( const Real radius );
   Real GetRadius() const;
   
   const Vector& GetAxis1();
   const Vector& GetAxis2();
   const Vector& GetAxis3();

   // Variable indexs
   enum { PointAVar, PointBVar, PointCVar, DistVar, CenterVar,
	  SliderVar, RadiusVar, AngleVar };
   // Material indexs
   enum { PointMatl, EdgeMatl, SliderMatl, ResizeMatl, SpecialMatl, HighMatl };
private:
   Vector oldaxis1, oldaxis2, oldaxis3;
};


#endif
