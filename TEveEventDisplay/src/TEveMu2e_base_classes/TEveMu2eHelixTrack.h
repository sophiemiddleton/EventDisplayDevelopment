#ifndef TEveMu2eHelixTrack_h
#define TEveMu2eHelixTrack_h

#include <TObject.h>
#include <THelix.h>
#include <TPolyLine3D.h>
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include <TEveTrackPropagator.h>

namespace mu2e {

class   TEveMu2eHelixTrack: public TEveTrack { //public THelix

    KalSeed *fKalSeed;

  public:
     #ifndef __CINT__
    explicit  TEveMu2eHelixTrack(){};
    virtual ~ TEveMu2eHelixTrack(){};
    #endif
    void DrawHelixTrack();
    XYZVec Postion;
    XYZVec Direction;
    int pdg_code;
    ClassDef(  TEveMu2eHelixTrack, 0);
};
}
#endif
