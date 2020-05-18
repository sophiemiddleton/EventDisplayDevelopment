//Notes: TEveMu2eTrkEllipse is an interface between mu2e hits and TEvePointSet which is a render-element holding a collection of 3D points, it has per-point operations and can be projected using TEveProjectionManager.

#ifndef TEveMu2eTrkEllipse_h
#define TEveMu2eTrkEllipse_h

#include <TObject.h>

#include <TEveElement.h>
#include <TArc.h>


namespace mu2e {

class TEveMu2eTrkEllipse :  public TEveElement, public TArc{

  public:
    #ifndef __CINT__
    explicit TEveMu2eTrkEllipse(){};

    virtual ~TEveMu2eTrkEllipse(){};
    Int_t mColor = kBlue;
    Int_t mSize= 1; 
    
    #endif
    ClassDef(TEveMu2eTrkEllipse, 0);
};
typedef std::vector<mu2e::TEveMu2eTrkEllipse> TEveMu2eTrkEllipseCollection;
}
#endif
