
#ifndef Collection_Filler_h
#define Collection_Filler_h
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
#include "RecoDataProducts/inc/CaloCrystalHitCollection.hh"
#include "RecoDataProducts/inc/CaloHitCollection.hh"
#include "RecoDataProducts/inc/CrvRecoPulseCollection.hh"
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



namespace mu2e{
	class Collection_Filler
	{
	   public:
      struct Config{
			  using Name=fhicl::Name;
			  using Comment=fhicl::Comment;
			  fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("for info"),0};
			  fhicl::Atom<art::InputTag>chTag{Name("ComboHitCollection"),Comment("chTag")};
			  fhicl::Atom<art::InputTag>gensTag{Name("GenParticleCollection"),Comment("gensTag")};
			  fhicl::Atom<art::InputTag>strawdigiTag{Name("StrawDigiCollection"),Comment("strawdigiTag")};
			  fhicl::Atom<art::InputTag>crvdigiTag{Name("CrvDigiCollection"),Comment("crvTag")};
			  fhicl::Atom<art::InputTag>cosmicTag{Name("CosmicTrackSeedCollection"),Comment("cosmicTag")};
			  fhicl::Atom<art::InputTag>cluTag{Name("CaloClusterCollection"),Comment("cluTag")};
        fhicl::Atom<art::InputTag>cryHitTag{Name("CaloCrystalHitCollection"),Comment("cryHitTag")};
			  fhicl::Atom<bool> addHits{Name("addHits"), Comment("set to add the hits"),false};
			  fhicl::Atom<bool> addTracks{Name("addTracks"), Comment("set to add tracks"),false};
			  fhicl::Atom<bool> addClusters{Name("addClusters"), Comment("set to add calo lusters"),false};
			  fhicl::Atom<bool> addCrvHits{Name("addCrvHits"), Comment("set to add crv hits"),false};	
        fhicl::Atom<bool> addCrystallHits{Name("addCrystalHits"), Comment("for calo cry hits"), false};
			  fhicl::Atom<bool> addCosmicSeedFit{Name("addCosmicSeedFit"), Comment("for fitted cosmic track"), false};
			  fhicl::Atom<bool> isCosmic{Name("isCosmic"), Comment("flag for cosmic track v helix track"), false};	
	    };

			#ifndef __CINT__

  
			explicit Collection_Filler(const Config& conf);
			Collection_Filler(const Collection_Filler &);
			Collection_Filler& operator=(const Collection_Filler &);
      const ComboHitCollection *chcol;
			const StrawDigiCollection* _stcol;
	    const ComboHitCollection* _chcol;
	    const StrawDigiCollection* _strawdigicol;
	    const CrvDigiCollection* _crvdigicol;
	    const CosmicTrackSeedCollection* _cosmiccol;
	    const GenParticleCollection* _gencol;
	    const CaloClusterCollection* _clustercol;
      const CaloCrystalHitCollection* _cryHitcol;
	    art::InputTag chTag_;
	    art::InputTag gensTag_;
	    art::InputTag strawdigiTag_;
	    art::InputTag crvdigiTag_;
	    art::InputTag cosmicTag_;
	    art::InputTag cluTag_;
      art::InputTag cryHitTrag_;
	    std::string g4ModuleLabel_;
			art::Event *_event;
			art::Run *_run;

			bool addHits_, addTracks_, addClusters_, addCrvHits_, addCosmicSeedFit_, isCosmic_;

			bool HasCluster(const art::Event& evt);

      bool HasTrack(const art::Event& evt);

      bool HasComboHits(const art::Event& evt);

      void FindData(const art::Event& evt);

      
      template<class collection>
      collection GetCollection(std::string name);

     //std::any

		  virtual ~Collection_Filler(){};

    private:
      Config _conf;
		  #endif
		  ClassDef(Collection_Filler,0);
	};

}

#endif 
