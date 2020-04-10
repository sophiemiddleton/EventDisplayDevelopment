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

//TEveEventDisplay Headers:
#include  "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"
#include  "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include  "TEveEventDisplay/src/dict_classes/Data_Collections.h"
// Mu2e Utilities
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/WorldG4.hh"
#include "GeometryService/inc/WorldG4Maker.hh"
#include "GeometryService/inc/Mu2eCoordTransform.hh"
#include "BFieldGeom/inc/BFieldManager.hh"
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkDiag/inc/TrkMCTools.hh"

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"


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
			fhicl::Atom<std::string> g4ModuleLabel{Name("g4ModuleLabel"), Comment("")};
			fhicl::Atom<bool> doDisplay{Name("doDisplay"), Comment(""), true};
			fhicl::Atom<bool> clickToAdvance{Name("clickToAdvance"), Comment(""), true}; 
			fhicl::Atom<bool> showEvent{Name("showEvent"), Comment(""),true};     
      fhicl::Table<Collection_Filler::Config> filler{Name("filler"),Comment("fill collections")};
			
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

    std::string g4ModuleLabel_;

    bool doDisplay_;
    bool clickToAdvance_;
    bool showEvent_;
       
    TApplication* application_;
    TDirectory*   directory_ = nullptr;   

    Collection_Filler _filler;

    TEveMu2eMainWindow *_frame;
    fhicl::ParameterSet _pset;

    bool foundEvent = false;
    void MakeTEveMu2eMainWindow();
    bool _firstLoop = true;
	       
	};

TEveEventDisplay::TEveEventDisplay(const Parameters& conf) :
	art::EDAnalyzer(conf),
	_diagLevel(conf().diagLevel()),
	g4ModuleLabel_(conf().g4ModuleLabel()),
	doDisplay_(conf().doDisplay()),
	clickToAdvance_(conf().clickToAdvance()),
	showEvent_(conf().showEvent()),
  _filler(conf().filler())
	{}


TEveEventDisplay::~TEveEventDisplay(){}

void TEveEventDisplay::beginJob(){
	directory_ = gDirectory;
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


void TEveEventDisplay::analyze(const art::Event& event){
  std::cout<<"[In TEveEventDisplay::analyze()]"<<std::endl;
  foundEvent = true;
  Data_Collections data;
  _filler.GetComboHitCollection(event, data);
  int N = data.chcol->size();
  cout<<N<<" hits extracted into Collection"<<endl;
  if(!_frame->isClosed()) _frame->setEvent(event, _firstLoop, data);
  _firstLoop = false;

} 


void TEveEventDisplay::endJob(){
  if(!foundEvent){
    char msg[300];
    sprintf(msg, "Reached end of file but #%i has not been found", true);
    new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(), "Event Not Found", msg, kMBIconExclamation,kMBOk);
	}

}  
	

}
using mu2e::TEveEventDisplay;
DEFINE_ART_MODULE(TEveEventDisplay);
