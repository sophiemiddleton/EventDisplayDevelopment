#include <TApplication.h>
#include<TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
// ... libRIO
#include <TFile.h>
// ... libGui
#include <TGIcon.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGString.h>
#include <TGTextView.h>
#include <TGLayout.h>
#include <TGTab.h>
#include <TG3DLine.h>
#include<TGLViewer.h>
#include <TGMsgBox.h>
#include<TPolyLine3D.h>

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

#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHelixTrack.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCustomHelix.h"

namespace fhicl
{
  class ParameterSet;
}

using namespace mu2e;

  void setRecursiveColorTransp(TGeoVolume *vol, Int_t color, Int_t transp)
  {
    if(color>=0)vol->SetLineColor(color);
    if(transp>=0)vol->SetTransparency(transp);
    Int_t nd = vol->GetNdaughters();
    for (Int_t i=0; i<nd; i++) {
     setRecursiveColorTransp(vol->GetNode(i)->GetVolume(), color, transp);
    }
  }

namespace mu2e{

	TEveMu2eMainWindow::TEveMu2eMainWindow() : TGMainFrame(gClient->GetRoot(), 320, 320) {}

  TEveMu2eMainWindow::TEveMu2eMainWindow(const TGWindow* p, UInt_t w, UInt_t h, fhicl::ParameterSet const &pset) : 
    TGMainFrame(p, w, h),
    fTeRun(0),
    fTeEvt(0),
    fTlRun(0),
    fTlEvt(0),
    fHitsList(0),
    fTrackList(0),
    tList(0)
    {
     
      TEveManager::Create();
      gEve->GetBrowser()->GetTabRight()->SetTab(0);
      gClient->GetRoot();
      TEveBrowser* browser = gEve->GetBrowser();
      FontStruct_t buttonfont = gClient->GetFontByName("-*-helvetica-medium-r-*-*-8-*-*-*-*-*-iso8859-1");
      GCValues_t gval;
      gval.fMask = kGCForeground | kGCFont;
      gval.fFont = gVirtualX->GetFontHandle(buttonfont);
      gClient->GetColorByName("black", gval.fForeground);

      browser->StartEmbedding(TRootBrowser::kLeft); // insert nav frame as new tab in left pane

      TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
      //_mainCanvas = new TRootEmbeddedCanvas("EventDisplayCanvas",frmMain,1000,600);
      frmMain->SetWindowName("EVT NAV");
      frmMain->SetCleanup(kDeepCleanup);

      TGHorizontalFrame* navFrame = new TGHorizontalFrame(frmMain);
      TGVerticalFrame* evtidFrame = new TGVerticalFrame(frmMain);
      {
        TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")) );

        // ... Example of use of picture button

        TGPictureButton *b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoBack.gif"),1100);
        navFrame->AddFrame(b);
        b->Associate(this);

        TGPictureButton *f = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoForward.gif"),1001);
        navFrame->AddFrame(f);
        f->Associate(this);

        //....Add in check list
        // TGGroupFrame *options = new TGGroupFrame(navFrame, "Options", kVerticalFrame);
        // options->SetTitlePos(TGGroupFrame::kLeft);
        // navFrame->AddFrame(options);

        // ... Create run num text entry widget and connect to "GotoEvent" rcvr in visutils
        TGHorizontalFrame* runoFrame = new TGHorizontalFrame(evtidFrame);
        fTlRun = new TGLabel(runoFrame,"Run Number");
        fTlRun->SetTextJustify(kTextLeft);
        fTlRun->SetMargins(5,5,5,0);
        runoFrame->AddFrame(fTlRun);

        fTeRun = new TGTextEntry(runoFrame, _runNumber = new TGTextBuffer(5), 1);
        _runNumber->AddText(0, "1");
        //fTeRun->Connect("ReturnPressed()","mu2e::EvtDisplayUtils", visutil_,"GotoEvent()");
        runoFrame->AddFrame(fTeRun,new TGLayoutHints(kLHintsExpandX));
        fTeRun->Associate(this);

        // ... Create evt num text entry widget and connect to "GotoEvent" rcvr in visutils
        TGHorizontalFrame* evnoFrame = new TGHorizontalFrame(evtidFrame);
        fTlEvt = new TGLabel(evnoFrame,"Evt Number");
        fTlEvt->SetTextJustify(kTextLeft);
        fTlEvt->SetMargins(5,5,5,0);
        evnoFrame->AddFrame(fTlEvt);

        fTeEvt = new TGTextEntry(evnoFrame, _eventNumber = new TGTextBuffer(5), 1);
        _eventNumber->AddText(0, "1");
        /// fTeEvt->Connect("ReturnPressed()","mu2e::EvtDisplayUtils", visutil_,"GotoEvent()");
        evnoFrame->AddFrame(fTeEvt,new TGLayoutHints(kLHintsExpandX));
        fTeEvt->Associate(this);

        TGTextButton *Gobutton         = new TGTextButton(navFrame, "&Go", 1999);
        navFrame->AddFrame(Gobutton, new TGLayoutHints(kLHintsLeft,3,0,3,0));         
        Gobutton->Associate(this);


        //Add logo
        std::string logoFile = "TEveEventDisplay/src/Icons/mu2e_logo_oval.png";
        const TGPicture *logo = gClient->GetPicture(logoFile.c_str());
        TGIcon *icon = new TGIcon(navFrame,logo,50,50);
        navFrame->AddFrame(icon,new TGLayoutHints(kLHintsLeft,20,0,0,0));

        // ... Add horizontal run & event number subframes to vertical evtidFrame
        evtidFrame->AddFrame(runoFrame,new TGLayoutHints(kLHintsExpandX));
        evtidFrame->AddFrame(evnoFrame,new TGLayoutHints(kLHintsExpandX));

        // ... Add navFrame and evtidFrame to MainFrame
        frmMain->AddFrame(navFrame);
        TGHorizontal3DLine *separator = new TGHorizontal3DLine(frmMain);
        frmMain->AddFrame(separator, new TGLayoutHints(kLHintsExpandX));
        frmMain->AddFrame(evtidFrame);

        frmMain->MapSubwindows();
        frmMain->Resize();
        frmMain->MapWindow();

        browser->StopEmbedding();
        browser->SetTabTitle("Event Nav", 0);

      }
      gEve->AddEvent(new TEveEventManager("Event", "Empty Event"));

      TGLViewer *glv = gEve->GetDefaultGLViewer();
      glv->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
      glv->CurrentCamera().RotateRad(camRotateCenterH_,camRotateCenterV_);
      glv->CurrentCamera().Dolly(camDollyDelta_,kFALSE,kFALSE);
    }

  void TEveMu2eMainWindow::SetRunGeometry(const art::Run& run, int _diagLevel){
    if(gGeoManager){
      gGeoManager->GetListOfNodes()->Delete();
      gGeoManager->GetListOfVolumes()->Delete();
      gGeoManager->GetListOfShapes()->Delete();
    }
    gEve->GetGlobalScene()->DestroyElements();

    // Import the GDML of entire Mu2e Geometry
    geom = mu2e_geom->Geom_Interface::getGeom("TEveEventDisplay/src/fix.gdml");

    //Get Top Volume
    TGeoVolume* topvol = geom->GetTopVolume();

    //Set Top Volume for gGeoManager:
    gGeoManager->SetTopVolume(topvol);
    gGeoManager->SetTopVisible(kTRUE);
    int nn = gGeoManager->GetNNodes();
    printf("nodes in geom = %d\n",nn);
    //Get Top Node:
    TGeoNode* topnode = gGeoManager->GetTopNode();
    TEveGeoTopNode* etopnode = new TEveGeoTopNode(gGeoManager, topnode);
    etopnode->SetVisLevel(4);
    etopnode->GetNode()->GetVolume()->SetVisibility(kFALSE);

    setRecursiveColorTransp(etopnode->GetNode()->GetVolume(), kWhite-10,70);

    mu2e_geom->SolenoidsOnly(topnode);
    mu2e_geom->hideTop(topnode, _diagLevel);
    mu2e_geom->InsideDS(topnode, false );

    //Add static detector geometry to global scene
    gEve->AddGlobalElement(etopnode);
    geom->Draw("ogl");
    }

  
    Bool_t TEveMu2eMainWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t param2){
      switch (GET_MSG(msg))
      {
        
        case kC_COMMAND:
          switch (GET_SUBMSG(msg))
          {
            case kCM_BUTTON: 
                 if(param1==1111)
                 {
                 
                    
                 }
                 if(param1==1001)//Forward
                 {
                   gApplication->Terminate(0);
                 }
                 if(param1==1100)//Back
                 {
                    std::cout<<"Still developing backwards navigation"<<std::endl;
                 }
                 if(param1==1999)//Go
                 {
                    eventToFind = atoi(fTeEvt->GetText());
                    runToFind = atoi(fTeRun->GetText());
                    gApplication->Terminate();
                 }
                break;
      }
      break;
  }
  return kTRUE;
}

  //SetEvent is called from the Module - it is were the drawing functions are called.
  void TEveMu2eMainWindow::setEvent(const art::Event& event, bool firstLoop, Data_Collections &data)
  {
    std::cout<<"BEGINNING....PLEASE WAIT...."<<std::endl;
    _event=event.id().event();
    _subrun=event.id().subRun();
    _run=event.id().run();
    _firstLoop = firstLoop;
    AddComboHits(firstLoop, data.chcol);
    //AddHelixPieceWise(firstLoop, data.kalseedcol);
    //TODO - add your functions here AddCrystalHits(firstLoop, data.cryHitcol);
    gSystem->ProcessEvents();
    gClient->NeedRedraw(fTeRun);

    gApplication->Run(true);
   }
 
  void TEveMu2eMainWindow::AddComboHits(bool firstloop, const ComboHitCollection *chcol){
    std::cout<<"[In AddComboHits()]"<<std::endl;
    if(chcol!=0){
    if (fHitsList == 0) {
      fHitsList = new TEveElementList("Hits");
      fHitsList->IncDenyDestroy();     
    }
    else {
      fHitsList->DestroyElements();  
    }
    TEveElementList *HitList = new TEveElementList("ComboHits");
    for(size_t i=0; i<chcol->size();i++){

      TEveMu2eHit *teve_hit = new TEveMu2eHit();
      ComboHit hit = (*chcol)[i];
      CLHEP::Hep3Vector HitPos(hit.pos().x(), hit.pos().y(), hit.pos().z());
      CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToTracker(HitPos);

      teve_hit->DrawHit("ComboHits",  1, pointInMu2e, HitList);
      fHitsList->AddElement(HitList);  
      gEve->AddElement(fHitsList);
      gEve->Redraw3D(kTRUE);  

      }
     }
	  }

   /* TODO --> You can start from this example: Edit as you wish:
  void TEveMu2eMainWindow::AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol){
      std::cout<<"[In AddCaloCrystalHits()]"<<std::endl;
      cout<<cryHitcol->size()<<endl;
      Calorimeter const &cal = *(GeomHandle<Calorimeter>());
      if(cryHitcol!=0){
	     if (fHitsList == 0) {
		      fHitsList = new TEveElementList("Hits");
		      fHitsList->IncDenyDestroy();     
	      }
	      else {
		       fHitsList->DestroyElements();  
	       }

        TEveElementList *HitList = new TEveElementList("CrystalHits");
        for(unsigned int i=0; i<cryHitcol->size();i++){
          //if(i%50 !=0) continue;
          TEveMu2eHit *teve_hit = new TEveMu2eHit();
          CaloCrystalHit const  &hit = (*cryHitcol)[i];
          int diskId = cal.crystal(hit.id()).diskId();
          CLHEP::Hep3Vector HitPos(cal.geomUtil().mu2eToDiskFF(diskId, cal.crystal(hit.id()).position()));
	        CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToCalo(HitPos,diskId);
         
          teve_hit->DrawHit("CrystalHits",  1, pointInMu2e, HitList);
          fHitsList->AddElement(HitList);  
          gEve->AddElement(fHitsList);
          gEve->Redraw3D(kTRUE);    
          
        }
      }
	  }
*/

 void TEveMu2eMainWindow::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol){
    std::cout<<"Adding Helix in custom pieces"<<std::endl;
    if(seedcol!=0){

    if (fTrackList == 0) {
      fTrackList = new TEveElementList("Hits");
      fTrackList->IncDenyDestroy();     
    }
    else {
      fTrackList->DestroyElements();  
    }


    for(size_t k = 0; k < seedcol->size(); k++){
      KalSeed kseed = (*seedcol)[k];
      TEveMu2eCustomHelix *line = new TEveMu2eCustomHelix();

      line->fKalSeed = kseed;
      line->SetMomentum();
      line->SetParticle();

      unsigned int nSteps = 1000;  
      double TrackerLength = 300.8;//cm FIXME - this should not be hardcoded this way!!
      double kStepSize = nSteps/TrackerLength;
      line->kStepSize = kStepSize;

      for(size_t i = 0 ; i< nSteps; i++){
      double zpos = (i*kStepSize)-TrackerLength/2;

      line->SetPostionAndDirectionFromKalRep(zpos);//need to start from
      if(i==0) {
        CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
        CLHEP::Hep3Vector InMu2e = mu2e_geom->PointToTracker(Pos);

        line->SetPoint(i,InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);


      } else {

        CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
        CLHEP::Hep3Vector InMu2e = mu2e_geom->PointToTracker(Pos);
        cout<<i<<endl;
        line->SetNextPoint(InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);

      }
    }

      line->SetLineColor(kGreen);
      line->SetLineWidth(3);
      fTrackList->AddElement(line);
      gEve->AddElement(fTrackList);
      gEve->Redraw3D(kTRUE);


      }
    }
}
  void TEveMu2eMainWindow::fillEvent(bool firstLoop)
   {
    // _findEvent=false;

    std::string eventInfoText;
    eventInfoText=Form("Event #: %i",_event);
    if(_eventNumberText==nullptr) 
    {
      _eventNumberText = new TText(0.6,-0.8, eventInfoText.c_str());
      _eventNumberText->SetTextColor(5);
      _eventNumberText->SetTextSize(0.025);
      _eventNumberText->Draw("same");
    }
    else _eventNumberText->SetTitle(eventInfoText.c_str());
    eventInfoText=Form("Sub Run #: %i",_subrun);
    if(_subrunNumberText==nullptr)
    {
      _subrunNumberText = new TText(0.6,-0.75,eventInfoText.c_str());
      _subrunNumberText->SetTextColor(5);
      _subrunNumberText->SetTextSize(0.025);
      _subrunNumberText->Draw("same");
    }
    else _subrunNumberText->SetTitle(eventInfoText.c_str());
    eventInfoText=Form("Run #: %i",_run);
    if(_runNumberText==nullptr)
    {
      _runNumberText = new TText(0.6,-0.7,eventInfoText.c_str());
      _runNumberText->SetTextColor(5);
      _runNumberText->SetTextSize(0.025);
      _runNumberText->Draw("same");
    }
    else _runNumberText->SetTitle(eventInfoText.c_str());

    this->Layout();

 }

  int TEveMu2eMainWindow::getEventToFind(bool &findEvent) const
  {
    findEvent=_findEvent;
    return _eventToFind;
  }

  bool TEveMu2eMainWindow::isClosed() const
  {
    return _isClosed;
  }

}
