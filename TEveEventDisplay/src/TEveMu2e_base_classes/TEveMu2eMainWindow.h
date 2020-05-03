#ifndef TEveMu2eMainWindow_h
#define TEveMu2eMainWindow_h

#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TText.h>
//libGeom
#include <TGeoManager.h>
//TEve
#include <TEveTrack.h>
#include <TEveManager.h>
//fcl:
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
//Mu2e:
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "ConfigTools/inc/SimpleConfig.hh"
#include "GeometryService/inc/GeomHandle.hh"
//TEveMu2e
#include  "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include  "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2e2DProjection.h"
#include "RecoDataProducts/inc/ComboHit.hh"


class TBox;
class TGTextEntry;
class TPad;
class TGCanvas;
class TRootEmbeddedCanvas;
class TGTextButton;
class TText;
namespace mu2e{
	class TEveMu2eMainWindow : public TGMainFrame {
    public:
      
      #ifndef __CINT__
      TEveMu2eMainWindow();
      TEveMu2eMainWindow(const TEveMu2eMainWindow &);
      TEveMu2eMainWindow& operator=(const TEveMu2eMainWindow &);
      TEveMu2eMainWindow(const TGWindow* p, UInt_t w, UInt_t h, fhicl::ParameterSet _pset);
      virtual ~TEveMu2eMainWindow(){};

      void StartTrackerProjectionTab();
      void PrepareTrackerProjectionTab(const art::Run& run);
      void SetRunGeometry(const art::Run& run, int _diagLevel);
      Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
      void  setEvent(const art::Event& event, bool firstLoop, Data_Collections &data);
      void  fillEvent(bool firstLoop=false);
      bool  isClosed() const;
      int   getEventToFind(bool &findEvent) const;

      #endif
      TGeoManager* geom = new TGeoManager("geom","Geom");
      Geom_Interface *mu2e_geom	=new Geom_Interface(); 

      int eventToFind, runToFind;

      TGTextEntry     *fTeRun,*fTeEvt;
      TGLabel         *fTlRun,*fTlEvt;
      Double_t        hitMarkerSize_;
      Double_t        trkMaxR_;
      Double_t        trkMaxZ_;
      Double_t        trkMaxStepSize_;
      Double_t        camRotateCenterH_;
      Double_t        camRotateCenterV_;
      Double_t        camDollyDelta_;

      TGTextBuffer *_eventNumber, *_subrunNumber, *_runNumber;
      int  _eventToFind = 0; ///TODO - this or one above>?

      bool _isClosed = false;
      bool _findEvent = true;
      bool _firstLoop = true;

      TEveMu2e2DProjection *tracker2Dproj = new TEveMu2e2DProjection();

      TEveElementList *fHitsList;
      TEveElementList *fTrackList;
      TEveElementList *fClusterList;
      TEveTrackList *tList;
      //TODO - these should be set in the fcl file
      bool _showBuilding = false;
      bool _showDSOnly = true;
      bool _showTracker = false;
      bool _showCalo = false;
      bool _showCRV = false;

      TEveElementList *test;

      TText  *_eventNumberText, *_subrunNumberText, *_runNumberText;
      int _event, _subrun, _run;

      void AddHelixEveTracks(bool firstloop, const KalSeedCollection *seedcol);
      void AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol);
      void AddComboHits(bool firstloop, const ComboHitCollection *chcol);
      void AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol);
      void AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol);


     ClassDef(TEveMu2eMainWindow,0);

    }; //end class def

}//end namespace mu2e

#endif /*TEveMu2eMainWindow.h*/
