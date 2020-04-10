#ifndef TEveMu2eBrowser_h
#define TEveMu2eBrowser_h

#include <TObject.h>
#include <TEveManager.h>


namespace mu2e {

  class TEveMu2eBrowser: public TEveManager { 
    
  public:
    #ifndef __CINT__
    TEveMu2eBrowser (unsigned int w, unsigned int h) : TEveManager (w,h){};
    virtual ~TEveMu2eBrowser(){};
    #endif
   

    ClassDef(TEveMu2eBrowser, 0);
};
}
#endif
