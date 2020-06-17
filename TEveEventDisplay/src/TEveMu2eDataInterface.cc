#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.h"

using namespace mu2e;
namespace mu2e{
 
  void TEveMu2eDataInterface::AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcol){
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


  void TEveMu2eDataInterface::AddComboHits(bool firstloop, const ComboHitCollection *chcol, Geom_Interface *mu2e_geom, TEveMu2e2DProjection *tracker2Dproj){
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

  void TEveMu2eDataInterface::AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *calo2Dproj){
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

    void TEveMu2eDataInterface::AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *calo2Dproj){
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


 void TEveMu2eDataInterface::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj){
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

  void TEveMu2eDataInterface::AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *_cosmiccol){
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
