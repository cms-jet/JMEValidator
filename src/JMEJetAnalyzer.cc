////////////////////////////////////////////////////////////////////////////////
//
// JMEJetAnalyzer
// ------------------
//
//                        01/07/2014 Alexx Perloff   <aperloff@physics.tamu.edu>
//                        04/2015    Sébastien Brochet <sebastien.brochet@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

#include "JMEAnalysis/JMEValidator/interface/JMEJetAnalyzer.h"

#include <vector>
#include <iostream>
#include <regex>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JMEJetAnalyzer::JMEJetAnalyzer(const edm::ParameterSet& iConfig)
  : JME::PhysicsObjectAnalyzer(iConfig)
  , JetCorLabel_   (iConfig.getParameter<std::string>("JetCorLabel"))
  , JetCorLevels_  (iConfig.getParameter<std::vector<std::string>>("JetCorLevels"))
  , srcJet_        (consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("srcJet")))
  , srcGenJet_        (consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("srcGenJet")))
  , srcVtx_        (consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("srcVtx")))
  , srcMuons_      (consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("srcMuons")))
  , doComposition_ (iConfig.getParameter<bool>("doComposition"))
  , doFlavor_      (iConfig.getParameter<bool>("doFlavor"))
  , nJetMax_       (iConfig.getParameter<unsigned int>("nJetMax"))
  , deltaRMax_(0.0)
  , deltaPhiMin_(3.141)
  , deltaRPartonMax_(0.0)
  , jetCorrector_(0)
  , srcGenJets_    (consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genjets")))
{
  if (iConfig.exists("deltaRMax")) {
    deltaRMax_=iConfig.getParameter<double>("deltaRMax");
  }
  else
    throw cms::Exception("MissingParameter")<<"Set *either* deltaRMax (matching)"
					    <<" *or* deltaPhiMin (balancing)";

  // Jet CORRECTOR
  // if(!JetCorLevels_.empty()) {
  //   vector<JetCorrectorParameters> vPar;
  //   string jetCorPar = "../data/PHYS14_V2_MC_L1FastJet_"+JetCorLabel_.substr(0,JetCorLabel_.size()-2)+".txt";
  //   cout << "Getting JEC from file " << jetCorPar  << " ... ";
  //   vPar.push_back(JetCorrectorParameters(jetCorPar));
  //   jetCorrector_ = new FactorizedJetCorrector(vPar);
  //   cout << "DONE" << endl;
  // }
  
  std::cout << "|---- JMEJetAnalyzer: Initialyzing..." << std::endl;
  std::cout << "|---- JMEJetAnalyzer: Applying these jet corrections: ( " << JetCorLabel_;
  for (const std::string& level: JetCorLevels_)
     std::cout << ", " << level;
  std::cout << " )" << std::endl;

  std::cout << "|---- JMEJetAnalyzer: RUNNING ON " << moduleLabel_ << " FOR "
       << JetCorLabel_.substr(0,3) << " JETS";
  if      (JetCorLabel_.find("chs") != std::string::npos)   std::cout << " USING CHS" << std::endl;
  else if (JetCorLabel_.find("PUPPI") != std::string::npos) std::cout << " USING PUPPI" << std::endl;
  else                                                      std::cout << std::endl;
}


//______________________________________________________________________________
JMEJetAnalyzer::~JMEJetAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________

//______________________________________________________________________________
void JMEJetAnalyzer::analyze(const edm::Event& iEvent,
                                  const edm::EventSetup& iSetup)
{

  // // EVENT DATA HANDLES
  edm::Handle<reco::CandidateView>               refs;
  edm::Handle<std::vector<pat::Jet> >            jets;
  edm::Handle<std::vector<reco::Vertex>>         vtx;
  edm::Handle<edm::View<pat::Muon> >             muons;
  edm::Handle<reco::GenJetCollection>            genjets;

  iEvent.getByToken(srcVtx_, vtx);

  // REFERENCES & RECOJETS
  iEvent.getByToken(srcJet_, jets);
  bool has_gen_jets = iEvent.getByToken(srcGenJet_, genjets);

  //loop over the jets and fill the ntuple
  size_t nJet = (nJetMax_ == 0) ? jets->size() : std::min(nJetMax_, (unsigned int) jets->size());
  for (size_t iJet = 0; iJet < nJet; iJet++) {

     pat::Jet const & jet = jets->at(iJet);
     if (jet.pt() < 5)
         continue;
     
     extractBasicProperties(jet);

     const reco::GenJet* ref = jet.genJet();
     if (ref) {
         refdrjt.push_back(reco::deltaR(jet, *ref));
         refpdgid.push_back(ref->pdgId());
         refarea.push_back(ref->jetArea());
     } else {
         refdrjt.push_back(0);
         refpdgid.push_back(0.);
         refarea.push_back(0.);
     }

     extractGenProperties(ref);

     // New jet flavor informations
     // See https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools
     partonFlavor.push_back(jet.partonFlavour());
     hadronFlavor.push_back(jet.hadronFlavour());

     // b-tagging discriminators
     // Create one branch per discriminators for disk-space reasons (more than 50% smaller)
     for (const auto& btag: jet.getPairDiscri()) {
         std::vector<float>& discr = tree[btag.first].write<std::vector<float>>();
         discr.push_back(btag.second);
     }

     // PU Jet Id
     for (const std::string& userFloatName: jet.userFloatNames()) {
        // Look for a string starting with 'pileupJetIdEvaluator'
        if (userFloatName.find("pileupJetIdEvaluator") == 0) {
            pujetid_fulldiscriminant.push_back(jet.userFloat(userFloatName));
            continue;
        }

        if (userFloatName.find("QGTagger") == 0)
            qg_tagger.push_back(jet.userFloat(userFloatName));
     }

     for (const std::string& userIntName: jet.userIntNames()) {
         static std::regex cutbasedIdRegex("pileupJetIdEvaluator(.*):cutbasedId");
         static std::regex fullIdRegex("pileupJetIdEvaluator(.*):fullId");

         if (std::regex_match(userIntName, cutbasedIdRegex)) {
             pujetid_cutbasedid.push_back(jet.userInt(userIntName));
             continue;
         }

         if (std::regex_match(userIntName, fullIdRegex))
             pujetid_fullid.push_back(jet.userInt(userIntName));
     }


     jtarea.push_back(jet.jetArea());

     jec_toraw.push_back(jet.jecSetsAvailable() ? jet.jecFactor(0) : 1);
     if (jet.jecSetsAvailable()) {
         std::map<std::string, float> factors;
         for (const auto& level: jet.availableJECLevels()) {
             factors.emplace(level, jet.jecFactor(level));
         }
         jec_factors.push_back(factors);
     }
     
     if (has_gen_jets) {
         float dRmin(1000);
         for(reco::GenJetCollection::const_iterator igen = genjets->begin();igen != genjets->end(); ++igen){
             float dR = deltaR(jet.eta(),jet.phi(),igen->eta(),igen->phi());
             if (dR < dRmin) {
                 dRmin = dR;
             }
         }
         dRMatch.push_back(dRmin);
     }

     chargedEmEnergyFraction.push_back(jet.chargedEmEnergyFraction());
     chargedHadronEnergyFraction.push_back(jet.chargedHadronEnergyFraction());
     chargedMuEnergyFraction.push_back(jet.chargedMuEnergyFraction());
     electronEnergyFraction.push_back(jet.electronEnergyFraction());
     HFEMEnergyFraction.push_back(jet.HFEMEnergyFraction());
     HFHadronEnergyFraction.push_back(jet.HFHadronEnergyFraction());
     hoEnergyFraction.push_back(jet.hoEnergyFraction());
     muonEnergyFraction.push_back(jet.muonEnergyFraction());
     neutralEmEnergyFraction.push_back(jet.neutralEmEnergyFraction());
     neutralHadronEnergyFraction.push_back(jet.neutralHadronEnergyFraction());
     photonEnergyFraction.push_back(jet.photonEnergyFraction());
     chargedMultiplicity.push_back(jet.chargedMultiplicity());

     // Calo jet specific
     //emEnergyFraction.push_back(jet.emEnergyFraction());
     //emEnergyInEB.push_back(jet.emEnergyInEB());
     //emEnergyInEE.push_back(jet.emEnergyInEE());
     //emEnergyInHF.push_back(jet.emEnergyInHF());
     //energyFractionHadronic.push_back(jet.energyFractionHadronic());

     computeBetaStar(jet, *vtx);
  }

  if (has_gen_jets) {
      for (size_t iGenJet = 0; iGenJet < genjets -> size(); iGenJet++) {

          const reco::GenJet & genjet = genjets->at(iGenJet)  ;

          if( genjet.pt () < 5 ) continue ;

          bool b_genjet_hasMatchedRecoJet = false ;
          bool b_genjet_hasMatchedRecoJetWithJetID = false ;

          for (size_t iJet = 0; iJet < nJet; iJet++) {

              pat::Jet const & jet = jets->at(iJet);
              if( jet.pt() < 5 ){ continue; }

              const reco::GenJet* matched_ref_jet = jet.genJet();
              if( ! matched_ref_jet ) {  continue ; }

              if( genjet.pt()  != matched_ref_jet->pt() ) { continue ;}
              if( genjet.eta() != matched_ref_jet->eta() ){ continue ;}
              if( genjet.phi() != matched_ref_jet->phi() ){ continue ;}

              b_genjet_hasMatchedRecoJet = true ;

              if( ! ( jet.neutralEmEnergyFraction()     < 0.99 ) )  continue  ; 
              if( ! ( jet.chargedHadronEnergyFraction() > 0.0  ) )  continue  ; 
              if( ! ( jet.neutralHadronEnergyFraction() < 0.99 ) )  continue  ; 
              if( ! ( jet.chargedEmEnergyFraction()     < 0.99 ) )  continue  ; 
              if( ! ( jet.numberOfDaughters()           > 1    ) )  continue  ; 
              if( ! ( jet.chargedMultiplicity()         > 0    ) )  continue  ; 

              b_genjet_hasMatchedRecoJetWithJetID = true ;

          }   

          allGenJet_pt  .push_back( genjet.pt  () );
          allGenJet_eta .push_back( genjet.eta () );
          allGenJet_phi .push_back( genjet.phi () );
          allGenJet_m   .push_back( genjet.mass() );
          allGenJet_PatJetMatched  .push_back( b_genjet_hasMatchedRecoJet ) ;
          allGenJet_PatJetWithJetIDMatched  .push_back( b_genjet_hasMatchedRecoJetWithJetID ) ;

      }
  }

  tree.fill();
}

void JMEJetAnalyzer::computeBetaStar(const pat::Jet& jet, const std::vector<reco::Vertex>& vertices) {

    int nCh_tmp(0), nNeutrals_tmp(0);
    float sumTkPt(0.0);
    float beta_tmp(0.0), betaStar_tmp(0.0), betaStarClassic_tmp(0.0), betaClassic_tmp(0.0);
    float pTMax(0.0), pTMaxChg(0.0),pTMaxNeutral(0.0), dZ2(-999),d02(-999);
    float sumW(0.0), sumW2(0.0), sumWdR2(0.0);
    float sum_deta(0.0),sum_dphi(0.0),sum_deta2(0.0),sum_dphi2(0.0),sum_detadphi(0.0),Teta(0.0),Tphi(0.0);
    float ave_deta(0.0), ave_dphi(0.0);
    float axis1(-1.0),axis2(-1.0);
    float Ttheta_tmp(-1.0);
    float pull_tmp(0.0);

    const size_t nRings = 9;
    float sum_rings[nRings] = {0};

    for (size_t j = 0; j < jet.numberOfDaughters(); j++) {
        const auto& part = jet.daughterPtr(j);
        if (!(part.isAvailable() && part.isNonnull()) ){
            continue;
        }

        if (fabs(part->charge()) > 0)
            nCh_tmp++;
        else 
            nNeutrals_tmp++;

        float dR = reco::deltaR(*part, jet);
        float weight = part->pt();
        float weight2 = weight * weight;
        sumWdR2      += weight2 * dR * dR;
        sumW         += weight;
        sumW2        += weight2;

        float deta = part->eta() - jet.eta();
        //float dphi = 2*atan(tan((part->phi()-jet.phi())/2));
        float dphi = reco::deltaPhi(*part, jet);
        sum_deta     += deta*weight2;
        sum_dphi     += dphi*weight2;
        sum_deta2    += deta*deta*weight2;
        sum_detadphi += deta*dphi*weight2;
        sum_dphi2    += dphi*dphi*weight2;
        Teta         += weight * dR * deta;
        Tphi         += weight * dR * dphi;

        size_t index = (int) (dR * 10);
        if (index > nRings - 1)
            index = nRings - 1;
        sum_rings[index] += weight;

        if (part->pt() > pTMax) pTMax = part->pt();

        reco::CandidatePtr pfJetConstituent = jet.sourceCandidatePtr(j);
        const reco::Candidate* icand = pfJetConstituent.get();
        const pat::PackedCandidate* lPack = dynamic_cast<const pat::PackedCandidate *>( icand );
        if (lPack) {

            if (fabs(lPack->charge()) > 0) {

                if (lPack->pt() > pTMaxChg) {
                    pTMaxChg = lPack->pt();
                    dZ2 = lPack->dz();
                    d02 = lPack->dxy();
                }

                float tkpt = lPack->pt();
                sumTkPt += tkpt;
                bool inVtx0 = (lPack->fromPV() == 3);
                bool inAnyOther = (lPack->fromPV() == 0);
                double dZ0 = lPack->dz();
                double dZ_tmp = dZ0;

                for (const auto& iv: vertices) {
                    if (iv.isFake())
                        continue;
                    if (fabs(lPack->dz(iv.position())) < fabs(dZ_tmp)) {
                        dZ_tmp = lPack->dz(iv.position());
                    }
                }

                if (inVtx0) {
                    betaClassic_tmp += tkpt;
                }
                else if (inAnyOther) {
                    betaStarClassic_tmp += tkpt;
                }
                if (fabs(dZ0) < 0.2) {
                    beta_tmp += tkpt;
                }
                else if (fabs(dZ_tmp) < 0.2) {
                    betaStar_tmp += tkpt;
                }
            }

            else if (part->pt() > pTMaxNeutral)
            {
            	pTMaxNeutral = part->pt();
            }
        }
    }

    if (sumW > 0) {
        DRweighted.push_back(sumWdR2 / sumW2);
        fRing0.push_back(sum_rings[0] / sumW);
        fRing1.push_back(sum_rings[1] / sumW);
        fRing2.push_back(sum_rings[2] / sumW);
        fRing3.push_back(sum_rings[3] / sumW);
        fRing4.push_back(sum_rings[4] / sumW);
        fRing5.push_back(sum_rings[5] / sumW);
        fRing6.push_back(sum_rings[6] / sumW);
        fRing7.push_back(sum_rings[7] / sumW);
        fRing8.push_back(sum_rings[8] / sumW);
        ptD.push_back(sqrt(sumW2) / sumW);
        jetRneutral.push_back(pTMaxNeutral/sumW);
        jetRchg.push_back(pTMaxChg/sumW);
        jetR.push_back(pTMax/sumW);
        Teta = Teta/sumW;
        Tphi = Tphi/sumW;
        if (Teta != 0 && Tphi !=0 ) {
        	Ttheta_tmp = atan2(Tphi,Teta);
        }
        ave_deta = sum_deta/sumW2;
        ave_dphi = sum_dphi/sumW2;
        float ave_deta2 = sum_deta2/sumW2;
        float ave_dphi2 = sum_dphi2/sumW2;
        float a = ave_deta2-ave_deta*ave_deta;
        float b = ave_dphi2-ave_dphi*ave_dphi;
        float c = -(sum_detadphi/sumW2-ave_deta*ave_dphi);
        float delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
        if (a+b+delta > 0) {
        	axis1 = sqrt(0.5*(a+b+delta));
        }
        if (a+b-delta > 0) {
        	axis2 = sqrt(0.5*(a+b-delta));
        }
    }
    else{
        DRweighted.push_back(-999);
        fRing0.push_back(-999);
        fRing1.push_back(-999);
        fRing2.push_back(-999);
        fRing3.push_back(-999);
        fRing4.push_back(-999);
        fRing5.push_back(-999);
        fRing6.push_back(-999);
        fRing7.push_back(-999);
        fRing8.push_back(-999);
        ptD.push_back(-999);
        jetRneutral.push_back(-999);
        jetRchg.push_back(-999);
        jetR.push_back(-999);
    }
    if (sumTkPt > 0) {
        beta.push_back(beta_tmp/sumTkPt);
        betaStar.push_back(betaStar_tmp/sumTkPt);
        betaClassic.push_back(betaClassic_tmp/sumTkPt);
        betaStarClassic.push_back(betaStarClassic_tmp/sumTkPt);
    }
    else{
        beta.push_back(-999);
        betaStar.push_back(-999);
        betaClassic.push_back(-999);
        betaStarClassic.push_back(-999);
    }

    dZ.push_back(dZ2);
    d0.push_back(d02);
    nCh.push_back(nCh_tmp);
    nNeutrals.push_back(nNeutrals_tmp);
    axisMajor.push_back(axis1);
    axisMinor.push_back(axis2);
    Ttheta.push_back(Ttheta_tmp);
    nTot.push_back(jet.numberOfDaughters());
    
    float ddetaR_sum(0.0), ddphiR_sum(0.0);
    for (size_t i = 0; i < jet.numberOfDaughters(); i++) {
        const auto& part = jet.daughterPtr(i);
        float weight =part->pt()*part->pt();
        float deta = part->eta() - jet.eta();
        float dphi = reco::deltaPhi(*part, jet);
        float ddeta, ddphi, ddR;
        ddeta = deta - ave_deta ;
        ddphi = 2*atan(tan((dphi - ave_dphi)/2.)) ;
        ddR = sqrt(ddeta*ddeta + ddphi*ddphi);
        ddetaR_sum += ddR*ddeta*weight;
        ddphiR_sum += ddR*ddphi*weight;
    }
    if (sumW2 > 0) {
        float ddetaR_ave = ddetaR_sum/sumW2;
        float ddphiR_ave = ddphiR_sum/sumW2;
        pull_tmp = sqrt(ddetaR_ave*ddetaR_ave+ddphiR_ave*ddphiR_ave);
    }
    pull.push_back(pull_tmp);
}


////////////////////////////////////////////////////////////////////////////////
// define JMEJetAnalyzer as a plugin
////////////////////////////////////////////////////////////////////////////////
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(JMEJetAnalyzer);
