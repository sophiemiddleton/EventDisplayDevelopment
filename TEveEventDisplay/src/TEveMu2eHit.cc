
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eHit.h"

using namespace mu2e;
namespace mu2e{

	TEveMu2eHit::TEveMu2eHit(){}

  
  void TEveMu2eHit::DrawHit3D(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
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

      if(AddErrorBar){ 
        TEveLine *error = new TEveLine();

        auto const& p = fComboHit.pos();
        auto const& w = fComboHit.wdir();
        auto const& s = fComboHit.wireRes();
        double x1 = (p.x()+s*w.x());
        double x2 = (p.x()-s*w.x());
        double z1 = (p.z()+s*w.z());
        double z2 = (p.z()-s*w.z());
        double y1 = (p.y()+s*w.y());
        double y2 = (p.y()-s*w.y());
        std::cout<<"first 3D"<<x1/10<<" "<<y1/10<<" "<<z1/10<<std::endl;
        std::cout<<" second 3D"<<x2/10<<" "<<y2/10<<" "<<z2/10<<std::endl;
        error->SetPoint(0, x1/10-390,y1/10,z1/10+1017);
        error->SetNextPoint(x2/10-390,y2/10,z2/10+1017);
        error->SetLineColor(kRed);
        HitList->AddElement(error);
      }
		  HitList->AddElement(this);

	  }
  void TEveMu2eHit::DrawHit2D(const std::string &pstr, Int_t n, CLHEP::Hep3Vector pointInMu2e, TEveElementList *HitList)
  	{
		  std::string hstr=" hit %d";
		  std::string dstr=" hit# %d\nLayer: %d";
		  std::string strlst=pstr+hstr;
		  std::string strlab=pstr+dstr;

		  this->SetTitle(Form(strlab.c_str(),n,hstr));

		  this->SetNextPoint(pointInMu2e.x()/10, pointInMu2e.y()/10, pointInMu2e.z()/10); 
		  this->SetMarkerColor(mColor);
		  this->SetMarkerSize(mSize);

      if(AddErrorBar){ 
        TEveLine *error = new TEveLine();

        auto const& p = fComboHit.pos();
        auto const& w = fComboHit.wdir();
        auto const& s = fComboHit.wireRes();
        double x1 = (p.x()+s*w.x());
        double x2 = (p.x()-s*w.x());
        double z1 = (p.z()+s*w.z());
        double z2 = (p.z()-s*w.z());
        double y1 = (p.y()+s*w.y());
        double y2 = (p.y()-s*w.y());
        std::cout<<"first "<<x1/10<<" "<<y1/10<<" "<<z1/10<<std::endl;
        std::cout<<" second "<<x2/10<<" "<<y2/10<<" "<<z2/10<<std::endl;
        error->SetPoint(0, x1/10,y1/10,z1/10);
        error->SetNextPoint(x2/10,y2/10,z2/10);
        error->SetLineColor(kRed);
        HitList->AddElement(error);
      }
		  HitList->AddElement(this);

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

