#include "TEveEventDisplay/src/TEveMu2e_base_classes/TEveMu2eParticle.h"

using namespace mu2e;
namespace mu2e{

  TEveMu2eParticle::TEveMu2eParticle(){}

  void TEveMu2eParticle::DrawParticle3D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector pointInMu2e, TEveElementList *ParticleList)
  {
    std::string hstr=" MC Sim Particle %d";
    std::string dstr=" particle# %d\nLayer: %d";
    std::string strlst=pstr+hstr;
    std::string strlab=pstr+dstr;
    hep3vectorTocm(pointInMu2e);
    this->SetTitle(Form(strlab.c_str(),b,hstr));
    this->SetNextPoint(pointInMu2e.x(), pointInMu2e.y(), pointInMu2e.z()); 
    //int colors[] = {-7, 3, -6, -1, 9, 0, -4, 10, 1}; //TODO: change this
    this->SetMarkerColor(kPink);// + colors[energylevel]);
    this->SetMarkerSize(mSize);
    this->SetPickable(kTRUE);
    ParticleList->AddElement(this);
  }

  void TEveMu2eParticle::DrawParticle2D(const std::string &pstr, Int_t b,CLHEP::Hep3Vector pointInMu2e, TEveElementList *ParticleList)
  {
    std::string hstr=" MC Sim Particle %d";
    std::string dstr=" particle# %d\nLayer: %d";
    std::string strlst=pstr+hstr;
    std::string strlab=pstr+dstr;
    hep3vectorTocm(pointInMu2e);
    this->SetTitle(Form(strlab.c_str(),b,hstr));
    this->SetNextPoint(pointInMu2e.x(), pointInMu2e.y(), pointInMu2e.z()); 
    //int colors[] = {-7, 3, -6, -1, 9, 0, -4, 10, 1};
    this->SetMarkerColor(kPink);// + colors[energylevel]);
    this->SetMarkerSize(mSize);
    this->SetPickable(kTRUE);
    ParticleList->AddElement(this);
    }
  }
