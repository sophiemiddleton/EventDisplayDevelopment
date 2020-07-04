#ifndef TEveMu2eCustomHelix_h
#define TEveMu2eCustomHelix_h

#include <TObject.h>
#include <THelix.h>
#include <TPolyLine3D.h>
#include "RecoDataProducts/inc/HelixSeed.hh"
#include "RecoDataProducts/inc/KalSeed.hh"

#include <TEveTrackPropagator.h>

namespace mu2e {
  class   TEveMu2eCustomHelix: public TEveLine {
    public:
       #ifndef __CINT__
      explicit  TEveMu2eCustomHelix(){};
      TEveMu2eCustomHelix(HelixSeed hseed){fHelixSeed = hseed;};
      virtual ~ TEveMu2eCustomHelix(){};
      #endif
      
      KalSeed fKalSeed;
      HelixSeed fHelixSeed;

      void DrawHelixTrack();
      void Draw2DProjection();

      void SetPostionAndDirectionFromHelixSeed(double zpos){
        fHelixSeed.helix().position(Position);
        fHelixSeed.helix().direction(zpos, Direction);
      }

      void SetPostionAndDirectionFromKalRep(double zpos){
        fKalSeed.helix()->helix().position(Position);
        fKalSeed.helix()->helix().direction(zpos, Direction);
      }

      double GetFlightLen(){
        return fKalSeed.flt0();
      }

      void SetMomentum(){
        this->Momentum = fKalSeed.helix()->helix().momentum();
      }

      void SetParticle(){
        this->PDGcode = fKalSeed.particle().particleType();
      }

      XYZVec Direction;
      XYZVec Position;
      double Momentum;
      int PDGcode  = 11;
      unsigned int nSteps = 100;
      double TrackerLength = 300.8;//cm
      int kStepSize = nSteps/TrackerLength;//cm
      ClassDef( TEveMu2eCustomHelix, 0);
    };
}
#endif
