#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{
 
  void TEveMu2eDataInterface::AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcoincol, Geom_Interface *mu2e_geom){// TEveMu2e2DProjection *CRV2Dproj){
    if(crvcoincol!=0){
      if (fCrvList3D== 0) {
        fCrvList3D = new TEveElementList("Hits");
        fCrvList3D->IncDenyDestroy();     
      }
      else {
        fCrvList3D->DestroyElements();  
      }
      //if (fCrvList2D== 0) {
      //  fCrvList2D = new TEveElementList("Hits");
     //   fCrvList2D->IncDenyDestroy();     
     // }
     // else {
     //   fCrvList2D->DestroyElements();  
     // }
      TEveElementList *CrvList3D = new TEveElementList("Crv3D");
      //TEveElementList *CrvList2D = new TEveElementList("Crv2D");
      GeomHandle<CosmicRayShield> CRS;

      for(unsigned int i=0; i <crvcoincol->size(); i++)
      {
        const CrvRecoPulse &crvRecoPulse = crvcoincol->at(i);
        //TEveMu2eCRVEvent *teve_crv2D = new TEveMu2eCRVEvent(crvRecoPulse);
        TEveMu2eCRVEvent *teve_crv3D = new TEveMu2eCRVEvent(crvRecoPulse);
        const CRSScintillatorBarIndex &crvBarIndex = crvRecoPulse.GetScintillatorBarIndex();
        const CRSScintillatorBar &crvCounter = CRS->getBar(crvBarIndex);
        CLHEP::Hep3Vector crvCounterPos = crvCounter.getPosition(); 
        //double time = crvRecoPulse.GetPulseTime();
        //int PEs = crvRecoPulse.GetPEs();
        hep3vectorTocm(crvCounterPos);
        string pos3D = "(" + to_string((double)crvCounterPos.x()) + ", " + to_string((double)crvCounterPos.y()) + ", " + to_string((double)crvCounterPos.z()) + ")";
       // string pos2D = "(" + to_string((double)crvCounterPos.x()) + ", " + to_string((double)crvCounterPos.y()) + ", " + to_string((double)crvCounterPos.z()) + ")";
        
        teve_crv3D->DrawHit3D("CRVHits3D, Position = " + pos3D + ", Pulse Time = " + to_string(crvRecoPulse.GetPulseTime()) + ", Pulse Height = "+ to_string(crvRecoPulse.GetPulseHeight()) + + "Pulse Width = " + to_string(crvRecoPulse.GetPulseWidth()),  i + 1, crvCounterPos, CrvList3D);

        //teve_crv2D->DrawHit2D("CRVHits2D, Position = " + pos2D + ", Pulse Time = " + to_string(crvRecoPulse.GetPulseTime()) + ", Pulse Height = "+ to_string(crvRecoPulse.GetPulseHeight()) + + "Pulse Width = " + to_string(crvRecoPulse.GetPulseWidth()),  i + 1, crvCounterPos, CrvList2D);

        //fCrvList2D->AddElement(CrvList2D); 
        fCrvList3D->AddElement(CrvList3D); 
        // ... Import elements of the list into the projected views
        //CRV2Dproj->fXYMgr->ImportElements(fCrvList2D, CRV2Dproj->fDetXYScene);
        gEve->AddElement(fCrvList3D);
        gEve->Redraw3D(kTRUE); 
      }
    }
  }

  void TEveMu2eDataInterface::AddComboHits(bool firstloop, const ComboHitCollection *chcol, Geom_Interface *mu2e_geom, TEveMu2e2DProjection *tracker2Dproj){
  if(chcol!=0){
    if (fHitsList3D== 0) {
      fHitsList3D = new TEveElementList("ComboHits3D");
      fHitsList3D->IncDenyDestroy();     
    }
    else {
      fHitsList3D->DestroyElements();  
    }
    if (fHitsList2D== 0) {
      fHitsList2D = new TEveElementList("ComboHits2D");
      fHitsList2D->IncDenyDestroy();     
    }
    else {
      fHitsList2D->DestroyElements();  
    }
    TEveElementList *HitList2D = new TEveElementList("Hits2D");
    TEveElementList *HitList3D = new TEveElementList("Hits3D");
    double Max_Energy = 0;
    double Min_Energy = 1000;
    
    for(size_t i=0; i<chcol->size();i++){
      ComboHit hit = (*chcol)[i];
      if ( hit.energyDep() > Max_Energy) Max_Energy =  hit.energyDep();
      if ( hit.energyDep() < Min_Energy)  Min_Energy =  hit.energyDep(); 
    }
    double interval = (Max_Energy - Min_Energy)/(9);
    int *energylevels;
    energylevels = new int[chcol->size()];

    for(size_t i=0; i<chcol->size();i++){
      ComboHit hit = (*chcol)[i];
      for(size_t n=0; n<9;n++){
         if(hit.energyDep() >= Min_Energy + n * interval && hit.energyDep() <=Min_Energy + (n+1)*interval){energylevels[i] = n;}
      }
    }
    for(size_t i=0; i<chcol->size();i++){
      ComboHit hit = (*chcol)[i];
      TEveMu2eHit *teve_hit2D = new TEveMu2eHit(hit);
      TEveMu2eHit *teve_hit3D = new TEveMu2eHit(hit);

      CLHEP::Hep3Vector HitPos(hit.pos().x(), hit.pos().y(), hit.pos().z());
      CLHEP::Hep3Vector pointInMu2e = PointToTracker(HitPos);
      string energy = to_string(teve_hit3D->GetEnergy());
      string pos3D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
      string pos2D = "(" + to_string((double)hit.pos().x()) + ", " + to_string((double)hit.pos().y()) + ", " + to_string((double)hit.pos().z()) + ")";
      teve_hit3D->DrawHit3D("ComboHits3D, Position = " + pos3D + ", Energy = " + energy + ", Time = " + to_string(hit.time()) + ", ", i + 1,  pointInMu2e, energylevels[i] , HitList3D);
      teve_hit2D->DrawHit2D("ComboHits2D, Position = " + pos2D + ", Energy = " + energy + ", Time = " + to_string(hit.time()) + ", ", i + 1, HitPos,energylevels[i] , HitList2D);

      fHitsList2D->AddElement(HitList2D); 
      fHitsList3D->AddElement(HitList3D); 
      // ... Import elements of the list into the projected views
      tracker2Dproj->fXYMgr->ImportElements(fHitsList2D, tracker2Dproj->fDetXYScene); 
      tracker2Dproj->fRZMgr->ImportElements(fHitsList2D, tracker2Dproj->fDetRZScene);

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
    TEveElementList *ClusterList3D = new TEveElementList("CaloClusters3D");
    if (fClusterList2D == 0) {
      fClusterList2D = new TEveElementList("Clusters2D");
      fClusterList2D->IncDenyDestroy();     
    }
    else {
      fClusterList2D->DestroyElements();  
    }
    TEveElementList *ClusterList2D = new TEveElementList("CaloClusters2D");
   double Max_Energy = 0;
    double Min_Energy = 1000;
    for(unsigned int i=0; i < clustercol->size();i++){
      CaloCluster cluster = (*clustercol)[i];
      if (cluster.energyDep() > Max_Energy){Max_Energy = cluster.energyDep();}
      if (cluster.energyDep()< Min_Energy){Min_Energy = cluster.energyDep();}
    }
    double interval = (Max_Energy - Min_Energy)/(12);
    int *energylevels;
    energylevels = new int[clustercol->size()];
    
    for(size_t i=0; i<clustercol->size();i++){
      CaloCluster cluster = (*clustercol)[i];
      for(size_t n=0; n<12;n++){
         if(cluster.energyDep() >= Min_Energy + n * interval && cluster.energyDep() <=Min_Energy + (n+1)*interval){energylevels[i] = n;}
       }
    }

    for(unsigned int i=0; i<clustercol->size();i++){
      CaloCluster const  &cluster= (*clustercol)[i];
      TEveMu2eCluster *teve_cluster3D = new TEveMu2eCluster(cluster);
      TEveMu2eCluster *teve_cluster2D = new TEveMu2eCluster(cluster);

      CLHEP::Hep3Vector COG(cluster.cog3Vector().x(),cluster.cog3Vector().y(), cluster.cog3Vector().z());
      CLHEP::Hep3Vector pointInMu2e = PointToCalo(COG,cluster.diskId());
      string pos3D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
      string pos2D = "(" + to_string((double)COG.x()) + ", " + to_string((double)COG.y()) + ", " + to_string((double)COG.z()) + ")";
      teve_cluster3D->DrawCluster("CaloCluster3D, Cluster #" + to_string(i + 1) + ", Position =" + pos3D + ", Energy = " + to_string(cluster.energyDep()) + ", Time = " + to_string(cluster.time()), pointInMu2e, energylevels[i], ClusterList3D);
      fClusterList3D->AddElement(ClusterList3D);  

      teve_cluster2D->DrawCluster("CaloCluster3D, Cluster #" + to_string(i + 1) + ", Position =" + pos2D + ", Energy = " + to_string(cluster.energyDep()) + ", Time = " + to_string(cluster.time()), pointInMu2e,energylevels[i], ClusterList2D);
      fClusterList2D->AddElement(ClusterList2D); 

      if(cluster.diskId()==0)  calo2Dproj->fXYMgr->ImportElements(fClusterList2D, calo2Dproj->fDetXYScene); 

      if(cluster.diskId()==1) calo2Dproj->fRZMgr->ImportElements(fClusterList2D, calo2Dproj->fDetRZScene);

      gEve->AddElement(fClusterList3D);
      gEve->Redraw3D(kTRUE);    
      }
    }
  }

  void TEveMu2eDataInterface::AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *calo2Dproj){
    Calorimeter const &cal = *(GeomHandle<Calorimeter>());
    if(cryHitcol!=0){
      if (fCrystalHitList == 0) {
          fCrystalHitList = new TEveElementList("CrystalHits");
          fCrystalHitList->IncDenyDestroy();     
        }
        else {
          fCrystalHitList->DestroyElements();  
        }

        TEveElementList *HitList = new TEveElementList("CrystalHits");
        double Max_Energy = 0;
        double Min_Energy = 1000;
      
        for(size_t i=0; i<cryHitcol->size();i++){
          CaloCrystalHit hit = (*cryHitcol)[i];
          if (hit.energyDep() > Max_Energy){Max_Energy = hit.energyDep();}
          if (hit.energyDep()< Min_Energy){Min_Energy = hit.energyDep();}
        }
        double interval = (Max_Energy - Min_Energy)/(9);
        int *energylevels;
        energylevels = new int[cryHitcol->size()];

        for(size_t i=0; i<cryHitcol->size();i++){
          CaloCrystalHit hit = (*cryHitcol)[i];
          for(size_t n=0; n<9;n++){
             if(hit.energyDep() >= Min_Energy + n * interval && hit.energyDep() <=Min_Energy + (n+1)*interval){energylevels[i] = n;}
          }
        }
        for(unsigned int i=0; i<cryHitcol->size();i++){
          TEveMu2eHit *teve_hit = new TEveMu2eHit();
          CaloCrystalHit const  &hit = (*cryHitcol)[i];
          int diskId = cal.crystal(hit.id()).diskId();
          CLHEP::Hep3Vector HitPos(cal.geomUtil().mu2eToDiskFF(diskId, cal.crystal(hit.id()).position()));
          CLHEP::Hep3Vector pointInMu2e = PointToCalo(HitPos,diskId);

          teve_hit->DrawHit3D("CrystalHits",  1, pointInMu2e, energylevels[i], HitList);
          fCrystalHitList->AddElement(HitList);  
          gEve->AddElement(fCrystalHitList);
          gEve->Redraw3D(kTRUE);    
          }
        }
  }


  void TEveMu2eDataInterface::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj){
  
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

      TEveElementList *TrackList2D = new TEveElementList("Ellipse");
      for(unsigned int k = 0; k < seedcol->size(); k++){
        KalSeed kseed = (*seedcol)[k];
        TEveMu2eCustomHelix *line = new TEveMu2eCustomHelix();

        line->fKalSeed = kseed;
        line->SetMomentum();
        line->SetParticle();

       /* unsigned int nSteps = 1000;  
        double TrackerLength = 300.8;//cm FIXME - this should not be hardcoded this way!!
        double kStepSize = nSteps/TrackerLength;
        line->kStepSize = kStepSize;

        for(unsigned int i = 0 ; i< nSteps; i++){
        double zpos = (i*kStepSize)-TrackerLength/2;
        line->SetPostionAndDirectionFromKalRep(zpos);//need to start from
        if(i==0) {
          CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
          CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
          line->SetPoint(i,InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
        } else {
          CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
          CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
          line->SetNextPoint(InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
        }
      }*/
      CLHEP::Hep3Vector trackCenter(kseed.helix()->helix().centerx(), kseed.helix()->helix().centery(), GetTrackerCenter().z());  
//void TEveMu2eTrkTrkRZ::DrawTrk(const std::string &pstr, TEveElementList *Track2DList);
      PointToTracker(trackCenter);
      hep3vectorTocm(trackCenter);
      TEveMu2eTrkEllipse *TrkEllipse = new TEveMu2eTrkEllipse();
      //TEveMu2eTrkRZ *TrkWave = new TEveMu2eTrkRZ("test", sin(kseed.helix()->helix().circleAzimuth(TrackerCenter)), 900, 1300));

      TrkEllipse->SetX1(trackCenter.x());
      TrkEllipse->SetY1(trackCenter.y());
      TrkEllipse->SetR1(kseed.helix()->helix().radius()/10);

      TrkEllipse->DrawIt("ellipse", TrackList2D);
      fTrackList2D->AddElement(TrackList2D);
      tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene); 
      //fTrackList2D->AddElement(line);
      //tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene);
      
      line->SetLineColor(kGreen);
      line->SetLineWidth(3);
      fTrackList3D->AddElement(line);
      gEve->AddElement(fTrackList3D);
      gEve->Redraw3D(kTRUE);

      }
    }
  }

  void TEveMu2eDataInterface::AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *_cosmiccol){
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
      gEve->AddElement(CosmicTrackList);
      gEve->Redraw3D(kTRUE);
	  
    }
  }

  template<class collection>
  void TEveMu2eDataInterface::AddHitType(bool firstloop, const collection *hitcol, const char* name, std::vector<std::string> detectors, std::vector<TEveMu2e2DProjection> *projs){
    if(hitcol!=0){
      if (fHitsList3D== 0) {
        fHitsList3D = new TEveElementList(name);
        fHitsList3D->IncDenyDestroy();     
      }
      else {
        fHitsList3D->DestroyElements();  
      }
      if (fHitsList2D== 0) {
        fHitsList2D = new TEveElementList(name);
        fHitsList2D->IncDenyDestroy();     
      }
      else {
        fHitsList2D->DestroyElements();  
      }
      TEveElementList *HitList2D = new TEveElementList("Hits3D");
      TEveElementList *HitList3D = new TEveElementList("Hits2D");
      for(unsigned int i=0; i<hitcol->size();i++){
        TEveMu2eHit *teve_hit = new TEveMu2eHit();
        auto &hit = (*hitcol)[i];
        for(unsigned int j=0; j<detectors.size(); j++){
          if(detectors[j] == "calo") std::cout<<"calo"<<std::endl;
          if(detectors[j] == "tracker") std::cout<<"calo"<<std::endl;
          if(detectors[j] == "crv") std::cout<<"calo"<<std::endl;
     
        }
      } 
    }
  }
}
