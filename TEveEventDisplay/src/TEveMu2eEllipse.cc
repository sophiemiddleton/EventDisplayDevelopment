#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eTrkEllipse.h"
#include "TEveEventDisplay/src/dict_classes/GeomUtils.h"
using namespace mu2e;
namespace mu2e{

  void TEveMu2eTrkEllipse::DrawIt(const std::string &pstr, TEveElementList *Track2DList)
  {
    this->SetLineColor(kYellow); 
    this->DrawArc(this->GetX1(), this->GetY1(), this->GetR1());
    Track2DList->AddElement(this);
  }
}
