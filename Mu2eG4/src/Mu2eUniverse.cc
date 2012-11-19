//
// Umbrela for the the Mu2e G4 world classes
//
// $Id: Mu2eUniverse.cc,v 1.1 2012/11/16 23:47:53 genser Exp $
// $Author: genser $
// $Date: 2012/11/16 23:47:53 $
//
// Original author Rob Kutschke
//
//

// C++ includes
#include <iostream>
#include <vector>
#include <iomanip>

// Framework includes
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib/exception.h"

// Mu2e includes
#include "G4Helper/inc/G4Helper.hh"
#include "Mu2eG4/inc/Mu2eUniverse.hh"

// G4 includes
#include "G4PhysicalVolumeStore.hh"

using namespace std;

namespace mu2e {

  Mu2eUniverse::Mu2eUniverse():
    _geom(*(art::ServiceHandle<GeometryService>())),
    _config(&((*(art::ServiceHandle<GeometryService>())).config())), 
    _helper(&(*(art::ServiceHandle<G4Helper>())))
  {} // can we use _geom in subsequent initializers???

  Mu2eUniverse::~Mu2eUniverse(){
  }

  // Convert to base units for all of the items in the vector.
  void Mu2eUniverse::setUnits( vector<double>& V, G4double unit ){
    for ( vector<double>::iterator b=V.begin(), e=V.end();
          b!=e; ++b){
      *b *= unit;
    }
  }

  // A helper function for debugging.  Print a subset of the physical volume store.
  void Mu2eUniverse::printPhys() {
    G4PhysicalVolumeStore* pstore = G4PhysicalVolumeStore::GetInstance();
    int n(0);
    for ( std::vector<G4VPhysicalVolume*>::const_iterator i=pstore->begin(); i!=pstore->end(); i++){
      cout << "Physical Volume: "
           << setw(5) << n++
           << (*i)->GetName()
           << endl;
      if ( n > 25 ) break;
    }

  }

} // end namespace mu2e