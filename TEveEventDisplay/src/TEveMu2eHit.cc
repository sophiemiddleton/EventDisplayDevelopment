
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"

using namespace mu2e;
namespace mu2e{

	TEveMu2eHit::TEveMu2eHit(){}

 /* void TEveMu2eHit::AddErrorBar(TEveElementList *list, CLHEP::Hep3Vector pointInMu2e){
    TPolyLine3D *errors = new TPolyLine3D;
    errors->SetLineColor(kRed);
    auto const &chit = this->fComboHit;
    auto const& p = chit->pos();
    auto const& w = chit->wdir();
    auto const& s = chit->wireRes();
    double x1 = pointInMu2e.x() - (p.x()+s*w.x());
    double x2 = pointInMu2e.x() + (p.x()-s*w.x());
    double z1 = pointInMu2e.z() - (p.z()+s*w.z());
    double z2 = pointInMu2e.z() + (p.z()-s*w.z());
    double y1 = pointInMu2e.y() - (p.y()+s*w.y());
    double y2 = pointInMu2e.z() + (p.y()-s*w.y());
    errors->SetPoint(0, x1,y1,z1);
    errors->SetNextPoint(x2,y2,z2);
    list->AddElement(errors);
  }*/
  
  void TEveMu2eHit::DrawHit(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
  	{
		  std::string hstr=" hit %d";
		  std::string dstr=" hit# %d\nLayer: %d";
		  std::string strlst=pstr+hstr;
		  std::string strlab=pstr+dstr;

		  TEvePointSet* h = new TEvePointSet(Form(strlst.c_str(),n));
		  h->SetTitle(Form(strlab.c_str(),n,hstr));

		  std::cout<<"in mu2e : "<<n<<" "<<pointInMu2e.x()/10<<" "<<pointInMu2e.y()/10<<" "<<pointInMu2e.z()/10<<std::endl;
		 

		  h->SetNextPoint(pointInMu2e.x()/10, pointInMu2e.y()/10, pointInMu2e.z()/10); 
		  h->SetMarkerColor(mColor);
		  h->SetMarkerSize(mSize);
		  HitList->AddElement(h);
	  }

   void TEveMu2eHit::DrawHitCollection(const std::string &pstr, size_t n, std::vector<CLHEP::Hep3Vector> pointInMu2e, TEveElementList *HitList)
  	{
	    for(auto const& hit : pointInMu2e){
		    
		    std::string hstr=" hit %d";
		    std::string dstr=" hit# %d\nLayer: %d";
		    std::string strlst=pstr+hstr;
		    std::string strlab=pstr+dstr;

		    TEvePointSet* h = new TEvePointSet(Form(strlst.c_str(),n));
		    h->SetTitle(Form(strlab.c_str(),n,hstr));

		    std::cout<<"in mu2e : "<<n<<" "<<hit.x()/10<<" "<<hit.y()/10<<" "<<hit.z()/10<<std::endl;


		    h->SetNextPoint(hit.x()/10, hit.y()/10, hit.z()/10); 
		    h->SetMarkerColor(mColor);
		    h->SetMarkerSize(mSize);
		    HitList->AddElement(h);
	   }
    }
  }

