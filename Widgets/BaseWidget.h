
/*
 *  BaseWidget.h
 *
 *  Written by:
 *   James Purciful
 *   Department of Computer Science
 *   University of Utah
 *   Aug. 1994
 *
 *  Copyright (C) 1994 SCI Group
 */


#ifndef SCI_project_Base_Widget_h
#define SCI_project_Base_Widget_h 1

#include <Constraints/manifest.h>
#include <Constraints/BaseConstraint.h>
#include <Constraints/ConstraintSolver.h>
#include <Geometry/Point.h>
#include <Geometry/Vector.h>
#include <Geom/Geom.h>
#include <Geom/Group.h>
#include <Geom/Material.h>
#include <Geom/Pick.h>
#include <Geom/Switch.h>

class CrowdMonitor;
class Module;

class BaseWidget {
public:
   BaseWidget( Module* module, CrowdMonitor* lock,
	       const Index vars, const Index cons,
	       const Index geoms, const Index picks,
	       const Index modes, const Index switches,
	       const Real widget_scale );
   BaseWidget( const BaseWidget& );
   virtual ~BaseWidget();

   void SetScale( const Real scale );
   double GetScale() const;

   void SetEpsilon( const Real Epsilon );

   GeomSwitch* GetWidget();

   virtual void MoveDelta( const Vector& delta )=0;
   virtual Point ReferencePoint() const = 0;
   
   int GetState();
   void SetState( const int state );

   // This rotates through the "minimizations" or "gaudinesses" for the widget.
   // Doesn't have to be overloaded.
   virtual void NextMode();
   Index GetMode() const;

   inline Point GetPointVar( const Index vindex ) const;
   inline Real GetRealVar( const Index vindex ) const;
   
   void execute();

   virtual void geom_pick(int);
   virtual void geom_release(int);
   virtual void geom_moved(int, double, const Vector&, int)=0;

   BaseWidget& operator=( const BaseWidget& );
   int operator==( const BaseWidget& );

   void print( ostream& os=cout ) const;

protected:
   Module* module;
   CrowdMonitor* lock;

   ConstraintSolver* solve;
   
   virtual void widget_execute()=0;
   Index NumVariables;
   Index NumConstraints;
   Index NumGeometries;
   Index NumPicks;

   Array1<BaseConstraint*> constraints;
   Array1<BaseVariable*> variables;
   Array1<GeomObj*> geometries;
   Array1<GeomPick*> picks;

   enum {Mode0,Mode1,Mode2,Mode3,Mode4,Mode5,Mode6,Mode7,Mode8,Mode9};
   Index NumModes;
   Index NumSwitches;
   Array1<long> modes;
   Array1<GeomSwitch*> mode_switches;
   Index CurrentMode;
   // modes contains the bitwise OR of Switch0-Switch8
   const long Switch0 = 0x0001;
   const long Switch1 = 0x0002;
   const long Switch2 = 0x0004;
   const long Switch3 = 0x0008;
   const long Switch4 = 0x0010;
   const long Switch5 = 0x0020;
   const long Switch6 = 0x0040;
   const long Switch7 = 0x0080;
   const long Switch8 = 0x0100;

   MaterialHandle PointMaterial;
   MaterialHandle EdgeMaterial;
   MaterialHandle SliderMaterial;
   MaterialHandle ResizeMaterial;
   MaterialHandle SpecialMaterial;
   MaterialHandle HighlightMaterial;

   GeomSwitch* widget;
   Real widget_scale;
   Real epsilon;

   void CreateModeSwitch( const Index snum, GeomObj* o );
   void SetMode( const Index mode, const long swtchs );
   void FinishWidget();

protected:
   // These affect ALL widgets!!!
   static MaterialHandle PointWidgetMaterial;
   static MaterialHandle EdgeWidgetMaterial;
   static MaterialHandle SliderWidgetMaterial;
   static MaterialHandle ResizeWidgetMaterial;
   static MaterialHandle SpecialWidgetMaterial;
   static MaterialHandle HighlightWidgetMaterial;
};

inline ostream& operator<<( ostream& os, BaseWidget& w );


inline ostream&
operator<<( ostream& os, BaseWidget& w )
{
   w.print(os);
   return os;
}


inline Point
BaseWidget::GetPointVar( const Index vindex ) const
{
   ASSERT(vindex<NumVariables);

   return variables[vindex]->point();
}


inline Real
BaseWidget::GetRealVar( const Index vindex ) const
{
   ASSERT(vindex<NumVariables);

   return variables[vindex]->real();
}


#endif
