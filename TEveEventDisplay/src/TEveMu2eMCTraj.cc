#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eMCTraj.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

  void TEveMu2eMCTraj::DrawHit3D(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
  {
    std::string hstr=" hit %d";
    std::string dstr=" hit# %d\nLayer: %d";
    std::string strlst=pstr+hstr;
    std::string strlab=pstr+dstr;
    hep3vectorTocm(pointInMu2e);
    this->SetTitle(Form(strlab.c_str(),n,hstr));
    this->SetNextPoint(pointInMu2e.x(), pointInMu2e.y(), pointInMu2e.z()); 
    this->SetMarkerColor(kBlue);
    this->SetMarkerSize(2);
    this->SetPickable(kTRUE);
    HitList->AddElement(this);
  }
}
