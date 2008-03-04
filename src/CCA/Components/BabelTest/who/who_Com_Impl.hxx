// 
// File:          who_Com_Impl.hxx
// Symbol:        who.Com-v1.0
// Symbol Type:   class
// Babel Version: 1.2.0
// Description:   Server-side implementation for who.Com
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// 

#ifndef included_who_Com_Impl_hxx
#define included_who_Com_Impl_hxx

#ifndef included_sidl_cxx_hxx
#include "sidl_cxx.hxx"
#endif
#ifndef included_who_Com_IOR_h
#include "who_Com_IOR.h"
#endif
#ifndef included_gov_cca_Component_hxx
#include "gov_cca_Component.hxx"
#endif
#ifndef included_gov_cca_Services_hxx
#include "gov_cca_Services.hxx"
#endif
#ifndef included_sidl_BaseClass_hxx
#include "sidl_BaseClass.hxx"
#endif
#ifndef included_sidl_BaseInterface_hxx
#include "sidl_BaseInterface.hxx"
#endif
#ifndef included_sidl_ClassInfo_hxx
#include "sidl_ClassInfo.hxx"
#endif
#ifndef included_who_Com_hxx
#include "who_Com.hxx"
#endif


// DO-NOT-DELETE splicer.begin(who.Com._hincludes)
// insert code here (includes or arbitrary code)
// DO-NOT-DELETE splicer.end(who.Com._hincludes)

namespace who { 

  /**
   * Symbol "who.Com" (version 1.0)
   */
  class Com_impl : public virtual ::who::Com 
  // DO-NOT-DELETE splicer.begin(who.Com._inherits)
  // Insert-Code-Here {who.Com._inherits} (optional inheritance here)
  // DO-NOT-DELETE splicer.end(who.Com._inherits)

  {

  // All data marked protected will be accessable by 
  // descendant Impl classes
  protected:

    bool _wrapped;

    // DO-NOT-DELETE splicer.begin(who.Com._implementation)
    ::gov::cca::Services svc;
    // DO-NOT-DELETE splicer.end(who.Com._implementation)

  public:
    // default constructor, used for data wrapping(required)
    Com_impl();
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
      Com_impl( struct who_Com__object * ior ) : StubBase(ior,true), 
    ::gov::cca::Component((ior==NULL) ? NULL : &((*ior).d_gov_cca_component)) , 
      _wrapped(false) {_ctor();}


    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~Com_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

    // true if this object was created by a user newing the impl
    inline bool _isWrapped() {return _wrapped;}

    // static class initializer
    static void _load();

  public:


    /**
     *  Starts up a component presence in the calling framework.
     * @param Svc the component instance's handle on the framework world.
     * Contracts concerning Svc and setServices:
     * 
     * The component interaction with the CCA framework
     * and Ports begins on the call to setServices by the framework.
     * 
     * This function is called exactly once for each instance created
     * by the framework.
     * 
     * The argument Svc will never be nil/null.
     * 
     * Those uses ports which are automatically connected by the framework
     * (so-called service-ports) may be obtained via getPort during
     * setServices.
     */
    void
    setServices_impl (
      /* in */::gov::cca::Services& services
    )
    ;

  };  // end class Com_impl

} // end namespace who

// DO-NOT-DELETE splicer.begin(who.Com._hmisc)
// insert code here (miscellaneous things)
// DO-NOT-DELETE splicer.end(who.Com._hmisc)

#endif
