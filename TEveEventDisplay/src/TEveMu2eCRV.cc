#include "TEveEventDisplay/src/shape_classes/TEveMu2eCRV.h"
#include <TBox.h>
#include <TGeoBBox.h>
using namespace mu2e;
namespace mu2e{

	TEveMu2eCRV::TEveMu2eCRV(){};

	void TEveMu2eCRV::DrawCRVDetector(art::Run const& run, TGeoVolume* topvol, TEveElementList *orthodetlist[]){
	    //material TODO:Change this
	    TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);
	    TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);
	    std::vector<double> halflen;
	    CLHEP::Hep3Vector position;

	    CosmicRayShield const &CRS = *(GeomHandle<CosmicRayShield>());
	    const std::vector<CRSScintillatorShield> &sectors = CRS.getCRSScintillatorShields();
	    std::cout<<"DrawCRVDetector2"<<std::endl;
      //for (unsigned int i=0; i<sectors.size(); i++){
      halflen = CRS.getSectorHalfLengths(sectors[0].getName().substr(4));
      position = CRS.getSectorPosition(sectors[0].getName().substr(4));
      TEveGeoShape *sectorshape = new TEveGeoShape();
      std::cout<<"DrawCRVDetector2.5"<<std::endl;
      sectorshape->SetShape(new TGeoBBox(halflen[0], halflen[1], halflen[2]));
      std::cout<<"DrawCRVDetector3"<<std::endl;
      sectorshape->SetMainTransparency(100);
      //TODO:change this for different orthodets
      orthodetlist[0]->AddElement(sectorshape);
      std::cout<<"DrawCRVDetector3.5"<<std::endl;
      TGeoShape *g = new TGeoBBox("CRV Sector 0",halflen[0], halflen[1], halflen[2]); 
      std::cout<<"DrawCRVDetector4"<<std::endl;
      TGeoVolume *crv0= new TGeoVolume("CRV Sector 0",g, Si);
      crv0->SetVisLeaves(kFALSE);
      crv0->SetInvisible();

      CLHEP::Hep3Vector crv0Pos(0,0,halflen[2]);
      std::cout<<"DrawCRVDetector5"<<std::endl;
      //TODO:Change the translation
      topvol->AddNode(crv0, 1, new TGeoTranslation(-390.4,0,1194.2));
      std::cout<<"DrawCRVDetector6"<<std::endl;
      //}

      }

}
