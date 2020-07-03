#ifndef TEveMu2eTrkEllipse_h
#define TEveMu2eTrkEllipse_h

#include <TObject.h>

#include <TEveElement.h>
#include <TArc.h>

namespace mu2e {

class TEveMu2eTrkEllipse :  public TEveElement, public TArc{

  public:
    #ifndef __CINT__
    using TArc::DrawArc;
    explicit TEveMu2eTrkEllipse(){};
    virtual ~TEveMu2eTrkEllipse(){};
    Int_t mColor = kBlue;
    Int_t mSize= 1; 
    void DrawArc(int);
    void DrawIt(const std::string &pstr, TEveElementList *HitList);
    #endif
    ClassDef(TEveMu2eTrkEllipse, 0);
};
typedef std::vector<mu2e::TEveMu2eTrkEllipse> TEveMu2eTrkEllipseCollection;
}
#endif
