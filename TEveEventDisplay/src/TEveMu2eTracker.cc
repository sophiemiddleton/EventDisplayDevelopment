#include "TEveEventDisplay/src/shape_classes/TEveMu2eTracker.h"
#include <TBox.h>
#include <TGeoBBox.h>
using namespace mu2e;
namespace mu2e{

    TEveMu2eTracker::TEveMu2eTracker(){};

    void TEveMu2eTracker::DrawTrackerDetector(art::Run const& run, TGeoVolume* topvol, TEveElementList *orthodet){
    GeomHandle<Tracker> trkr;
    
    TubsParams envelope(trkr->getInnerTrackerEnvelopeParams());
   
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
    //CLHEP::Hep3Vector pointInMu2e = mu2e_geom->PointToTracker(trackerPos);
    topvol->AddNode(tracker, 1, new TGeoTranslation((trackerPos.x()-3904)/10,trackerPos.y()/10, (trackerPos.z()+10171)/10));

}

}
