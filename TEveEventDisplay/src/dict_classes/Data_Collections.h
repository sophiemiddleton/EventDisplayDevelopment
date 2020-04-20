
#ifndef Data_Collections_h
#define Data_Collections_h
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


namespace mu2e{
	class Data_Collections
	{
  public:
    

    #ifndef __CINT__

    explicit Data_Collections(){};
    Data_Collections(const Data_Collections &){};
    Data_Collections& operator=(const Data_Collections &);

    //RecoDataProducts:

    const ComboHitCollection *chcol = 0;
    const CrvCoincidenceClusterCollection* crvcoincol = 0;
    const CosmicTrackSeedCollection* cosmiccol = 0;
    const GenParticleCollection* gencol = 0;
    const CaloClusterCollection* clustercol = 0;
    const CaloCrystalHitCollection* cryHitcol = 0;
    const HelixSeedCollection* hseedcol = 0;
    const KalSeedCollection* kalseedcol = 0;

    //MCDataProducts:
    //TODO

    virtual ~Data_Collections(){};
    #endif
    ClassDef(Data_Collections,0);
	};

}

#endif 