#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCRVEvent.h"

using namespace mu2e;
namespace mu2e{

	TEveMu2eCRVEvent::TEveMu2eCRVEvent(){}

  
  void TEveMu2eCRVEvent::DrawHit3D(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
  	{
      std::string hstr=" hit %d";
      std::string dstr=" hit# %d\nLayer: %d";
      std::string strlst=pstr+hstr;
      std::string strlab=pstr+dstr;

      this->SetTitle(Form(strlab.c_str(),n,hstr));

      std::cout<<"in mu2e :3D "<<n<<" "<<pointInMu2e.x()/10<<" "<<pointInMu2e.y()/10<<" "<<pointInMu2e.z()/10<<std::endl;
	
      this->SetNextPoint(pointInMu2e.x()/10, pointInMu2e.y()/10, pointInMu2e.z()/10); 
      this->SetMarkerColor(mColor);
      this->SetMarkerSize(mSize);
      this->SetPickable(kTRUE);
      HitList->AddElement(this);
      std::cout<<"CRVRecoPulseDrawn"<<std::endl;
	  }
  void TEveMu2eCRVEvent::DrawHit2D(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
  	{
      std::string hstr=" hit %d";
      std::string dstr=" hit# %d\nLayer: %d";
      std::string strlst=pstr+hstr;
      std::string strlab=pstr+dstr;

      this->SetTitle(Form(strlab.c_str(),n,hstr));

      this->SetNextPoint(pointInMu2e.x()/10, pointInMu2e.y()/10, pointInMu2e.z()/10); 
      this->SetMarkerColor(mColor);
      this->SetMarkerSize(mSize);
      this->SetPickable(kTRUE);
      HitList->AddElement(this);

	  }

  }


