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
#include<TEveParamList.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEveStraightLineSet.h>
//TEveMu2e:
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHelixTrack.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCustomHelix.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCRVEvent.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eBField.h"

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


	TEveMu2eMainWindow::TEveMu2eMainWindow() : TGMainFrame(gClient->GetRoot(), 320, 320){}

  TEveMu2eMainWindow::TEveMu2eMainWindow(const TGWindow* p, UInt_t w, UInt_t h, fhicl::ParameterSet _pset): 
    TGMainFrame(p, w, h),
    fTeRun(0),
    fTeEvt(0),
    fTlRun(0),
    fTlEvt(0)
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

        TEveParamList *edep = new TEveParamList("CaloEnergySelection");
        gEve->AddToListTree(edep,0);
        edep->AddParameter(TEveParamList::FloatConfig_t("Min Energy Depositied",20,0,100));

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

    void TEveMu2eMainWindow::StartCRVProjectionTab(){
      // Create detector and event scenes for ortho views
      CRV2Dproj->fDetXYScene = gEve->SpawnNewScene("CRV Top", "");
      CRV2Dproj->fEvtXYScene = gEve->SpawnNewScene("CRV Top event", "");

      // Create XY/RZ calo2Dprojection mgrs, draw projected axes, & add them to scenes
      CRV2Dproj->fXYMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
      TEveProjectionAxes* axes_xy = new TEveProjectionAxes(CRV2Dproj->fXYMgr);
      CRV2Dproj->fDetXYScene->AddElement(axes_xy);
      gEve->AddToListTree(axes_xy,kTRUE);
      gEve->AddToListTree(CRV2Dproj->fXYMgr,kTRUE);

      // Create side-by-side ortho D1, D2 views in new tab & add det/evt scenes
      TEveWindowSlot *slot = 0;
      TEveWindowPack *pack = 0;

      slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
      pack = slot->MakePack();
      pack->SetElementName("CRV Views");
      pack->SetHorizontal();
      pack->SetShowTitleBar(kFALSE);

      pack->NewSlot()->MakeCurrent();
      CRV2Dproj->fXYView = gEve->SpawnNewViewer("CRV View", "");
      CRV2Dproj->fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
      CRV2Dproj->fXYView->AddScene(CRV2Dproj->fDetXYScene);
      CRV2Dproj->fXYView->AddScene(CRV2Dproj->fEvtXYScene);

      gEve->GetBrowser()->GetTabRight()->SetTab(0);

}

    void TEveMu2eMainWindow::StartCaloProjectionTab(){
    // Create detector and event scenes for ortho views
    calo2Dproj->fDetXYScene = gEve->SpawnNewScene("Calo XY D0 Scene", "");
    calo2Dproj->fDetRZScene = gEve->SpawnNewScene("Calo XY D1 Scene", "");
    calo2Dproj->fEvtXYScene = gEve->SpawnNewScene("Calo Evt XY D0 Scene", "");
    calo2Dproj->fEvtRZScene = gEve->SpawnNewScene("Calo Evt XY D1 Scene", "");

    // Create XY/RZ calo2Dprojection mgrs, draw projected axes, & add them to scenes
    calo2Dproj->fXYMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    TEveProjectionAxes* axes_xy = new TEveProjectionAxes(calo2Dproj->fXYMgr);
    calo2Dproj->fDetXYScene->AddElement(axes_xy);
    gEve->AddToListTree(axes_xy,kTRUE);
    gEve->AddToListTree(calo2Dproj->fXYMgr,kTRUE);

    calo2Dproj->fRZMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    TEveProjectionAxes* axes_rz = new TEveProjectionAxes(calo2Dproj->fRZMgr);
    calo2Dproj->fDetRZScene->AddElement(axes_rz);
    gEve->AddToListTree(axes_rz,kTRUE);
    gEve->AddToListTree(calo2Dproj->fRZMgr,kTRUE);

    // Create side-by-side ortho D1, D2 views in new tab & add det/evt scenes
    TEveWindowSlot *slot = 0;
    TEveWindowPack *pack = 0;

    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    pack = slot->MakePack();
    pack->SetElementName("Calo Views");
    pack->SetHorizontal();
    pack->SetShowTitleBar(kFALSE);

    pack->NewSlot()->MakeCurrent();
    calo2Dproj->fXYView = gEve->SpawnNewViewer("Disk0 View", "");
    calo2Dproj->fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    calo2Dproj->fXYView->AddScene(calo2Dproj->fDetXYScene);
    calo2Dproj->fXYView->AddScene(calo2Dproj->fEvtXYScene);

    pack->NewSlot()->MakeCurrent();
    calo2Dproj->fRZView = gEve->SpawnNewViewer("Disk1 View", "");
    calo2Dproj->fRZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    calo2Dproj->fRZView->AddScene(calo2Dproj->fDetRZScene);
    calo2Dproj->fRZView->AddScene(calo2Dproj->fEvtRZScene);

    gEve->GetBrowser()->GetTabRight()->SetTab(0);

  }

  void TEveMu2eMainWindow::StartTrackerProjectionTab(){
    // Create detector and event scenes for ortho views
    tracker2Dproj->fDetXYScene = gEve->SpawnNewScene("Det XY Scene", "");
    tracker2Dproj->fDetRZScene = gEve->SpawnNewScene("Det RZ Scene", "");
    tracker2Dproj->fEvtXYScene = gEve->SpawnNewScene("Evt XY Scene", "");
    tracker2Dproj->fEvtRZScene = gEve->SpawnNewScene("Evt RZ Scene", "");

    // Create XY/RZ tracker2Dprojection mgrs, draw projected axes, & add them to scenes
    tracker2Dproj->fXYMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    TEveProjectionAxes* axes_xy = new TEveProjectionAxes(tracker2Dproj->fXYMgr);
    tracker2Dproj->fDetXYScene->AddElement(axes_xy);
    gEve->AddToListTree(axes_xy,kTRUE);
    gEve->AddToListTree(tracker2Dproj->fXYMgr,kTRUE);

    tracker2Dproj->fRZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    TEveProjectionAxes* axes_rz = new TEveProjectionAxes(tracker2Dproj->fRZMgr);
    tracker2Dproj->fDetRZScene->AddElement(axes_rz);
    gEve->AddToListTree(axes_rz,kTRUE);
    gEve->AddToListTree(tracker2Dproj->fRZMgr,kTRUE);

    // Create side-by-side ortho XY & RZ views in new tab & add det/evt scenes
    TEveWindowSlot *slot = 0;
    TEveWindowPack *pack = 0;

    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    pack = slot->MakePack();
    pack->SetElementName("Tracker Views");
    pack->SetHorizontal();
    pack->SetShowTitleBar(kFALSE);

    pack->NewSlot()->MakeCurrent();
    tracker2Dproj->fXYView = gEve->SpawnNewViewer("XY View", "");
    tracker2Dproj->fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    tracker2Dproj->fXYView->AddScene(tracker2Dproj->fDetXYScene);
    tracker2Dproj->fXYView->AddScene(tracker2Dproj->fEvtXYScene);

    pack->NewSlot()->MakeCurrent();
    tracker2Dproj->fRZView = gEve->SpawnNewViewer("RZ View", "");
    tracker2Dproj->fRZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    tracker2Dproj->fRZView->AddScene(tracker2Dproj->fDetRZScene);
    tracker2Dproj->fRZView->AddScene(tracker2Dproj->fEvtRZScene);

    gEve->GetBrowser()->GetTabRight()->SetTab(0);

  }


  void TEveMu2eMainWindow::PrepareCaloProjectionTab(const art::Run& run){
    calo2Dproj->fDetXYScene->DestroyElements();
    calo2Dproj->fDetRZScene->DestroyElements();
    TEveElementList *orthodet0 = new TEveElementList("OrthoDet0");
    TEveElementList *orthodet1 = new TEveElementList("OrthoDet1");
    TGeoVolume* topvol = geom->GetTopVolume(); 
    Mu2eCalo->DrawCaloDetector(run, topvol,orthodet0,orthodet1);

    gEve->AddGlobalElement(orthodet0);
    gEve->AddGlobalElement(orthodet1);

    // ... Import elements of the list into the projected views
    calo2Dproj->fXYMgr->ImportElements(orthodet0, calo2Dproj->fDetXYScene);
    calo2Dproj->fRZMgr->ImportElements(orthodet1, calo2Dproj->fDetRZScene);

    // ... Turn OFF rendering of duplicate detector in main 3D view
    gEve->GetGlobalScene()->FindChild("OrthoDet")->SetRnrState(kFALSE);

    // ... Turn ON rendering of detector in RPhi and RZ views
    calo2Dproj->fDetXYScene->FindChild("OrthoDet0 [P]")->SetRnrState(kTRUE);
    calo2Dproj->fDetRZScene->FindChild("OrthoDet1 [P]")->SetRnrState(kTRUE);
  }

  void TEveMu2eMainWindow::PrepareTrackerProjectionTab(const art::Run& run){
    tracker2Dproj->fDetXYScene->DestroyElements();
    tracker2Dproj->fDetRZScene->DestroyElements();
    TEveElementList *orthodet = new TEveElementList("OrthoDet");
    TGeoVolume* topvol = geom->GetTopVolume();
    Mu2eTracker->DrawTrackerDetector(run, topvol, orthodet);

    gEve->AddGlobalElement(orthodet);

    // ... Import elements of the list into the projected views
    tracker2Dproj->fXYMgr->ImportElements(orthodet, tracker2Dproj->fDetXYScene);
    tracker2Dproj->fRZMgr->ImportElements(orthodet, tracker2Dproj->fDetRZScene);

    // ... Turn OFF rendering of duplicate detector in main 3D view
    gEve->GetGlobalScene()->FindChild("OrthoDet")->SetRnrState(kFALSE);

    // ... Turn ON rendering of detector in RPhi and RZ views
    tracker2Dproj->fDetXYScene->FindChild("OrthoDet [P]")->SetRnrState(kTRUE);
    tracker2Dproj->fDetRZScene->FindChild("OrthoDet [P]")->SetRnrState(kTRUE);
  }

  void TEveMu2eMainWindow::PrepareCRVProjectionTab(const art::Run& run){
    std::cout<<"PrepareCRV"<<std::endl;
    CRV2Dproj->fDetXYScene->DestroyElements();
    CRV2Dproj->fEvtXYScene->DestroyElements();
    std::cout<<"2Dprojection"<<std::endl;
    TGeoVolume* topvol = geom->GetTopVolume();
    std::cout<<"Geometry"<<std::endl;	
    TEveElementList *orthodets0 = new TEveElementList("OrthoDet");
    //TEveElementList *orthodets1 = new TEveElementList("OrthoDet");
    //TEveElementList *orthodets2 = new TEveElementList("OrthoDet");
    TEveElementList *orthodetlist[] = {orthodets0};
    std::cout<<"DrawCRVDetector1"<<std::endl;
    // orthodets1, orthodets2
    Mu2eCRV->DrawCRVDetector(run, topvol, orthodetlist);

    for (unsigned int i=0; i<1; i++){
    gEve->AddGlobalElement(orthodetlist[i]);
    CRV2Dproj->fXYMgr->ImportElements(orthodetlist[i], CRV2Dproj->fDetXYScene);
    //CRV2Dproj->fRZMgr->ImportElements(orthodetlist[i], CRV2Dproj->fDetRZScene);
    }

    // ... Turn OFF rendering of duplicate detector in main 3D view
    gEve->GetGlobalScene()->FindChild("OrthoDet")->SetRnrState(kFALSE);

    // ... Turn ON rendering of detector in RPhi and RZ views
    CRV2Dproj->fDetXYScene->FindChild("OrthoDets0 [P]")->SetRnrState(kTRUE);
    CRV2Dproj->fDetRZScene->FindChild("OrthoDets1 [P]")->SetRnrState(kTRUE);

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

    if(!this->_showBuilding){   
      mu2e_geom->SolenoidsOnly(topnode);
      mu2e_geom->hideTop(topnode, _diagLevel);
    }
    if(this->_showDSOnly) mu2e_geom->InsideDS(topnode, false );
    if(this->_showCRV) mu2e_geom->InsideCRV(topnode, true);
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
    _event=event.id().event();
    _subrun=event.id().subRun();
    _run=event.id().run();
    _firstLoop = firstLoop;
    pass_data->AddCRVInfo(firstLoop, data.crvcoincol, mu2e_geom, CRV2Dproj);
    pass_data->AddComboHits(firstLoop, data.chcol, mu2e_geom, tracker2Dproj);
    pass_data->AddCaloClusters(firstLoop, data.clustercol, mu2e_geom, calo2Dproj);
    pass_data->AddHelixPieceWise(firstLoop, data.kalseedcol,mu2e_geom, tracker2Dproj);
    gSystem->ProcessEvents();
    gClient->NeedRedraw(fTeRun);
    gApplication->Run(true);
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
