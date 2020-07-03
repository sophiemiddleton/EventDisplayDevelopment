#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

	TEveMu2eCluster::TEveMu2eCluster(){}

  void TEveMu2eCluster::DrawCluster(const std::string &pstr,  CLHEP::Hep3Vector cog, TEveElementList *ClusterList)
  {
    double edep = fCaloCluster.energyDep();
    std::string hstr=" cluster %d";
    std::string dstr=" cluster# %d\nLayer: %d";
    std::string strlst=pstr+hstr+std::to_string(edep);
    std::string strlab=pstr+dstr;
    hep3vectorTocm(cog);
    this->SetTitle(Form(strlab.c_str(),1,hstr));
    Int_t mColor, mSize;
    if(edep<15) mColor = kCyan;
    if(edep>=15 and edep<20) mColor = kGreen;
    if(edep>=20 and edep<25) mColor = kYellow;
    if(edep>=25 and edep <40) mColor = kOrange;
    if(edep>=40 and edep < 50) mColor = kRed;
    if(edep>=50) mColor = kRed+2;
    mSize = 2;
    this->SetNextPoint(cog.x(), cog.y(), cog.z()); 
    this->SetMarkerColor(mColor);
    this->SetMarkerStyle(4);
    this->SetMarkerSize(mSize);
    this->SetPickable(kTRUE);

    ClusterList->AddElement(this);
  }
}

