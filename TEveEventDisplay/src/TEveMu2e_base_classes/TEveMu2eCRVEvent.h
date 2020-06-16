

#ifndef TEveMu2eCRVEvent_h
#define TEveMu2eCRVEvent_h

#include <TObject.h>

#include <TEvePointSet.h>
#include <TEveLine.h>
#include "RecoDataProducts/inc/CrvCoincidenceCluster.hh"

namespace mu2e {

class TEveMu2eCRVEvent : public TEvePointSet {

  public:
    #ifndef __CINT__
    explicit TEveMu2eCRVEvent();
    TEveMu2eCRVEvent(CrvCoincidenceCluster chit) : fCrvCoincidenceCluster(chit){};
    virtual ~TEveMu2eCRVEvent(){};
    
    CrvCoincidenceCluster fCrvCoincidenceCluster; 
   
    Int_t mColor = kBlue;
    Int_t mSize= 1; 

    bool AddErrorBar = true;
    void DrawHit2D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector HitPos, TEveElementList *list); 
    void DrawHit3D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector HitPos, TEveElementList *list); 

    #endif
    ClassDef(TEveMu2eCRVEvent, 0);
};
typedef std::vector<mu2e::TEveMu2eCRVEvent> TEveMu2eCRVEventCollection;
}
#endif
