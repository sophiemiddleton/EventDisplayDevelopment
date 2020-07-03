#ifndef TEveMu2eTrkRZ_h
#define TEveMu2eTrkRZ_h

#include <TObject.h>

#include <TEveElement.h>
#include <TF1.h>

namespace mu2e {

class TEveMu2eTrkRZ :  public TEveElement, public TF1{

  public:
    #ifndef __CINT__
    explicit TEveMu2eTrkRZ(){};
    TEveMu2eTrkRZ(double r, double p) : radius(r), phi0(p){};
    virtual ~TEveMu2eTrkRZ(){};
    void DrawWave(const std::string &pstr, TEveElementList *HitList);
    double radius, phi0;
    #endif
    ClassDef(TEveMu2eTrkRZ, 0);
};

}
#endif
