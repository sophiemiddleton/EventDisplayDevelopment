//Notes: TEvemu2eHit is an interface between mu2e hits and TEvePointSet which is a render-element holding a collection of 3D points, it has per-point operations and can be projected using TEveProjectionManager.

#ifndef TEveMu2eHit_h
#define TEveMu2eHit_h

#include <TObject.h>

#include <TEvePointSet.h>

#include "RecoDataProducts/inc/ComboHit.hh"

namespace mu2e {

class TEveMu2eHit : public TEvePointSet, public ComboHit {

    

  public:
    #ifndef __CINT__
    explicit TEveMu2eHit();
    virtual ~TEveMu2eHit(){};
    
    const ComboHit *fComboHit;

    TEveElementList *HitList;
    Int_t mColor = 2;//default color = kRed (see TColor ROOT class for other names)
    Int_t mSize= 1; 
    void DrawHit(const std::string &pstr, Int_t b,CLHEP::Hep3Vector HitPos); //single point
    void DrawHitCollection(const std::string &pstr, size_t i, std::vector<CLHEP::Hep3Vector> HitPos); //draw collection of points
    void GetPositon();
    #endif
    ClassDef(TEveMu2eHit, 0);
};
typedef std::vector<mu2e::TEveMu2eHit> TEveMu2eHitCollection;
}
#endif
