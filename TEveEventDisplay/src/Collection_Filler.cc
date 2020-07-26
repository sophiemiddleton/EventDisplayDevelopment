#include <TObject.h>
#include <TSystem.h>
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
    mcdigisTag_(conf.mcdigisTag()),
    mccHitTag_(conf.mccHitTag()),
    mccHitSPTag_(conf.mccHitSPTag()),
    mccrvcoincluTag_(conf.mccrvcoincluTag()),
    mccrvdigiTag_(conf.mccrvdigiTag()),
    mctrajTag_(conf.mctrajTag()),
    mcstrawdigiTag_(conf.mcstrawdigiTag()),
    addHits_(conf.addHits()),
    addTracks_(conf.addTracks()),
    addClusters_(conf.addClusters()),
    addCrvHits_(conf.addCrvHits()),
    addCosmicSeedFit_(conf.addCosmicSeedFit()),
    isCosmic_(conf.isCosmic()),
    RecoOnly_(conf.RecoOnly()),
    FillAll_(conf.FillAll()),
    addMCCaloDigis_(conf.addMCCaloDigis()), 
    addMCHits_(conf.addMCHits()), 
    addMCHitsSP_(conf.addMCHitsSP()), 
    addMCCrvCC_(conf.addMCCrvCC()), 
    addMCCrvDigis_(conf.addMCCrvDigis()), 
    addMCTraj_(conf.addMCTraj()), 
    addMCStrawDigis_(conf.addMCStrawDigis()),
    MCOnly_(conf.MCOnly())
  {}

  void Collection_Filler::FillRecoCollections(const art::Event& evt, Data_Collections &data, RecoDataProductName CollectionName){
    if(FillAll_ or RecoOnly_ or (addHits_ and CollectionName == ComboHits)){ 
      auto chH = evt.getValidHandle<mu2e::ComboHitCollection>(chTag_);
      data.chcol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or CollectionName == CaloCrystalHits){
      auto chH = evt.getValidHandle<mu2e::CaloCrystalHitCollection>(cryHitTag_);
      data.cryHitcol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or (addClusters_ and CollectionName==CaloClusters)){
      auto chH = evt.getValidHandle<mu2e::CaloClusterCollection>(cluTag_);
      data.clustercol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or (addCosmicSeedFit_ and CollectionName==CosmicTracks)){
      auto chH = evt.getValidHandle<mu2e::CosmicTrackSeedCollection>(cosmicTag_);
      data.cosmiccol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or (!isCosmic_ and addTracks_ and CollectionName==HelixSeeds)){
      auto chH = evt.getValidHandle<mu2e::HelixSeedCollection>(hseedTag_);
      data.hseedcol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or (!isCosmic_ and addTracks_ and CollectionName==KalSeeds)){
      auto chH = evt.getValidHandle<mu2e::KalSeedCollection>(kalseedTag_);
      data.kalseedcol = chH.product();
    }
    if(FillAll_ or RecoOnly_ or (addCrvHits_ and CollectionName==CRVRecoPulses)){
      auto chH = evt.getValidHandle<mu2e::CrvRecoPulseCollection>(crvcoinTag_);
      data.crvcoincol = chH.product();
    }
  }


  void Collection_Filler::FillMCCollections(const art::Event& evt, Data_Collections &data, MCDataProductName CollectionName){
    if(FillAll_ or MCOnly_ or (addMCCaloDigis_ and CollectionName == CaloDigisMC)){ 
      auto chH = evt.getValidHandle<mu2e::CaloDigiMCCollection>(mcdigisTag_);
      data.mccdigicol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCHits_ and CollectionName == CaloHitsMCTruth)){ 
      auto chH = evt.getValidHandle<mu2e::CaloHitMCTruthCollection>(mccHitTag_);
      data.mchittruthcol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCHitsSP_ and CollectionName == CaloHitSimPartsMC)){ 
      auto chH = evt.getValidHandle<mu2e::CaloHitSimPartMCCollection>(mccHitSPTag_);
      data.mcchitspcol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCCrvCC_ and CollectionName == CrvCoincidenceClustersMC)){ 
      auto chH = evt.getValidHandle<mu2e::CrvCoincidenceClusterMCCollection>(mccrvcoincluTag_);
      data.mccrvcccol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCCrvDigis_ and CollectionName == CrvDigisMC)){ 
      auto chH = evt.getValidHandle<mu2e::CrvDigiMCCollection>(mccrvdigiTag_);
      data.mccrvdigicol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCTraj_ and CollectionName == MCTrajectories)){ 
      auto chH = evt.getValidHandle<mu2e::MCTrajectoryCollection>(mctrajTag_);
      data.mctrajcol = chH.product();
    }
    if(FillAll_ or MCOnly_ or (addMCStrawDigis_ and CollectionName == StrawDigisMC)){ 
      auto chH = evt.getValidHandle<mu2e::StrawDigiMCCollection>(mcstrawdigiTag_);
      data.mcsdigicol = chH.product();
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



