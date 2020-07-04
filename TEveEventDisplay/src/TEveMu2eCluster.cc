#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

	TEveMu2eCluster::TEveMu2eCluster(){}

  void TEveMu2eCluster::DrawCluster(const std::string &pstr,  CLHEP::Hep3Vector cog, int energylevel, TEveElementList *ClusterList)
  {
    double edep = fCaloCluster.energyDep();
    std::string hstr=" cluster %d";
    std::string dstr=" cluster# %d\nLayer: %d";
    std::string strlst=pstr+hstr+std::to_string(edep);
    std::string strlab=pstr+dstr;
    hep3vectorTocm(cog);
    this->SetTitle(Form(strlab.c_str(),1,hstr));
    Int_t mSize;
    int colors[] = {+10, +5, +7, +8, -3, +1, -5, 0, -2, -4, +6, -9};
    this->SetMarkerColor(kViolet + colors[energylevel]);
    mSize = 3;
    this->SetNextPoint(cog.x(), cog.y(), cog.z()); 
    this->SetMarkerStyle(9);
    this->SetMarkerSize(mSize);
    this->SetPickable(kTRUE);

    ClusterList->AddElement(this);
  }
}

