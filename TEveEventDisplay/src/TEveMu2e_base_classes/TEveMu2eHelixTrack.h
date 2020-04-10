#ifndef TEveMu2eHelixTrack_h
#define TEveMu2eHelixTrack_h

#include <TObject.h>
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include <TEveTrackPropagator.h>

namespace mu2e {

class   TEveMu2eHelixTrack: public TEveTrack, TEveTrackPropagator, public KalSeed {

    KalSeed *fKalSeed;

  public:
     #ifndef __CINT__
    explicit  TEveMu2eHelixTrack(){};
    virtual ~ TEveMu2eHelixTrack(){};
    #endif
    void DrawHelixTrack();
    void GetParam();
    void GetBField();
    ClassDef(  TEveMu2eHelixTrack, 0);
};
}
#endif
