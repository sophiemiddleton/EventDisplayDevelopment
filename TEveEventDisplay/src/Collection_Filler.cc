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
	  crvcoinTag_(conf.crvdigiTag()),
    cluTag_(conf.cluTag()),
    cryHitTag_(conf.cryHitTag()),
    hseedTag_(conf.hseedTag()),
    kalseedTag_(conf.kalseedTag()),
	  addHits_(conf.addHits()),
	  addTracks_(conf.addTracks()),
	  addClusters_(conf.addClusters()),
	  addCrvHits_(conf.addCrvHits()),
	  addCosmicSeedFit_(conf.addCosmicSeedFit()),
	  isCosmic_(conf.isCosmic()),
    MCOnly_(conf.MCOnly())
    {}
  
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


      void Collection_Filler::FillRecoCollection(const art::Event& evt, Data_Collections &data, int code){

        if(code==1){ //ComboHits TODO -make this a string - it will be easier, or build an enum
          if(!addHits_) std::cout<<"you are adding hits when parameter is off "<<std::endl;
          auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
          data.chcol = chH.product();
        }
        if(code==2){
          if(!addHits_) std::cout<<"you are adding hits when parameter is off "<<std::endl;
          auto chH = evt.getValidHandle<mu2e::CaloClusterCollection>(cluTag_);
          data.clustercol = chH.product();
        }
         if(code==3){
          if(!addHits_) std::cout<<"you are adding hits when parameter is off "<<std::endl;
          auto chH = evt.getValidHandle<mu2e::CosmicTrackSeedCollection>(cosmicTag_);
          data.cosmiccol = chH.product();
        }
       
    }

//TODO- below is some thoughts - need to discuss - might need moving to the DataCollection
  template<class collection>
      void Collection_Filler::GetCollection(const art::Event& evt, collection &c, int code){
     
        if(code==1){ //ComboHits TODO - build an enum to allocate code to typename
          if(!addHits_) std::cout<<"you are adding hits when parameter is off "<<std::endl;
          _chcol = 0; 
          auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
	        _chcol = chH.product();
          c=_chcol;
        }
        if(code==2){
          c=_clustercol;
        }
         if(code==3){
          c=_cosmiccol;
        }
        return c;
    }

}
