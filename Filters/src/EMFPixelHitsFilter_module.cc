// $Id: EMFPixelHitsFilter_module.cc,v 1.1 2012/11/01 23:35:45 gandr Exp $
// $Author: gandr $
// $Date: 2012/11/01 23:35:45 $
//
// Andrei Gaponenko, 2012

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

// art includes.
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "RecoDataProducts/inc/ExtMonFNALRawHitCollection.hh"
#include "MCDataProducts/inc/ExtMonFNALHitTruthAssn.hh"

#include "Mu2eUtilities/inc/compressSimParticleCollection.hh"

namespace mu2e {
  namespace ExtMonFNAL {

    // Use art::Ptr instead of bare ptr to get the desired sorting
    typedef std::set<art::Ptr<SimParticle> > TrackSet;

    // Adapter for compressSimParticleCollection()
    class ParticleSelector {
    public:
      ParticleSelector(const TrackSet& m) {
        for(TrackSet::const_iterator i = m.begin(); i!=m.end(); ++i) {
          m_keys.insert((*i)->id());
        }
      }

      bool operator[]( cet::map_vector_key key ) const {
        return m_keys.find(key) != m_keys.end();
      }

    private:
      std::set<cet::map_vector_key> m_keys;
    };

    //================================================================
    class EMFPixelHitsFilter : public art::EDFilter {
      std::string rawHitsModuleLabel_;
      std::string simParticlesModuleLabel_;

      unsigned cutMinHits_;

      unsigned nPassed_;

    public:
      explicit EMFPixelHitsFilter(const fhicl::ParameterSet& pset);
      virtual bool filter(art::Event& event);
      virtual void endJob();
    };

    //================================================================
    EMFPixelHitsFilter::EMFPixelHitsFilter(const fhicl::ParameterSet& pset)
      : rawHitsModuleLabel_(pset.get<std::string>("rawHitsModuleLabel"))
      , simParticlesModuleLabel_(pset.get<std::string>("simParticlesModuleLabel"))
      , cutMinHits_(pset.get<unsigned>("cutMinHits"))
      , nPassed_()
    {
      produces<SimParticleCollection>();
      produces<ExtMonFNALHitTruthAssn>();
    }

    //================================================================
    bool EMFPixelHitsFilter::filter(art::Event& event) {
      std::auto_ptr<SimParticleCollection> outparts(new SimParticleCollection());
      std::auto_ptr<ExtMonFNALHitTruthAssn> outTruth(new ExtMonFNALHitTruthAssn());

      art::Handle<ExtMonFNALRawHitCollection> hitsh;
      event.getByLabel(rawHitsModuleLabel_, hitsh);
      const ExtMonFNALRawHitCollection& hits(*hitsh);

      const bool passed = (cutMinHits_ <= hits.size());
      if(passed) {

        ++nPassed_;

        art::FindManyP<SimParticle,ExtMonFNALHitTruthBits>
          particleFinder(hitsh, event, rawHitsModuleLabel_);

        // Use art::Ptr instead of bare ptr to get the desired sorting
        TrackSet particlesWithHits;
        for(unsigned i=0; i<hits.size(); ++i) {

          const std::vector<art::Ptr<SimParticle> >& particles = particleFinder.at(i);

          std::copy(particles.begin(), particles.end(),
                    std::inserter(particlesWithHits, particlesWithHits.begin()));
        }

        art::Handle<SimParticleCollection> inparts;
        event.getByLabel(simParticlesModuleLabel_, inparts);

        ParticleSelector selector(particlesWithHits);

        art::ProductID newParticlesPID(getProductID<SimParticleCollection>(event));
        const art::EDProductGetter *newParticlesGetter(event.productGetter(newParticlesPID));

        compressSimParticleCollection(newParticlesPID, newParticlesGetter, *inparts, selector, *outparts);

        // Fill outpupt hit<==>truth assns using the new particle collection

        for(unsigned ihit=0; ihit<hits.size(); ++ihit) {

          const std::vector<art::Ptr<SimParticle> >& oldParticles = particleFinder.at(ihit);
          const std::vector<const ExtMonFNALHitTruthBits*>& oldBits = particleFinder.data(ihit);

          for(unsigned ipart=0; ipart<oldParticles.size(); ++ipart) {

            art::Ptr<SimParticle> oldPtr(oldParticles[ipart]);

            outTruth->addSingle(art::Ptr<SimParticle>(newParticlesPID, oldPtr->id().asInt(), newParticlesGetter),
                                art::Ptr<ExtMonFNALRawHit>(hitsh, ihit),
                                *oldBits[ipart]
                                );

          }
        }
      }

      event.put(outparts);
      event.put(outTruth);

      return passed;
    }

    //================================================================
    void EMFPixelHitsFilter::endJob() {
      mf::LogInfo("Summary")
        << "EMFPixelHitsFilter_module: Number of events passing the filter: "
        << nPassed_
        << "\n";
    }

    //================================================================
  } // namespace ExtMonFNAL
} // namespace mu2e

DEFINE_ART_MODULE(mu2e::ExtMonFNAL::EMFPixelHitsFilter);