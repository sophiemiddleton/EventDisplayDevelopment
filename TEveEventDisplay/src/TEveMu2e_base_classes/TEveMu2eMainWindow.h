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
//Mu2e:
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "ConfigTools/inc/SimpleConfig.hh"
//TEveMu2e
#include "TEveEventDisplay/src/dict_classes/Draw_Interface.h" 
#include  "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include  "TEveEventDisplay/src/dict_classes/Data_Interface.h"
#include  "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
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

                 TEveMu2eMainWindow(const TGWindow* p, UInt_t w, UInt_t h, fhicl::ParameterSet const &pset);
            
		         virtual ~TEveMu2eMainWindow(){};


                 Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
                 void  setEvent(const art::Event& event, bool firstLoop, Data_Collections &data);
                 void  fillEvent(bool firstLoop=false);
                 bool  isClosed() const;
                 int   getEventToFind(bool &findEvent) const;
                 #endif
                 TGeoManager* geom = new TGeoManager("geom","Geom");


                 Draw_Interface *draw = new Draw_Interface();
                 Geom_Interface *mu2e_geom	=new Geom_Interface(); 
                 Data_Interface *Data = new Data_Interface();
                 
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
                 int  _eventToFind = 0;
                 bool _showBuilding = false;
                 bool _showCRV=false;
                 bool _showDSOnly = true;
                 bool _showTracker = true;
                 bool _showCalo = true;
                 bool _isClosed = false;
                 bool _findEvent = true;
                 bool _firstLoop = true;
		             TEveElementList *fHitsList;

                 TEveElementList *test;

                 TText  *_eventNumberText, *_subrunNumberText, *_runNumberText;
                 int _event, _subrun, _run;

                 void AddHelix(bool firstloop, const KalSeedCollection *seedcol);
                 void AddComboHits(bool firstloop, const ComboHitCollection *chcol);
                 void SetRunGeometry(const art::Run& run, int _diagLevel);

		         ClassDef(TEveMu2eMainWindow,0);

	}; //end class def

}//end namespace mu2e

#endif /*TEveMu2eMainWindow.h*/
