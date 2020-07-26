#ifndef TEveMu2eDataInterface_h
#define TEveMu2eDataInterface_h

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
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCRVEvent.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCustomHelix.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eStraightTrack.h"
namespace mu2e{
    class TEveMu2eDataInterface {
    public:
      #ifndef __CINT__
      TEveMu2eDataInterface() : fHitsList2D(0),fHitsList3D(0),fCrystalHitList(0),fTrackList2D(0),fTrackList3D(0), fClusterList2D(0), fClusterList3D(0),fCrvList2D(0),fCrvList3D(0), fExtTrackList2D(0), fExtTrackList3D(0){};
      TEveMu2eDataInterface(const TEveMu2eDataInterface &);
      TEveMu2eDataInterface& operator=(const TEveMu2eDataInterface &);
      virtual ~TEveMu2eDataInterface(){};
      #endif
      TEveElementList *fHitsList2D;
      TEveElementList *fHitsList3D;
      TEveElementList *fCrystalHitList;
      TEveElementList *fTrackList2D;
      TEveElementList *fTrackList3D;
      TEveElementList *fClusterList2D;
      TEveElementList *fClusterList3D;
      TEveElementList *fCrvList2D;
      TEveElementList *fCrvList3D;
      TEveElementList *fExtTrackList2D;
      TEveElementList *fExtTrackList3D;
      std::vector<double> getTimeRange(bool firstloop, const ComboHitCollection *chcol, const CrvRecoPulseCollection *crvcoincol, const CaloClusterCollection *clustercol, const KalSeedCollection *seedcol);
      void AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcoincol, Geom_Interface *mu2e_geom, double time);
      void AddComboHits(bool firstloop, const ComboHitCollection *chcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj, double time);
      void AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol,Geom_Interface *mu2e_geom,TEveMu2e2DProjection *calo2Dproj,  double time);
      void AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *calo2Dproj,  double time);
      void AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *cosmiccol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj);
      void AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *trackerDproj,  double time);
      void AddTrackExitTrajectories(bool firstloop, const TrkExtTrajCollection *trkextcol, Geom_Interface *mu2e_geom);
      template<class collection>
      void AddHitType(bool firstloop, const collection *hitcol, const char* name, std::vector<std::string> detectors, std::vector<TEveMu2e2DProjection> *projs);
      ClassDef(TEveMu2eDataInterface,0);

  }; //end class def

}//end namespace mu2e

#endif /*TEveMu2eDataInterface.h*/
