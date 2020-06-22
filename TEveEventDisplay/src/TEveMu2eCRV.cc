#include "TEveEventDisplay/src/shape_classes/TEveMu2eCRV.h"
#include <TBox.h>
#include <TGeoBBox.h>
using namespace mu2e;
namespace mu2e{

	TEveMu2eCRV::TEveMu2eCRV(){};

	void TEveMu2eCRV::DrawCRVDetector(art::Run const& run, TGeoVolume* topvol, TEveElementList *orthodetlist[]){
    /*//material TODO:Change this
    TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);
    TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);
    std::vector<double> halflen;
    CLHEP::Hep3Vector position;

    CosmicRayShield const &CRS = *(GeomHandle<CosmicRayShield>());
    const std::vector<CRSScintillatorShield> &sectors = CRS.getCRSScintillatorShields();
    for (unsigned int i=0; i<sectors.size(); i++){
      halflen = CRS.getSectorHalfLengths(sectors[i].getName());
      position = CRS.getSectorPosition(sectors[i].getName());
      TEveGeoShape *sectorshape = new TEveGeoShape();
      sectorshape->SetShape(new TGeoBBox(halflen[0], halflen[1], halflen[2]));
      sectorshape->SetMainTransparency(100);
      //TODO:change this for different orthodets
      orthodetlist[i]->AddElement(sectorshape);

      TGeoShape *g = new TGeoBBox("CRV Sector 0",halflen[0], halflen[1], halflen[2]); 
      TGeoVolume *crv0= new TGeoVolume("CRV Sector 0",g, Si);
      crv0->SetVisLeaves(kFALSE);
      crv0->SetInvisible();

      CLHEP::Hep3Vector crv0Pos(0,0,halflen[2]);
      //TODO:Change the translation
      topvol->AddNode(crv0, 1, new TGeoTranslation(-390.4,0,1194.2));
		}*/

	}

}
