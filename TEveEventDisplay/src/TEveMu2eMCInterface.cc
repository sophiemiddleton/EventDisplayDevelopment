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

  void TEveMu2eMCInterface::AddMCTrajectory(bool firstloop, const MCTrajectoryCollection *trajcol, TEveMu2e2DProjection *tracker2Dproj){
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
          //double pdgId=trajectoryIter->first->pdgId();
          //primaryPos=trajectoryIter->first->startPosition();
          //primaryEnergy=trajectoryIter->first->startMomentum().e();
          for(unsigned int i=0; i<points.size();i++){
            TEveMu2eMCTraj *teve_hit3D = new TEveMu2eMCTraj();

            CLHEP::Hep3Vector HitPos(points[i].x(), points[i].y(), points[i].z());
            CLHEP::Hep3Vector pointInMu2e = PointToTracker(HitPos);
            string energy = to_string(points[i].kineticEnergy());
            string pos3D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
            teve_hit3D->DrawHit3D("ComboHits3D, Position = " + pos3D + ", Energy = " + energy  + ", ", i + 1,  pointInMu2e, HitList3D);

            fTrackList3D->AddElement(HitList3D); 
            gEve->AddElement(fTrackList3D);
            gEve->Redraw3D(kTRUE);
        }
      }
    }
  }
}
