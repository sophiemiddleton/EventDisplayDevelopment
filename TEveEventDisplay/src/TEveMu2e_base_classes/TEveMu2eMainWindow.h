#ifndef TEveMu2eMainWindow_h
#define TEveMu2eMainWindow_h

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
#include "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2e2DProjection.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eTrkEllipse.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eCalorimeter.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eTracker.h"
#include "TEveEventDisplay/src/shape_classes/TEveMu2eCRV.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.h"

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
      void StartCaloProjectionTab();
      void PrepareCaloProjectionTab(const art::Run& run);
      void StartCRVProjectionTab();
      void PrepareCRVProjectionTab(const art::Run& run);

      void SetRunGeometry(const art::Run& run, int _diagLevel, bool _showBuilding, bool _showDSOnly, bool _showCRV);
      Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
      void  setEvent(const art::Event& event, bool firstLoop, Data_Collections &data);
      void  fillEvent(bool firstLoop=false);
      bool  isClosed() const;
      int   getEventToFind(bool &findEvent) const;
      #endif

      TGeoManager* geom = new TGeoManager("geom","Geom");
      Geom_Interface *mu2e_geom	=new Geom_Interface(); 
      TEveMu2eDataInterface *pass_data	=new TEveMu2eDataInterface(); 

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
      TEveMu2e2DProjection *calo2Dproj = new TEveMu2e2DProjection();
      TEveMu2e2DProjection *CRV2Dproj = new TEveMu2e2DProjection();
      TEveMu2eCalorimeter *Mu2eCalo = new TEveMu2eCalorimeter();
      TEveMu2eTracker *Mu2eTracker  = new TEveMu2eTracker();
      TEveMu2eCRV *Mu2eCRV = new TEveMu2eCRV();

      TText  *_eventNumberText, *_subrunNumberText, *_runNumberText;
      int _event, _subrun, _run;

     ClassDef(TEveMu2eMainWindow,0);

    }; //end class def

}//end namespace mu2e

#endif /*TEveMu2eMainWindow.h*/
