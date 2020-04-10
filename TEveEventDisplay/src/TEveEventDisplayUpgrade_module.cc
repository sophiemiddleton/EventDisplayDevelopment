//Author: SMiddleton 
//Date: Jan 2020
//Purpose: To make TEVe based event displays in Offline environment
//This is the first stages of this development 
// ... libCore
#include <TApplication.h>
#include <TString.h>
#include <TSystem.h>
#include <TList.h>
#include <TObjArray.h>
#include <Rtypes.h>
#include <TPolyLine3D.h>
#include <TCanvas.h>
// ... libRIO
#include <TFile.h>
// ... libGui
#include <TGString.h>
#include <TGLabel.h>
#include <TGIcon.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextView.h>
#include <TGLayout.h>
#include <TGTab.h>
#include <TG3DLine.h>
#include<TGLViewer.h>
#include <TGMsgBox.h>
#include <TRootEmbeddedCanvas.h>
// ... libGeom
#include <TGeoManager.h>
#include <TGeoBoolNode.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>
// ... libEG
#include <TParticle.h>
// ... libRGL
#include <TGLViewer.h>
// ... libEve
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveViewer.h>
#include <TEveScene.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEvePointSet.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveStraightLineSet.h>
#include "fstream"

//TEveEventDisplayUpgrade Headers:
#include  "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include  "TEveEventDisplay/src/dict_classes/Draw_Interface.h" 
#include  "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"
#include  "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
// Mu2e Utilities
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/WorldG4.hh"
#include "GeometryService/inc/WorldG4Maker.hh"
#include "GeometryService/inc/Mu2eCoordTransform.hh"
#include "BFieldGeom/inc/BFieldManager.hh"
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkDiag/inc/TrkMCTools.hh"


//Mu2e Tracker Geom:
#include "TrackerGeom/inc/Tracker.hh"
#include "TrackerGeom/inc/Straw.hh"
#include "TrkDiag/inc/ComboHitInfo.hh"

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"

//Collections:
#include "RecoDataProducts/inc/StrawDigiCollection.hh"
#include "RecoDataProducts/inc/CrvDigiCollection.hh"
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
// Mu2e diagnostics
using namespace std;
using namespace mu2e;

/*
void setRecursiveColorTransp(TGeoVolume *vol, Int_t color, Int_t transp)
  {
     if(color>=0)vol->SetLineColor(color);
     if(transp>=0)vol->SetTransparency(transp);
     Int_t nd = vol->GetNdaughters();
     for (Int_t i=0; i<nd; i++) {
        setRecursiveColorTransp(vol->GetNode(i)->GetVolume(), color, transp);
     }
  }
*/
namespace mu2e 
{
  class TEveEventDisplayUpgrade : public art::EDAnalyzer {
	public:

		struct Config{
			using Name=fhicl::Name;
			using Comment=fhicl::Comment;
			fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("for info"),0};
			fhicl::Atom<std::string> g4ModuleLabel{Name("g4ModuleLabel"), Comment("")};
			fhicl::Atom<bool> doDisplay{Name("doDisplay"), Comment(""), true};
			fhicl::Atom<bool> clickToAdvance{Name("clickToAdvance"), Comment(""), true}; 
			fhicl::Atom<bool> showEvent{Name("showEvent"), Comment(""),true};     
      fhicl::Table<Collection_Filler::Config> filler{Name("filler"),Comment("fill collections")};
			
	  };
		typedef art::EDAnalyzer::Table<Config> Parameters;
		explicit TEveEventDisplayUpgrade(const Parameters& conf);
		virtual ~TEveEventDisplayUpgrade();
		virtual void beginJob() override;
		virtual void beginRun(const art::Run& run) override;
		virtual void analyze(const art::Event& e);
		virtual void endJob() override;
	private:
		     Config _conf;
		     int _diagLevel;
		     Int_t _evt; 

		     const StrawDigiCollection* _stcol;
		     const ComboHitCollection* _chcol;
		     const StrawDigiCollection* _strawdigicol;
		     const CrvDigiCollection* _crvdigicol;
		     const CosmicTrackSeedCollection* _cosmiccol;
		     const GenParticleCollection* _gencol;
		     const CaloClusterCollection* _clustercol;
         const CaloCrystalHitCollection* _cryHitcol;
		     art::InputTag chTag_;
		     art::InputTag gensTag_;
		     art::InputTag strawdigiTag_;
		     art::InputTag crvdigiTag_;
		     art::InputTag cosmicTag_;
		     art::InputTag cluTag_;
         art::InputTag cryHitTrag_;
		     std::string g4ModuleLabel_;

		      bool doDisplay_;
		      bool clickToAdvance_;
		      bool showEvent_;
             
		      TApplication* application_;
		      TDirectory*   directory_ = nullptr;
		      
		      Double_t        hitMarkerSize_;
		      Double_t        trkMaxR_;
		      Double_t        trkMaxZ_;
		      Double_t        trkMaxStepSize_;
		      Double_t        camRotateCenterH_;
		      Double_t        camRotateCenterV_;
		      Double_t        camDollyDelta_;

          Collection_Filler _filler;

		      TGTextEntry      *fTeRun,*fTeEvt;
		      TGLabel          *fTlRun,*fTlEvt;
		   
		     // GeomHandle<mu2e::BFieldManager> bfmgr;
		     
		      TEveTrackList *fTrackList;
		      TEveElementList *fHitsList;
		      TEveElementList *fClusters;
		      
		      bool addHits_, addTracks_, addClusters_, addCrvHits_, addCosmicSeedFit_, isCosmic_;
		     
		      Geom_Interface *gdml_geom	=new Geom_Interface(); 
		      Draw_Interface *draw = new Draw_Interface();
		    
          TEveMu2eMainWindow *_frame;
          fhicl::ParameterSet _pset;

		     // TGeoManager* geom = new TGeoManager("geom","Geom");

		      bool foundEvent = false;
		      void MakeTEveMu2eMainWindow();
          bool _firstLoop = true;
	       
	};

TEveEventDisplayUpgrade::TEveEventDisplayUpgrade(const Parameters& conf) :
	art::EDAnalyzer(conf),
	_diagLevel(conf().diagLevel()),
	g4ModuleLabel_(conf().g4ModuleLabel()),
	doDisplay_(conf().doDisplay()),
	clickToAdvance_(conf().clickToAdvance()),
	showEvent_(conf().showEvent()),
  _filler(conf().filler())
	{}


TEveEventDisplayUpgrade::~TEveEventDisplayUpgrade(){}

void TEveEventDisplayUpgrade::beginJob(){
	directory_ = gDirectory;
	// Create application environment:
	if ( !gApplication ){
		int    tmp_argc(0);
		char** tmp_argv(0);
		application_ = new TApplication( "noapplication", &tmp_argc, tmp_argv );
	
	}
   _frame = new TEveMu2eMainWindow(gClient->GetRoot(), 1000,600, _pset);

}


void TEveEventDisplayUpgrade::beginRun(const art::Run& run){
  _frame->SetRunGeometry(run, _diagLevel);
}


void TEveEventDisplayUpgrade::analyze(const art::Event& event){
  std::cout<<"[In TEveEventDisplay::analyze()]"<<std::endl;
  _filler.HasComboHits(event);
  _frame->combohits = _filler.chcol;
  if(!_frame->isClosed()) _frame->setEvent(event, _firstLoop);
  _firstLoop = false;

} 


void TEveEventDisplayUpgrade::endJob(){
	if(foundEvent){
		char msg[300];
		sprintf(msg, "Reached end of file but #%i has not been found", true);
	        new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(), "Event Not Found", msg, kMBIconExclamation,kMBOk);
	}

}  
	

}
using mu2e::TEveEventDisplayUpgrade;
DEFINE_ART_MODULE(TEveEventDisplayUpgrade);
