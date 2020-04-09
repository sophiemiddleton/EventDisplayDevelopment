#ifndef TEveMu2eBField_h
#define TEveMu2eBField_h

#include <TObject.h>
#include <TEveTrackPropagator.h>
#include <TEveVector.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
#include "BFieldGeom/inc/BFieldConfig.hh"
#include "BFieldGeom/inc/BFieldManager.hh"
#include "BeamlineGeom/inc/Beamline.hh"
#pragma GCC diagnostic pop

namespace mu2e {

  class TEveMu2eBField: public TEveMagField { //TEveMagField is defined in TrackPropagator

    Beamline*           fBeamline;
    BFieldConfig*       fBfc;
    BFieldManager*      fBmgr;
    
  public:
    #ifndef __CINT__
    explicit TEveMu2eBField(){};
    virtual ~TEveMu2eBField(){};
    #endif
    //TEveVector GetField(float X, float Y, float Z) const ;

    ClassDef(TEveMu2eBField, 0);
};
}
#endif
