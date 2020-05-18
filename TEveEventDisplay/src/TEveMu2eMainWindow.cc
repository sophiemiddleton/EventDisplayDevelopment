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
#include<TEveParamList.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEveStraightLineSet.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMainWindow.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHelixTrack.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCustomHelix.h"
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
    fTlEvt(0),
    fHitsList2D(0),
    fHitsList3D(0),
    fCrystalHitList(0),
    fTrackList2D(0),
    fTrackList3D(0),
    fClusterList2D(0),
    fClusterList3D(0),
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

    void TEveMu2eMainWindow::StartCaloProjectionTab(){
    // Create detector and event scenes for ortho views
    calo2Dproj->fDetXYScene = gEve->SpawnNewScene("Calo XY Scene", "");
    calo2Dproj->fDetRZScene = gEve->SpawnNewScene("Calo RZ Scene", "");
    calo2Dproj->fEvtXYScene = gEve->SpawnNewScene("Calo Evt XY Scene", "");
    calo2Dproj->fEvtRZScene = gEve->SpawnNewScene("Calo Evt RZ Scene", "");

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

    // Create side-by-side ortho XY & RZ views in new tab & add det/evt scenes
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
    Calorimeter const &cal = *(GeomHandle<Calorimeter>());
    const Disk& disk0 = cal.disk(0);
    const Disk& disk1 = cal.disk(1);
    
    TGeoVolume* topvol = geom->GetTopVolume();
   
    TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);
    TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);

    Double_t dz0{disk0.geomInfo().crateDeltaZ()/10};
    Double_t dz1{disk1.geomInfo().crateDeltaZ()/10};
    Double_t rmin{disk0.innerRadius()/10};
    Double_t rmax{disk0.outerRadius()/10};
    cout<<"calo "<<rmin<<" "<<rmax<<" "<<dz1<<endl;
    TEveGeoShape *calShape0 = new TEveGeoShape();
    TEveGeoShape *calShape1 = new TEveGeoShape();

    calShape0->SetShape(new TGeoTube(rmin, rmax, dz0));
    calShape0->SetMainTransparency(100);
    calShape1->SetShape(new TGeoTube(rmin, rmax, dz1));
    calShape1->SetMainTransparency(100);
    orthodet0->AddElement(calShape0);
    orthodet1->AddElement(calShape1);

    // ... Create cal out of Silicon using the composite shape defined above
    TGeoShape *g0 = new TGeoTube("calo 2D disk0",rmin,rmax,dz0); 
    TGeoVolume *calo0= new TGeoVolume("Calorimeter D0",g0, Si);
    calo0->SetVisLeaves(kFALSE);
    calo0->SetInvisible();

    TGeoShape *g1 = new TGeoTube("calo 2D disk1",rmin,rmax,dz1); 
    TGeoVolume *calo1= new TGeoVolume("Calorimeter D1",g1, Si);
    calo1->SetVisLeaves(kFALSE);
    calo1->SetInvisible();

    CLHEP::Hep3Vector calo0Pos(0,0,dz0);
    CLHEP::Hep3Vector pointInMu2e0 = mu2e_geom->PointToCalo(calo0Pos,0);
    topvol->AddNode(calo0, 1, new TGeoTranslation(-390.4,0,-1194.2));

    CLHEP::Hep3Vector calo1Pos(0,0,dz1);
    CLHEP::Hep3Vector pointInMu2e1 = mu2e_geom->PointToCalo(calo1Pos,1);
    topvol->AddNode(calo1, 1, new TGeoTranslation(-390.4,0,1262.0));
   
  
    //...Add in the crystals:
    for(unsigned int i = 0; i <674 ; i++){
	    Crystal const &crystal = cal.crystal(i);
      double crystalXLen = crystal.size().x();
      double crystalYLen = crystal.size().y();
      double crystalZLen = crystal.size().z();

      CLHEP::Hep3Vector crystalPos   = cal.geomUtil().mu2eToDiskFF(0,crystal.position());
      Double_t origin[3];
      origin [0] = crystalPos.x()/10;
      origin [1] = crystalPos.y()/10;
      origin [2] = crystalPos.z()/10;

      TEveGeoShape *crystalShape   = new TEveGeoShape();
      crystalShape->SetMainTransparency(100);
      crystalShape->SetShape(new TGeoBBox("crystal", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10, origin));//Add position in loop
      orthodet0->AddElement(crystalShape);
      
      TGeoShape *c = new TGeoBBox("crystal", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10);
      TGeoVolume *cry= new TGeoVolume("cry",c, Si);
      cry->SetVisLeaves(kFALSE);
      cry->SetInvisible();
      topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x()/10,crystalPos.y()/10,crystalPos.z()/10));
		
	  }

     //...Add in the crystals:
    for(unsigned int i = 0; i <674 ; i++){
	    Crystal const &crystal = cal.crystal(i);
      double crystalXLen = crystal.size().x();
      double crystalYLen = crystal.size().y();
      double crystalZLen = crystal.size().z();

      CLHEP::Hep3Vector crystalPos   = cal.geomUtil().mu2eToDiskFF(1,crystal.position());
      Double_t origin[3];
      origin [0] = crystalPos.x()/10;
      origin [1] = crystalPos.y()/10;
      origin [2] = crystalPos.z()/10;

      TEveGeoShape *crystalShape   = new TEveGeoShape();
      crystalShape->SetMainTransparency(100);
      crystalShape->SetShape(new TGeoBBox("crystal1", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10, origin));//Add position in loop
      orthodet1->AddElement(crystalShape);
      
      TGeoShape *c = new TGeoBBox("crystal1", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10);
      TGeoVolume *cry= new TGeoVolume("cry1",c, Si);
      cry->SetVisLeaves(kFALSE);
      cry->SetInvisible();
      topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x()/10,crystalPos.y()/10,crystalPos.z()/10));
		
	  }

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

    GeomHandle<Tracker> trkr;
    
    TubsParams envelope(trkr->getInnerTrackerEnvelopeParams());
    TGeoVolume* topvol = geom->GetTopVolume();
   
    TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);
    TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);

    Double_t dz{envelope.zHalfLength()/10};
    Double_t rmin{envelope.innerRadius()/10};
    Double_t rmax{envelope.outerRadius()/10};
    TEveGeoShape *tr = new TEveGeoShape();
    tr->SetShape(new TGeoTube(rmin, rmax, dz));

    tr->SetMainTransparency(100);
    orthodet->AddElement(tr);
      
    // ... Create tracker out of Silicon using the composite shape defined above
    TGeoShape *gs = new TGeoTube("tracker 2D",rmin,rmax,dz+1288); 
    TGeoVolume *tracker = new TGeoVolume("Tracker",gs, Si);
    tracker->SetVisLeaves(kFALSE);
    tracker->SetInvisible();
    CLHEP::Hep3Vector trackerPos(0,0,dz);
    CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToTracker(trackerPos);
    topvol->AddNode(tracker, 1, new TGeoTranslation(pointInMu2e.x()/10,pointInMu2e.y()/10, pointInMu2e.z()/10));
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
    AddCaloClusters(firstLoop, data.clustercol);
    AddHelixPieceWise(firstLoop, data.kalseedcol);
    //TODO - add your functions here AddCrystalHits(firstLoop, data.cryHitcol);
    gSystem->ProcessEvents();
    gClient->NeedRedraw(fTeRun);

    gApplication->Run(true);
   }
 
  void TEveMu2eMainWindow::AddComboHits(bool firstloop, const ComboHitCollection *chcol){
    std::cout<<"[In AddComboHits()]"<<std::endl;
    if(chcol!=0){
    if (fHitsList3D== 0) {
      fHitsList3D = new TEveElementList("Hits");
      fHitsList3D->IncDenyDestroy();     
    }
    else {
      fHitsList3D->DestroyElements();  
    }
    if (fHitsList2D== 0) {
      fHitsList2D = new TEveElementList("Hits");
      fHitsList2D->IncDenyDestroy();     
    }
    else {
      fHitsList2D->DestroyElements();  
    }
    TEveElementList *HitList2D = new TEveElementList("ComboHits2D");
    TEveElementList *HitList3D = new TEveElementList("ComboHits3D");
    
    for(size_t i=0; i<chcol->size();i++){
      ComboHit hit = (*chcol)[i];
      TEveMu2eHit *teve_hit2D = new TEveMu2eHit(hit);
      TEveMu2eHit *teve_hit3D = new TEveMu2eHit(hit);

      CLHEP::Hep3Vector HitPos(hit.pos().x(), hit.pos().y(), hit.pos().z());
      CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToTracker(HitPos);

      teve_hit3D->DrawHit3D("ComboHits3D",  1, pointInMu2e, HitList3D);
      teve_hit2D->DrawHit2D("ComboHits2D",  1, HitPos, HitList2D);

      fHitsList2D->AddElement(HitList2D); 
      fHitsList3D->AddElement(HitList3D); 
      // ... Import elements of the list into the projected views
      tracker2Dproj->fXYMgr->ImportElements(fHitsList2D, tracker2Dproj->fDetXYScene); 
      tracker2Dproj->fRZMgr->ImportElements(fHitsList2D, tracker2Dproj->fDetRZScene);

      //gEve->AddElement(fHitsList2D);
      gEve->AddElement(fHitsList3D);
      gEve->Redraw3D(kTRUE);  
      
      }
     }
	  }

  void AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *_cosmiccol){
    std::cout<<"[In AddCosmicTrack() ] found Track "<<std::endl;
		TEveStraightLineSet *CosmicTrackList = new TEveStraightLineSet();
		for(size_t ist = 0; ist < _cosmiccol->size(); ++ist){
			CosmicTrackSeed sts =(*_cosmiccol)[ist];
			CosmicTrack st = sts._track;
			
			CosmicTrackList->SetLineColor(kGreen);
			Float_t tz1 = -150;
			Float_t tz2 = 150;
			Float_t tx1 = st.InitParams.A0  + st.InitParams.A1*tz1;
			Float_t tx2 = st.InitParams.A0  + st.InitParams.A1*tz2;
			Float_t ty1 = st.InitParams.B0  + st.InitParams.B1*tz1;
			Float_t ty2 = st.InitParams.B0  + st.InitParams.B1*tz2; 	
			CosmicTrackList->AddLine(tx1, ty1, tz1, tx2, ty2, tz2);
		
			cout<<st.InitParams.A0<<"track "<<st.InitParams.A1<<st.InitParams.B1<<st.InitParams.B0<<endl;
			gEve->AddElement(CosmicTrackList);
		  gEve->Redraw3D(kTRUE);
		
	  }
  }

  void TEveMu2eMainWindow::AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol){
      std::cout<<"[In AddCaloCrystalHits()]"<<std::endl;
      cout<<cryHitcol->size()<<endl;
      Calorimeter const &cal = *(GeomHandle<Calorimeter>());
      if(cryHitcol!=0){
	     if (fCrystalHitList == 0) {
		      fCrystalHitList = new TEveElementList("Hits");
		      fCrystalHitList->IncDenyDestroy();     
	      }
	      else {
		       fCrystalHitList->DestroyElements();  
	       }

        TEveElementList *HitList = new TEveElementList("CrystalHits");
        for(unsigned int i=0; i<cryHitcol->size();i++){
          TEveMu2eHit *teve_hit = new TEveMu2eHit();
          CaloCrystalHit const  &hit = (*cryHitcol)[i];
          int diskId = cal.crystal(hit.id()).diskId();
          CLHEP::Hep3Vector HitPos(cal.geomUtil().mu2eToDiskFF(diskId, cal.crystal(hit.id()).position()));
	        CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToCalo(HitPos,diskId);
         
          teve_hit->DrawHit3D("CrystalHits",  1, pointInMu2e, HitList);
          fCrystalHitList->AddElement(HitList);  
          gEve->AddElement(fCrystalHitList);
          gEve->Redraw3D(kTRUE);    
        }
      }
	}

   void TEveMu2eMainWindow::AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol){
      
      if(clustercol!=0){
	     if (fClusterList3D == 0) {
		      fClusterList3D = new TEveElementList("Clusters3D");
		      fClusterList3D->IncDenyDestroy();     
	      }
	      else {
		       fClusterList3D->DestroyElements();  
	       }
        TEveElementList *ClusterList3D = new TEveElementList("CaloClusters");
        if (fClusterList2D == 0) {
          fClusterList2D = new TEveElementList("Clusters2D");
          fClusterList2D->IncDenyDestroy();     
        }
        else {
         fClusterList2D->DestroyElements();  
        }
        TEveElementList *ClusterList2D = new TEveElementList("CaloClusters");
        for(unsigned int i=0; i<clustercol->size();i++){

          CaloCluster const  &cluster= (*clustercol)[i];
          TEveMu2eCluster *teve_cluster3D = new TEveMu2eCluster(cluster);
          TEveMu2eCluster *teve_cluster2D = new TEveMu2eCluster(cluster);

          CLHEP::Hep3Vector COG(cluster.cog3Vector().x(),cluster.cog3Vector().y(), cluster.cog3Vector().z());
	        CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToCalo(COG,cluster.diskId());
          teve_cluster3D->DrawCluster("CaloCluster3D", pointInMu2e, ClusterList3D);
          fClusterList3D->AddElement(ClusterList3D);  

          teve_cluster2D->DrawCluster("CaloCluster2D", COG, ClusterList2D);
          fClusterList2D->AddElement(ClusterList2D); 
          
          if(cluster.diskId()==0)  calo2Dproj->fXYMgr->ImportElements(fClusterList2D, calo2Dproj->fDetXYScene); 

          if(cluster.diskId()==1) calo2Dproj->fRZMgr->ImportElements(fClusterList2D, calo2Dproj->fDetRZScene);
     
         
        
          gEve->AddElement(fClusterList3D);
          gEve->Redraw3D(kTRUE);    
        }
      }
	}

 void TEveMu2eMainWindow::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol){
    std::cout<<"Adding Helix in custom pieces"<<std::endl;
    if(seedcol!=0){

    if (fTrackList3D == 0) {
      fTrackList3D = new TEveElementList("Helix3D");
      fTrackList3D->IncDenyDestroy();     
    }
    else {
      fTrackList3D->DestroyElements();  
    }

    if (fTrackList2D == 0) {
      fTrackList2D  = new TEveElementList("Helix2D");
      fTrackList2D->IncDenyDestroy();     
    }
    else {
      fTrackList2D->DestroyElements();  
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
 
        line->SetNextPoint(InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);

      }
    }
  /*TEveMu2eTrkEllipse *TrkEllipse = new TEveMu2eTrkEllipse();
    TrkEllipse->SetX1(kseed.helix()->helix().centerx()/10);
    TrkEllipse->SetY1(kseed.helix()->helix().centery()/10);
    TrkEllipse->SetR1(kseed.helix()->helix().radius()/10);
    TrkEllipse->SetLineColor(kYellow); 
    
    fTrackList2D->AddElement(TrkEllipse);*/
    tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene); 

   line->SetLineColor(kGreen);
    line->SetLineWidth(3);
    fTrackList3D->AddElement(line);
    gEve->AddElement(fTrackList3D);
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
