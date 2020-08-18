#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eDataInterface.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"

struct TimeOrderClusters : public std::binary_function<mu2e::CaloCluster, mu2e::CaloCluster, bool> {
  bool operator()(mu2e::CaloCluster const& p1, mu2e::CaloCluster const& p2) { 
    return p1.time() > p2.time(); 
  }
};

struct TimeOrderHits : public std::binary_function<mu2e::ComboHit, mu2e::ComboHit, bool> {
  bool operator()(mu2e::ComboHit const& p1, mu2e::ComboHit const& p2) { 
    return p1.time() > p2.time(); 
  }
};

using namespace mu2e;
namespace mu2e{
 
  std::vector<double> TEveMu2eDataInterface::getTimeRange(bool firstloop, const ComboHitCollection *chcol, const CrvRecoPulseCollection *crvcoincol, const CaloClusterCollection *clustercol){
	vector <double> time = {-1, -1};

	if (crvcoincol != 0){
		for(unsigned int i=0; i <crvcoincol->size(); i++){
			const CrvRecoPulse &crvRecoPulse = crvcoincol->at(i);
			if (crvRecoPulse.GetPulseTime() > time.at(1)){time.at(1) = crvRecoPulse.GetPulseTime();}
			if (crvRecoPulse.GetPulseTime() < time.at(0)){time.at(0) = crvRecoPulse.GetPulseTime();}
		}
	}
        if (chcol != 0){
		for(size_t i=0; i<chcol->size();i++){
	      		ComboHit hit = (*chcol)[i];
			if (hit.time() > time.at(1)){time.at(1) = hit.time();}
			if (hit.time() < time.at(0)){time.at(0) = hit.time();}
		}
	}

	if (clustercol != 0){
	    	for(unsigned int i=0; i<clustercol->size();i++){
	      		CaloCluster const  &cluster= (*clustercol)[i];
			if (cluster.time() > time.at(1)){time.at(1) = cluster.time();}
			if (cluster.time() < time.at(0)){time.at(0) = cluster.time();}
		}
	}
	if (time.at(0) == -1){time.at(0) = 0;}
  if (time.at(1) == -1){time.at(1) = 5;} //TODO:Change after testing
	return time;

}

   void TEveMu2eDataInterface::AddCRVInfo(bool firstloop, const CrvRecoPulseCollection *crvcoincol,  double time, bool Redraw, bool show2D){
      DataLists<const CrvRecoPulseCollection*, TEveMu2e2DProjection*>(crvcoincol, Redraw, show2D, &fCrvList3D, &fCrvList2D);
        if(crvcoincol!=0){
          TEveElementList *CrvList3D = new TEveElementList("Crv3D");
          GeomHandle<CosmicRayShield> CRS;
          for(unsigned int i=0; i <crvcoincol->size(); i++)
          {
            const CrvRecoPulse &crvRecoPulse = crvcoincol->at(i);

            TEveMu2eCRVEvent *teve_crv3D = new TEveMu2eCRVEvent(crvRecoPulse);
            const CRSScintillatorBarIndex &crvBarIndex = crvRecoPulse.GetScintillatorBarIndex();
            const CRSScintillatorBar &crvCounter = CRS->getBar(crvBarIndex);
            CLHEP::Hep3Vector crvCounterPos = crvCounter.getPosition(); 
            hep3vectorTocm(crvCounterPos);
            string pos3D = "(" + to_string((double)crvCounterPos.x()) + ", " + to_string((double)crvCounterPos.y()) + ", " + to_string((double)crvCounterPos.z()) + ")";
            teve_crv3D->DrawHit3D("CRVHits3D, Position = " + pos3D + ", Pulse Time = " + to_string(crvRecoPulse.GetPulseTime()) + ", Pulse Height = "+ to_string(crvRecoPulse.GetPulseHeight()) + + "Pulse Width = " + to_string(crvRecoPulse.GetPulseWidth()),  i + 1, crvCounterPos, CrvList3D);
            fCrvList3D->AddElement(CrvList3D); 
            gEve->AddElement(fCrvList3D);
            gEve->Redraw3D(kTRUE); 
          } 
          }
        
    }

  std::vector<double> TEveMu2eDataInterface::AddComboHits(bool firstloop, const ComboHitCollection *chcol, TEveMu2e2DProjection *tracker2Dproj, double time, bool Redraw, bool show2D){
  std::vector<double> energies = {0,0};
  DataLists<const ComboHitCollection*, TEveMu2e2DProjection*>(chcol, Redraw, show2D, &fHitsList3D, &fHitsList2D, tracker2Dproj);
  if(chcol!=0){
    TEveElementList *HitList2D = new TEveElementList("Hits2D");
    TEveElementList *HitList3D = new TEveElementList("Hits3D");
    int *energylevels = new int[chcol->size()];
    energies = Energies<const ComboHitCollection*>(chcol, &energylevels);
    for(size_t i=0; i<chcol->size();i++){
      ComboHit hit = (*chcol)[i];
      DrawTrackerHitData<ComboHit, TEveMu2e2DProjection*>(firstloop, hit, time, show2D, -1, "ComboHit", i, &energylevels, &fHitsList3D, &HitList3D, &fHitsList2D, &HitList2D, tracker2Dproj);
            }

     }
      
   return energies;
  }

std::vector<double> TEveMu2eDataInterface::AddCaloClusters(bool firstloop, const CaloClusterCollection *clustercol, TEveMu2e2DProjection *calo2Dproj, double time, bool Redraw, bool show2D){
  vector <double> energies = {0, 0};
  DataLists<const CaloClusterCollection*, TEveMu2e2DProjection*>(clustercol, Redraw, show2D, &fClusterList3D, &fClusterList2D, calo2Dproj);
  if(clustercol!=0){ 
    TEveElementList *ClusterList3D = new TEveElementList("CaloClusters3D");
    TEveElementList *ClusterList2D = new TEveElementList("CaloClusters2D");
    int *energylevels = new int[clustercol->size()];
    energies = Energies<const CaloClusterCollection*>(clustercol, &energylevels);
    for(unsigned int i=0; i<clustercol->size();i++){
      CaloCluster const  &cluster= (*clustercol)[i];
      DrawCaloClusterData<CaloCluster, TEveMu2e2DProjection*>(firstloop, cluster, time, show2D, "Calo Cluster", i, &energylevels, &fClusterList3D,  &ClusterList3D, &fClusterList2D, &ClusterList2D, calo2Dproj);
      }
    }
  return energies;
  }

  void TEveMu2eDataInterface::AddCrystalHits(bool firstloop, const CaloCrystalHitCollection *cryHitcol, TEveMu2e2DProjection *calo2Dproj, double time, bool Redraw, bool show2D){
    Calorimeter const &cal = *(GeomHandle<Calorimeter>());
    DataLists<const CaloCrystalHitCollection*, TEveMu2e2DProjection*>(cryHitcol, Redraw, show2D, &fHitsList3D, &fHitsList2D, calo2Dproj);
    if(cryHitcol!=0){

        TEveElementList *HitList = new TEveElementList("CrystalHits");
    int *energylevels = new int[cryHitcol->size()];
    energies = Energies<const CaloCrystalHitCollection*>(cryHitcol, &energylevels);

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

/*void TEveMu2eDataInterface::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, Geom_Interface *mu2e_geom,TEveMu2e2DProjection *tracker2Dproj){
  
    if(seedcol!=0){
      if (fTrackList3D == 0) {
        fTrackList3D = new TEveElementList("Helix3D");
        fTrackList3D->IncDenyDestroy();     
      }
      else {
        fTrackList3D->DestroyElements();  
      }
      if (fTrackList2D == 0) {
        fTrackList2D = new TEveElementList("Helix2D");
        fTrackList2D->IncDenyDestroy();     
      }
      else {
        fTrackList2D->DestroyElements();  
      }
      for(unsigned int k = 0; k < seedcol->size(); k++){
        KalSeed kseed = (*seedcol)[k];
        TEveMu2eCustomHelix *line = new TEveMu2eCustomHelix(kseed);
        //TEveMu2eCustomHelix *line_twoD = new TEveMu2eCustomHelix(kseed);

        unsigned int nSteps = 1670;  
        double CaloLength = 70 + 118+ 132; //FIXME - add to GeomUtils
        double TrackerLength = 300.8;//cm FIXME - GeomUtil
        double kStepSize = nSteps/(CaloLength + TrackerLength);

        for(unsigned int i = 0 ; i< nSteps; i++){
          double zpos = (i*kStepSize)-TrackerLength/2;
          line->SetPostionAndDirectionFromKalRep(zpos);//need to start from

          if(i==0) {
            CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
            CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
            line->SetPoint(i,InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
             //line_twoD->SetPoint(i,Pos.x()/10+line->Direction.x()*line->Momentum/10,Pos.y()/10+line->Direction.y()*line->Momentum/10,Pos.z()/10-TrackerLength/2);
          } else {
            CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
            CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
            line->SetNextPoint(InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
            //line_twoD->SetNextPoint(Pos.x()/10+line->Direction.x()*line->Momentum/10,Pos.y()/10+line->Direction.y()*line->Momentum/10, Pos.z()/10-TrackerLength/2);
          }
      }
      //line_twoD->SetLineColor(kGreen);
      //line_twoD->SetLineWidth(3);
      //fTrackList2D->AddElement(line_twoD);
     // tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene);
      //tracker2Dproj->fRZMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetRZScene);

      line->SetPickable(kTRUE);
      const std::string title = line->Title();
      line->SetTitle(Form(title.c_str()));
      if(line->Charge < 0) line->SetLineColor(kRed);
      if(line->Charge > 0) line->SetLineColor(kBlue);
      line->SetLineWidth(3);
      fTrackList3D->AddElement(line);
      gEve->AddElement(fTrackList3D);
      gEve->Redraw3D(kTRUE);

      }
    }
  }
*/

  void TEveMu2eDataInterface::AddHelixPieceWise(bool firstloop, const KalSeedCollection *seedcol, TEveMu2e2DProjection *tracker2Dproj, double time, bool Redraw, bool show2D){
   DataLists<const KalSeedCollection*, TEveMu2e2DProjection*>(seedcol, Redraw, show2D, &fTrackList3D, &fTrackList2D, tracker2Dproj); 
    if(seedcol!=0){  
      for(unsigned int k = 0; k < seedcol->size(); k++){
        KalSeed kseed = (*seedcol)[k];
        TEveMu2eCustomHelix *line = new TEveMu2eCustomHelix();
        TEveMu2eCustomHelix *line_twoD = new TEveMu2eCustomHelix();
        line->fKalSeed = kseed;
        line->SetSeedInfo(kseed);

        unsigned int nSteps = 1670;  
        double CaloLength = 70 + 118+ 132; //FIXME - add to GeomUtils
        double TrackerLength = 300.8;//cm FIXME - GeomUtil
        double kStepSize = nSteps/(CaloLength + TrackerLength);
        for(unsigned int i = 0 ; i< nSteps; i++){
        double zpos = (i*kStepSize)-TrackerLength/2;
        line->SetPostionAndDirectionFromKalRep(zpos);//need to start from

        if(i==0) {
          CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
          CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
          line->SetPoint(i,InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
           if(show2D){ line_twoD->SetPoint(i,Pos.x()/10+line->Direction.x()*line->Momentum/10,Pos.y()/10+line->Direction.y()*line->Momentum/10,Pos.z()/10-TrackerLength/2);}
        } else {
          CLHEP::Hep3Vector Pos(line->Position.x(), line->Position.y(), zpos+line->Position.z());
          CLHEP::Hep3Vector InMu2e = PointToTracker(Pos);
          line->SetNextPoint(InMu2e.x()/10+line->Direction.x()*line->Momentum/10,InMu2e.y()/10+line->Direction.y()*line->Momentum/10, InMu2e.z()/10-TrackerLength/2);
          if(show2D){ line_twoD->SetNextPoint(Pos.x()/10+line->Direction.x()*line->Momentum/10,Pos.y()/10+line->Direction.y()*line->Momentum/10, Pos.z()/10-TrackerLength/2);}
        }
      }
      if(show2D){
        line_twoD->SetLineColor(kGreen);
        line_twoD->SetLineWidth(3);
        fTrackList2D->AddElement(line_twoD);
        tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene);
        tracker2Dproj->fRZMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetRZScene);
      }
      line->SetPickable(kTRUE);
      const std::string title = "Helix #" + to_string(k + 1) + ", Momentum = " + to_string(line->Momentum);
      line->SetTitle(Form(title.c_str()));
      line->SetLineColor(kGreen);
      line->SetLineWidth(3);
      fTrackList3D->AddElement(line);
      gEve->AddElement(fTrackList3D);
      gEve->Redraw3D(kTRUE);

      }
    }
  }

  void TEveMu2eDataInterface::AddTrackExitTrajectories(bool firstloop, const TrkExtTrajCollection *trkextcol) {
    if(trkextcol!=0){
      if (fExtTrackList3D == 0) {
        fExtTrackList3D = new TEveElementList("Helix3D");
        fExtTrackList3D->IncDenyDestroy();     
      }
      else {
        fExtTrackList3D->DestroyElements();  
      }

      if (fExtTrackList2D == 0) {
        fExtTrackList2D  = new TEveElementList("Helix2D");
        fExtTrackList2D->IncDenyDestroy();     
      }
      else {
        fExtTrackList2D->DestroyElements();  
      }
      for(unsigned int i = 0; i < trkextcol->size(); i++){
        TrkExtTraj trkext = (*trkextcol)[i];
        TEveMu2eCustomHelix *line = new TEveMu2eCustomHelix();
        line->fTrkExtTraj = trkext;

        line->SetMomentumExt();
        line->SetParticleExt();

        line->SetPoint(0,trkext.front().x(),trkext.front().y(),trkext.front().z());
        for(unsigned int k = 0 ; k< trkext.size(); k+=10){
          const mu2e::TrkExtTrajPoint &trkextpoint = trkext[k];
          line->SetNextPoint(trkextpoint.x(), trkextpoint.y(), trkextpoint.z()); //might have to divide by 10 for accurate translation
        }
        //     fExtTrackList2D->AddElement(line);
        //     tracker2Dproj->fXYMgr->ImportElements(fExtTrackList2D, tracker2Dproj->fDetXYScene);
        //     tracker2Dproj->fRZMgr->ImportElements(fExtTrackList2D, tracker2Dproj->fDetRZScene);
        line->SetPickable(kTRUE);
        const std::string title = "Helix #" + to_string(i + 1) + ", Momentum = " + to_string(line->Momentum);
        line->SetTitle(Form(title.c_str()));
        line->SetLineColor(kRed);
        line->SetLineWidth(3);
        fExtTrackList3D->AddElement(line);
        gEve->AddElement(fExtTrackList3D);
        gEve->Redraw3D(kTRUE);
      }
    }

}
  void TEveMu2eDataInterface::AddCosmicTrack(bool firstloop, const CosmicTrackSeedCollection *cosmiccol, TEveMu2e2DProjection *tracker2Dproj, double time, bool Redraw, bool show2D){
     if(cosmiccol !=0){
      if (fTrackList3D == 0) {
        fTrackList3D = new TEveElementList("Cosmic3D");
        fTrackList3D->IncDenyDestroy();     
      }
      else {
        fTrackList3D->DestroyElements();  
      }
      if(show2D){
        if (fTrackList2D == 0) {
          fTrackList2D = new TEveElementList("Cosmic2D");
          fTrackList2D->IncDenyDestroy();     
        }
        else {
          fTrackList2D->DestroyElements();  
        }  
      }
      TEveMu2eStraightTrack *line = new TEveMu2eStraightTrack();
      for(unsigned int ist = 0; ist < cosmiccol->size(); ++ist){

        CosmicTrackSeed sts =(*cosmiccol)[ist];
        CosmicTrack st = sts._track;

        line->SetLineColor(kGreen);
        Float_t tz1 = -150;
        Float_t tz2 = 150;
        Float_t tx1 = st.InitParams.A0  + st.InitParams.A1*tz1;
        Float_t tx2 = st.InitParams.A0  + st.InitParams.A1*tz2;
        Float_t ty1 = st.InitParams.B0  + st.InitParams.B1*tz1;
        Float_t ty2 = st.InitParams.B0  + st.InitParams.B1*tz2; 	
        line->AddLine(tx1, ty1, tz1, tx2, ty2, tz2);

        cout<<st.InitParams.A0<<"track "<<st.InitParams.A1<<st.InitParams.B1<<st.InitParams.B0<<endl;
        if(show2D){
          fTrackList2D->AddElement(line);
          tracker2Dproj->fXYMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetXYScene);
          tracker2Dproj->fRZMgr->ImportElements(fTrackList2D, tracker2Dproj->fDetRZScene);
        }
        line->SetPickable(kTRUE);

        line->SetLineColor(kGreen);
        line->SetLineWidth(3);
        fTrackList3D->AddElement(line);
        gEve->AddElement(fTrackList3D);
        gEve->Redraw3D(kTRUE);
        gEve->AddElement(fTrackList3D);
        gEve->Redraw3D(kTRUE);
      }
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
