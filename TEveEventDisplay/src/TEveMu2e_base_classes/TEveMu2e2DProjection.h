#ifndef TEveMu2e2DProjection_h
#define TEveMu2e2DProjection_h

#include <TObject.h>
#include <TEveProjectionManager.h>


namespace mu2e {

  class TEveMu2e2DProjection: public TEveProjectionManager { 
    
  public:
    #ifndef __CINT__
    TEveMu2e2DProjection(){};
    virtual ~TEveMu2e2DProjection(){};
    #endif
    //TODO - here is some example members for an XY and RZ view. We can use it to make cross secional views of the tracker and calo. What else?
    TEveViewer *fXYView;
    TEveViewer *fRZView;
    TEveProjectionManager *fXYMgr;
    TEveProjectionManager *fRZMgr;
    TEveScene *fDetXYScene;
    TEveScene *fDetRZScene;
    TEveScene *fEvtXYScene;
    TEveScene *fEvtRZScene;

    ClassDef(TEveMu2e2DProjection, 0);
};
}
#endif


/*

At Job Level: - add function to "Main window" to do this:

// Create detector and event scenes for ortho views
  fDetXYScene = gEve->SpawnNewScene("Det XY Scene", "");
  fDetRZScene = gEve->SpawnNewScene("Det RZ Scene", "");
  fEvtXYScene = gEve->SpawnNewScene("Evt XY Scene", "");
  fEvtRZScene = gEve->SpawnNewScene("Evt RZ Scene", "");

  // Create XY/RZ projection mgrs, draw projected axes, & add them to scenes
  fXYMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
  TEveProjectionAxes* axes_xy = new TEveProjectionAxes(fXYMgr);
  fDetXYScene->AddElement(axes_xy);

  fRZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
  TEveProjectionAxes* axes_rz = new TEveProjectionAxes(fRZMgr);
  fDetRZScene->AddElement(axes_rz);

  // Create side-by-side ortho XY & RZ views in new tab & add det/evt scenes
  TEveWindowSlot *slot = 0;
  TEveWindowPack *pack = 0;

  slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  pack = slot->MakePack();
  pack->SetElementName("Ortho Views");
  pack->SetHorizontal();
  pack->SetShowTitleBar(kFALSE);

  pack->NewSlot()->MakeCurrent();
  fXYView = gEve->SpawnNewViewer("XY View", "");
  fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  fXYView->AddScene(fDetXYScene);
  fXYView->AddScene(fEvtXYScene);

  pack->NewSlot()->MakeCurrent();
  fRZView = gEve->SpawnNewViewer("RZ View", "");
  fRZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  fRZView->AddScene(fDetRZScene);
  fRZView->AddScene(fEvtRZScene);

  gEve->GetBrowser()->GetTabRight()->SetTab(0);

At Run Level:

 fDetXYScene->DestroyElements();
  fDetRZScene->DestroyElements();
  


*/
