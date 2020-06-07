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
    addCRV_(conf.addCRV()),
    isCosmic_(conf.isCosmic()),
    MCOnly_(conf.MCOnly()),
    FillAll_(conf.FillAll())
  {}

//Note RecoDataProducts is a list inside the header of this class (in dictclasses) add any additional classes to that list//
    
  void Collection_Filler::FillRecoCollection(const art::Event& evt, Data_Collections &data, RecoDataProductName CollectionName){

  if(FillAll_ or(addHits_ and CollectionName == ComboHits)){ 
    auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
    data.chcol = chH.product();
  }
  if(FillAll_ or CollectionName == CaloCrystalHits){
    auto chH = evt.getValidHandle<mu2e::CaloCrystalHitCollection>(cryHitTag_);
    data.cryHitcol = chH.product();
  }
  if(FillAll_ or (addClusters_ and CollectionName==CaloClusters)){
    auto chH = evt.getValidHandle<mu2e::CaloClusterCollection>(cluTag_);
    data.clustercol = chH.product();
  }
  if(FillAll_ or (isCosmic_ and addTracks_ and CollectionName==CosmicTracks)){
    auto chH = evt.getValidHandle<mu2e::CosmicTrackSeedCollection>(cosmicTag_);
    data.cosmiccol = chH.product();
  }
  if(FillAll_ or (!isCosmic_ and addTracks_ and CollectionName==HelixSeeds)){
    auto chH = evt.getValidHandle<mu2e::HelixSeedCollection>(hseedTag_);
    data.hseedcol = chH.product();
  }
  if(FillAll_ or (!isCosmic_ and addTracks_ and CollectionName==KalSeeds)){
    auto chH = evt.getValidHandle<mu2e::KalSeedCollection>(kalseedTag_);
    data.kalseedcol = chH.product();
  }
  if(FillAll_ or (addCRV_ and CollectionName==CRVCoincidences)){
    auto chH = evt.getValidHandle<mu2e::CrvCoincidenceClusterCollection>(crvcoinTag_);
    data.crvcoincol = chH.product();
  }
  }

/*
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
*/
}
