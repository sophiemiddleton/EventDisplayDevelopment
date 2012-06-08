//
// Free function to construct version 3 of the TTracker
//
// $Id: constructTTrackerv3.cc,v 1.26 2012/06/08 22:32:18 kutschke Exp $
// $Author: kutschke $
// $Date: 2012/06/08 22:32:18 $
//
// Original author KLG based on RKK's version using different methodology
//
// Notes

//     This version makes logical mother volumes per device and per
//     sector and places sectors in device and straws in sector
//     It has only one sector/device logical volume placed several times
//     This versoin has a negligeable construction time and a much smaler memory footprint


// C++ includes
#include <iomanip>
#include <iostream>
#include <string>

// Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// Mu2e includes
#include "G4Helper/inc/G4Helper.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "Mu2eG4/inc/SensitiveDetectorName.hh"
#include "Mu2eG4/inc/StrawSD.hh"
#include "Mu2eG4/inc/constructTTracker.hh"
#include "Mu2eG4/inc/findMaterialOrThrow.hh"
#include "Mu2eG4/inc/finishNesting.hh"
#include "Mu2eG4/inc/nestTubs.hh"
#include "TTrackerGeom/inc/TTracker.hh"

// G4 includes
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4IntersectionSolid.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"


using namespace std;

namespace mu2e{

  VolumeInfo constructTTrackerv3( VolumeInfo const& mother,
                                  double zOff,
                                  SimpleConfig const& config ){

    G4Helper    & _helper = *(art::ServiceHandle<G4Helper>());
    AntiLeakRegistry & reg = _helper.antiLeakRegistry();

    int verbosityLevel = config.getInt("ttracker.verbosityLevel",0);

    // Control of graphics for debugging the geometry.
    // Only instantiate sectors to be drawn.
    int devDraw = config.getInt("ttracker.devDraw",-1);
    int secDraw = config.getInt("ttracker.secDraw",-1);
    bool const doSurfaceCheck = config.getBool("g4.doSurfaceCheck",false);
    bool const forceAuxEdgeVisible = config.getBool("g4.forceAuxEdgeVisible",false);

    G4ThreeVector const zeroVector(0.0,0.0,0.0);

    // Master geometry for the TTracker.
    TTracker const & ttracker = *(GeomHandle<TTracker>());

    // Make the envelope volume that holds the full tracker.
    TubsParams envelopeParams = ttracker.getTrackerEnvelopeParams();

    static int const newPrecision = 8;
    static int const newWidth = 14;

    if (verbosityLevel > 0) {
      int oldPrecision = cout.precision(newPrecision);
      int oldWidth = cout.width(newWidth);
      std::ios::fmtflags oldFlags = cout.flags();
      cout.setf(std::ios::fixed,std::ios::floatfield);
      cout << "Debugging tracker env envelopeParams ir,or,zhl,phi0,phimax:            " <<
	"   " <<
	envelopeParams.innerRadius() << ", " <<
	envelopeParams.outerRadius() << ", " <<
	envelopeParams.zHalfLength() << ", " <<
	envelopeParams.phi0()        << ", " <<
	envelopeParams.phiMax()      << ", " <<
	endl;
      cout.setf(oldFlags);
      cout.precision(oldPrecision);
      cout.width(oldWidth);
    }

    G4ThreeVector trackerOffset( 0., 0., ttracker.z0()-zOff );

    G4Material* envelopeMaterial = findMaterialOrThrow(ttracker.envelopeMaterial());

    VolumeInfo motherInfo = nestTubs( "TrackerMother",
                                      envelopeParams,
                                      envelopeMaterial,
                                      0,
                                      trackerOffset,
                                      mother,
                                      0,
                                      config.getBool("ttracker.envelopeVisible",false),
                                      G4Colour::Blue(),
                                      config.getBool("ttracker.envelopeSolid",true),
                                      forceAuxEdgeVisible,
                                      true,
                                      doSurfaceCheck
                                      );

    if ( verbosityLevel > 0) {
      double zhl         = static_cast<G4Tubs*>(motherInfo.solid)->GetZHalfLength();
      double motherOffsetInMu2eZ = motherInfo.centerInMu2e()[CLHEP::Hep3Vector::Z];
      int oldPrecision = cout.precision(3);
      std::ios::fmtflags oldFlags = cout.flags();
      cout.setf(std::ios::fixed,std::ios::floatfield);
      cout << __func__ << " motherOffsetZ           in Mu2e    : " <<
        motherOffsetInMu2eZ << endl;
      cout << __func__ << " mother         Z extent in Mu2e    : " <<
        motherOffsetInMu2eZ - zhl << ", " << motherOffsetInMu2eZ + zhl << endl;
      cout.setf(oldFlags);
      cout.precision(oldPrecision);
    }

    TubsParams deviceEnvelopeParams = ttracker.getDeviceEnvelopeParams();

    bool ttrackerDeviceEnvelopeVisible = config.getBool("ttracker.deviceEnvelopeVisible",false);
    bool ttrackerDeviceEnvelopeSolid   = config.getBool("ttracker.deviceEnvelopeSolid",true);
    bool ttrackerSupportVisible        = config.getBool("ttracker.supportVisible",false);
    bool ttrackerSupportSolid          = config.getBool("ttracker.supportSolid",true);
    bool ttrackerSectorEnvelopeVisible = config.getBool("ttracker.sectorEnvelopeVisible",false);
    bool ttrackerSectorEnvelopeSolid   = config.getBool("ttracker.sectorEnvelopeSolid",true);
    bool ttrackerStrawVisible          = config.getBool("ttracker.strawVisible",false);
    bool ttrackerStrawSolid            = config.getBool("ttracker.strawSolid",true);

    // construct one logical device (device # 0) with straws inside it

    //nestSomething create a physical volume, we need a logical one first

    size_t idev = 0;
    const Device& device = ttracker.getDevice(idev);

    VolumeInfo devInfo = nestTubs( "TTrackerDeviceEnvelope",
                                   deviceEnvelopeParams,
                                   envelopeMaterial,
                                   0,
                                   zeroVector,
                                   0,
                                   0,
                                   ttrackerDeviceEnvelopeVisible,
                                   G4Colour::Magenta(),
                                   ttrackerDeviceEnvelopeSolid,
                                   forceAuxEdgeVisible,
                                   false,
                                   doSurfaceCheck
                                   );


    // place the support material here

    TubsParams ttrackerDeviceSupportParams = ttracker.getSupportParams().getTubsParams();


    G4Colour  lightBlue (0.0, 0.0, 0.75);
    VolumeInfo supportInfo = nestTubs( "TTrackerDeviceSupport",
                                       ttrackerDeviceSupportParams,
                                       findMaterialOrThrow(ttracker.getSupportParams().materialName()),
                                       0,
                                       zeroVector,
                                       devInfo.logical,
                                       0,
                                       ttrackerSupportVisible,
                                       lightBlue,
                                       ttrackerSupportSolid,
                                       forceAuxEdgeVisible,
                                       true,
                                       doSurfaceCheck
                                       );

    if ( verbosityLevel > 0) {
      cout << "TTrackerDeviceSupport params: "
           << ttrackerDeviceSupportParams.innerRadius() << " "
           << ttrackerDeviceSupportParams.outerRadius() << " "
           << ttrackerDeviceSupportParams.zHalfLength() << " "
           << endl;
    }

    // Make TTrackerDeviceSupport a sensitive detector for radiation damage studies

    supportInfo.logical->
      SetSensitiveDetector(G4SDManager::GetSDMpointer()->
                           FindSensitiveDetector(SensitiveDetectorName::TTrackerDeviceSupport()) );


    if (verbosityLevel > 0 ) {
      int oldPrecision = cout.precision(newPrecision);
      int oldWidth = cout.width(newWidth);
      std::ios::fmtflags oldFlags = cout.flags();
      cout.setf(std::ios::fixed,std::ios::floatfield);
      cout << "Debugging device env idev, deviceEnvelopeParams ir,or,zhl,phi0,phimax: " <<
      idev << ", " <<
      deviceEnvelopeParams.innerRadius() << ", " <<
      deviceEnvelopeParams.outerRadius() << ", " <<
      deviceEnvelopeParams.zHalfLength() << ", " <<
      deviceEnvelopeParams.phi0()        << ", " <<
      deviceEnvelopeParams.phiMax()      << ", " <<
      endl;
      cout.setf(oldFlags);
      cout.precision(oldPrecision);
      cout.width(oldWidth);
    }

    // place straws etc... wrt the envelope

    // create a "sector" volume

    // Construct One sector logical volume (and then place it N times)

    const size_t isec = 0;

    const Sector& sector = device.getSector(isec);

    // constructing sector envelope

    // Make a logical volume for this sector,

    // G4IntersectionSolid of G4Box and G4Trd to avoid overlaps of two envelopes

    // reuse device attributes for now

    // get the length of the innermost straw
    Layer const& layer0        = sector.getLayer(0);
    Straw const& straw0        = layer0.getStraw(0);
    StrawDetail const& detail0 = straw0.getDetail();

    verbosityLevel > 0 &&
      cout << "Debugging sector box isec detail0.halfLength(): " << detail0.halfLength() << endl;


    VolumeInfo secInfo;
    secInfo.name = "TTrackerSectorEnvelope";

    G4Box* secBox = new G4Box(secInfo.name+"Box",
                              detail0.halfLength(),
                              sector.boxHalfLengths()[2],
                              sector.boxHalfLengths()[1]
                              );

    G4Trd* secTrd = new G4Trd(secInfo.name+"Trd",
                              sector.boxHalfLengths()[4],
                              sector.boxHalfLengths()[3],
                              sector.boxHalfLengths()[2],
                              sector.boxHalfLengths()[2],
                              sector.boxHalfLengths()[1]
                              );

    // one could also intersect it with a ring to decrease its radial spread

    secInfo.solid =
      new G4IntersectionSolid(secInfo.name, secBox, secTrd);

    // false for placing physical volume, just create a logical one
    finishNesting(secInfo,
                  envelopeMaterial,
                  0,
                  zeroVector,
                  0,
                  0,
                  ttrackerSectorEnvelopeVisible,
                  G4Colour::Cyan(),
                  ttrackerSectorEnvelopeSolid,
                  forceAuxEdgeVisible,
                  false,
                  doSurfaceCheck
                  );

    if (verbosityLevel > 0 ){
      int oldPrecision = cout.precision(newPrecision);
      int oldWidth = cout.width(newWidth);
      std::ios::fmtflags oldFlags = cout.flags();
      cout.setf(std::ios::fixed,std::ios::floatfield);
      cout << "Debugging sector box isec, sector.boxHalfLengths().at(4,3,2,2,1): " <<
	isec << ", " <<
	sector.boxHalfLengths().at(4) << ", " <<
	sector.boxHalfLengths().at(3) << ", " <<
	sector.boxHalfLengths().at(2) << ", " <<
	sector.boxHalfLengths().at(2) << ", " <<
	sector.boxHalfLengths().at(1) << ", " <<
	endl;
      cout.setf(oldFlags);
      cout.precision(oldPrecision);
      cout.width(oldWidth);
    }

    static double const tRAngle  = M_PI_2;
    static double const tRAngle2 = M_PI;
    CLHEP::HepRotationX RXForTrapezoids(tRAngle);
    CLHEP::HepRotationX RX2ForTrapezoids(tRAngle2);
    CLHEP::HepRotationY RYForTrapezoids(tRAngle);
    CLHEP::HepRotationZ RZForTrapezoids(tRAngle);

    G4RotationMatrix* rotTub = reg.add(G4RotationMatrix(RYForTrapezoids));

    for ( int ilay =0; ilay<sector.nLayers(); ++ilay ){

      verbosityLevel > 1 &&   cout << "Debugging constructTTrackerv3 ilay: " << ilay << endl;

      const Layer& layer = sector.getLayer(ilay);

      for ( int istr=0; istr<layer.nStraws(); ++istr ){

        // "second" layer will have fewer straws (for now) also see TTrackerMaker
        // no, it complicates StrawSD and TTrackerMaker
        // if( ilay%2==1 && istr+1 == layer.nStraws() ) break;

        const Straw& straw = layer.getStraw(istr);

        StrawDetail const& detail = straw.getDetail();

        TubsParams strawWallParams( 0.0, detail.outerRadius(), detail.halfLength() );
        TubsParams strawGasParams ( 0.0, detail.innerRadius(), detail.halfLength() );
        TubsParams strawWireParams( 0.0, detail.wireRadius(),  detail.halfLength() );

        // we are placing the straw w.r.t the trapezoid...
        // the trapezoid aka device envelope has a different coordinate system x->z, z->y, y->x

        G4ThreeVector const mid(straw.getMidPoint().y() - sector.boxOffset().y(),
                                straw.getMidPoint().z() - sector.boxOffset().z(),
                                straw.getMidPoint().x() - sector.boxOffset().x());

        G4ThreeVector const zeroVector(0.0,0.0,0.0);

        if ( verbosityLevel > 2 ) {

          cout << "Debugging istr: " << istr <<
            " mid: " << mid <<
            ", straw.MidPoint " << straw.getMidPoint() <<
            ", sector.boxOffset " <<  sector.boxOffset() <<
            ", device.origin " << device.origin() <<
            endl;

          cout << "Debugging istr: " << istr << " mid: " <<
            mid << ", halflenght " << detail.halfLength() << endl;

          // look at StrawSD to see how the straw index is reconstructed

          cout << "Debugging straw.id(), straw.index() " <<
            straw.id() << ", " << straw.index() << endl;

        }

        // make the straws more distinguishable when displayed
        G4Colour wallColor = (ilay%2 == 1) ?
          ((istr%2 == 0) ? G4Colour::Green() : G4Colour::Yellow()) :
          ((istr%2 == 0) ? G4Colour::Red() : G4Colour::Blue());

        G4Colour gasColor = (ilay%2 == 0) ?
          ((istr%2 == 0) ? G4Colour::Green() : G4Colour::Yellow()) :
          ((istr%2 == 0) ? G4Colour::Red() : G4Colour::Blue());

        G4Colour wireColor = G4Colour::Cyan();

        if (verbosityLevel > 2) {
	  int oldPrecision = cout.precision(newPrecision);
	  int oldWidth = cout.width(newWidth);
	  std::ios::fmtflags oldFlags = cout.flags();
	  cout.setf(std::ios::fixed,std::ios::floatfield);
          cout << "Debugging Straw istr, RYForTrapezoids, midpoint: " <<
	    istr << ", " << RYForTrapezoids << ", " <<
	    mid << ", " <<
	    endl;
	  cout.setf(oldFlags);
	  cout.precision(oldPrecision);
	  cout.width(oldWidth);
	}

        VolumeInfo strawWallInfo  = nestTubs(straw.name("TTrackerStrawWall_"),
                                             strawWallParams,
                                             findMaterialOrThrow(detail.wallMaterialName() ),
                                             rotTub,
                                             mid,
                                             secInfo.logical,
                                             straw.index().asInt(),
                                             ttrackerStrawVisible,
                                             wallColor,
                                             ttrackerStrawSolid,
                                             forceAuxEdgeVisible,
                                             true,
                                             doSurfaceCheck
                                             );

        // may be not all straws have to be in the volInfo registry, another param?
        // we use the Straw name facility to make them unique

        VolumeInfo strawGasInfo  = nestTubs(straw.name("TTrackerStrawGas_"),
                                            strawGasParams,
                                            findMaterialOrThrow(detail.gasMaterialName()),
                                            0,
                                            zeroVector,
                                            strawWallInfo.logical,
                                            straw.index().asInt(),
                                            ttrackerStrawVisible,
                                            gasColor,
                                            ttrackerStrawSolid,
                                            forceAuxEdgeVisible,
                                            true,
                                            doSurfaceCheck
                                            );

        VolumeInfo strawWireInfo  = nestTubs(straw.name("TTrackerStrawWire_"),
                                             strawWireParams,
                                             findMaterialOrThrow(detail.wireMaterialName()),
                                             0,
                                             zeroVector,
                                             strawGasInfo.logical,
                                             straw.index().asInt(),
                                             ttrackerStrawVisible,
                                             wireColor,
                                             ttrackerStrawSolid,
                                             forceAuxEdgeVisible,
                                             true,
                                             doSurfaceCheck
                                             );

        // Make gas of this straw a sensitive detector.

        strawGasInfo.logical->
          SetSensitiveDetector(G4SDManager::GetSDMpointer()->
                               FindSensitiveDetector(SensitiveDetectorName::TrackerGas()) );

      }   // end loop over straws
    }     // end loop over layers

    // We have constructed one sector, Now place the sectors (in the logical device)

    for ( int isec = 0; isec<device.nSectors(); ++isec){

      if ( secDraw > -1 && isec > secDraw ) continue;

      verbosityLevel > 1 &&
        cout << "Debugging sector: " << isec << " " << secInfo.name << " secDraw: " << secDraw << endl;

      const Sector& sector = device.getSector(isec);

      // place the trapezoid in its position ready for the RZ rotation

      CLHEP::HepRotationZ RZ(sector.boxRzAngle() - device.rotation()); // well we know it is only arround z...

      verbosityLevel > 1 &&
        cout << "Debugging sector.boxRzAngle(), device.rotation(): " << sector.boxRzAngle() << " " <<
        device.rotation() << endl;

      // we add an 180deg rotation for even sectors
      G4RotationMatrix* secRotation = ((isec%2)==1) ?
      reg.add(G4RotationMatrix(RXForTrapezoids*RZForTrapezoids*RZ.inverse())):
      reg.add(G4RotationMatrix(RXForTrapezoids*RZForTrapezoids*RX2ForTrapezoids*RZ.inverse()));

      // origin a.k.a offset wrt current mother volume
      CLHEP::Hep3Vector origin = sector.boxOffset() - device.origin();

      verbosityLevel > 1 &&
        cout << "Debugging sector.origin:      "   << isec << " " << secInfo.name << origin << endl;
      verbosityLevel > 1 &&
        cout << "Debugging sector.boxOffset(): " << isec << " " << secInfo.name << sector.boxOffset() << endl;

      // we may need to keep those pointers somewhre... (this is only the last one...)

      secInfo.physical =  new G4PVPlacement(secRotation,
                                            origin,
                                            secInfo.logical,
                                            secInfo.name,
                                            devInfo.logical,
                                            false,
                                            isec,
                                            doSurfaceCheck);

    }       // end loop over sectors

    // we have constructed one logical device above, we need to place it a few times now...

    for ( int idev=0; idev<ttracker.nDevices(); ++idev ){

      // changes here affect StrawSD

      if ( devDraw > -1 && idev > devDraw ) continue;

      verbosityLevel > 1 &&
        cout << "Debugging dev: " << idev << " " << devInfo.name << " devDraw: " << devDraw << endl;

      const Device& device = ttracker.getDevice(idev);

      CLHEP::HepRotationZ RZ(-device.rotation()); //It is arround z

      G4RotationMatrix* devRotation  = reg.add(G4RotationMatrix(RZ));

      verbosityLevel > 1 &&
        cout << "Debugging -device.rotation(): " << -device.rotation() << " " << endl;
      verbosityLevel > 1 &&
        cout << "Debugging device.origin(): " << device.origin() << " " << endl;

      // could we descend the final hierarchy and set the "true" copy numbers?

      // we may need to keep those pointers somewhre... (this is only the last one...)
      devInfo.physical =  new G4PVPlacement(devRotation,
                                            device.origin(),
                                            devInfo.logical,
                                            devInfo.name,
                                            motherInfo.logical,
                                            false,
                                            idev,
                                            doSurfaceCheck);


    } // end loop over devices

    return motherInfo;

  } // end of constructTTrackerv3

} // end namespace mu2e
