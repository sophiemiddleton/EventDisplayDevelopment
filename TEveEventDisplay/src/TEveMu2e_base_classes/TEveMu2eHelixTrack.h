#ifndef TEveMu2eHelixTrack_h
#define TEveMu2eHelixTrack_h

#include <TObject.h>
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include <TEveTrackPropagator.h>

namespace mu2e {

class   TEveMu2eHelixTrack: public TEveTrack {

    KalSeed *fKalSeed;

  public:
     #ifndef __CINT__
    explicit  TEveMu2eHelixTrack(){};
    //TEveHelixTrack(double x1, double y1, double z1, double t1, double x2, double y2, double z2, double t2)
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
