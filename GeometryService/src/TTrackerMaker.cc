//
// Construct and return a TTracker.
//
// Original author Rob Kutschke
//
// Significant change mf 5/30/11:
//    Insertion into the Straw objects data useful for early (hit/miss
//    based) pattern recognition.  See identifyDirectionalNeighborStraws().

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "ConfigTools/inc/SimpleConfig.hh"
#include "TTrackerGeom/inc/TTracker.hh"
#include "GeometryService/inc/TTrackerMaker.hh"
#include "TrackerGeom/inc/Panel.hh"
#include "TrackerGeom/inc/Straw.hh"
#include "cetlib/pow.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <unordered_map>

using cet::square;
using cet::diff_of_squares;
using cet::sum_of_squares;

using namespace std;

namespace mu2e {

  // Constructor that gets information from the config file instead of
  // from arguments.
  TTrackerMaker::TTrackerMaker( SimpleConfig const& config){
    parseConfig(config);
    buildIt( );

    // print straw layout for debbugging pourposes

    if (_verbosityLevel>2) {

      int idev = -1;
      int isec = -1;
      int ilay = -1;
      double iang = -36000;

      size_t nstraws = (_tt->_allStraws).size();
      for (size_t istr=0; istr!=nstraws; ++istr) {

        const Straw& straw = _tt->getStraw(StrawIndex(istr));

        int cdev = straw.id().getPlane();
        int csec = straw.id().getPanel();
        int clay = straw.id().getLayer();

        const Plane& plane = _tt->getPlane(cdev);
        const Panel& panel = plane.getPanel(csec);
        const Layer&  layer  = panel.getLayer(clay);

        size_t nStrawsPerPanel = panel.nLayers()  * layer.nStraws();
        size_t nStrawsPerPlane = plane.nPanels() * nStrawsPerPanel;

        double cang = panel.boxRzAngle()/M_PI*180.;
        double dang = plane.rotation()/M_PI*180.;
        double sroz = (panel.boxOffset() - plane.origin()).z();

        size_t isecf = nStrawsPerPanel*csec + nStrawsPerPlane*cdev;

        cout << __func__ << " Straw "
             << fixed << setw(6) << istr
             << " secfloor " << setw(6) << isecf << " "
             << straw.id()
             << " mid point " << straw.getMidPoint()
             << " r " << sqrt(straw.getMidPoint()[0]*straw.getMidPoint()[0]+
                              straw.getMidPoint()[1]*straw.getMidPoint()[1])
             << " direction " << straw.getDirection()
             << " panel rotation: " << cang
             << " origin " << panel.boxOffset()
             << " plane rotation: " << dang
             << " origin " << plane.origin()
             << " sec rel origin z " << sroz
             << " straw exists " << _tt->strawExists(StrawIndex(istr))
             << " plane exists " << plane.exists();

        if (isec>csec && idev==cdev) cout << " <--S";
        if (iang>cang && idev==cdev) cout << " <--A";
        if (ilay>clay && isec==csec) cout << " <--L";
        if ((csec%2 == 0 && sroz>0.) || (csec%2 != 0 && sroz<0.)) cout << " <--Z";
        if (idev!=cdev) idev=cdev;
        if (isec!=csec) isec=csec;
        if (ilay!=clay) ilay=clay;

        cout << endl;

      }

    }

  }

  void TTrackerMaker::parseConfig( const SimpleConfig& config ){

    _verbosityLevel     = config.getInt("ttracker.verbosityLevel",0);


    _motherRIn        = config.getDouble("ttracker.mother.rIn"        )*CLHEP::mm;
    _motherROut       = config.getDouble("ttracker.mother.rOut"       )*CLHEP::mm;
    _motherHalfLength = config.getDouble("ttracker.mother.halfLength" )*CLHEP::mm;
    _motherZ0         = config.getDouble("ttracker.mother.z0"         )*CLHEP::mm;

    _numPlanes         = config.getInt("ttracker.numPlanes");
    _panelsPerPlane   = config.getInt("ttracker.panelsPerPlane");
    _layersPerPanel    = config.getInt("ttracker.layersPerPanel");
    _manifoldsPerEnd    = config.getInt("ttracker.manifoldsPerEnd");
    _strawsPerManifold  = config.getInt("ttracker.strawsPerManifold");
    _rotationPattern    = config.getInt("ttracker.rotationPattern");
    _spacingPattern     = config.getInt("ttracker.spacingPattern");

    _oddStationRotation   =  config.getDouble("ttracker.oddStationRotation")*CLHEP::degree;
    _zCenter              =  config.getDouble("ttracker.z0")*CLHEP::mm;
    _xCenter              = -config.getDouble("mu2e.solenoidOffset")*CLHEP::mm;
    _envelopeInnerRadius  =  config.getDouble("ttracker.envelopeInnerRadius")*CLHEP::mm;
    _strawOuterRadius     =  config.getDouble("ttracker.strawOuterRadius")*CLHEP::mm;
    _strawWallThickness   =  config.getDouble("ttracker.strawWallThickness")*CLHEP::mm;
    _strawGap             =  config.getDouble("ttracker.strawGap")*CLHEP::mm;
    _planeSpacing        =  config.getDouble("ttracker.planeSpacing")*CLHEP::mm;
    _planeHalfSeparation =  config.getDouble("ttracker.planeHalfSeparation")*CLHEP::mm;

    _outerSupportRadius   =  config.getDouble("ttracker.outerSupportRadius")*CLHEP::mm;
    _innerSupportRadius   =  config.getDouble("ttracker.innerSupportRadius")*CLHEP::mm;
    _supportHalfThickness =  config.getDouble("ttracker.supportHalfThickness")*CLHEP::mm;
    _wireRadius           =  config.getDouble("ttracker.wireRadius")*CLHEP::mm;
    _manifoldYOffset      =  config.getDouble("ttracker.manifoldYOffset")*CLHEP::mm;
    _virtualDetectorHalfLength = config.getDouble("vd.halfLength")*CLHEP::mm;

    config.getVectorInt("ttracker.nonExistingPlanes", _nonExistingPlanes,  vector<int>() );

    _verbosityLevel > 0 && _nonExistingPlanes.size()>0 &&
      cout << __func__ << " inactive planes : f/l   "
           << _nonExistingPlanes.front() << " / "
           << _nonExistingPlanes.back()
           << endl;

    config.getVectorDouble("ttracker.manifoldHalfLengths", _manifoldHalfLengths, 3);
    for ( size_t i=0; i<_manifoldHalfLengths.size(); ++i ){
      _manifoldHalfLengths.at(i) *= CLHEP::mm;
    }

    config.getVectorString("ttracker.strawMaterials", _strawMaterials, 3);

    _envelopeMaterial = config.getString("ttracker.mat.vacuum");
    _supportMaterial = config.getString("ttracker.mat.support");

    _passivationMargin        =  config.getDouble("ttracker.passivationMargin")*CLHEP::mm;

    // For the detailv0 support model there are lots of extra parameters.
    _supportModel = SupportModel( config.getString("ttrackerSupport.model","simple"));
    if ( _supportModel == SupportModel::detailedv0 ) {
      _endRingOuterRadius      = config.getDouble( "ttrackerSupport.endRing.outerRadius" );
      _endRingInnerRadius      = config.getDouble( "ttrackerSupport.endRing.innerRadius" );
      _endRingHalfLength       = config.getDouble( "ttrackerSupport.endRing.halfLength"  );
      _endRingZOffset          = config.getDouble( "ttrackerSupport.endRing.zOffset"     );
      _endRingMaterial         = config.getString( "ttrackerSupport.endRing.material"    );

      config.getVectorInt( "ttrackerSupport.midRing.slot", _midRingSlot );
      _midRingHalfLength       = config.getDouble(    "ttrackerSupport.midRing.halfLength" );

      // support beams;
      // fixme use vectors to contain them all (e.g. vector<SupportBeamParams>)

      config.getVectorDouble( "ttrackerSupport.beam0.phiRange", _beam0_phiRange );
      _beam0_innerRadius     = config.getDouble( "ttrackerSupport.beam0.innerRadius" );
      _beam0_outerRadius     = config.getDouble( "ttrackerSupport.beam0.outerRadius" );
      _beam0_material        = config.getString( "ttrackerSupport.beam0.material" );

      config.getVectorDouble( "ttrackerSupport.beam1.phiRange", _beam1_phiRange );
      config.getVectorDouble( "ttrackerSupport.beam1.phiSpans", _beam1_phiSpans );
      config.getVectorDouble( "ttrackerSupport.beam1.servicePhi0s", _beam1_servicePhi0s );
      config.getVectorDouble( "ttrackerSupport.beam1.servicePhiEnds", _beam1_servicePhiEnds );
      _beam1_innerRadius = config.getDouble( "ttrackerSupport.beam1.innerRadius" );
      _beam1_midRadius1  = config.getDouble( "ttrackerSupport.beam1.midRadius1" );
      _beam1_midRadius2  = config.getDouble( "ttrackerSupport.beam1.midRadius2" );
      _beam1_outerRadius = config.getDouble( "ttrackerSupport.beam1.outerRadius" );
      _beam1_material    = config.getString( "ttrackerSupport.beam1.material" );
      config.getVectorDouble( "ttrackerSupport.beam1.serviceOuterRadii", _beam1_serviceOuterRadii );
      config.getVectorString( "ttrackerSupport.beam1.serviceMaterials", _beam1_serviceMaterials );
      config.getVectorDouble( "ttrackerSupport.beam1.serviceCovRelThickness", _beam1_serviceCovRelThickness );
      config.getVectorString( "ttrackerSupport.beam1.serviceMaterialsCov", _beam1_serviceMaterialsCov );

      _innerRingInnerRadius    = config.getDouble( "ttrackerSupport.innerRing.innerRadius" );
      _innerRingOuterRadius    = config.getDouble( "ttrackerSupport.innerRing.outerRadius" );
      _innerRingHalfLength     = config.getDouble( "ttrackerSupport.innerRing.halfLength"  );
      _innerRingMaterial       = config.getString( "ttrackerSupport.innerRing.material"    );

      _centerPlateHalfLength   = config.getDouble( "ttrackerSupport.centerPlate.halfLength" );
      _centerPlateMaterial     = config.getString( "ttrackerSupport.centerPlate.material"   );

      _outerRingInnerRadius    = config.getDouble( "ttrackerSupport.outerRing.innerRadius" );
      _outerRingOuterRadius    = config.getDouble( "ttrackerSupport.outerRing.outerRadius" );
      _outerRingMaterial       = config.getString( "ttrackerSupport.outerRing.material"    );

      _coverHalfLength          = config.getDouble( "ttrackerSupport.cover.halfLength"           );
      _coverMaterial            = config.getString( "ttrackerSupport.cover.material"             );
      _electronicsG10HalfLength = config.getDouble( "ttrackerSupport.electronics.g10.halfLength" );
      _electronicsG10Material   = config.getString( "ttrackerSupport.electronics.g10.material"   );
      _electronicsCuHhalfLength = config.getDouble( "ttrackerSupport.electronics.cu.halfLength"  );
      _electronicsCuMaterial    = config.getString( "ttrackerSupport.electronics.cu.material"    );
      _channelZOffset           = config.getDouble( "ttrackerSupport.channel.zOffset"            );
      _channelDepth             = config.getDouble( "ttrackerSupport.channel.depth"              );
      _channelMaterial          = config.getString( "ttrackerSupport.channel.material"           );
      _electronicsSpaceMaterial = config.getString( "ttrackerSupport.electronicsSpace.material"  );

      _wallOuterMetalThickness  = config.getDouble("ttracker.straw.wallOuterMetal.thickness")*CLHEP::mm;
      _wallInnerMetal1Thickness = config.getDouble("ttracker.straw.wallInnerMetal1.thickness")*CLHEP::mm;
      _wallInnerMetal2Thickness = config.getDouble("ttracker.straw.wallInnerMetal2.thickness")*CLHEP::mm;
      _wirePlateThickness       = config.getDouble("ttracker.straw.wirePlate.thickness")*CLHEP::mm;
      _wallOuterMetalMaterial   = config.getString("ttracker.straw.wallOuterMetal.material");
      _wallInnerMetal1Material  = config.getString("ttracker.straw.wallInnerMetal1.material");
      _wallInnerMetal2Material  = config.getString("ttracker.straw.wallInnerMetal2.material");
      _wirePlateMaterial        = config.getString("ttracker.straw.wirePlate.material");

    }

    //string ttracker.mat.manifold  = "G4_Al";  // Placeholder.

    // Also define some parameters that may become variable some day.
    _panelBaseRotations.clear();
    _panelZSide.clear();

    if (_panelsPerPlane == 6 ){
  // the Z pattern here is forced by the 'alternating panel id' convention
  // that permeates the rest of the code.
        _panelZSide.push_back(-1.);
        _panelZSide.push_back(+1.);
        _panelZSide.push_back(-1.);
        _panelZSide.push_back(+1.);
        _panelZSide.push_back(-1.);
        _panelZSide.push_back(+1.);
      if(_rotationPattern == 1){
// cdr geometry, taken from DOC 888, also alternatives 1 and 3 from doc 2799
        // faces overlap by 60 degrees
// Implicitly define the rotations for the even and odd (sequentially) panels.
        _panelBaseRotations.push_back(  45.*CLHEP::degree);
        _panelBaseRotations.push_back( 105.*CLHEP::degree);
        _panelBaseRotations.push_back( 165.*CLHEP::degree);
        _panelBaseRotations.push_back( 225.*CLHEP::degree);
        _panelBaseRotations.push_back( 285.*CLHEP::degree);
        _panelBaseRotations.push_back( 345.*CLHEP::degree);
        _panelBaseRotations.push_back(  75.*CLHEP::degree);
        _panelBaseRotations.push_back(  15.*CLHEP::degree);
        _panelBaseRotations.push_back( 195.*CLHEP::degree);
        _panelBaseRotations.push_back( 135.*CLHEP::degree);
        _panelBaseRotations.push_back( 315.*CLHEP::degree);
        _panelBaseRotations.push_back( 255.*CLHEP::degree);

      } else if(_rotationPattern==2){
        // alternative 2 from DOC 2799
        // faces overlap by 60 degrees
        _panelBaseRotations.push_back(  45.*CLHEP::degree);
        _panelBaseRotations.push_back(  75.*CLHEP::degree);
        _panelBaseRotations.push_back( 165.*CLHEP::degree);
        _panelBaseRotations.push_back( 195.*CLHEP::degree);
        _panelBaseRotations.push_back( 285.*CLHEP::degree);
        _panelBaseRotations.push_back( 315.*CLHEP::degree);
        _panelBaseRotations.push_back( 105.*CLHEP::degree);
        _panelBaseRotations.push_back(  15.*CLHEP::degree);
        _panelBaseRotations.push_back( 225.*CLHEP::degree);
        _panelBaseRotations.push_back( 135.*CLHEP::degree);
        _panelBaseRotations.push_back( 345.*CLHEP::degree);
        _panelBaseRotations.push_back( 255.*CLHEP::degree);

      } else if(_rotationPattern==3){
      // faces overlap by 60 degrees, second plane 'flipped'
        _panelBaseRotations.push_back(   0.*CLHEP::degree);
        _panelBaseRotations.push_back(  90.*CLHEP::degree);
        _panelBaseRotations.push_back(  120.*CLHEP::degree);
        _panelBaseRotations.push_back(  210.*CLHEP::degree);
        _panelBaseRotations.push_back(  240.*CLHEP::degree);
        _panelBaseRotations.push_back(  330.*CLHEP::degree);
        _panelBaseRotations.push_back(  30.*CLHEP::degree);
        _panelBaseRotations.push_back(  60.*CLHEP::degree);
        _panelBaseRotations.push_back(  150.*CLHEP::degree);
        _panelBaseRotations.push_back(  180.*CLHEP::degree);
        _panelBaseRotations.push_back(  270.*CLHEP::degree);
        _panelBaseRotations.push_back(  300.*CLHEP::degree);
      } else if(_rotationPattern==4){
//-----------------------------------------------------------------------------
// Mu2e-2 studies: 2 faces within one plane have parallel straws within
//                 each 120 deg panel
//-----------------------------------------------------------------------------
        _panelBaseRotations.push_back(   0.*CLHEP::degree);
        _panelBaseRotations.push_back(   0.*CLHEP::degree);
        _panelBaseRotations.push_back(  120.*CLHEP::degree);
        _panelBaseRotations.push_back(  120.*CLHEP::degree);
        _panelBaseRotations.push_back(  240.*CLHEP::degree);
        _panelBaseRotations.push_back(  240.*CLHEP::degree);
        _panelBaseRotations.push_back(   60.*CLHEP::degree);
        _panelBaseRotations.push_back(   60.*CLHEP::degree);
        _panelBaseRotations.push_back(  210.*CLHEP::degree);
        _panelBaseRotations.push_back(  210.*CLHEP::degree);
        _panelBaseRotations.push_back(  270.*CLHEP::degree);
        _panelBaseRotations.push_back(  270.*CLHEP::degree);
      } else {
        throw cet::exception("GEOM")
          << "Unrecognized rotation pattern in TTrackerMaker. \n";
      }
    } else {
      throw cet::exception("GEOM")
        << "Unrecognized rotation pattern in TTrackerMaker. \n";
    }

    // Parts of the algorithm require that the Aseet style tracker is built
    // of stations with 2 planes each.
    if ( _spacingPattern == 1 ){
      if ( _numPlanes%2 == 1 ){
        throw cet::exception("GEOM")
          << "Aseet style tracker requires 2 planes per station.\n"
          << "So ttracker.numPlanes must be even.  It was: "
          << _numPlanes
          << "\n";
      }
    }

    _panelBoxXOffsetMag = 0.0;
    _panelBoxZOffsetMag = 0.0;
    _layerHalfSpacing = 0.0;
    _layerHalfShift = 0.0;
    _manifoldXEdgeExcessSpace = 0.0;
    _manifoldZEdgeExcessSpace = 0.0;

  } // end TTrackerMaker::parseConfig

  void lptest( const Layer& lay){
    cout << lay.id() << " |  "
         << lay.nStraws()  <<  " |  "
         << lay.getStraws().capacity() << " "
         << endl;
  }

  void devtest( const Plane& dev){
    cout << "Plane: "
         << dev.id() << " "
         << dev.origin() << " "
         << dev.rotation()
         << endl;
  }

  void positionTest( const Layer& lay){
    const Straw& straw = lay.getStraw( 0 );
    cout << "Layer: "
         << lay.id() << " "
         << straw.getMidPoint().z() <<  " "
         << straw.getDirection().z() << " "
         << endl;
  }

  void TTrackerMaker::buildIt(){

    // Make an empty TTracker.
    _tt = unique_ptr<TTracker>(new TTracker());

    // Stations
    // Construct the stations and their internals based on planes internals
    if ( _numPlanes%2 != 0 ) {
      throw cet::exception("GEOM")  << "_numPlanes = " << _numPlanes
                                    << ": Current TTracker geometry assumes even number of planes  \n";
    }
    _numStations = _numPlanes/2;

    makeMother();

    computeLayerSpacingAndShift();
    computeManifoldEdgeExcessSpace();

    _tt->_supportModel = _supportModel;

    // Fill information about the new style, fully detailed support structure.
    if ( _supportModel == SupportModel::detailedv0 ) {
      makeSupportStructure();
    }

    // Fill the information about the, old style minimal supports.
    _tt->_supportParams = Support( _innerSupportRadius,
                                   _outerSupportRadius,
                                   _supportHalfThickness,
                                   _supportMaterial);

    _tt->_z0                  = _zCenter;
    _tt->_envelopeInnerRadius = _envelopeInnerRadius;
    _tt->_manifoldHalfLengths = _manifoldHalfLengths;
    _tt->_envelopeMaterial    = _envelopeMaterial;

    // Z location of the first plane.
    _z0 = -findFirstDevZ0();

    makeDetails();

    // Reserve space for straws so that pointers are valid.
    _nStrawsToReserve = _numPlanes * _panelsPerPlane * _layersPerPanel *
      _manifoldsPerEnd * _strawsPerManifold;
    //_tt->_allStraws.reserve(_nStrawsToReserve); // see makeLayer

    // we need to be able to assign true/false for a given straw index
    _tt->_strawExists.resize(_nStrawsToReserve);

    _tt->_planes.reserve(_numPlanes);
    // Construct the planes and their internals.
    for ( int idev=0; idev<_numPlanes; ++idev ){
      makePlane( PlaneId(idev) );
    }

    // Fill all of the non-persistent information.
    _tt->fillPointers();

    identifyNeighbourStraws();
    identifyDirectionalNeighbourStraws();

    _tt->_stations.reserve(_numStations);
    // Construct the planes and their internals.
    for ( int istation=0; istation<_numStations; ++istation ){
      makeStation( StationId(istation) );
    }

    // Order is important here.
    computePlaneEnvelope();
    computeTrackerEnvelope();
    recomputeHalfLengths();
    makeStrawTubs();

    // This uses information from the planes
    makeThinSupportRings();


    finalCheck();

    if ( _verbosityLevel > 0 ) {
      cout << "TTracker Support Structure: \n" << _tt->_supportStructure << endl;
    }

    // Test the forAll methods.
    //_tt->forAllLayers( lptest);
    //_tt->forAllPlanes( devtest);
    //_tt->forAllLayers( positionTest);

  } //end TTrackerMaker::buildIt.

  void TTrackerMaker::makeMother(){

    _tt->_mother = PlacedTubs ( "TrackerMother",
                                TubsParams( _motherRIn, _motherROut, _motherHalfLength),
                                CLHEP::Hep3Vector( _xCenter, 0., _motherZ0),
                                _envelopeMaterial );

  } //end TTrackerMaker::makeMother

  // In the present code the straw positions are computed using the manifold information.
  // The channel position is computed using the SupportStructure information.  These two
  // must be self consistent.  Relatively soon the manifold information will be removed
  // and they will be replaced with the support structure information.
  // For now, check for self-consistency.
  void TTrackerMaker::finalCheck( ){

    // Only do this test for the new model(s).
    if ( _supportModel == SupportModel::simple ) return;

    double ztest = 0.5*(_tt->getStraw ( StrawId(0,1,0,0) ).getMidPoint().z()+_tt->getStraw ( StrawId(0,1,1,0) ).getMidPoint().z())
      -  _tt->getPlane( PlaneId(0) ).origin().z();

    double tolerance = 1.e-6;
    if ( std::abs(ztest-_channelZOffset) > tolerance ){
      throw cet::exception("GEOM")  << "Inconsistent channel center and wire z location. \n"
                                    << "channel Z offset: " <<  _channelZOffset << "\n"
                                    << "plane z center:  " << _tt->getPlane( PlaneId(0) ).origin().z() << "\n "
                                    << "Straw z layer 0:  " << _tt->getStraw ( StrawId(0,1,0,0) ).getMidPoint().z() << "\n"
                                    << "Straw z layer 1:  " << _tt->getStraw ( StrawId(0,1,1,0) ).getMidPoint().z() << "\n"
                                    << "z test:           " << ztest << " delta " << std::abs(ztest-_channelZOffset) << "\n";
    }

  }


  void TTrackerMaker::makePlane( PlaneId devId ){

//std::cout << "->->-> makePlane\n";
    int idev = devId;

    double devDeltaZ = choosePlaneSpacing(idev);
    CLHEP::Hep3Vector origin( 0., 0., _z0+devDeltaZ);

    // plane rotation is no longer used.
    double phi = 0.0;
    _tt->_planes.push_back(Plane(devId, origin, phi));
    Plane& dev = _tt->_planes.back();
    dev._exists = ( find ( _nonExistingPlanes.begin(), _nonExistingPlanes.end(), idev) ==
                      _nonExistingPlanes.end() );

    dev._panels.reserve(_panelsPerPlane);

    for ( int isec=0; isec<_panelsPerPlane; ++isec ){
      makePanel ( PanelId(devId,isec), dev );
    }

//std::cout << "<-<-<- makePlane\n";
  }

  void TTrackerMaker::makePanel( const PanelId& secId, Plane& dev ){
//std::cout << "->->-> makePanel\n";

    dev._panels.push_back( Panel(secId) );
    Panel& panel = dev._panels.back();
    panel._layers.reserve(_layersPerPanel);

    // check if the opposite panels do not overlap
    static double const tolerance = 1.e-6; // this should be in a config file

    if ((2.*_manifoldHalfLengths.at(2)+_supportHalfThickness)>_planeHalfSeparation + tolerance) {
      cout << "(2.*_manifoldHalfLengths.at(2)+_supportHalfThickness), _planeHalfSeparation " <<
        (2.*_manifoldHalfLengths.at(2)+_supportHalfThickness) << ", " <<_planeHalfSeparation << endl;
      throw cet::exception("GEOM")  << "Planes are too close \n";
    }

    makeManifolds( secId );

    double strawSpacing = _strawGap+2.*_strawOuterRadius;

    for ( int ilay=0; ilay<_layersPerPanel; ++ilay ){
      makeLayer( LayerId(secId,ilay), panel );

      // checking spacing of the individual layers
      // are the manifolds sized correctly for the straws?

      Layer const & layer = panel.getLayer(ilay);
      //      cout << "Debugging looking at the layer   : " << layer.id() << endl;
      for (int ns = 0; ns<(layer.nStraws()*2-2); ns+=2) {

        if (_verbosityLevel>2) {
          cout << __func__ << " Checking spacig"
               << " for layer " << layer.id() << " straw " << layer.getStraw(ns).id()  << endl;
        }

        double layerDeltaMag =
          (layer.getStraw(ns+2).getMidPoint() - layer.getStraw(ns).getMidPoint()).mag();
        if ( abs(layerDeltaMag-strawSpacing)> tolerance ) {
          cout << "Layer straw spacing is (mm)   : " << layerDeltaMag
               << " for layer " << layer.id() << " straw " << layer.getStraw(ns).id()  << endl;
          cout << "It should be                  : " << strawSpacing << " diff: "
               << (layerDeltaMag-strawSpacing) << endl;

          throw cet::exception("GEOM")  << "Incorrect intralayer straw spacing, check manifold sizes rtc..\n";

        }
      }
    }

    // check spacing between layers/straws

    if (_layersPerPanel>1) {

      // we should do this using iterators

      Layer const & layer0 = panel.getLayer(0);
      Layer const & layer1 = panel.getLayer(1);

      for (int ns = 0; ns<layer0.nStraws()*2; ns+=2) {
        double xLayerDeltaMag =
          (layer0.getStraw(ns).getMidPoint() - layer1.getStraw(ns).getMidPoint()).mag();

        if (_verbosityLevel>2) {
          cout << __func__ << " Checking spacig"
               << " for layer " << layer0.id() << " straw " << layer0.getStraw(ns).id()
               << " and for layer " << layer1.id() << " straw " << layer1.getStraw(ns).id()  << endl;
        }

        if ( abs(xLayerDeltaMag-strawSpacing)> tolerance ) {
          cout << "xLayer straw spacing is (mm)   : "
               << xLayerDeltaMag
               << " for straws: "
               << layer0.getStraw(ns).id() << ", " << layer1.getStraw(ns).id()
               << endl;
          cout << "It should be                   : "
               << strawSpacing << " diff: "
               << (xLayerDeltaMag-strawSpacing) << endl;

          throw cet::exception("GEOM")  << "Incorrect interlayer straw spacing \n";

        }
      }

      for (int ns = 1; ns<layer0.nStraws()*2; ns+=2) {
        double xLayerDeltaMag =
          (layer0.getStraw(ns).getMidPoint() - layer1.getStraw(ns-2).getMidPoint()).mag();
        if (_verbosityLevel>2) {
          cout << __func__ << " Checking spacig"
               << " for layer " << layer0.id() << " straw " << layer0.getStraw(ns).id()
               << " and for layer " << layer1.id() << " straw " << layer1.getStraw(ns-2).id()  << endl;
        }
        if ( abs(xLayerDeltaMag-strawSpacing)> tolerance ) {
          cout << "xLayer straw spacing is (mm)   : "
               << xLayerDeltaMag
               << " for straws: "
               << layer0.getStraw(ns).id() << ", " << layer1.getStraw(ns-2).id()
               << endl;
          cout << "It should be                   : "
               << strawSpacing << " diff: "
               << (xLayerDeltaMag-strawSpacing) << endl;

          throw cet::exception("GEOM")  << "Incorrect interlayer straw spacing \n";

        }
      }
    }

    // calculate/make a panel envelope
    computePanelBoxParams(panel, dev);

//std::cout << "<-<-<- makePanel\n";
  }  // makePanel

  void TTrackerMaker::makeLayer ( const LayerId& layId, Panel& panel ){
//std::cout << "->->-> makeLayer\n";

    // Make an empty layer object.
    panel._layers.push_back( Layer(layId) );
    Layer& layer = panel._layers.back();

    // Get additional bookkeeping info.
    deque<Straw>& allStraws = _tt->_allStraws;
    const Plane& plane = _tt->getPlane( layId.getPlaneId() );
    int ilay = layId.getLayer();
    int isec = layId.getPanel();

    //    cout << "Debugging TTrackerMaker ilay: " << ilay << endl;

    // Start to populate the layer.
    layer._nStraws = _manifoldsPerEnd*_strawsPerManifold;
    layer._straws.reserve(_manifoldsPerEnd*_strawsPerManifold);

    // |z| of straw center, relative to the center of the plane.
    // Sign is taken care of elsewhere.

    // see similar calc in computePanelBoxParams
    //    double zOffset = _supportHalfThickness + _strawOuterRadius + ilay*2.*_layerHalfSpacing;
    // the above commented out calculation places the straws at the edge of the manifold in Z

    double zOffset = _supportHalfThickness + _manifoldZEdgeExcessSpace +
      _strawOuterRadius + ilay*2.*_layerHalfSpacing;

    // Rotation that puts wire direction and wire mid-point into their
    // correct orientations.
    // CLHEP::HepRotationZ RZ(_panelBaseRotations.at(isec));
    CLHEP::HepRotationZ RZ(panelRotation(isec,layId.getPlaneId()));

    // Unit vector in the wire direction. (nominal is the panel 0 to the right?)
    CLHEP::Hep3Vector unit = RZ*CLHEP::Hep3Vector(0.,1.,0.);

    // Straw number within the layer; does not reset to zero at each manifold.
    // we number the straws starting from the most inner one across the two layers in the panel/panel
    // it will be 0 for layer0 and 1 for layer1
    int _istraw(ilay-2);

    // we increase the number by 2, not 1

    // Add all of the straws
    for ( int iman=0; iman<_manifoldsPerEnd; ++iman ){

      // Inner edge of the innermost wire connected to this manifold.
      // this is layer dependent, take care of it at xstraw below

      double xA = _envelopeInnerRadius + 2.*_manifoldHalfLengths.at(0)*iman + _manifoldXEdgeExcessSpace;

      // Add all of the straws connected to this manifold.
      for ( int istr=0; istr<_strawsPerManifold; ++istr ){
        _istraw +=2;

        // layers with fewer straws would complicate StrawSD, constructTTrackerv, TTrackerMaker

        // Construct straw midpoint in its base position in the
        // coord system of the plane envelope.
        // we will shift the "second" layer from the manifold edge

        double xstraw = (ilay%2==0) ?
          xA + (1 + 2*istr)*_strawOuterRadius + istr*_strawGap :
          xA + (1 + 2*istr)*_strawOuterRadius + istr*_strawGap + 2.0*_layerHalfShift;

        CLHEP::Hep3Vector mid( xstraw, 0., zOffset*_panelZSide.at(isec) );
        mid += plane.origin();

        // Rotate straw midpoint to its actual location.
        CLHEP::Hep3Vector offset = RZ*mid;

        StrawIndex index(allStraws.size());

        _tt->_strawExists[index.asInt()] = plane.exists();

        allStraws.push_back( Straw( StrawId( layId, _istraw),
                                    index,
                                    offset,
                                    &_tt->_strawDetails.at(iman*2+ilay%2),
                                    iman*2+ilay%2,
                                    unit
                                    )
                             );
        layer._straws.push_back(&allStraws.back());
        layer._indices.push_back(index);


        //         if ( layId.getPlane() != -1 ){
        //           cout << "Position: " << setw(3) <<
        //             layId << " | " << setw(3) <<
        //             iman << " " << setw(3) <<
        //             istr                << " | " << setw(3) <<
        //             _istraw << " " << fixed << setprecision(2) << setw(8) <<
        //             xstraw << " " << fixed << setprecision(2) << setw(8) <<
        //             2.*_strawHalfLengths.at(iman) << " " << fixed << setprecision(2) <<
        //             mid << " " << fixed << setprecision(2) << setw(8) <<
        //             plane.origin() << " | "  << setw(3) <<
        //             index <<  " " << setw(3) <<
        //             allStraws.size() << " "  << setw(3) <<
        //             layer._straws.size() << " | " << setw(5) <<
        //             (allStraws.back()).id() << ", " << setw(5) <<
        //             (allStraws.back()).index()
        //           << endl;
        //           }

      }
    }

//std::cout << "<-<-<- makeLayer\n";
  }

  void TTrackerMaker::makeManifolds( const PanelId& secId){
//std::cout << "->->-> makeManifolds\n";

    if ( _panelsPerPlane != 4 && _panelsPerPlane != 6 ) {
      throw cet::exception("GEOM")
        << "This code only knows how to do 4 or 6 panels per plane.\n";
    }

//    double phi = _tt->getPlane(secId.getPlane()).rotation();
//    CLHEP::HepRotationZ RZ(phi);


    // manifold objects are not used for now...

    for ( int i=0; i<_manifoldsPerEnd; ++i){

      // First compute everything in their nominal positions: panel 0, right ?
      double x0 = _envelopeInnerRadius +
        _strawsPerManifold*_strawOuterRadius +
        _manifoldHalfLengths.at(0);

      double y0 = _tt->_strawDetails.at(i).halfLength() + _manifoldHalfLengths.at(2);

      double z0 = ( _supportHalfThickness + _manifoldHalfLengths.at(2) );
      //if ( secId.getPanel() <= 1 ) z0 = -z0; // is this correct for the 6 panels?
      // why not *_panelZSide.at(secId.getPanel())

      z0 = z0*_panelZSide.at(secId.getPanel());

      // is the above assuming correct Z? why not secId.getPanel()%2?
      // are manifolds ever used?
      // is it correct at all? I mean the origin? It is always the same for each plane...
      // x never changes

      CLHEP::Hep3Vector origin(x0,y0,z0);

      //       cout << "Manifold plane, panel, origin, length[0] :" <<
      //         _tt->getPlane(secId.getPlane()).id() << ", " <<
      //         secId.getPanel() << ", " <<
      //         origin << ", " << _manifoldHalfLengths.at(0) <<endl;


      _tt->_allManifolds.push_back( Manifold( origin, _manifoldHalfLengths) );
    }

//std::cout << "<-<-<- makeManifolds\n";
  }

// ======= Station view makers ============

  void TTrackerMaker::makeStation( StationId stationId ){
//std::cout << "->->-> makeStation\n";

    int ist = stationId;
    int idev1 = 2*ist;
    int idev2 = idev1 + 1;
    double stationZ = 0.5 *
        ( _tt->_planes.at(idev1).origin().z() +
          _tt->_planes.at(idev2).origin().z() );
    _tt->_stations.push_back(Station(stationId, stationZ));
    _tt->_stations.back();
//std::cout << "<-<-<- makeStation\n";
  }

  // Assumes all planes and all panels are the same.
  // The straw length depends only on the manifold number.
  // See Mu2e-doc-??? for the algorithm.

  void TTrackerMaker::computeStrawHalfLengths(){

    // we use resize as we will set specific straw parameters using a priori known straw numbers

    _strawHalfLengths.clear();
    _strawHalfLengths.resize(_manifoldsPerEnd*2);
    _strawActiveHalfLengths.clear();
    _strawActiveHalfLengths.resize(_manifoldsPerEnd*2);

    for ( int i=0; i<_manifoldsPerEnd; ++i ){
      double xA =
        _envelopeInnerRadius + 2.*_manifoldHalfLengths.at(0)*i + _manifoldXEdgeExcessSpace;

      //    double xA = (_layersPerPanel==1) ?
      //      _envelopeInnerRadius + 2.*_manifoldHalfLengths.at(0)*i + _manifoldXEdgeExcessSpace :
      //      _envelopeInnerRadius + 2.*_manifoldHalfLengths.at(0)*i + _manifoldXEdgeExcessSpace +
      //       _strawOuterRadius;

      // we ignore the further laying straws in the multi layer case,
      // as this would make the straws shorter than they need to be
      // the wire positioning is not affected by this though

      double yA = sqrt( diff_of_squares(_innerSupportRadius, xA) );
      double yB = yA + _manifoldYOffset;

      // this needs to be inserted at a specific position

      _strawHalfLengths[i*2]=yB;

      // This variable is only used if SupportModel==simple.
      double activeHLen=sqrt( diff_of_squares(_innerSupportRadius,xA+2.5))-_passivationMargin;
      activeHLen = std::max( activeHLen, 1.0);

      // this needs to be an insert at a specific position

      _strawActiveHalfLengths[i*2]=activeHLen;

    }

  } // end TTrackerMaker::computeStrawHalfLengths

  void TTrackerMaker::makeDetails(){

    computeConstantPanelBoxParams();

    if ( _supportModel == SupportModel::simple ){
      _tt->_strawDetails.reserve(_manifoldsPerEnd);
    } else{
      // This will be extended in recomputeHalfLengths - reserve enough space for the extention.
      // as we need to insert sparsly will resize now
      _tt->_strawDetails.resize(_manifoldsPerEnd*_layersPerPanel);
    }

    for ( int i=0; i<_manifoldsPerEnd; ++i ){
      // inserting at a specific place
      _tt->_strawDetails[i*2]=
        StrawDetail
        ( i*2,
          _strawMaterials,
          _strawOuterRadius,
          _strawWallThickness,
          _strawHalfLengths.at(i*2),
          _strawActiveHalfLengths.at(i*2),
          _wireRadius
          );
    }

  } // end TTrackerMaker::makeDetails

  void  TTrackerMaker::computePanelBoxParams(Panel& panel, Plane& dev){

    // get panel number
    int isec = panel.id().getPanel();
    //    int idev = dev.id();
    //    cout << "Debugging TTrackerMaker isec,idev: " << isec << ", " << idev << endl;

    // we copy precalculated _panelBoxHalfLengths etc.. into the panel

    panel._boxHalfLengths = _panelBoxHalfLengths;
    double xOffset = _panelBoxXOffsetMag;
    double zOffset = _panelBoxZOffsetMag * _panelZSide.at(isec);

    // Now calculate the rotations and placement of the panel envelope

    panel._boxRxAngle = 0.;
    panel._boxRyAngle = 0.;
    panel._boxRzAngle = panelRotation(isec,dev.id() );

    CLHEP::HepRotationZ RZ(panel._boxRzAngle);

    panel._boxOffset  = RZ*(CLHEP::Hep3Vector( xOffset, 0., zOffset) + dev.origin());

    // we set to 0.0  values smaller than a small number
    static const double max0val = 1.e-06;
    for (int ii=0; ii!=panel._boxOffset.SIZE; ++ii) {
      if (abs(panel._boxOffset[ii])<max0val) panel._boxOffset[ii]=0.0;
    }

    //     cout << "Debugging panel box isec, by, bz, bxl, bxs, boxRzAngle, boxOffset: " <<
    //       isec << ", " <<
    //       by << ", " <<
    //       bz << ", " <<
    //       bxl << ", " <<
    //       bxs << ", " <<
    //       panel._boxRzAngle << ", " <<
    //       panel._boxOffset <<
    //       endl;

    //     cout << "Debugging panel box isec, straw lengths: ";
    //     for ( int i=0; i<_manifoldsPerEnd; ++i ){
    //       cout << i << " " << _strawHalfLengths.at(i);
    //     }
    //     cout << endl;

    return;

  } // end TTrackerMaker::computePanelBoxParams

  void TTrackerMaker::computeLayerSpacingAndShift(){

    _layerHalfSpacing = (_layersPerPanel<=1) ? 0.0 :
      sqrt(3.0*(square(_strawOuterRadius)+_strawOuterRadius*_strawGap+0.25*square(_strawGap)))*0.5;
    _layerHalfShift   = (_layersPerPanel<=1) ? 0.0 : _strawGap*0.25 + _strawOuterRadius*0.5;

  }

  void TTrackerMaker::computeManifoldEdgeExcessSpace(){

    // Computes space between first/last straw and edge of manifold

    _manifoldXEdgeExcessSpace = _manifoldHalfLengths.at(0) -
      _strawOuterRadius*_strawsPerManifold -
      _strawGap*(_strawsPerManifold-1)*0.5;

    _manifoldZEdgeExcessSpace = _manifoldHalfLengths.at(2) - _strawOuterRadius -
      (_layersPerPanel-1)*_layerHalfSpacing;

    //cout << "Debugging,  _manifoldXEdgeExcessSpace, _manifoldZEdgeExcessSpace: " <<
    //     _manifoldXEdgeExcessSpace << ", " << _manifoldZEdgeExcessSpace << endl;

    if ( _manifoldXEdgeExcessSpace < 0.0 || _manifoldZEdgeExcessSpace < 0.0){
      throw cet::exception("GEOM")
        << "Manifolds are too small to hold straws!\n";
    }

  }

  void TTrackerMaker::computeConstantPanelBoxParams() {

    computeStrawHalfLengths();

    // we do not use the trapezoid any more; but it is a good check while renumbering the straws

    // the box is a trapezoid ;

    // note that G4 has it own coordinate convention for each solid
    // (see mu2e<->G4 translation below):

    // trapezoid y dimensions (or x in G4)
    // trapezoid x dimension  (or z in G4)
    // trapezoid z dimension  (or y in G4)

    // shorter y             is the length of the straws in the top/last (shortest) manifold
    // longer  y is longer than the length of the straws in the longest manifold
    // the other dimentions are "x", the combined manifold width + _manifoldXEdgeExcessSpace
    // the "thickness" of the trpezoid z, ~ the layer thickness * number of layers

    // x

    // if there are more than one layer per panel, manifolds have a
    // straw "sticking out" by a straw radius (and half of the straw
    // gap), but only the last one extends beyond the entire structure
    // in the z direction; remember that those are "halfLengths", so
    // the sticking out straw radius has to be divided by 2 and the gap by 4

    // _layerHalfShift is 0 in one layer case
    double bx = _manifoldHalfLengths.at(0)*double(_manifoldsPerEnd) + _layerHalfShift;

    // calculating "longer" x;
    // starting from the tng of the slope

    // calculate the largest slope starting from the longest straws manifold
    double maxtg = 0.0;

    // the code below looks at the slope "seen" from the longest set of straws
    for (int i=1; i!=_manifoldsPerEnd; ++i) {

      // _strawHalfLengths has changed its indexing

      double ttg = ( _manifoldHalfLengths.at(0) + _layerHalfShift )*double(i) /
        ( _strawHalfLengths.at(0) - _strawHalfLengths.at(i*2) ) ;

      if (maxtg < ttg ) {
        maxtg = ttg;
      }

    }

    // finally y (long, short)
    double byl = (_manifoldHalfLengths.at(0) + _layerHalfShift)/maxtg + _strawHalfLengths.at(0);

    double bys = byl - bx/maxtg;

    // z
    // manifold better be thicker than the straws:

    double bz = _manifoldHalfLengths.at(2);

    // now push it all back into the vector
    // std::vector<double> _panelBoxHalfLengths;

    static const size_t panelBoxHalfLengthsSize= 5;

    _panelBoxHalfLengths.reserve(panelBoxHalfLengthsSize);

    // Pad the trapezoid to be slightly larger than it needs to be
    static const double pad = 0.0; // this needs to be in the geom file... if to be non-zero

    // the order is forced by the nestTrp/G4Trd and Panel data

    _panelBoxHalfLengths.push_back(0.0); //dummy to be compatible with LTracker
    _panelBoxHalfLengths.push_back(bx+pad);
    _panelBoxHalfLengths.push_back(bz+pad);

    _panelBoxHalfLengths.push_back(bys+pad);
    _panelBoxHalfLengths.push_back(byl+pad);

    if (_panelBoxHalfLengths.size()!=panelBoxHalfLengthsSize) {
      cout << " _panelBoxHalfLengths.size() sould be " << panelBoxHalfLengthsSize <<
        ", but is : " << _panelBoxHalfLengths.size() << endl;
      throw cet::exception("GEOM")
        << "something is wrong with panel _panelBoxHalfLengths calculations \n";
    }

    _panelBoxXOffsetMag = _envelopeInnerRadius  + _panelBoxHalfLengths.at(1); // bx + pad
    _panelBoxZOffsetMag = _supportHalfThickness + _panelBoxHalfLengths.at(2); // bz + pad

    // we need to make sure the trapezoids do not extend beyond the plane envelope...
    // we will check if the dev envelope radius acomodates the newly created box

    double outerSupportRadiusRequireds =
      sqrt( sum_of_squares(_envelopeInnerRadius + 2.0*_panelBoxHalfLengths.at(1),
                           _panelBoxHalfLengths.at(3)
                           )
            );
    double outerSupportRadiusRequiredl =
      max( outerSupportRadiusRequireds,
           sqrt( sum_of_squares(_envelopeInnerRadius-pad,
                                _panelBoxHalfLengths.at(4))
                 )
           );

    //     if (true) {
    //       cout << "Debugging _strawHalfLengths: ";
    //       for (size_t i=0; i!=_manifoldsPerEnd; ++i) {
    //         cout << _strawHalfLengths.at(i)  << ", ";
    //       }
    //       cout << endl;
    //       cout << "Debugging _supportParams.innerRadius   :   " << _tt->_supportParams.innerRadius() << endl;
    //       cout << "Debugging _supportParams.outerRadius   :   " << _tt->_supportParams.outerRadius() << endl;
    //       cout << "Debugging _supportParams.outerRadius rs:   " << outerSupportRadiusRequireds << endl;
    //       cout << "Debugging _supportParams.outerRadius rl:   " << outerSupportRadiusRequiredl << endl;
    //     }

    if (_tt->_supportParams.outerRadius() < outerSupportRadiusRequiredl) {
      cout << " _supportParams.outerRadius         :   " << _tt->_supportParams.outerRadius() << endl;
      cout << " _supportParams.outerRadius required:   " << outerSupportRadiusRequiredl << endl;
      throw cet::exception("GEOM")
        << "outerSupportRadius is to small given other paramters \n";
    }

    return;

  } // end TTrackerMaker::computeConstantPanelBoxParams


  // Compute the spacing for the given plane.
  double TTrackerMaker::choosePlaneSpacing( int idev ) const {


    if ( _spacingPattern == 0 ) {
      return idev * _planeSpacing;
    }

    else if ( _spacingPattern == 1 ) {
      int station = idev/2;
      int k = idev%2;
      if (k == 0 ) {
        return  station*_planeSpacing - _planeHalfSeparation;
      } else if (k == 1 ) {
        return  station*_planeSpacing + _planeHalfSeparation;
      }
    }

    throw cet::exception("GEOM")
      << "Unrecognized separation pattern in TTrackerMaker. \n";

  }

  double TTrackerMaker::findFirstDevZ0() const{

    if ( _spacingPattern == 0 ) {
      return _planeSpacing*double(_numPlanes-1)/2.0;
    }

    else if ( _spacingPattern == 1 ) {
      int nStations = _numPlanes/2;
      return double(nStations-1)/2.0 * _planeSpacing;
    }
    else {
      throw cet::exception("GEOM")
        << "Unrecognized separation pattern in TTrackerMaker. \n";
    }
    return 0.0;
  }


  // Identify the neighbour straws for all straws in the tracker
  void TTrackerMaker::identifyNeighbourStraws() {

    deque<Straw>& allStraws = _tt->_allStraws;

    for (deque<Straw>::iterator i = allStraws.begin();
         i != allStraws.end(); ++i) {
      // throw exception if more than 2 layers per panel

      if (_tt->getPanel(i->id().getPanelId()).nLayers() > 2 ) {
        throw cet::exception("GEOM")
          << "The code works with no more than 2 layers per panel. \n";
      }

      if (_verbosityLevel>2) {
        cout << __func__ << " "
             << i->id() << ", index "
             << i->index()
             << " Straw " << i->id().getStraw()
             << endl;
      }

      LayerId lId = i->id().getLayerId();
      int layer = lId.getLayer();
      int nStrawLayer = _tt->getLayer(lId)._nStraws;

      if ( _verbosityLevel>2 ) {
        cout << __func__ << " layer " << lId << " has " << nStrawLayer << " straws" << endl;
        cout << __func__ << " Analyzed straw: " << i->id() << '\t' << i->index() << endl;
      }

      // add the "same layer" n-2 neighbours straw (if exist)
      // in the new model straw numbers increase by 2 in a given layer

      if ( i->id().getStraw() > 1 ) {
        const StrawId nsId(lId, (i->id().getStraw()) - 2 );
        i->_nearestById.push_back( nsId );
        if ( _verbosityLevel>2 ) {
          const Straw& temp = _tt->getStraw( nsId );
          cout << __func__ << " Neighbour left straw: " << temp.id() << '\t' << temp.index() << endl;
        }
        i->_nearestByIndex.push_back( _tt->getStraw(nsId).index() );
      }

      // add the "same layer" n+2 neighbours straw (if exist)
      // in the new model straw numbers increase by 2 in a given layer

      // is there anything which uses the straw number to look up the position in a container?
      // looks like it is done in many places

      if ( i->id().getStraw() < (2*nStrawLayer-2) ) {
        const StrawId nsId(lId, (i->id().getStraw()) + 2 );
        i->_nearestById.push_back( nsId );
        if ( _verbosityLevel>2 ) {
          const Straw& temp = _tt->getStraw( nsId );// is this correct for the new model? <------------
          cout << __func__ << " Neighbour right straw: " << temp.id() << '\t' << temp.index() << endl;
        }
        i->_nearestByIndex.push_back( _tt->getStraw(nsId).index() );
      }

      // add the "opposite layer" n neighbours straw (if more than 1 layer)

      if (_layersPerPanel == 2) {

        // throw exception if the two layer of the same panel have different
        // number of straws
        if (_tt->getLayer(lId)._nStraws != nStrawLayer) {
          throw cet::exception("GEOM")
            << "The code works only with the same number of straws "
            << "per layer in the same panel. \n";
        }

        if (layer==0 && i->id().getStraw()<2*nStrawLayer) {
          const StrawId nsId( i->id().getPanelId(), 1 , i->id().getStraw() + 1 );
          i->_nearestById.push_back( nsId );
          i->_nearestByIndex.push_back( _tt->getStraw( nsId ).index() );
          if ( _verbosityLevel>2 ) {
            cout << __func__ << " Neighbour opposite up straw: "
                 << i->_nearestById.back() << '\t' <<  i->_nearestByIndex.back() << endl;
          }
        }

        if (layer==1 && i->id().getStraw()<2*nStrawLayer-1) {
          const StrawId nsId( i->id().getPanelId(), 0 , i->id().getStraw() + 1 );
          i->_nearestById.push_back( nsId );
          i->_nearestByIndex.push_back( _tt->getStraw( nsId ).index() );
          if ( _verbosityLevel>2 ) {
            cout << __func__ << " Neighbour opposite up straw: "
                 << i->_nearestById.back() << '\t' <<  i->_nearestByIndex.back() << endl;
          }
        }

        if (layer==0 && i->id().getStraw()>0) {
          const StrawId nsId( i->id().getPanelId(), 1 , i->id().getStraw() - 1 );
          i->_nearestById.push_back( nsId );
          i->_nearestByIndex.push_back( _tt->getStraw( nsId ).index() );
          if ( _verbosityLevel>2 ) {
            cout << __func__ << " Neighbour opposite down straw: "
                 << i->_nearestById.back() << '\t' <<  i->_nearestByIndex.back() << endl;
          }
        }

        if (layer==1 && i->id().getStraw()>0) { // layer 1 straw 1 is ok
          const StrawId nsId( i->id().getPanelId(), 0 , i->id().getStraw() - 1 );
          i->_nearestById.push_back( nsId );
          i->_nearestByIndex.push_back( _tt->getStraw( nsId ).index() );
          if ( _verbosityLevel>2 ) {
            cout << __func__ << " Neighbour opposite down straw: "
                 << i->_nearestById.back() << '\t' <<  i->_nearestByIndex.back() << endl;
          }
        }

      }
    }

  } // identifyNeighborStraws

  // Identify the neighbour straws in inner/outer same-layer or zig-zag order
  void TTrackerMaker::identifyDirectionalNeighbourStraws() {

    // TODO:  The following algorithm relies on a few more geometry assumptions
    //        than would strictly be necessary.  For example, it relies on
    //        the layer labelling such that leayer 0 lies half a straw to the
    //        inner side of layer 1, in each panel.  Some of these assumptions
    //        can be lifted, and others at least checked, so that if the
    //        geometry changes, the code will still produce the right results,
    //        or will at least throw to indicate a serious problem.

    deque<Straw>& allStraws = _tt->_allStraws;

    for (deque<Straw>::iterator straw = allStraws.begin();
         straw != allStraws.end(); ++straw) {

      // throw exception if more than 2 layers in the panel of this straw
      if (_tt->getPanel(straw->id().getPanelId()).nLayers() != 2 ) {
        throw cet::exception("GEOM")
          << "The code expects exactly 2 layers per panel. \n";
      }

      LayerId layerId = straw->id().getLayerId();
      int layerNumber = layerId.getLayer();
      bool layerStaggeredToInside = (layerNumber == 0);

      // In all cases, layer 0 is staggered to the inside,
      // layer 1 is staggered to the outside.  We will now check this:
      // FIXME the above is probably not true
      // TODO -- Do the check

      LayerId otherLayerId ( layerId.getPanelId(), 1-layerNumber );

      int nStrawLayer = _tt->getLayer(layerId)._nStraws;

      // since the logic relies on the index and the straw number

      int strawNumberWithinLayer = straw->id().getStraw();
      int incrementedStrawNumber =
        ( strawNumberWithinLayer + 2 < 2*nStrawLayer )
        ? strawNumberWithinLayer + 2
        : StrawIndex::NO_STRAW;
      int decrementedStrawNumber =
        ( strawNumberWithinLayer - 2 >=  0)
        ? strawNumberWithinLayer - 2
        : StrawIndex::NO_STRAW;

      straw->_nextOuterL = ttStrawIndex (layerId, incrementedStrawNumber);
      straw->_nextInnerL = ttStrawIndex (layerId, decrementedStrawNumber);
      if (layerStaggeredToInside) {
        straw->_nextOuterP = ttStrawIndex(otherLayerId, strawNumberWithinLayer);
        straw->_nextInnerP = ttStrawIndex(otherLayerId, decrementedStrawNumber);
      } else {
        straw->_nextOuterP = ttStrawIndex(otherLayerId, incrementedStrawNumber);
        straw->_nextInnerP = ttStrawIndex(otherLayerId, strawNumberWithinLayer);
      }

      if ( _verbosityLevel>2 ) {

        cout << "Straw " << straw->id() << ": " << straw->id().getStraw() << endl;

        cout << " _nextOuterL: "   << setw(5) << straw->_nextOuterL.asInt() << " : ";
        if ( straw->_nextOuterL.asInt()!=StrawIndex::NO_STRAW ) {
          cout << _tt->getStraw(straw->_nextOuterL).id();
        }
        cout << endl;

        cout << " _nextInnerL: " << setw(5) << straw->_nextInnerL.asInt() << " : ";
        if ( straw->_nextInnerL.asInt()!=StrawIndex::NO_STRAW ) {
          cout << _tt->getStraw(straw->_nextInnerL).id();
        }
        cout << endl;

        cout << " _nextOuterP: " << setw(5) << straw->_nextOuterP.asInt() << " : " ;
        if ( straw->_nextOuterP.asInt()!=StrawIndex::NO_STRAW ) {
          cout << _tt->getStraw(straw->_nextOuterP).id();
        }
        cout << endl;

        cout << " _nextInnerP: " << setw(5) << straw->_nextInnerP.asInt() << " : " ;
        if ( straw->_nextInnerP.asInt()!=StrawIndex::NO_STRAW ) {
          cout << _tt->getStraw(straw->_nextInnerP).id();
        }
        cout << endl;

        // now print _nearestById & _nearestByIndex

        for (auto const& s : straw->nearestNeighboursByIndex()) {
          cout << s << endl;
        }

        for (auto const& s : straw->nearestNeighboursById()) {
          cout << s << endl;
        }

      }

      // TODO -- Insert logic to check that the radius of the purported
      // next straw differs by the right amount and sign, in each of these
      // four cases.

    } // end of loop over all straws

  } // identifyDirectionalNeighbourStraws

  StrawIndex TTrackerMaker::ttStrawIndex(LayerId const & layerId, int snum)
  {
    if ( snum == StrawIndex::NO_STRAW ) {
      return  StrawIndex(StrawIndex::NO_STRAW);
    }
    StrawId sid ( layerId, snum );
    return _tt->getStraw(sid).index();
  } //; ttStrawIndex

  void TTrackerMaker::makeSupportStructure(){

    SupportStructure& sup  = _tt->_supportStructure;

    // Positions for the next few objects in Mu2e coordinates.
    TubsParams endRingTubs( _endRingInnerRadius, _endRingOuterRadius, _endRingHalfLength);

    //    TubsParams midRingTubs ( _endRingInnerRadius, _endRingOuterRadius, _midRingHalfLength);
    sup._stiffRings.push_back(PlacedTubs ( "TTrackerEndRingUpstream",   endRingTubs, CLHEP::Hep3Vector( _xCenter, 0., _zCenter-_endRingZOffset), _endRingMaterial ));

    {
      if ( _numPlanes%2 !=0 ){
        throw cet::exception("GEOM")
          << "TTrackerMaker::makeSupportStructure expected and even number of planes. Saw " << _numPlanes << " planes.\n";
      }

      // From upstream end of most upstream station to the downstream end of the most downstream station.
      // Including all materials.
      double overallLength = (_numPlanes/2-1)*_planeSpacing + 2.*_planeHalfSeparation + 2.* _innerRingHalfLength;

      // we make support beams here (they used to be called staves)

      // Staves touch the big ring at the upstream end; they end flush
      // with the downstream edge of the most downstream station.

      double z1 = -(_endRingZOffset-_endRingHalfLength);
      double z2 = overallLength/2.;
      double zoff = (z1+z2)/2.;
      double zHalf = ( z2-z1)/2.;

      // hold the beamTubsParams in a map

      std::unordered_map<std::string,TubsParams> supportBeamParams;
      std::unordered_map<std::string,TubsParams> supportServiceParams;

      // the top beam is different

      size_t ibeam(0);

      std::ostringstream bos("TTrackerSupportBeam_",std::ios_base::ate); // to write at the end
      bos << std::setfill('0') << std::setw(2) << ibeam;

      supportBeamParams.insert(std::pair<std::string,
                               TubsParams>(bos.str(),
                                           TubsParams(_beam0_innerRadius,
                                                      _beam0_outerRadius,
                                                      zHalf,
                                                      _beam0_phiRange[0]*CLHEP::degree,
                                                      (_beam0_phiRange[1]- _beam0_phiRange[0])*CLHEP::degree)));

      sup._beamBody.push_back( PlacedTubs( bos.str(),
                                           supportBeamParams.at(bos.str()), // to make sure it exists
                                           CLHEP::Hep3Vector(_xCenter, 0., _zCenter+zoff),
                                           _beam0_material) );

      // make the first support beam (1) , the other one (2) is a mirror reflection
      // phi0 can be negative, deltaPhi must not

      size_t nsServices =  _beam1_servicePhi0s.size();

      if ( nsServices !=  _beam1_servicePhiEnds.size()
           || nsServices != _beam1_serviceMaterials.size()
           || nsServices != _beam1_serviceOuterRadii.size()
           || nsServices != _beam1_serviceCovRelThickness.size()
           || nsServices != _beam1_serviceMaterialsCov.size() ) {
        throw cet::exception("GEOM")
          << __func__ << "the number of all the beam service paramters has to be the same"
          << endl;
      }

      const size_t nssbeams = _beam1_phiSpans.size() - 1;

      for (size_t ibeam = 1; ibeam!=3; ++ibeam) {

        double phi00 = (ibeam == 1)
          ? _beam1_phiRange[0] -_beam1_phiSpans[0]
          : 180.0 - _beam1_phiRange[0] + _beam1_phiSpans[0]; // effectively 180 or 0;

        for (size_t ssbeam = 0; ssbeam != nssbeams; ++ssbeam) {

          bos.str("TTrackerSupportBeam_");
          bos << std::setw(1) << ibeam << ssbeam;

          if ( _verbosityLevel > 0 ) {
            cout << __func__ << " making " <<  bos.str() << endl;
          }

          double deltaPhi = _beam1_phiSpans[ssbeam+1] - _beam1_phiSpans[ssbeam];
          double phi0     = (ibeam == 1)
            ? phi00 + _beam1_phiSpans[ssbeam]
            : phi00 - _beam1_phiSpans[ssbeam] - deltaPhi;

          double outerRadius = (ssbeam != 1)
            ? _beam1_outerRadius
            : _beam1_midRadius1; // the service section is different

          supportBeamParams.insert(std::pair<std::string,
                                   TubsParams>(bos.str(),
                                               TubsParams(_beam1_innerRadius,
                                                          outerRadius,
                                                          zHalf,
                                                          phi0*CLHEP::degree,
                                                          deltaPhi*CLHEP::degree)));

          sup._beamBody.push_back(PlacedTubs( bos.str(),
                                              supportBeamParams.at(bos.str()),
                                              CLHEP::Hep3Vector(_xCenter, 0., _zCenter+zoff),
                                              _beam1_material) );

        }

        // a service envelope

        {

          size_t ssbeam = 1;

          bos.str("TTrackerSupportServiceEnvelope_");
          bos << std::setw(1) << ibeam << ssbeam;

          if ( _verbosityLevel > 0 ) {
            cout << __func__ << " making " <<  bos.str() << endl;
          }

          double deltaPhi = _beam1_phiSpans[ssbeam+1]-_beam1_phiSpans[ssbeam];
          double phi0     =  (ibeam == 1)
            ? phi00 + _beam1_phiSpans[ssbeam]
            : phi00 - _beam1_phiSpans[ssbeam] - deltaPhi;

          supportBeamParams.insert(std::pair<std::string,
                                   TubsParams>(bos.str(),
                                               TubsParams(_beam1_midRadius1,
                                                          _beam1_midRadius2,
                                                          zHalf,
                                                          phi0*CLHEP::degree,
                                                          deltaPhi*CLHEP::degree)));

          sup._beamBody.push_back( PlacedTubs( bos.str(),
                                               supportBeamParams.at(bos.str()),
                                               CLHEP::Hep3Vector(_xCenter, 0., _zCenter+zoff),
                                               _envelopeMaterial) );

        }

        // services

        // subdivide the services into a number of groups of same lengts for a given phi span

        // adjust the lengts and place all tubes touching the last station
        // adjust the covered arc accordingly

        for (size_t sservice = 0; sservice!=nsServices; ++sservice) {

          // the span of this service section
          double deltaPhi0 = _beam1_servicePhiEnds[sservice] - _beam1_servicePhi0s[sservice];

          bos.str("TTrackerSupportServiceSectionEnvelope_");
          bos << std::setw(1) << ibeam << sservice;

          if ( _verbosityLevel > 0 ) {
            cout << __func__ << " making " <<  bos.str() << endl;
          }

          std::string boses =  bos.str();

          bos.str("TTrackerSupportService_");
          bos << std::setw(1) << ibeam << sservice << "_";

          std::string boss =  bos.str();

          if ( _verbosityLevel > 0 ) {
            cout << __func__ << " making " <<  boss << endl;
          }

          for ( int ssservice = 0; ssservice!=_numStations; ++ssservice) {

            bos.str(boss);
            bos << std::setw(2) << ssservice;

            if ( _verbosityLevel > 0 ) {
              cout << __func__ << " making " <<  bos.str() << endl;
            }

            double sHLength = zHalf*(_numStations-ssservice)/_numStations;
            double sOffset  = zoff + zHalf - sHLength;

            if ( _verbosityLevel > 0 ) {
              cout << __func__ << " sHLength, sOffset "
                   << sHLength << ", " << sOffset << endl;
            }

            // the span of one "sub" service of this section
            double deltaPhi  = deltaPhi0/_numStations;
            // the starting position of the "sub" service
            double phi0      = (ibeam == 1)
              ? phi00 + _beam1_servicePhi0s[sservice] + deltaPhi*ssservice
              : phi00 - _beam1_servicePhi0s[sservice] - deltaPhi*(1+ssservice);

            if ( _verbosityLevel > 0 ) {
              cout << __func__ << " deltaPhi0, phi0, deltaPhi "
                   << deltaPhi0 << ", " << phi0 << ", " << deltaPhi << endl;
            }

            // approximate the service by the main part an a top cover/envelope with different materials

            if ( _beam1_serviceCovRelThickness[sservice] > 1.
                 || _beam1_serviceCovRelThickness[sservice] < 0. ) {
              throw cet::exception("GEOM")
                << __func__ << " beam1_serviceCovRelThickness out of 0...1 range "
                << _beam1_serviceCovRelThickness[sservice]
                << endl;
            }

            double cRadius = _beam1_serviceOuterRadii[sservice] +
              _beam1_serviceCovRelThickness[sservice] *
              ( _beam1_midRadius1 - _beam1_serviceOuterRadii[sservice] );

            // an envelope for this service section

            if ( ssservice == 0) {

              double phi0      = (ibeam == 1)
                ? phi00 + _beam1_servicePhi0s[sservice]
                : phi00 - _beam1_servicePhi0s[sservice] - deltaPhi0;

              supportServiceParams.insert(std::pair<std::string,
                                          TubsParams>(boses,
                                                      TubsParams(_beam1_midRadius1,
                                                                 _beam1_serviceOuterRadii[sservice],
                                                                 sHLength,
                                                                 phi0*CLHEP::degree,
                                                                 deltaPhi0*CLHEP::degree)));

              sup._beamServices.push_back(PlacedTubs( boses,
                                                      supportServiceParams.at(boses),
                                                      CLHEP::Hep3Vector(_xCenter, 0., _zCenter+sOffset),
                                                       _envelopeMaterial));


            }

            supportServiceParams.insert(std::pair<std::string,
                                        TubsParams>(bos.str(),
                                                    TubsParams(_beam1_midRadius1,
                                                               cRadius,
                                                               sHLength,
                                                               phi0*CLHEP::degree,
                                                               deltaPhi*CLHEP::degree)));

            sup._beamServices.push_back(PlacedTubs( bos.str(),
                                                    supportServiceParams.at(bos.str()),
                                                    CLHEP::Hep3Vector(_xCenter, 0., _zCenter+sOffset),
                                                    _beam1_serviceMaterials[sservice]));

            if (_beam1_serviceCovRelThickness[sservice]>0.) {

              bos << std::setw(1) << "_c";

              supportServiceParams.insert(std::pair<std::string,
                                          TubsParams>(bos.str(),
                                                      TubsParams(cRadius,
                                                                  _beam1_serviceOuterRadii[sservice],
                                                                 sHLength,
                                                                 phi0*CLHEP::degree,
                                                                 deltaPhi*CLHEP::degree)));

              sup._beamServices.push_back(PlacedTubs( bos.str(),
                                                      supportServiceParams.at(bos.str()),
                                                      CLHEP::Hep3Vector(_xCenter, 0., _zCenter+sOffset),
                                                      _beam1_serviceMaterialsCov[sservice]));

            }

          }

        }

      }

    }

    // Positions from here onward are in the coordinates of the plane envelope.
    {
      TubsParams centerPlateTubs( _innerRingOuterRadius, _outerRingOuterRadius, _centerPlateHalfLength);
      sup._centerPlate = PlacedTubs( "TTrackerSupportCenterPlate", centerPlateTubs, CLHEP::Hep3Vector(0.,0.,0.), _centerPlateMaterial);
    }

    {
      TubsParams innerRingTubs( _innerRingInnerRadius, _innerRingOuterRadius, _innerRingHalfLength);
      sup._innerRing = PlacedTubs( "TTrackerSupportInnerRing", innerRingTubs, CLHEP::Hep3Vector(0.,0.,0), _innerRingMaterial );
    }

    {
      double outerRadius        = _innerRingInnerRadius + _channelDepth;
      double channelHalfLength  = (_layerHalfSpacing-_strawOuterRadius) + 2.*_strawOuterRadius;
      TubsParams innerChannelTubs( _innerRingInnerRadius, outerRadius, channelHalfLength );
      sup._innerChannelUpstream   = PlacedTubs( "TTrackerSupportInnerChannelUpstream",   innerChannelTubs, CLHEP::Hep3Vector(0.,0.,-_channelZOffset), _envelopeMaterial );
      sup._innerChannelDownstream = PlacedTubs( "TTrackerSupportInnerChannelDownstream", innerChannelTubs, CLHEP::Hep3Vector(0.,0., _channelZOffset), _envelopeMaterial );
    }

    {
      double halfLength = (_innerRingHalfLength - _centerPlateHalfLength)/2.;
      double dz         = _centerPlateHalfLength + halfLength;
      TubsParams outerRingTubs( _outerRingInnerRadius, _outerRingOuterRadius, halfLength);
      sup._outerRingUpstream   = PlacedTubs ( "TTrackerSupportOuterRingUpstream",   outerRingTubs, CLHEP::Hep3Vector(0.,0.,-dz), _outerRingMaterial );
      sup._outerRingDownstream = PlacedTubs ( "TTrackerSupportOuterRingDownstream", outerRingTubs, CLHEP::Hep3Vector(0.,0., dz), _outerRingMaterial );
    }

    {
      double dz = _innerRingHalfLength-_coverHalfLength;
      TubsParams coverTubs( _innerRingOuterRadius, _outerRingInnerRadius, _coverHalfLength);
      sup._coverUpstream   = PlacedTubs( "TTrackerSupportCoverUpstream",   coverTubs, CLHEP::Hep3Vector(0.,0.,-dz), _coverMaterial );
      sup._coverDownstream = PlacedTubs( "TTrackerSupportCoverDownstream", coverTubs, CLHEP::Hep3Vector(0.,0., dz), _coverMaterial );
    }

    {
      double halfLength = (_innerRingHalfLength - _centerPlateHalfLength - 2.*_coverHalfLength)/2.;
      double dz         = _centerPlateHalfLength + halfLength;
      TubsParams gasTubs( _innerRingOuterRadius, _outerRingInnerRadius, halfLength );
      sup._gasUpstream   = PlacedTubs ( "TTrackerSupportGasUpstream",  gasTubs, CLHEP::Hep3Vector(0.,0.,-dz), _electronicsSpaceMaterial );
      sup._gasDownstream = PlacedTubs ( "TTrackerSupportGasDownstream", gasTubs, CLHEP::Hep3Vector(0.,0., dz), _electronicsSpaceMaterial );
    }

    // Positions for the next two are in the coordinates of the electronics space.
    {
      TubsParams g10Tubs( _innerRingOuterRadius, _outerRingInnerRadius, _electronicsG10HalfLength );
      sup._g10Upstream   = PlacedTubs ( "TTrackerSupportElecG10Upstream",   g10Tubs, CLHEP::Hep3Vector(0.,0.,-_electronicsG10HalfLength), _electronicsG10Material );
      sup._g10Downstream = PlacedTubs ( "TTrackerSupportElecG10Downstream", g10Tubs, CLHEP::Hep3Vector(0.,0.,-_electronicsG10HalfLength), _electronicsG10Material );
    }

    {
      TubsParams cuTubs( _innerRingOuterRadius, _outerRingInnerRadius, _electronicsCuHhalfLength);
      sup._cuUpstream   = PlacedTubs ( "TTrackerSupportElecCuUpstream",   cuTubs, CLHEP::Hep3Vector(0.,0.,_electronicsCuHhalfLength), _electronicsCuMaterial);
      sup._cuDownstream = PlacedTubs ( "TTrackerSupportElecCuDownstream", cuTubs, CLHEP::Hep3Vector(0.,0.,_electronicsCuHhalfLength), _electronicsCuMaterial);
    }

  }

  // This needs to know the z positions of the
  void TTrackerMaker::makeThinSupportRings(){
    SupportStructure& sup  = _tt->_supportStructure;

    TubsParams thinRingTubs ( _endRingInnerRadius, _outerRingOuterRadius, _midRingHalfLength,
                              CLHEP::pi, CLHEP::pi); // half rings, on the bottom part

    for ( size_t i=0; i< _midRingSlot.size(); ++i){
      std::ostringstream name;
      int station = _midRingSlot.at(i);
      int idev1 = station*2+1;
      int idev2 = idev1+1;
      if ( idev2 >= _tt->nPlanes() ){
        throw cet::exception("GEOM")
          << "Requested a thin support after station: "
          << station
          << " This is between planes: "
          << idev1 << " and "
          << idev2 << "\n"
          << "But there are only "
          << _tt->nPlanes()
          << " planes\n";
      }
      name << "ThinSupportRing_" << i;

      // Center the support in the gap between two stations.
      double z = 0.5*( _tt->getPlane(idev1).origin().z() +  _tt->getPlane(idev2).origin().z());
      sup._stiffRings.push_back(PlacedTubs ( name.str(),  thinRingTubs, CLHEP::Hep3Vector( _xCenter, 0., _zCenter+z), _endRingMaterial ));
    }

  }

  // Create all of the Tubs objects needed for G4 to describe a straw.
  void TTrackerMaker::makeStrawTubs(){
    std::vector<StrawDetail>& details = _tt->_strawDetails;

    for ( std::vector<StrawDetail>::iterator i=details.begin();
          i != details.end(); ++i ){

      StrawDetail& detail(*i);

      detail._outerMetalThickness  = _wallOuterMetalThickness;
      detail._innerMetal1Thickness = _wallInnerMetal1Thickness;
      detail._innerMetal2Thickness = _wallInnerMetal2Thickness;
      detail._wirePlateThickness   = _wirePlateThickness;
      detail._outerMetalMaterial   = _wallOuterMetalMaterial;
      detail._innerMetal1Material  = _wallInnerMetal1Material ;
      detail._innerMetal2Material  = _wallInnerMetal2Material;
      detail._wirePlateMaterial    = _wirePlateMaterial;

    }
  }

  // Envelope that holds one plane ("TTrackerPlaneEnvelope")
  void TTrackerMaker::computePlaneEnvelope(){

    if ( _supportModel == SupportModel::simple ){
      double halfThick = _tt->_supportParams.halfThickness() + 2.*_tt->_manifoldHalfLengths[2];
      _tt->_planeEnvelopeParams = TubsParams( _tt->_envelopeInnerRadius,
                                               _tt->_supportParams.outerRadius(),
                                               halfThick);
    } else if ( _supportModel == SupportModel::detailedv0 ){
      _tt->_planeEnvelopeParams = TubsParams( _envelopeInnerRadius,
                                               _outerRingOuterRadius,
                                               _innerRingHalfLength );
    }else{
      throw cet::exception("GEOM")
        << "Unknown value of _supportModel in TTrackerMaker::computePlaneEnvelopeParams "
        << _supportModel
        << "\n";
    }
  }

  // Envelope that holds the full TTracker ("TrackerMother")
  void TTrackerMaker::computeTrackerEnvelope(){

    if ( _supportModel == SupportModel::simple ){

      // Envelope of a single plane.
      TubsParams planeEnvelope = _tt->getPlaneEnvelopeParams();

      // Full length from center to center of the first and last planes.
      double fullLength = _tt->_planes.back().origin().z()-_tt->_planes.front().origin().z();

      // Remember the thickness of the planes.
      double halfLength = fullLength/2. + planeEnvelope.zHalfLength();

      _tt->_innerTrackerEnvelopeParams = TubsParams( planeEnvelope.innerRadius(),
                                                     planeEnvelope.outerRadius(),
                                                     halfLength);

    } else if ( _supportModel == SupportModel::detailedv0 ){

      double fullLength = _tt->_planes.back().origin().z()-_tt->_planes.front().origin().z();
      double halfLength = fullLength/2. + _tt->getPlaneEnvelopeParams().zHalfLength();

      TubsParams val( _envelopeInnerRadius, _outerRingOuterRadius, halfLength);
      _tt->_innerTrackerEnvelopeParams = val;

    } else{

      throw cet::exception("GEOM")
        << "Unknown value of _supportModel in TTrackerMaker::computeTrackerEnvelopeParams "
        << _supportModel
        << "\n";
    }

  } //end TTrackerMaker::computeTrackerEvnvelopeParams


  void TTrackerMaker::recomputeHalfLengths(){

    // This code is only valid for the model detailedv0.
    if ( _supportModel != SupportModel::detailedv0 ) {
      return;
    }

    size_t idev0(0);
    size_t isec0(0);

    deque<Straw>& allStraws              = _tt->_allStraws;
    std::vector<StrawDetail>& allDetails = _tt->_strawDetails;
    //    size_t originalSize                  = allDetails.size();

    if (_verbosityLevel>2) {
      cout << __func__ << " Initial allDetails size "
           <<  allDetails.size()
           << endl;
    }

    // Step 1: Check that the pattern of _detailIndex is as expected.
    int nBad(0);
    for ( deque<Straw>::const_iterator i=allStraws.begin();
          i != allStraws.end(); ++i ){
      Straw const & straw(*i);

      if (_verbosityLevel>2) {
        cout << __func__ << " checking straw "
             << straw._id.getStraw()
             << " id: "
             << straw.id()
             << " has detailIndex of: "
             << straw._detailIndex
             << endl;
      }

      if ( straw._detailIndex != straw._id.getStraw() ){
        ++nBad;
        cout << "Unexpected value of detailIndex. Straw "
             << straw.id()
             << " has detailIndex of: "
             << straw._detailIndex
             << endl;
      }
    }
    if ( nBad > 0 ){
      throw cet::exception("GEOM")
        << "TTRackerMaker::recomputeHalfLengths: patterm of _detailIndex is not as expected."
        << "\n";
    }

    // Inner and outer radii of the channel in inner ring.
    double rmin = _innerRingInnerRadius;
    double rmax = rmin + _channelDepth;

    // Number of straws that are too short.
    int nShort(0);

    // Number of straws that have an illegal active length.
    int nIllegalActive(0);

    // Step 2: For all layers in PanelId(0,0) recompute the straw lengths.
    //         For layers > 0:
    //            - create a new StrawDetail object to hold the new length.
    //            - Reseat the _detail and _detailIndex objects in the Straw object.
    vector<Layer>& lays = _tt->_planes.at(idev0)._panels.at(isec0)._layers;
    for ( size_t ilay=0; ilay<lays.size(); ++ilay){
      Layer& lay(lays.at(ilay));

      // straws in layer are layed out contiguously, only their numbers increase by 2
      for (int ist=0; ist<lay.nStraws(); ++ist ){
        int idx             = lay._indices.at(ist).asInt();
        Straw& straw        = allStraws.at(idx);

        if (_verbosityLevel>2) {
          cout << __func__ << " recomputing: ist, idx "
               << ist << ", "
               << idx
               << " Straw " << straw._id.getStraw()
               << " id: "
               << straw._id
               << " detail index "
               << straw._detailIndex
               << endl;
        }

        StrawDetail& detail = ( ilay == 0 )
          ? allDetails.at(straw._detailIndex)
          : allDetails.at(straw._detailIndex-1);

        double r0 = straw.getMidPoint().perp();
        double r1 = r0 - _strawOuterRadius;
        double r2 = r0 + _strawOuterRadius;

        // Choose half length so that the outer edge of straw just touches the outer
        // limit of the channel.
        double hlen = sqrt(diff_of_squares( rmax, r2));

        // Active half-length of the straw.
        double activeHalfLen = sqrt( diff_of_squares(rmin,r0) )-_passivationMargin;
        activeHalfLen = std::max( activeHalfLen, 1.0);

        if ( activeHalfLen > hlen ){
          ++nIllegalActive;
          cout << "TTRackerMaker::recomputeHalfLengths: illegal passivation Margin.\n"
               << "The active length of straw " << ist << " is longer than its physical length\n";
        }

        // Check that the inner edge of the straw reaches the support
        double r3 = sqrt(sum_of_squares(hlen,r1));

        if ( r3 < rmin ){
          ++nShort;
          cout << "Straw is too short to reach the inner edge of the support.\n"
               << "Straw; " << straw.id()
               << " Radius at inner corner: " << r3 << " mm\n"
               << "Radius at inner edge of the support ring: " << rmin
               << endl;
        }

        if ( ilay == 0 ){
          detail._halfLength       = hlen;
          detail._activeHalfLength = activeHalfLen;
        } else {
          StrawDetail newDetail       = detail;
          newDetail._halfLength       = hlen;
          newDetail._activeHalfLength = activeHalfLen;

          // we need to use/set the detail index

          newDetail._id               = straw._detailIndex;

          allDetails[straw._id.getStraw()] = newDetail;
          straw._detail      = &allDetails.at(straw._id.getStraw());
          // straw._detailIndex = newDetail._id; // not needed in the new model

        }

        if (_verbosityLevel>2) {
          cout << __func__ << " after recomputing: ist, idx "
               << ist << ", "
               << idx
               << " Straw " << straw._id.getStraw()
               << " id: "
               << straw._id
               << endl;
        }

        const StrawDetail& theDetail = straw.getDetail();

        if ( _verbosityLevel > 2 ){

          cout << "Detail for: " << straw.id() << " " << theDetail.Id()            << endl;
          cout << "           outerTubsParams: " << theDetail.getOuterTubsParams()
               << theDetail.gasMaterialName()             << endl;
          cout << "           wallMother:      " << theDetail.wallMother()
               << theDetail.wallMotherMaterialName()      << endl;
          cout << "           wallOuterMetal:  " << theDetail.wallOuterMetal()
               << theDetail.wallOuterMetalMaterialName()  << endl;
          cout << "           wallCore         " << theDetail.wallCore()
               << theDetail.wallCoreMaterialName()        << endl;
          cout << "           wallInnerMetal1: " << theDetail.wallInnerMetal1()
               << theDetail.wallInnerMetal1MaterialName() << endl;
          cout << "           wallInnerMetal2: " << theDetail.wallInnerMetal2()
               << theDetail.wallInnerMetal2MaterialName() << endl;
          cout << "           wireMother:      " << theDetail.wireMother()
               << theDetail.wireMotherMaterialName()      << endl;
          cout << "           wirePlate:       " << theDetail.wirePlate()
               << theDetail.wirePlateMaterialName()       << endl;
          cout << "           wireCore:        " << theDetail.wireCore()
               << theDetail.wireCoreMaterialName()        << endl;
        }

      }
    }
    if ( nShort > 0 ){
      throw cet::exception("GEOM")
        << "TTRackerMaker::recomputeHalfLengths: some straaws are too short.\n"
        << "Probably the answer is to deepen the channel."
        << "\n";
    }

    if ( nIllegalActive > 0 ){
      throw cet::exception("GEOM")
        << "TTRackerMaker::recomputeHalfLengths: some straaws have an active length longer than their physical length.\n"
        << "Probably the passivationMargin is too negative."
        << "\n";
    }

    // Step 4: reseat _detail and _detailIndex for all other straws.
    for ( deque<Straw>::iterator i=allStraws.begin();
          i != allStraws.end(); ++i ){
      Straw& straw(*i);

      // These are already done:
      if ( straw.id().getPanelId() == PanelId(0,0) ) continue;
      if ( straw.id().getLayer()    ==            0  ) continue;

      // Get the new detail object for this straw.
      int idx = straw.id().getStraw();

      if (_verbosityLevel>2) {
        cout << __func__ << " about to reset "
             << " Straw " << straw._id.getStraw()
             << " id: "
             << straw._id << " using detail: "
             << idx
             << endl;
      }

      StrawDetail const& detail = allDetails.at(idx);

      // Update the info about the detail object.
      straw._detail      = &detail;
      straw._detailIndex = idx;
    }

  } //end TTrackerMaker::recomputeHalfLengths
  double
  TTrackerMaker::panelRotation(int isec,int idev) const {
    int jdev = idev%2;
    int ista = (idev/2)%2;
    int jsec = isec + jdev*_panelsPerPlane;
    double phi = _panelBaseRotations.at(jsec);
    if(ista==1)phi += _oddStationRotation;
    return phi;
  }

} // namespace mu2e
