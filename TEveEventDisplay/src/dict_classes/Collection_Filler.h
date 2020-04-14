
#ifndef Collection_Filler_h
#define Collection_Filler_h
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
#include "RecoDataProducts/inc/CaloCrystalHitCollection.hh"
#include "RecoDataProducts/inc/CaloHitCollection.hh"
#include "RecoDataProducts/inc/CrvCoincidenceClusterCollection.hh"
#include "MCDataProducts/inc/PhysicalVolumeInfoCollection.hh"
#include "MCDataProducts/inc/PhysicalVolumeInfoMultiCollection.hh"
#include "MCDataProducts/inc/MCTrajectoryCollection.hh"
#include "MCDataProducts/inc/SimParticleCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "RecoDataProducts/inc/KalSeed.hh"
#include "RecoDataProducts/inc/StrawHitCollection.hh"
#include "RecoDataProducts/inc/StrawHitFlagCollection.hh"
#include "RecoDataProducts/inc/StrawHitPositionCollection.hh"
#include "RecoDataProducts/inc/StrawDigiCollection.hh"
#include "RecoDataProducts/inc/CrvDigiCollection.hh"
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
#include "RecoDataProducts/inc/KalRepCollection.hh"
#include "RecoDataProducts/inc/TrkExtTrajCollection.hh"
#include "TEveEventDisplay/src/dict_classes/Data_Collections.h"

namespace mu2e{

  enum RecoDataProductName {ComboHits, CaloCrystalHits, CaloClusters, CosmicTracks, HelixSeeds, KalSeeds, CRVCoincidences};

	class Collection_Filler
	{
  public:
    struct Config{
      using Name=fhicl::Name;
      using Comment=fhicl::Comment;
      fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("for info"),0};
      fhicl::Atom<art::InputTag>chTag{Name("ComboHitCollection"),Comment("chTag"), "makePH"};
      fhicl::Atom<art::InputTag>gensTag{Name("GenParticleCollection"),Comment("gensTag")};
      fhicl::Atom<art::InputTag>strawdigiTag{Name("StrawDigiCollection"),Comment("strawdigiTag")};
      fhicl::Atom<art::InputTag>crvdigiTag{Name("CrvCoincidenceClusterCollection"),Comment("crvTag")};
      fhicl::Atom<art::InputTag>cosmicTag{Name("CosmicTrackSeedCollection"),Comment("cosmicTag")};
      fhicl::Atom<art::InputTag>cluTag{Name("CaloClusterCollection"),Comment("cluTag")};
      fhicl::Atom<art::InputTag>cryHitTag{Name("CaloCrystalHitCollection"),Comment("cryHitTag")};
      fhicl::Atom<art::InputTag>hseedTag{Name("HelixSeedCollection"),Comment("hseedTag")};
      fhicl::Atom<art::InputTag>kalseedTag{Name("KalSeedCollection"),Comment("kalseedTag")};
      fhicl::Atom<bool> addHits{Name("addHits"), Comment("set to add the hits"),false};
      fhicl::Atom<bool> addTracks{Name("addTracks"), Comment("set to add tracks"),false};
      fhicl::Atom<bool> addClusters{Name("addClusters"), Comment("set to add calo lusters"),false};
      fhicl::Atom<bool> addCrvHits{Name("addCrvHits"), Comment("set to add crv hits"),false};	
      fhicl::Atom<bool> addCrystallHits{Name("addCrystalHits"), Comment("for calo cry hits"), false};
      fhicl::Atom<bool> addCosmicSeedFit{Name("addCosmicSeedFit"), Comment("for fitted cosmic track"), false};
      fhicl::Atom<bool> addCRV{Name("addCoRV"), Comment("for CRV reco"), false};
      fhicl::Atom<bool> isCosmic{Name("isCosmic"), Comment("flag for cosmic track v helix track"), false};
      fhicl::Atom<bool> MCOnly{Name("MCOnly"), Comment("set to see only MC Data Products"), false};
      fhicl::Atom<bool> FillAll{Name("FillAll"), Comment("to see all available products"), false};		
    };

    #ifndef __CINT__


    explicit Collection_Filler(const Config& conf);
    Collection_Filler(const Collection_Filler &);
    Collection_Filler& operator=(const Collection_Filler &);
    
    //RecoDataProducts: //TODO - maybe remove these if not used!

    art::InputTag chTag_;
    art::InputTag gensTag_;
    art::InputTag strawdigiTag_;
    art::InputTag crvcoinTag_;
    art::InputTag cosmicTag_;
    art::InputTag cluTag_;
    art::InputTag cryHitTag_;
    art::InputTag hseedTag_;
    art::InputTag kalseedTag_;

    //MCDataProdutcs:
    std::string g4ModuleLabel_;
    //TODO

    art::Event *_event;
    art::Run *_run;


    bool addHits_, addTracks_, addClusters_, addCrvHits_, addCosmicSeedFit_, addCRV_, isCosmic_, MCOnly_, FillAll_;

    bool HasCluster(const art::Event& evt);

    bool HasTrack(const art::Event& evt);

    bool HasComboHits(const art::Event& evt);

    void FindData(const art::Event& evt);

    //TODO FindData is a function which just fills all data collections for products which are presentent.
   
    void FillRecoCollection(const art::Event& evt, Data_Collections &data, RecoDataProductName code);
    void FillMCCollections(const art::Event& evt, Data_Collections &data, RecoDataProductName code);

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
