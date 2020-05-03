
#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eCluster.h"

using namespace mu2e;
namespace mu2e{

	TEveMu2eCluster::TEveMu2eCluster(){}

  void TEveMu2eCluster::DrawCluster(const std::string &pstr, double edep, CLHEP::Hep3Vector cog, TEveElementList *ClusterList)
  	{
		  std::string hstr=" cluster %d";
		  std::string dstr=" cluster# %d\nLayer: %d";
		  std::string strlst=pstr+hstr;
		  std::string strlab=pstr+dstr;

		  TEvePointSet* c = new TEvePointSet(Form(strlst.c_str(),1));
		  c->SetTitle(Form(strlab.c_str(),1,hstr));

		  std::cout<<"in mu2e : "<<" "<<cog.x()/10<<" "<<cog.y()/10<<" "<<cog.z()/10<<std::endl;

      Int_t mColor, mSize;
      if(edep<15) mColor = kCyan;
      if(edep>=15 and edep<20) mColor = kGreen;
      if(edep>=20 and edep<25) mColor = kYellow;
      if(edep>=25 and edep <35) mColor = kOrange;
      if(edep>=35 and edep < 50) mColor = kRed;
      if(edep>=50) mColor = kRed+2;
      mSize = 2;//TODO
		  c->SetNextPoint(cog.x()/10, cog.y()/10, cog.z()/10); 
		  c->SetMarkerColor(mColor);
      c->SetMarkerStyle(21);
		  c->SetMarkerSize(mSize);
		  ClusterList->AddElement(c);
	  }


}

