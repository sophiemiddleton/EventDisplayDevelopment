//Author: S Middleton
//Date: Feb 2020
//Purpose: For carrying out functions related to the GeomServices in TEve event display
#include <TObject.h>
#include <TSystem.h>
// ... libRIO
#include <TFile.h>
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
//Geom:
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/WorldG4.hh"
#include "GeometryService/inc/WorldG4Maker.hh"
#include "GeometryService/inc/TrackerMaker.hh"
//Mu2e Tracker Geom:
#include "TrackerGeom/inc/Tracker.hh"
#include "GeometryService/inc/Mu2eCoordTransform.hh"
#include "BFieldGeom/inc/BFieldManager.hh"
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkDiag/inc/TrkMCTools.hh"

#include "TEveEventDisplay/src/dict_classes/Geom_Interface.h"
using namespace mu2e;
namespace mu2e{

	Geom_Interface::Geom_Interface(){}

	CLHEP::Hep3Vector Geom_Interface::GetTrackerCenter(){
                std::string filename("Mu2eG4/geom/geom_common_current.txt");
		SimpleConfig GeomConfig(filename);
		double zCenter  =  GeomConfig.getDouble("mu2e.detectorSystemZ0")*CLHEP::mm;
		double xCenter  = -GeomConfig.getDouble("mu2e.solenoidOffset")*CLHEP::mm;
		CLHEP::Hep3Vector c(xCenter, 0, zCenter);//y=0 as unchanged
		return c;
	}

	double Geom_Interface::GetOffsetFromMu2e(){
		std::string filename("Mu2eG4/geom/mu2eHall.txt");
		SimpleConfig HallConfig(filename);
		double yCenter  =  0;//HallConfig.getDouble("yOfFloorSurface.below.mu2eOrigin")*CLHEP::mm;
		//double xCenter  = -HallConfig.getDouble("mu2e.solenoidOffset")*CLHEP::mm;
		return yCenter;
	}
	CLHEP::Hep3Vector Geom_Interface::GetGDMLTrackerCenter(TString filename) {
		TGeoManager *geom;
		geom = geom->TGeoManager::Import(filename);
		
  		TGeoNode* node = gGeoManager->GetTopNode();
		std::string _name = (node->GetVolume()->GetName());
		if ( node->GetMotherVolume() ) {
			std::string motherName(node->GetMotherVolume()->GetName());
			bool isinDS = false;
			if ( motherName == "DS2Vacuum" or motherName =="DS3Vacuum"){
			    cout<<"got DS coords"<<endl;
			    isinDS = true;
			}
			if ( isinDS==true and _name.find("TrackerMother")!=0 ){
			    cout<<"got tracker - the tracker is inside the DS - need to transform from there backwards "<<endl;

			}
		}
		double zCenter  =  -1610;
		double xCenter  = 0;
		CLHEP::Hep3Vector c(xCenter, 0, zCenter);
	        cout<<"center forund at : "<<zCenter<<" 0 "<<xCenter<<endl;
		return c;
	}

	CLHEP::Hep3Vector Geom_Interface::PointToGDML(CLHEP::Hep3Vector point){
		//Step 1: Get Tracker origin ~ (-3904,0,10171):
		CLHEP::Hep3Vector Mu2eTrackerOrigin = Geom_Interface::GetTrackerCenter();
		//Step 2: Extract origin from GDML file:
		CLHEP::Hep3Vector GDMLTrackerOrigin(0, GetOffsetFromMu2e()  ,0);
		//Step 3: Transform Tracker origin to the GDML origin. Need to find the transform of tracker ofigin -->GDML origin:
		CLHEP::Hep3Vector Tracker2GDMLOrigin(GDMLTrackerOrigin - Mu2eTrackerOrigin);
		//Step 4: Transfrom a point first to tracker, then to GDML
		CLHEP::Hep3Vector PointToTracker(point.x() + Mu2eTrackerOrigin.x(), point.y()+Mu2eTrackerOrigin.y(), point.z() +Mu2eTrackerOrigin.z());
		cout<<" in tracker "<<PointToTracker.x()<<" "<<PointToTracker.y()<<" "<<PointToTracker.z()<<endl;
		CLHEP::Hep3Vector PointToGDML(PointToTracker.x()+Tracker2GDMLOrigin.x(),PointToTracker.y() +Tracker2GDMLOrigin.y(),PointToTracker.z()+Tracker2GDMLOrigin.z());
		return PointToGDML;
	}

	CLHEP::Hep3Vector Geom_Interface::TransformToG4(CLHEP::Hep3Vector point){
		//GeomHandle<WorldG4> world;
		//WorldG4Maker worldMaker;
		//std::unique_ptr<WorldG4> world = worldMaker.res(new WorldG4());

		//1) Get origin of the Tracker from GeomServices:
		//CLHEP::Hep3Vector center = GetTrackerCenter();
		//Mu2eCoordTransform trans();
		//CLHEP::Hep3Vector vec2 = trans.Mu2eToG4bl(vec1);
		//CLHEP::Hep3Vector vec2 = res->mu2eOriginInWorld();//world.mu2eOriginInWorld();
		
		return point;

	}

	CLHEP::Hep3Vector Geom_Interface::TransformToDet(CLHEP::Hep3Vector vec1){
		GeomHandle<DetectorSystem> det;
		CLHEP::Hep3Vector vec2 = det->toDetector(vec1);
		return vec2;
    	}


}
