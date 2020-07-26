#ifndef Collection_Filler_h
#define Collection_Filler_h
//Cosmics:
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
//Calo:
#include "RecoDataProducts/inc/CaloCrystalHitCollection.hh"
#include "RecoDataProducts/inc/CaloHitCollection.hh"
//MC Products:
#include "MCDataProducts/inc/PhysicalVolumeInfoCollection.hh"
#include "MCDataProducts/inc/PhysicalVolumeInfoMultiCollection.hh"
#include "MCDataProducts/inc/MCTrajectoryCollection.hh"
#include "MCDataProducts/inc/SimParticleCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
//Kalman Tracks
#include "RecoDataProducts/inc/KalSeed.hh"
#include "RecoDataProducts/inc/KalRepCollection.hh"
#include "RecoDataProducts/inc/TrkExtTrajCollection.hh"
//Tracker Hits:
#include "RecoDataProducts/inc/StrawHitCollection.hh"
#include "RecoDataProducts/inc/StrawHitFlagCollection.hh"
#include "RecoDataProducts/inc/StrawHitPositionCollection.hh"
#include "RecoDataProducts/inc/StrawDigiCollection.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
//CRV:
#include "RecoDataProducts/inc/CrvRecoPulseCollection.hh"
#include "RecoDataProducts/inc/CrvCoincidenceClusterCollection.hh"
//Art:
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

#include<TObject.h>
#include <TROOT.h>
#include <TGComboBox.h>
#include <TGListBox.h>
#include <iostream>
#include <vector>

using namespace CLHEP;

#include "TEveEventDisplay/src/dict_classes/Data_Collections.h"

namespace mu2e{

  enum RecoDataProductName {ComboHits, CaloCrystalHits, CaloClusters, CosmicTracks, HelixSeeds, KalSeeds, CRVRecoPulses, TrkExtTrajectories};
  enum MCDataProductName {CaloDigisMC, CaloHitsMCTruth, CaloHitSimPartsMC, CrvCoincidenceClustersMC, CrvDigisMC, MCTrajectories, StrawDigisMC};

	class Collection_Filler
	{
  public:
    struct Config{
      using Name=fhicl::Name;
      using Comment=fhicl::Comment;
      //RecoData Products
      fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("for info"),0};
      fhicl::Atom<art::InputTag>chTag{Name("ComboHitCollection"),Comment("chTag"), "makePH"};
      fhicl::Atom<art::InputTag>gensTag{Name("GenParticleCollection"),Comment("gensTag")};
      fhicl::Atom<art::InputTag>strawdigiTag{Name("StrawDigiCollection"),Comment("strawdigiTag")};
      fhicl::Atom<art::InputTag>crvdigiTag{Name("CrvRecoPulseCollection"),Comment("crvTag")};
      fhicl::Atom<art::InputTag>cosmicTag{Name("CosmicTrackSeedCollection"),Comment("cosmicTag")};
      fhicl::Atom<art::InputTag>cluTag{Name("CaloClusterCollection"),Comment("cluTag")};
      fhicl::Atom<art::InputTag>cryHitTag{Name("CaloCrystalHitCollection"),Comment("cryHitTag")};
      fhicl::Atom<art::InputTag>hseedTag{Name("HelixSeedCollection"),Comment("hseedTag")};
      fhicl::Atom<art::InputTag>kalseedTag{Name("KalSeedCollection"),Comment("kalseedTag")};
      fhicl::Atom<art::InputTag>trkexttrajTag{Name("TrkExtTrajCollection"),Comment("trkexttrajTag")};
      //MC Data Products
      fhicl::Atom<art::InputTag>mcdigisTag{Name("CaloDigiMCCollection"),Comment("mcdigisTag")};
      fhicl::Atom<art::InputTag>mccHitTag{Name("CaloHitMCTruthCollection"),Comment("mccHitTag")};
      fhicl::Atom<art::InputTag>mccHitSPTag{Name("CaloHitSimPartMCCollection"),Comment("mccHitSPTag")};
    fhicl::Atom<art::InputTag>mccrvcoincluTag{Name("CrvCoincidenceClusterMCCollection"),Comment("mccrvcoincluTag")};
      fhicl::Atom<art::InputTag>mccrvdigiTag{Name("CrvDigiMCCollection"),Comment("mccrvdigiTag")};
      fhicl::Atom<art::InputTag>mctrajTag{Name("MCTrajectoryCollection"),Comment("mctrajTag")};
      fhicl::Atom<art::InputTag>mcstrawdigiTag{Name("StrawDigiMCCollection"),Comment("mcstrawdigiTag")};

     //RecoData Products
      fhicl::Atom<bool> addHits{Name("addHits"), Comment("set to add the hits"),false};
      fhicl::Atom<bool> addTracks{Name("addTracks"), Comment("set to add tracks"),false};
      fhicl::Atom<bool> addClusters{Name("addClusters"), Comment("set to add calo lusters"),false};
      fhicl::Atom<bool> addCrvHits{Name("addCrvHits"), Comment("set to add crv hits"),false};	
      fhicl::Atom<bool> addCrystallHits{Name("addCrystalHits"), Comment("for calo cry hits"), false};
      fhicl::Atom<bool> addCosmicSeedFit{Name("addCosmicSeedFit"), Comment("for fitted cosmic track"), false};
      fhicl::Atom<bool> isCosmic{Name("isCosmics"), Comment("for straight cosmics"), false};
      fhicl::Atom<bool> addTrkExtTrajs{Name("addTrkExtTrajs"), Comment("set to add track exit trajectories"), false};
      fhicl::Atom<bool> RecoOnly{Name("RecoOnly"), Comment("set to see only Reco Data Products"), false};
      fhicl::Atom<bool> FillAll{Name("FillAll"), Comment("to see all available products"), false};

      fhicl::Atom<bool> addMCCaloDigis{Name("addMCCaloDigis"), Comment("set to add the MC calo digis"),false};
      fhicl::Atom<bool> addMCHits{Name("addMCHits"), Comment("set to add MC hits"),false};
      fhicl::Atom<bool> addMCHitsSP{Name("addMCHitsSP"), Comment("set to add MC hits sim part"),false};
      fhicl::Atom<bool> addMCCrvCC{Name("addMCCrvCC"), Comment("set to add MC crv coin clust"),false};	
      fhicl::Atom<bool> addMCCrvDigis{Name("addMCCrvDigis"), Comment("set to add MC crv digis"), false};
      fhicl::Atom<bool> addMCTraj{Name("addMCTraj"), Comment("set to add MC trajectories"), false};
      fhicl::Atom<bool> addMCStrawDigis{Name("addMCStrawDigis"), Comment("set to add MC straw digis"), false};
      fhicl::Atom<bool> MCOnly{Name("MCOnly"), Comment("set to see only MC Data Products"), false};
    };

    #ifndef __CINT__

    explicit Collection_Filler(const Config& conf);
    Collection_Filler(const Collection_Filler &);
    Collection_Filler& operator=(const Collection_Filler &);

    //RecoDataProducts: 
    art::InputTag chTag_;
    art::InputTag gensTag_;
    art::InputTag strawdigiTag_;
    art::InputTag crvcoinTag_;
    art::InputTag cosmicTag_;
    art::InputTag cluTag_;
    art::InputTag cryHitTag_;
    art::InputTag hseedTag_;
    art::InputTag kalseedTag_;
    art::InputTag trkexttrajTag_;
    //MCDataProdutcs:
    art::InputTag mcdigisTag_;
    art::InputTag mccHitTag_;
    art::InputTag mccHitSPTag_;
    art::InputTag mccrvcoincluTag_;
    art::InputTag mccrvdigiTag_;
    art::InputTag mctrajTag_;
    art::InputTag mcstrawdigiTag_;

    std::string g4ModuleLabel_;

    art::Event *_event;
    art::Run *_run;

    bool addHits_, addTracks_, addClusters_, addCrvHits_, addCosmicSeedFit_, isCosmic_, addTrkExtTrajs_, RecoOnly_,  FillAll_, addMCCaloDigis_, addMCHits_, addMCHitsSP_, addMCCrvCC_, addMCCrvDigis_, addMCTraj_, addMCStrawDigis_, MCOnly_;

    void FillRecoCollections(const art::Event& evt, Data_Collections &data, RecoDataProductName code);
    void FillMCCollections(const art::Event& evt, Data_Collections &data, MCDataProductName code);

    template<class collection>
    void GetCollection(const art::Event& evt, collection &c, int code);
    //TODO what about using std::any?

    virtual ~Collection_Filler(){};

  private:
    Config _conf;
    #endif
    ClassDef(Collection_Filler,0);
};

}

#endif 
