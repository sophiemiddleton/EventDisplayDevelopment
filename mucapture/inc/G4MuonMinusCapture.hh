#ifndef G4MuonMinusCapture_h
#define G4MuonMinusCapture_h 1

#include "globals.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4MuonMinusAtomicCapture.hh"

class G4MuonMinusCapture: public G4VPhysicsConstructor 
{
public:
  explicit G4MuonMinusCapture(G4int verbosity = 1);
  explicit G4MuonMinusCapture(G4String const & name,
                              G4int verbosity = 1);

  virtual ~G4MuonMinusCapture();
  
  // Construct particles and physics processes 
  // FIXME check that em physics is there (if it is really needed)
  void ConstructParticle();
  void ConstructProcess();

  G4MuonMinusAtomicCapture* GetMuonMinusAtomicCaptureProcess();

private:
  // all the process etc... pointers go here
  // FIXME put the right data here to be deleted afterwards if needed in ~G4MuonMinusCapture();

  // we may need this process to be available externally
  G4MuonMinusAtomicCapture* muproc;

  // FIXME need physics name/type ???

  //void 	SetPhysicsName (const G4String &="")
  //const G4String & 	GetPhysicsName () const
  //void 	SetPhysicsType (G4int)


  // aux data
  //  G4int    verbosity;

  // this is a parent class data member:
  // G4int 	verboseLevel

  G4bool   particlesConstructed;
  G4bool   processesConstructed;

};

#endif
