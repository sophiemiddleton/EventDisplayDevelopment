#include <TApplication.h>
#include <TEvePad.h>
#include <TObject.h>
#include <TSystem.h>
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCInterface.h"
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
      //std::vector<std::vector<Point>> traj_points
      for(unsigned int k = 0; k < trajcol->size(); k++){
        //MCTrajectory traj = (*trajcol)[k];
        //std::vector<Point> temp = traj->points();
        //traj_points.push_back(temp);
      }

    
      gEve->AddElement(fTrackList3D);
      gEve->Redraw3D(kTRUE);
    }
  }

}
