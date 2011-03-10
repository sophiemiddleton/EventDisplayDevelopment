#ifndef CosmicRayShield_hh
#define CosmicRayShield_hh

//
// Representation of CosmicRayShield
//
// $Id: CosmicRayShield.hh,v 1.3 2011/03/10 22:02:20 genser Exp $
// $Author: genser $ 
// $Date: 2011/03/10 22:02:20 $
//
// Original author KLG
//

// c++ includes
#include <string>
#include <map>

// clhep includes
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

// Includes from Mu2e
#include "GeometryService/inc/Detector.hh"
#include "CosmicRayShieldGeom/inc/CRSSteelShield.hh"
#include "CosmicRayShieldGeom/inc/CRSScintillatorShield.hh"


namespace mu2e {

  // Forward reference.
  class SimpleConfig;
  class CosmicRayShieldMaker;

  class CosmicRayShield : public Detector {

    friend class CosmicRayShieldMaker;

  public:

    CosmicRayShield():
      _name("CosmicRayShield")
    {};

    ~CosmicRayShield(){};

    std::string name() const { return _name;};

    // Get SteelShield
    CRSSteelShield         const & getCRSSteelShield(std::string name) const;

    std::map<std::string,CRSSteelShield> const & getCRSSteelShields() const {
      return _steelShields;
    }

    // Get ScintillatorShield
    CRSScintillatorShield  const & getCRSScintillatorShield(std::string name)  const;

    std::map<std::string,CRSScintillatorShield> const & getCRSScintillatorShields() const {
      return _scintillatorShields;
    }

    CRSScintillatorBarDetail const & getCRSScintillatorBarDetail() const {
      return _barDetails;
    }

    std::vector<CRSScintillatorBar> const & getAllCRSScintillatorBars() const {
      return _allCRSScintillatorBars;
    }

    const CRSScintillatorBar& getCRSScintillatorBar ( CRSScintillatorBarIndex index ) const {
      return _allCRSScintillatorBars.at(index.asInt());
    }

    CLHEP::Hep3Vector const & getGlobalOffset() const {
      return _globalOffset;
    }

    CLHEP::Hep3Vector const & getLocalOffset() const {
      return _localOffset;
    }

  private:

    std::string _name;

    // position of the center in the parent frame
    CLHEP::Hep3Vector _localOffset;

    // position of the center in  Mu2e frame 
    CLHEP::Hep3Vector _globalOffset;

    std::map<std::string,CRSSteelShield>         _steelShields;

    std::map<std::string,CRSScintillatorShield>  _scintillatorShields;

    // Detailed info about scintillators etc...
    CRSScintillatorBarDetail _barDetails;

    // global holder of all scintillator bars
    std::vector<CRSScintillatorBar>  _allCRSScintillatorBars;



  };

}
#endif
