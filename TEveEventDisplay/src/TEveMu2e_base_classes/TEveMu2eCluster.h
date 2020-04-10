#ifndef TEveMu2eCluster_h
#define TEveMu2eCluster_h

#include <TObject.h>
#include "RecoDataProducts/inc/CaloCluster.hh"
#include <TEvePointSet.h>
namespace mu2e {

class TEveMu2eCluster: public TEvePointSet, public CaloCluster {

    CaloCluster* fCaloCluster;

  public:
    #ifndef __CINT__
    explicit TEveMu2eCluster();
    virtual ~TEveMu2eCluster(){};
    #endif 

    void DrawCluster();
    void GetPositon();

    ClassDef(TEveMu2eCluster, 0);
};
}
#endif
