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
    
    ComboHit* fComboHit;
    void DrawHit();
    void GetPositon();
    #endif
    ClassDef(TEveMu2eHit, 0);
};
}
#endif
