//
// Make a ExtinctionMonitor.
//
// $Id: ExtMonUCI.hh,v 1.2 2011/12/27 21:53:01 youzy Exp $
// $Author: youzy $
// $Date: 2011/12/27 21:53:01 $

#ifndef EXTMONUCI_HH
#define EXTMONUCI_HH

#include <vector>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include "GeometryService/inc/Detector.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/Mu2eBuilding.hh"

#include "ExtinctionMonitorUCIGeom/inc/ExtMonUCICol.hh"
#include "ExtinctionMonitorUCIGeom/inc/ExtMonUCIMag.hh"
#include "ExtinctionMonitorUCIGeom/inc/ExtMonUCITof.hh"

using namespace std;

namespace mu2e {

  namespace ExtMonUCI {
    
    class ExtMon : public Detector {

    friend class ExtMonMaker;

    public: 
      // implement Detector's method
      virtual std::string name() const { return "ExtMonUCI"; }

      // Envelope
      const vector<double>& envelopeParams() const { return _envelopeParams; }
      const vector<double>& envelopeOrigin() const { return _envelopeOrigin; }

      void setHallOriginInMu2e(double x, double y, double z) { _hallOriginInMu2e = CLHEP::Hep3Vector(x,y,z); }

      CLHEP::Hep3Vector origin() const;
      CLHEP::Hep3Vector originLocal() const;
      const CLHEP::HepRotation& rotation() const { return _rotation; }
      const CLHEP::Hep3Vector&  hallOriginInMu2e() const { return _hallOriginInMu2e; }

      // Collimators
      int nCols() const { return _nCols; }
      const vector<double>& colOuterHalfLengths() const { return _colOuterHalfLengths; }
      const vector<double>& colInnerHalfLengths() const { return _colInnerHalfLengths; }
      const vector<double>& colPosition1() const { return _colPosition1; }
      const vector<double>& colPosition2() const { return _colPosition2; }
      const ExtMonCol* col(unsigned int iCol) const; 

      // Magnets
      int nMags() const { return _nMags; }
      const vector<double>& magOuterHalfLengths() const { return _magOuterHalfLengths; }
      const vector<double>& magInnerHalfLengths() const { return _magInnerHalfLengths; }
      const vector<double>& magPosition1() const { return _magPosition1; }
      const vector<double>& magPosition2() const { return _magPosition2; }
      const ExtMonMag* mag(unsigned int iMag) const;

      // Tofs
      int nTofStations() const { return _nTofStations; }
      int nTofSegments() const { return _nTofSegments; }
      int nTofs() const { return _nTofStations * _nTofSegments; }
      const vector<double>& tofHalfLengths() const { return _tofHalfLengths; }
      const vector<double>& tofPosition() const { return _tofPosition; }
      const ExtMonTof* tof(unsigned int iTofStation, unsigned int iTofSegment) const;
      const ExtMonTof* tof(unsigned int iTof) const;

      // Coordinate conversion to/from the Mu2e frame
      // 
      // - The (0,0,0) point is in the middle of the ExtMon detector.
      // 
      // In G4 this is the coordinate system of the ExtMonUCI volume.
      CLHEP::Hep3Vector extMonToMu2ePoint( CLHEP::Hep3Vector const& v ) const;      
      CLHEP::Hep3Vector extMonToMu2eMomentum( CLHEP::Hep3Vector const& v ) const;      
      CLHEP::Hep3Vector mu2eToExtMonPoint( CLHEP::Hep3Vector const& v ) const;
      CLHEP::Hep3Vector mu2eToExtMonMomentum( CLHEP::Hep3Vector const& v ) const;

    private: 

      ExtMon(const std::vector<double>& envelopeParams, 
	     const std::vector<double>& envelopeOrigin);

      CLHEP::Hep3Vector   _hallOriginInMu2e;

      std::vector<double> _envelopeParams;
      std::vector<double> _envelopeOrigin;
      CLHEP::HepRotation  _rotation;

      std::vector<double> _abs1Params;
      std::vector<double> _abs1Origin;

      // Collimators
      int _nCols;
      std::vector<double> _colOuterHalfLengths;
      std::vector<double> _colInnerHalfLengths;
      std::vector<double> _colPosition1;
      std::vector<double> _colPosition2;
      std::vector<ExtMonCol> _cols;

      // Magnets
      int _nMags;
      std::vector<double> _magOuterHalfLengths;
      std::vector<double> _magInnerHalfLengths;
      std::vector<double> _magPosition1;
      std::vector<double> _magPosition2;
      std::vector<ExtMonMag> _mags;

      // Tofs
      int _nTofStations;
      int _nTofSegments;
      std::vector<double> _tofHalfLengths;
      std::vector<double> _tofPosition;
      std::vector<ExtMonTof> _tofs;

    };

  }
}

#endif/*EXTMONUCI_HH*/
