#ifndef TEveMu2eCluster_h
#define TEveMu2eCluster_h

#include <TObject.h>
#include "RecoDataProducts/inc/CaloCluster.hh"
#include <TEvePointSet.h>
namespace mu2e {

class TEveMu2eCluster: public TEvePointSet {

    CaloCluster* fCaloCluster;

  public:
    #ifndef __CINT__
    explicit TEveMu2eCluster();
    virtual ~TEveMu2eCluster(){};
    #endif 

    void DrawCluster(const std::string &pstr, double edep, CLHEP::Hep3Vector COG, TEveElementList *list); 
    void GetPositon();
    ClassDef(TEveMu2eCluster, 0);
};
}
#endif
