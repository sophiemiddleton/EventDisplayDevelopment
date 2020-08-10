#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCInterface.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

  void TEveMu2eMCInterface::AddMCSimParticle(bool firstloop, const CaloHitSimPartMCCollection *mcchitspcol, TEveMu2e2DProjection *calo2Dproj, double time, bool Redraw, bool show2D){
	if(mcchitspcol == 0 && Redraw){
	    if (fSimPartList3D != 0){
	      fSimPartList3D ->DestroyElements();
	    }
	    if (fSimPartList2D != 0){
	      fSimPartList2D->DestroyElements();
	    }
	    calo2Dproj->fXYMgr->ImportElements(fSimPartList2D, calo2Dproj->fDetXYScene); 
	    calo2Dproj->fRZMgr->ImportElements(fSimPartList2D, calo2Dproj->fDetRZScene);
	    gEve->AddElement(fSimPartList3D);
	    gEve->Redraw3D(kTRUE); 
	  }
      if(mcchitspcol!=0){
	    if (fSimPartList3D == 0) {
	      fSimPartList3D = new TEveElementList("SimParticles3D");
	      fSimPartList3D->IncDenyDestroy();     
	    }
	    else {
	      fSimPartList3D->DestroyElements();  
	    }
	    if (fSimPartList2D== 0) {
	      fSimPartList2D = new TEveElementList("SimParticles2D");
	      fSimPartList2D->IncDenyDestroy();     
	    }
	    else {
	      fSimPartList2D->DestroyElements();  
	    }
	    TEveElementList *SimPartList2D = new TEveElementList("MCSimParticles2D");
	    TEveElementList *SimPartList3D = new TEveElementList("MCSimParticles3D");
	    for(size_t i=0; i<mcchitspcol->size();i++){
	      CaloHitSimPartMC simpartmc = (*mcchitspcol)[i];
	      TEveMu2eParticle *teve_particle2D = new TEveMu2eParticle(simpartmc);
	      TEveMu2eParticle *teve_particle3D = new TEveMu2eParticle(simpartmc);

	      CLHEP::Hep3Vector PartPos = simpartmc.position().at(0);
	      CLHEP::Hep3Vector pointInMu2e = PointToCalo(PartPos, 0);
	      //string energy = to_string(teve_hit3D->GetEnergy());
	      string pos3D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
	      string pos2D = "(" + to_string((double)PartPos.x()) + ", " + to_string((double)PartPos.y()) + ", " + to_string((double)PartPos.z()) + ")";
	      if (time == -1 || (simpartmc.time().at(0) <= time && time != -1)){
		teve_particle3D->DrawParticle3D("MCSimParticle3D, Position = " + pos3D + ", Time = " + to_string(simpartmc.time().at(0)) + ", ", i + 1,  pointInMu2e, SimPartList3D);
		teve_particle2D->DrawParticle2D("MCSimParticle2D, Position = " + pos2D + ", Time = " + to_string(simpartmc.time().at(0)) + ", ", i + 1, PartPos, SimPartList2D);
		if(show2D){
		fSimPartList2D->AddElement(SimPartList2D); 
		fSimPartList3D->AddElement(SimPartList3D); 
		
		// ... Import elements of the list into the projected views
		calo2Dproj->fXYMgr->ImportElements(fSimPartList2D, calo2Dproj->fDetXYScene); 
		calo2Dproj->fRZMgr->ImportElements(fSimPartList2D, calo2Dproj->fDetRZScene);
		}
		gEve->AddElement(fSimPartList3D);
		gEve->Redraw3D(kTRUE);  
		}
	      }
      	}
}

  void TEveMu2eMCInterface::AddMCTrajectory(bool firstloop, const MCTrajectoryCollection *trajcol, TEveMu2e2DProjection *tracker2Dproj, bool Redraw, bool show2D){
      if(trajcol == 0 && Redraw){
        if (fTrackList3D != 0){
          fTrackList3D->DestroyElements();
        }
        if (fTrackList2D != 0){
          fTrackList2D->DestroyElements();
        }
        gEve->AddElement(fTrackList3D);
        gEve->Redraw3D(kTRUE); 
    }
    if(trajcol!=0){
      if (fTrackList3D == 0) {
        fTrackList3D = new TEveElementList("MCTraj3D");
        fTrackList3D->IncDenyDestroy();     
      }
      else {
        fTrackList3D->DestroyElements();  
      }

      if (fTrackList2D == 0) {
        fTrackList2D  = new TEveElementList("MCTraj2D");
        fTrackList2D->IncDenyDestroy();     
      }
      else {
        fTrackList2D->DestroyElements();  
      }
      TEveElementList *HitList3D = new TEveElementList("MCtraj3D");
      std::map<art::Ptr<mu2e::SimParticle>,mu2e::MCTrajectory>::const_iterator trajectoryIter;
      for(trajectoryIter=trajcol->begin(); trajectoryIter!=trajcol->end(); trajectoryIter++)
        {
          const std::vector<MCTrajectoryPoint> &points = trajectoryIter->second.points();
          std::cout<<"Trajectory has "<<points.size()<<" points"<<std::endl;
          string pdgId= to_string(trajectoryIter->first->pdgId());
          //primaryPos=trajectoryIter->first->startPosition();
          //primaryEnergy=trajectoryIter->first->startMomentum().e();
          CLHEP::Hep3Vector StartHitPos, EndHitPos;
          for(unsigned int i=0; i<points.size();i++){
            TEveMu2eMCTraj *teve_hit3D = new TEveMu2eMCTraj();

            if(i==0) CLHEP::Hep3Vector StartHitPos(points[i].x(), points[i].y(), points[i].z());
            if(i==points.size()-1){ 
              CLHEP::Hep3Vector EndHitPos(points[i].x(), points[i].y(), points[i].z());
              //CLHEP::Hep3Vector StartInMu2e = PointToTracker(StartHitPos);
              //CLHEP::Hep3Vector EndInMu2e = PointToTracker(EndHitPos);
              string energy = to_string(points[i].kineticEnergy());
              //string mom = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
              teve_hit3D->DrawLine3D("MCTraj PDG " + pdgId + "Energy = " + energy  + ", ",  StartHitPos, EndHitPos, HitList3D);
            }
            fTrackList3D->AddElement(HitList3D); 
            gEve->AddElement(fTrackList3D);
            gEve->Redraw3D(kTRUE);
        }
      }
    }
  }
}
