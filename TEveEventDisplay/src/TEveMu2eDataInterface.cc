#include <TApplication.h>
#include<TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.h"

using namespace mu2e;
namespace mu2e{
 
  void AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcol,TEveElementList *fCrvList2D, TEveElementList *fCrvList3D){
    std::cout<<"[In AddCCRVInfo()]"<<std::endl;
    if(crvcol!=0){
    if (fCrvList3D== 0) {
      fCrvList3D = new TEveElementList("Hits");
      fCrvList3D->IncDenyDestroy();     
    }
    else {
     fCrvList3D->DestroyElements();  
    }
    if (fCrvList2D== 0) {
      fCrvList2D = new TEveElementList("Hits");
      fCrvList2D->IncDenyDestroy();     
    }
    else {
      fCrvList2D->DestroyElements();  
    }
    //TEveElementList *CrvList2D = new TEveElementList("Crv2D");
    GeomHandle<CosmicRayShield> CRS;

    for(unsigned int i=0; i <crvcol->size(); i++)
    {
      const CrvRecoPulse &crvRecoPulse = crvcol->at(i);
      const CRSScintillatorBarIndex &crvBarIndex = crvRecoPulse.GetScintillatorBarIndex();
      const CRSScintillatorBar &crvCounter = CRS->getBar(crvBarIndex);
      CLHEP::Hep3Vector crvCounterPos = crvCounter.getPosition(); //The Position we need!
      //double time = crvRecoPulse.GetPulseTime();
      //int PEs = crvRecoPulse.GetPEs();

    }
    }
  }


  void TEveMu2eDataInterface::AddComboHits(bool firstloop, const ComboHitCollection *chcol, TEveElementList *b, TEveElementList *a, Geom_Interface *mu2e_geom, TEveMu2e2DProjection *tracker2Dproj){
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

}
