#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCInterface.h"
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCTraj.h"
#include "MCDataProducts/inc/MCTrajectoryPoint.hh"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

  void TEveMu2eMCInterface::AddMCTrajectory(bool firstloop, const MCTrajectoryCollection *trajcol, TEveMu2e2DProjection *tracker2Dproj, bool Redraw){
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
