#ifndef TEveMu2eCustomHelix_h
#define TEveMu2eCustomHelix_h

#include <TObject.h>
#include <THelix.h>
#include <TPolyLine3D.h>
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include <TEveTrackPropagator.h>

namespace mu2e {

class   TEveMu2eCustomHelix: public TEveElement, public TPolyLine3D { //public THelix

    KalSeed *fKalSeed;
    HelixSeed *fHelixSeed;
  public:
     #ifndef __CINT__
    explicit  TEveMu2eCustomHelix(){};
    TEveMu2eCustomHelix(HelixSeed *hseed){fHelixSeed = hseed;};
    virtual ~ TEveMu2eCustomHelix(){};
    #endif
    void DrawHelixTrack();
    void SetPostionAndDirectionFromHelixSeed(double zpos){
      fHelixSeed->helix().position(Position);
      fHelixSeed->helix().direction(zpos, Direction);
    }

    void SetPostionAndDirectionFromKalRep(double zpos){
      fKalSeed->helix()->helix().position(Position);
      fKalSeed->helix()->helix().direction(zpos, Direction);
    }

    double GetFlightLen(){
      return fKalSeed->flt0();
    }

    XYZVec Direction;
    XYZVec Position;
    int pdg_code;
    ClassDef( TEveMu2eCustomHelix, 0);
};
}
#endif
