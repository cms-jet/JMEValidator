#pragma once

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "JMEAnalysis/JMEValidator/interface/Analyzer.h"

#include <Math/Vector4D.h>


namespace JME {

    class PhysicsObjectAnalyzer : public Analyzer {
        public:

            typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float>> PtEtaPhiEVector;

            // construction/destruction
            explicit PhysicsObjectAnalyzer(const edm::ParameterSet& iConfig)
                : Analyzer(iConfig) {
                    // Empty;
                }

            virtual ~PhysicsObjectAnalyzer() {
                // Empty
            }

        protected:

            template<typename T>
                void extractBasicProperties(const T& object) {
                    PtEtaPhiEVector p(object.pt(), object.eta(), object.phi(), object.energy());
                    p4.push_back(p);

                    y.push_back(object.rapidity());
                    charge.push_back(object.charge());
                }

            template<typename T>
                void extractGenProperties(const T* genObject_) {
                    if (genObject_) {
                        const T& genObject = *genObject_;
                        is_matched.push_back(true);
                        PtEtaPhiEVector p(genObject.pt(), genObject.eta(), genObject.phi(), genObject.energy());
                        gen_p4.push_back(p);

                        gen_y.push_back(genObject.rapidity());
                        gen_charge.push_back(genObject.charge());
                    } else {
                        is_matched.push_back(false);
                        PtEtaPhiEVector p;
                        gen_p4.push_back(p);

                        gen_y.push_back(0);
                        gen_charge.push_back(0);
                    }
                }


        protected:
            std::vector<PtEtaPhiEVector>& p4 = tree["p4"].write<std::vector<PtEtaPhiEVector>>();
            std::vector<float>& y = tree["y"].write<std::vector<float>>();
            std::vector<float>& charge = tree["charge"].write<std::vector<float>>();

            std::vector<bool>& is_matched = tree["has_gen_particle"].write<std::vector<bool>>();
            std::vector<PtEtaPhiEVector>& gen_p4 = tree["gen_p4"].write<std::vector<PtEtaPhiEVector>>();
            std::vector<float>& gen_y = tree["gen_y"].write<std::vector<float>>();
            std::vector<float>& gen_charge = tree["gen_charge"].write<std::vector<float>>();
    };
}
