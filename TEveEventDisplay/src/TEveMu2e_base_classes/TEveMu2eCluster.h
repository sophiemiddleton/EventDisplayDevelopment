#ifndef TEveMu2eCluster_h
#define TEveMu2eCluster_h

#include <TObject.h>
#include "RecoDataProducts/inc/CaloCluster.hh"

#include <TEvePointSet.h>
#include <TEveCalo.h>

namespace mu2e {

class TEveMu2eCluster: public TEvePointSet{

    CaloCluster fCaloCluster;
    
  public:
    #ifndef __CINT__
    explicit TEveMu2eCluster();
    TEveMu2eCluster(CaloCluster cluster) : fCaloCluster(cluster){};
    virtual ~TEveMu2eCluster(){};
    #endif 

    void DrawCluster(const std::string &pstr, CLHEP::Hep3Vector COG, TEveElementList *list); 
    const  CLHEP::Hep3Vector GetPositon() { return fCaloCluster.cog3Vector() ;}
    double GetEnergy() { return fCaloCluster.energyDep(); }
    ClassDef(TEveMu2eCluster, 0);
};
}
#endif
