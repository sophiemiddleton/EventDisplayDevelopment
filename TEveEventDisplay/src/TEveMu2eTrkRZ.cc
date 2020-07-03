#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eTrkRZ.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

  void TEveMu2eTrkRZ::DrawWave(const std::string &pstr, TEveElementList *Track2DList)
  {
    this->SetLineColor(kYellow); 
    Track2DList->AddElement(this);
  }
}
