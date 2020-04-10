#include <TObject.h>
#include <TSystem.h>
// ... libRIO
#include <TFile.h>

#include "TEveEventDisplay/src/dict_classes/Collection_Filler.h"
#include "art/Framework/Principal/SubRun.h"

using namespace mu2e;
namespace mu2e{

  Collection_Filler::Collection_Filler(const Config& conf) :
    chTag_(conf.chTag()),
	  gensTag_(conf.gensTag()),
	  strawdigiTag_(conf.strawdigiTag()),
	  crvdigiTag_(conf.crvdigiTag()),
	  addHits_(conf.addHits()),
	  addTracks_(conf.addTracks()),
	  addClusters_(conf.addClusters()),
	  addCrvHits_(conf.addCrvHits()),
	  addCosmicSeedFit_(conf.addCosmicSeedFit()),
	  isCosmic_(conf.isCosmic()){}
  
    bool Collection_Filler::HasCluster(const art::Event& evt){
	    _clustercol = 0; 
      auto chH = evt.getValidHandle<mu2e::CaloClusterCollection>(cluTag_);
	    _clustercol = chH.product();
	    return _clustercol != 0;
    }

    bool Collection_Filler::HasTrack(const art::Event& evt){
	    _cosmiccol = 0; 
      auto chH = evt.getValidHandle<mu2e::CosmicTrackSeedCollection>(cosmicTag_);
	    _cosmiccol = chH.product();
	    return _cosmiccol != 0;
    }

    bool Collection_Filler::HasComboHits(const art::Event& evt){
	    _chcol = 0; 
        auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
	    _chcol = chH.product();
	    return _chcol != 0;
    }

    void Collection_Filler::FindData(const art::Event& evt){
    if(addClusters_){
         _clustercol = 0; 
         auto chH = evt.getValidHandle<mu2e::CaloClusterCollection>(cluTag_);
	       _clustercol = chH.product();
      }
     if(addHits_){
        _chcol = 0; 
        auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
	      _chcol = chH.product();
     }
    }

  template<class collection>
      collection Collection_Filler::GetCollection(std::string name){
        collection c;
        if(name=="ComboHit"){
          c=_chcol;
        }
        if(name=="CaloCluster"){
          c=_clustercol;
        }
         if(name=="CosmicTrackSeed"){
          c=_cosmiccol;
        }
        return c;
    }

}
