#ifndef TEveMu2eMCInterface_h
#define TEveMu2eMCInterface_h

//libGeom
#include <TGeoManager.h>
//TEve
#include <TEveManager.h>
#include <TEveStraightLineSet.h>
//TEveMu2e
#include "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2e2DProjection.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eCalorimeter.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eTracker.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCTraj.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eParticle.h"
#include "MCDataProducts/inc/MCTrajectoryPoint.hh"
#include "MCDataProducts/inc/CaloHitSimPartMC.hh"

namespace mu2e{
    class TEveMu2eMCInterface {
    public:
      #ifndef __CINT__
      TEveMu2eMCInterface():fSimPartList2D(0), fSimPartList3D(0), fTrackList2D(0),fTrackList3D(0){};
      TEveMu2eMCInterface(const TEveMu2eMCInterface &);
      TEveMu2eMCInterface& operator=(const TEveMu2eMCInterface &);
      virtual ~TEveMu2eMCInterface(){};
      #endif

      TEveElementList *fSimPartList2D;
      TEveElementList *fSimPartList3D;
      TEveElementList *fTrackList2D;
      TEveElementList *fTrackList3D;

      void AddMCSimParticle(bool firstloop, const CaloHitSimPartMCCollection *mcchitspcol, TEveMu2e2DProjection *calo2Dproj, double time, bool Redraw, bool show2D);
      void AddMCTrajectory(bool firstloop, const MCTrajectoryCollection *trajcol, TEveMu2e2DProjection *tracker2Dproj, bool Redraw, bool show2D);
      #include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.hpp" 

      ClassDef(TEveMu2eMCInterface,0);

  }; //end class def

}//end namespace mu2e

#endif /*TEveMu2eMCInterface.h*/
