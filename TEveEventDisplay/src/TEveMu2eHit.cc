
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"

using namespace mu2e;
namespace mu2e{

	TEveMu2eHit::TEveMu2eHit(){}

  
  void TEveMu2eHit::DrawHit(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e)
  	{
	
		  
		  std::string hstr=" hit %d";
		  std::string dstr=" hit# %d\nLayer: %d";
		  std::string strlst=pstr+hstr;
		  std::string strlab=pstr+dstr;

		  TEvePointSet* h = new TEvePointSet(Form(strlst.c_str(),n));
		  h->SetTitle(Form(strlab.c_str(),n,hstr));

		  std::cout<<"in mu2e : "<<n<<" "<<pointInMu2e.x()/10<<" "<<pointInMu2e.y()/10<<" "<<pointInMu2e.z()/10<<std::endl;
		  h->SetMarkerColor(kRed);
		  h->SetMarkerSize(mSize);

		  h->SetNextPoint(pointInMu2e.x()/10, pointInMu2e.y()/10, pointInMu2e.z()/10); 
		  h->SetMarkerColor(mColor);
		  h->SetMarkerSize(mSize);
		  HitList->AddElement(h);
	  }

   void TEveMu2eHit::DrawHitCollection(const std::string &pstr, size_t n, std::vector<CLHEP::Hep3Vector> pointInMu2e)
  	{
	    for(auto const& hit : pointInMu2e){
		    
		    std::string hstr=" hit %d";
		    std::string dstr=" hit# %d\nLayer: %d";
		    std::string strlst=pstr+hstr;
		    std::string strlab=pstr+dstr;

		    TEvePointSet* h = new TEvePointSet(Form(strlst.c_str(),n));
		    h->SetTitle(Form(strlab.c_str(),n,hstr));

		    std::cout<<"in mu2e : "<<n<<" "<<hit.x()/10<<" "<<hit.y()/10<<" "<<hit.z()/10<<std::endl;
		    h->SetMarkerColor(kRed);
		    h->SetMarkerSize(mSize);

		    h->SetNextPoint(hit.x()/10, hit.y()/10, hit.z()/10); 
		    h->SetMarkerColor(mColor);
		    h->SetMarkerSize(mSize);
		    HitList->AddElement(h);
	   }
    }
  }

