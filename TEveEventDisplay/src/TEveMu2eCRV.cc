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
      const std::string TopSectorNames[] = {"T1", "T2", "T3", "T4"};
	    for (unsigned int i=0; i<4; i++){
		      halflen = CRS.getSectorHalfLengths(TopSectorNames[i]);
		      position = CRS.getSectorPosition(TopSectorNames[i]);
		      TEveGeoShape *sectorshape = new TEveGeoShape();
		      sectorshape->SetShape(new TGeoBBox((2*halflen[0])/10, (2*halflen[2])/10, (2*halflen[1])/10));
		      sectorshape->SetMainTransparency(100);
		      //TODO:change this for different orthodets
		      orthodetlist[i]->AddElement(sectorshape);
		      TGeoShape *g = new TGeoBBox("CRV Sector",(2*halflen[0])/10, 2*halflen[2]/10, (2*halflen[1])/10); 
		      TGeoVolume *crv0= new TGeoVolume("CRV Sector",g, Si);
		      crv0->SetVisLeaves(kFALSE);
		      crv0->SetInvisible();
		      CLHEP::Hep3Vector crv0Pos(0,0,halflen[2]);
		      //TODO:Change the translation
          std::string filename("Mu2eG4/geom/crv_counters_v07.txt");
          SimpleConfig Config(filename);
          std::vector<double> Center;
          if(i==0)  Config.getVectorDouble("crs.firstCounterT1", Center);
          if(i==1)  Config.getVectorDouble ("crs.firstCounterT2",Center);
          if(i==2)  Config.getVectorDouble("crs.firstCounterT3", Center);
          if(i==3)  Config.getVectorDouble("crs.firstCounterT4", Center) ;  
std::cout<<Center[2]<<std::endl;      
          topvol->AddNode(crv0, 1, new TGeoTranslation(Center[0]/10,Center[1]/10,Center[2]/10));

	      }

      }

}
