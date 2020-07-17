#include "TEveEventDisplay/src/shape_classes/TEveMu2eCalorimeter.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
#include <TBox.h>
#include <TGeoBBox.h>
using namespace mu2e;
namespace mu2e{

    TEveMu2eCalorimeter::TEveMu2eCalorimeter(){};

    void TEveMu2eCalorimeter::DrawCaloDetector(art::Run const& run, TGeoVolume* topvol, TEveElementList *orthodet0,TEveElementList *orthodet1){
      TGeoMaterial *mat = new TGeoMaterial("CsI", 28.085,14,2.33);
      TGeoMedium *CsI = new TGeoMedium("CsI",2, mat);

      Calorimeter const &cal = *(GeomHandle<Calorimeter>());
   /*   const Disk& disk0 = cal.disk(0);
      const Disk& disk1 = cal.disk(1);

      Double_t dz0{pointmmTocm(disk0.geomInfo().crateDeltaZ())};
      Double_t dz1{pointmmTocm(disk1.geomInfo().crateDeltaZ())};
      Double_t rmin{pointmmTocm(disk0.innerRadius())};
      Double_t rmax{pointmmTocm(disk0.outerRadius())};

     TEveGeoShape *calShape0 = new TEveGeoShape();
      TEveGeoShape *calShape1 = new TEveGeoShape();

      calShape0->SetShape(new TGeoTube(rmin, rmax, dz0));
      calShape0->SetMainTransparency(100);
      calShape1->SetShape(new TGeoTube(rmin, rmax, dz1));
      calShape1->SetMainTransparency(100);
      orthodet0->AddElement(calShape0);
      orthodet1->AddElement(calShape1);

     // ... Create cal out of Silicon using the composite shape defined above
      TGeoShape *g0 = new TGeoTube("calo 2D disk0",rmin,rmax,dz0); 
      TGeoVolume *calo0= new TGeoVolume("Calorimeter D0",g0, CsI);
      calo0->SetVisLeaves(kFALSE);
      calo0->SetInvisible();

      TGeoShape *g1 = new TGeoTube("calo 2D disk1",rmin,rmax,dz1); 
      TGeoVolume *calo1= new TGeoVolume("Calorimeter D1",g1, CsI);
      calo1->SetVisLeaves(kFALSE);
      calo1->SetInvisible();

      CLHEP::Hep3Vector calo0Pos(0,0,dz0);
      CLHEP::Hep3Vector CaloCenterD0 = PointToCalo(calo0Pos,0);
      hep3vectorTocm(CaloCenterD0);
      topvol->AddNode(calo0, 1, new TGeoTranslation(CaloCenterD0.x(),CaloCenterD0.y(),CaloCenterD0.z()));

      CLHEP::Hep3Vector calo1Pos(0,0,dz1);
      CLHEP::Hep3Vector CaloCenterD1 = PointToCalo(calo1Pos,1);
      hep3vectorTocm(CaloCenterD1);
      topvol->AddNode(calo1, 1, new TGeoTranslation(CaloCenterD1.x(),CaloCenterD1.y(),CaloCenterD1.z())); 
*/
      for(unsigned int i = 0; i < 1348 ; i++){ 
        Crystal const &crystal = cal.crystal(i);
        double crystalXLen = pointmmTocm(crystal.size().x());
        double crystalYLen = pointmmTocm(crystal.size().y());
        double crystalZLen = pointmmTocm(crystal.size().z());

        CLHEP::Hep3Vector crystalPos   = cal.geomUtil().mu2eToDiskFF(0,crystal.position());
        Double_t origin[3];
        if(i < 674) crystalPos = PointToCalo(crystalPos, 0);
        else crystalPos = PointToCalo(crystalPos, 1);
        hep3vectorTocm(crystalPos);
        origin [0] = crystalPos.x();
        origin [1] = crystalPos.y();
        origin [2] = crystalPos.z();
        TEveGeoShape *crystalShape   = new TEveGeoShape();
        crystalShape->SetMainTransparency(100);
        crystalShape->SetShape(new TGeoBBox("crystalD0", (crystalXLen/2), (crystalYLen/2), (crystalZLen/2)/10, origin));
        if(i < 674){
          orthodet0->AddElement(crystalShape);  
          TGeoShape *c = new TGeoBBox("crystalD0", (crystalXLen/2), (crystalYLen/2), (crystalZLen/2));
          TGeoVolume *cry= new TGeoVolume("cryD0",c, CsI);
          cry->SetVisLeaves(kFALSE);
          cry->SetInvisible();
          topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x(),crystalPos.y(),crystalPos.z()));
        } else {
          crystalShape->SetShape(new TGeoBBox("crystalD1", (crystalXLen/2), (crystalYLen/2), (crystalZLen/2), origin));
          orthodet1->AddElement(crystalShape);
          TGeoShape *c = new TGeoBBox("crystalD1", (crystalXLen/2), (crystalYLen/2), (crystalZLen/2));
          TGeoVolume *cry= new TGeoVolume("cryD1",c, CsI);
          cry->SetVisLeaves(kFALSE);
          cry->SetInvisible();
          topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x(),crystalPos.y(),crystalPos.z())); 
        }
      }
  }

}