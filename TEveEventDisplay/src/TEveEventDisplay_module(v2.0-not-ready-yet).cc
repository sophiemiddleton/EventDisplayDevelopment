//Author: SMiddleton 
//Date: April 2020
// Purpose: Development module - this will be the TEveEventDipslay soon...
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

//TEveEventDisplay Headers:
#include  "TEveEventDisplay/src/dict_classes/NavState.h"
#include  "TEveEventDisplay/src/dict_classes/EvtDisplayUtils.h"
#include  "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
#include  "TEveEventDisplay/src/dict_classes/Draw_Interface.h" 
#include  "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"

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

namespace mu2e 
{
  class TEveEventDisplay : public art::EDAnalyzer {
	public:

		struct Config{
			using Name=fhicl::Name;
			using Comment=fhicl::Comment;
			fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("for info"),0};
			fhicl::Atom<art::InputTag>chTag{Name("ComboHitCollection"),Comment("chTag")};
			fhicl::Atom<art::InputTag>gensTag{Name("GenParticleCollection"),Comment("gensTag")};
			fhicl::Atom<art::InputTag>strawdigiTag{Name("StrawDigiCollection"),Comment("strawdigiTag")};
			fhicl::Atom<art::InputTag>crvdigiTag{Name("CrvDigiCollection"),Comment("crvTag")};
			fhicl::Atom<art::InputTag>cosmicTag{Name("CosmicTrackSeedCollection"),Comment("cosmicTag")};
			fhicl::Atom<art::InputTag>cluTag{Name("CaloClusterCollection"),Comment("cluTag")};
      fhicl::Atom<art::InputTag>cryHitTag{Name("CaloCrystalHitCollection"),Comment("cryHitTag")};
			fhicl::Atom<std::string> g4ModuleLabel{Name("g4ModuleLabel"), Comment("")};
			fhicl::Atom<double> minEnergyDep{Name("minEnergyDep"), Comment("choose minium energy"), 50};//TODO - this doesnt belong here!
			fhicl::Atom<int> minHits{Name("minHits"), Comment(""), 2};//TODO (as above)
			fhicl::Atom<bool> doDisplay{Name("doDisplay"), Comment(""), true};
			fhicl::Atom<bool> clickToAdvance{Name("clickToAdvance"), Comment(""), true}; 
			fhicl::Atom<bool> showEvent{Name("showEvent"), Comment(""),true};     
			fhicl::Atom<bool> addHits{Name("addHits"), Comment("set to add the hits"),false};
			fhicl::Atom<bool> addTracks{Name("addTracks"), Comment("set to add tracks"),false};
			fhicl::Atom<bool> addClusters{Name("addClusters"), Comment("set to add calo lusters"),false};
			fhicl::Atom<bool> addCrvHits{Name("addCrvHits"), Comment("set to add crv hits"),false};	
      fhicl::Atom<bool> addCrystallHits{Name("addCrystalHits"), Comment("for calo cry hits"), false};
			fhicl::Atom<bool> addCosmicSeedFit{Name("addCosmicSeedFit"), Comment("for fitted cosmic track"), false};
			fhicl::Atom<bool> isCosmic{Name("isCosmic"), Comment("flag for cosmic track v helix track"), false};	
	    };
		typedef art::EDAnalyzer::Table<Config> Parameters;
		explicit TEveEventDisplay(const Parameters& conf);
		virtual ~TEveEventDisplay();
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


		      double minEnergyDep_;
		      size_t minHits_;
		      bool doDisplay_;
		      bool clickToAdvance_;
		      bool showEvent_;
             
		      TApplication* application_;
		      TDirectory*   directory_ = nullptr;
		    
		      
		      bool addHits_, addTracks_, addClusters_, addCrvHits_, addCosmicSeedFit_, isCosmic_;
		     
		      EvtDisplayUtils *visutil_ = new EvtDisplayUtils();
              
		      Geom_Interface *gdml_geom	=new Geom_Interface(); 
		      Draw_Interface *draw = new Draw_Interface();
		      //Particle_Interface *particle_info = new Particle_Interface();
          TEveMu2eMainWindow *_frame;
          fhicl::ParameterSet _pset;


		      bool foundEvent = false;
          bool _firstLoop = false;
	       
	};

TEveEventDisplay::TEveEventDisplay(const Parameters& conf) :
	art::EDAnalyzer(conf),
	_diagLevel(conf().diagLevel()),
	chTag_(conf().chTag()),
	gensTag_(conf().gensTag()),
	strawdigiTag_(conf().strawdigiTag()),
	crvdigiTag_(conf().crvdigiTag()),
	g4ModuleLabel_(conf().g4ModuleLabel()),
	minEnergyDep_(conf().minEnergyDep()),
	minHits_(conf().minHits()),
	doDisplay_(conf().doDisplay()),
	clickToAdvance_(conf().clickToAdvance()),
	showEvent_(conf().showEvent()),
	addHits_(conf().addHits()),
	addTracks_(conf().addTracks()),
	addClusters_(conf().addClusters()),
	addCrvHits_(conf().addCrvHits()),
	addCosmicSeedFit_(conf().addCosmicSeedFit()),
	isCosmic_(conf().isCosmic()){
		visutil_ = new EvtDisplayUtils();
	}


TEveEventDisplay::~TEveEventDisplay(){}



void TEveEventDisplay::beginJob(){
	directory_ = gDirectory;
	// Create application environment:
	if ( !gApplication ){
		int    tmp_argc(0);
		char** tmp_argv(0);
		application_ = new TApplication( "noapplication", &tmp_argc, tmp_argv );
	
	}
   _frame = new TEveMu2eMainWindow(gClient->GetRoot(), 1000,600, _pset);


}


void TEveEventDisplay::beginRun(const art::Run& run){
  _frame->SetRunGeometry(run, _diagLevel);
}

/*void TEveEventDisplay::analyze(const art::Event& event)
  {
    TVirtualPad *temp_pad=gPad;
    TDirectory  *temp_dir=gDirectory;
    if(_firstLoop)
    {
      int x,y;
      unsigned int width,height;
      gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),x,y,width,height);
      width-=30;
      height-=70;
      _frame = new TEveMu2eMainWindow(gClient->GetRoot(), width, height, _pset);
      if(!_frame->isClosed()) if(addHits_) AddComboHits(event);
    }
    if(!_frame->isClosed())
    {
      bool findEvent=false;
      int eventToFind=_frame->getEventToFind(findEvent);
      if(findEvent)
      {
        int eventNumber=event.id().event();
        if(eventNumber==eventToFind) _frame->setEvent(event);
        else std::cout<<"event skipped, since this is not the event we are looking for"<<std::endl;
      }
      else
      {
        _frame->setEvent(event,_firstLoop);
      }
    }

    _firstLoop=false;
    if(temp_pad) temp_pad->cd(); else gPad=nullptr;
    if(temp_dir) temp_dir->cd(); else gDirectory=nullptr;

    if(_frame->isClosed()) 
    {
      throw cet::exception("CONTROL")<<"QUIT\n";
    }
  }
*/
void TEveEventDisplay::analyze(const art::Event& event){
 
	cout<<"[TEveEventDisplay :: analyze] Analyzing Event :"<<event.id()<<endl;
  if(!_frame->isClosed()) _frame->setEvent(event, _firstLoop);
  //TODO - need a function to call to the data somewhere - then add it to the frame.
  _firstLoop = false;
	
} 



void TEveEventDisplay::endJob(){
	if(foundEvent){
		char msg[300];
		sprintf(msg, "Reached end of file but #%i has not been found", true);
	        new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(), "Event Not Found", msg, kMBIconExclamation,kMBOk);
	}

}  
	

}
using mu2e::TEveEventDisplay;
DEFINE_ART_MODULE(TEveEventDisplay);
