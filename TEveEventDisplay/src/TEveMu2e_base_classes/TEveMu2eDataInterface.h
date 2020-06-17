#ifndef TEveMu2eDataInterface_h
#define TEveMu2eDataInterface_h

#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TText.h>
//libGeom
#include <TGeoManager.h>
#include <TBox.h>
#include <TGeoBBox.h>
//TEve
#include <TEveTrack.h>
#include <TEveManager.h>
#include <TEveStraightLineSet.h>
//fcl:
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
//Mu2e:
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "ConfigTools/inc/SimpleConfig.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
//TEveMu2e
#include "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2e2DProjection.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eTrkEllipse.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eCalorimeter.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eTracker.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCRVEvent.h"


class TGTextEntry;

namespace mu2e{
	class TEveMu2eDataInterface {
    public:
      
      #ifndef __CINT__
      TEveMu2eDataInterface() : fHitsList2D(0), fHitsList3D(0){};
      TEveMu2eDataInterface(const TEveMu2eDataInterface &);
      TEveMu2eDataInterface& operator=(const TEveMu2eDataInterface &);
      virtual ~TEveMu2eDataInterface(){};
      #endif
      TEveElementList *fHitsList2D;
      TEveElementList *fHitsList3D;
      //TODO - these should be set in the fcl file
      bool _showBuilding = false;
      bool _showDSOnly = true;
      bool _showTracker = false;
      bool _showCalo = false;
      bool _showCRV = false;

      void AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcol, TEveElementList *fHitsList2D, TEveElementList *fHitsList3D);
      void AddComboHits(bool firstloop, const ComboHitCollection *chcol, TEveElementList *fHitsList2D, TEveElementList *fHitsList3D, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj);
      void AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *cosmiccol);
      void AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol, TEveElementList *fHitsList2D, TEveElementList *fHitsList3D);
      void AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol, TEveElementList *fHitsList2D, TEveElementList *fHitsList3D);
      void AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, TEveElementList *fHitsList2D, TEveElementList *fHitsList3D);

     ClassDef(TEveMu2eDataInterface,0);

    }; //end class def

}//end namespace mu2e

#endif /*TEveMu2eDataInterface.h*/
