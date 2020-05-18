//Notes: TEvemu2eHit is an interface between mu2e hits and TEvePointSet which is a render-element holding a collection of 3D points, it has per-point operations and can be projected using TEveProjectionManager.

#ifndef TEveMu2eHit_h
#define TEveMu2eHit_h

#include <TObject.h>

#include <TEvePointSet.h>
#include <TEveLine.h>
#include "RecoDataProducts/inc/ComboHit.hh"

namespace mu2e {

class TEveMu2eHit : public TEvePointSet {

  public:
    #ifndef __CINT__
    explicit TEveMu2eHit();
    TEveMu2eHit(ComboHit chit) : fComboHit(chit){};
    virtual ~TEveMu2eHit(){};
    
    ComboHit fComboHit; 
   
    Int_t mColor = kBlue;
    Int_t mSize= 1; 

    bool AddErrorBar = true;
    void DrawHit2D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector HitPos, TEveElementList *list); 
    void DrawHit3D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector HitPos, TEveElementList *list); 
    void DrawHitCollection(const std::string &pstr, size_t i, std::vector<CLHEP::Hep3Vector> HitPos, TEveElementList *list); 

    //Access to the ComboHit members:
    XYZVec const GetPositon() { return fComboHit.pos() ;}//Just some examples - need to think what we want.
    double GetEnergy(){ return fComboHit.energyDep(); }

    #endif
    ClassDef(TEveMu2eHit, 0);
};
typedef std::vector<mu2e::TEveMu2eHit> TEveMu2eHitCollection;
}
#endif
