#include "TEveEventDisplay/src/shape_classes/TEveMu2eCalorimeter.h"
#include <TBox.h>
#include <TGeoBBox.h>
using namespace mu2e;
namespace mu2e{

    TEveMu2eCalorimeter::TEveMu2eCalorimeter(){};

    void TEveMu2eCalorimeter::DrawCaloDetector(art::Run const& run, TGeoVolume* topvol, TEveElementList *orthodet0,TEveElementList *orthodet1){
      TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);
      TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);
      

      Calorimeter const &cal = *(GeomHandle<Calorimeter>());
      const Disk& disk0 = cal.disk(0);
      const Disk& disk1 = cal.disk(1);

      Double_t dz0{disk0.geomInfo().crateDeltaZ()/10};
      Double_t dz1{disk1.geomInfo().crateDeltaZ()/10};
      Double_t rmin{disk0.innerRadius()/10};
      Double_t rmax{disk0.outerRadius()/10};

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
      TGeoVolume *calo0= new TGeoVolume("Calorimeter D0",g0, Si);
      calo0->SetVisLeaves(kFALSE);
      calo0->SetInvisible();

      TGeoShape *g1 = new TGeoTube("calo 2D disk1",rmin,rmax,dz1); 
      TGeoVolume *calo1= new TGeoVolume("Calorimeter D1",g1, Si);
      calo1->SetVisLeaves(kFALSE);
      calo1->SetInvisible();

      CLHEP::Hep3Vector calo0Pos(0,0,dz0);

      topvol->AddNode(calo0, 1, new TGeoTranslation(-390.4,0,1194.2));

      CLHEP::Hep3Vector calo1Pos(0,0,dz1);

      topvol->AddNode(calo1, 1, new TGeoTranslation(-390.4,0,1262.0));


      //...Add in the crystals in D0:
      for(unsigned int i = 0; i <674 ; i++){
        Crystal const &crystal = cal.crystal(i);
        double crystalXLen = crystal.size().x();
        double crystalYLen = crystal.size().y();
        double crystalZLen = crystal.size().z();

        CLHEP::Hep3Vector crystalPos   = cal.geomUtil().mu2eToDiskFF(0,crystal.position());
        Double_t origin[3];
        origin [0] = crystalPos.x()/10;
        origin [1] = crystalPos.y()/10;
        origin [2] = crystalPos.z()/10;

        TEveGeoShape *crystalShape   = new TEveGeoShape();
        crystalShape->SetMainTransparency(100);
        crystalShape->SetShape(new TGeoBBox("crystalD0", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10, origin));
        orthodet0->AddElement(crystalShape);
        
        TGeoShape *c = new TGeoBBox("crystalD0", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10);
        TGeoVolume *cry= new TGeoVolume("cryD0",c, Si);
        cry->SetVisLeaves(kFALSE);
        cry->SetInvisible();
        topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x()/10,crystalPos.y()/10,crystalPos.z()/10));

      }

      //...Add in crystals to D1:
      for(unsigned int i = 0; i <674 ; i++){
        Crystal const &crystal = cal.crystal(i);
        double crystalXLen = crystal.size().x();
        double crystalYLen = crystal.size().y();
        double crystalZLen = crystal.size().z();

        CLHEP::Hep3Vector crystalPos   = cal.geomUtil().mu2eToDiskFF(1,crystal.position());
        Double_t origin[3];
        origin [0] = crystalPos.x()/10;
        origin [1] = crystalPos.y()/10;
        origin [2] = crystalPos.z()/10;

        TEveGeoShape *crystalShape   = new TEveGeoShape();
        crystalShape->SetMainTransparency(100);
        crystalShape->SetShape(new TGeoBBox("crystalD1", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10, origin));
        orthodet1->AddElement(crystalShape);
        
        TGeoShape *c = new TGeoBBox("crystalD1", (crystalXLen/2)/10, (crystalYLen/2)/10, (crystalZLen/2)/10);
        TGeoVolume *cry= new TGeoVolume("cryD1",c, Si);
        cry->SetVisLeaves(kFALSE);
        cry->SetInvisible();
        topvol->AddNode(cry, 1, new TGeoTranslation(crystalPos.x()/10,crystalPos.y()/10,crystalPos.z()/10));

      }
      
    }

}
