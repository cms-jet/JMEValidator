#include "JMEAnalysis/JMEValidator/interface/MuonAnalyzer.h"

#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

MuonAnalyzer::MuonAnalyzer(const edm::ParameterSet& iConfig): JME::LeptonAnalyzer(iConfig),
    muons_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("src"))),
    vertices_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
    rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rho")))
{
    if (iConfig.existsAs<std::vector<edm::InputTag>>("ids")) {
        const std::vector<edm::InputTag>& ids = iConfig.getParameter<std::vector<edm::InputTag>>("ids");

        for (const edm::InputTag& id: ids) {
            idTokens_.push_back(std::make_pair(id.instance(), consumes<edm::ValueMap<bool>>(id)));
        }
    }

    isoValue_neutralHadrons_pfWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_NH_pfWeighted_R04"));
    isoValue_photons_pfWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_Ph_pfWeighted_R04"));

    isoValue_chargedHadrons_puppiWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_CH_puppiWeighted_R04"));
    isoValue_neutralHadrons_puppiWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_NH_puppiWeighted_R04"));
    isoValue_photons_puppiWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_Ph_puppiWeighted_R04"));

    isoValue_chargedHadrons_puppiNoMuonWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_CH_puppiNoMuonWeighted_R04"));
    isoValue_neutralHadrons_puppiNoMuonWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_NH_puppiNoMuonWeighted_R04"));
    isoValue_photons_puppiNoMuonWeighted_R04_token = consumes<edm::ValueMap<double>>(iConfig.getParameter<edm::InputTag>("isoValue_Ph_puppiNoMuonWeighted_R04"));
}

MuonAnalyzer::~MuonAnalyzer() {
    // Empty
}

void MuonAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

    edm::Handle<pat::MuonCollection> muonsHandle;
    iEvent.getByToken(muons_, muonsHandle);

    edm::Handle<reco::VertexCollection> verticesHandle;
    iEvent.getByToken(vertices_, verticesHandle);
    const auto& primaryVertex = verticesHandle->at(0);

    edm::Handle<double> rhoHandle;
    iEvent.getByToken(rhoToken_, rhoHandle);
    double rho = *rhoHandle;

    edm::Handle<edm::ValueMap<double>> isoValue_neutralHadrons_pfWeighted_R04;
    iEvent.getByToken(isoValue_neutralHadrons_pfWeighted_R04_token, isoValue_neutralHadrons_pfWeighted_R04);
    edm::Handle<edm::ValueMap<double>> isoValue_photons_pfWeighted_R04;
    iEvent.getByToken(isoValue_photons_pfWeighted_R04_token, isoValue_photons_pfWeighted_R04);

    edm::Handle<edm::ValueMap<double>> isoValue_chargedHadrons_puppiWeighted_R04;
    iEvent.getByToken(isoValue_chargedHadrons_puppiWeighted_R04_token, isoValue_chargedHadrons_puppiWeighted_R04);
    edm::Handle<edm::ValueMap<double>> isoValue_neutralHadrons_puppiWeighted_R04;
    iEvent.getByToken(isoValue_neutralHadrons_puppiWeighted_R04_token, isoValue_neutralHadrons_puppiWeighted_R04);
    edm::Handle<edm::ValueMap<double>> isoValue_photons_puppiWeighted_R04;
    iEvent.getByToken(isoValue_photons_puppiWeighted_R04_token, isoValue_photons_puppiWeighted_R04);

    edm::Handle<edm::ValueMap<double>> isoValue_chargedHadrons_puppiNoMuonWeighted_R04;
    iEvent.getByToken(isoValue_chargedHadrons_puppiNoMuonWeighted_R04_token, isoValue_chargedHadrons_puppiNoMuonWeighted_R04);
    edm::Handle<edm::ValueMap<double>> isoValue_neutralHadrons_puppiNoMuonWeighted_R04;
    iEvent.getByToken(isoValue_neutralHadrons_puppiNoMuonWeighted_R04_token, isoValue_neutralHadrons_puppiNoMuonWeighted_R04);
    edm::Handle<edm::ValueMap<double>> isoValue_photons_puppiNoMuonWeighted_R04;
    iEvent.getByToken(isoValue_photons_puppiNoMuonWeighted_R04_token, isoValue_photons_puppiNoMuonWeighted_R04);

    std::vector<std::pair<std::string, edm::Handle<edm::ValueMap<bool>>>> idHandles;
    for (auto& token: idTokens_) {
        edm::Handle<edm::ValueMap<bool>> idHandle;
        iEvent.getByToken(token.second, idHandle);
        idHandles.push_back(std::make_pair(token.first, idHandle));
    }

    // Loop over muons
    size_t index = 0;
    for (const pat::Muon& muon: *muonsHandle) {
        const pat::MuonRef muonRef(muonsHandle, index++);

        extractBasicProperties(muon);
        extractGenProperties(muon.genLepton());

	{
	  const long PDGID_MUON = 13 ;
	  const reco::GenParticle * truth_mu = muon.genLepton();
	  
	  const bool b_matched_to_prompt_muon =
	    ( truth_mu != 0 )
	    &&
	    abs( truth_mu -> pdgId())== PDGID_MUON // just a sanity check.
	    &&
	    truth_mu  -> isPromptFinalState() ;
	  
	  const bool b_matched_to_nonTauprompt_muon =
	    b_matched_to_prompt_muon
	    &&
	    ( ! ( truth_mu -> isDirectPromptTauDecayProductFinalState() ) ) ; 
	  
	  isTruthMatch_PromptMuon_       . push_back ( b_matched_to_prompt_muon );
	  isTruthMatch_NonTauPromptMuon_ . push_back ( b_matched_to_nonTauprompt_muon );
	}


        reco::MuonPFIsolation pfIso = muon.pfIsolationR03();
        computeRelativeIsolationR03(muon, pfIso.sumChargedHadronPt, pfIso.sumNeutralHadronEt, pfIso.sumPhotonEt, pfIso.sumPUPt, muon.eta(), rho);

        pfIso = muon.pfIsolationR04();
        computeRelativeIsolationR04(muon, pfIso.sumChargedHadronPt, pfIso.sumNeutralHadronEt, pfIso.sumPhotonEt, pfIso.sumPUPt, muon.eta(), rho);

        neutralHadronIsoR04_pfWeighted_.push_back((*isoValue_neutralHadrons_pfWeighted_R04)[muonRef]);
        photonIsoR04_pfWeighted_.push_back((*isoValue_photons_pfWeighted_R04)[muonRef]);

        relativeIsoR04_pfWeighted_.push_back((pfIso.sumChargedHadronPt + (*isoValue_neutralHadrons_pfWeighted_R04)[muonRef] + (*isoValue_photons_pfWeighted_R04)[muonRef]) / muon.pt());

        chargedHadronIsoR04_puppiWeighted_.push_back((*isoValue_chargedHadrons_puppiWeighted_R04)[muonRef]);
        neutralHadronIsoR04_puppiWeighted_.push_back((*isoValue_neutralHadrons_puppiWeighted_R04)[muonRef]);
        photonIsoR04_puppiWeighted_.push_back((*isoValue_photons_puppiWeighted_R04)[muonRef]);

        relativeIsoR04_puppiWeighted_.push_back(((*isoValue_chargedHadrons_puppiWeighted_R04)[muonRef] + (*isoValue_neutralHadrons_puppiWeighted_R04)[muonRef] + (*isoValue_photons_puppiWeighted_R04)[muonRef]) / muon.pt());

        chargedHadronIsoR04_puppiNoMuonWeighted_.push_back((*isoValue_chargedHadrons_puppiNoMuonWeighted_R04)[muonRef]);
        neutralHadronIsoR04_puppiNoMuonWeighted_.push_back((*isoValue_neutralHadrons_puppiNoMuonWeighted_R04)[muonRef]);
        photonIsoR04_puppiNoMuonWeighted_.push_back((*isoValue_photons_puppiNoMuonWeighted_R04)[muonRef]);

        relativeIsoR04_puppiNoMuonWeighted_.push_back(((*isoValue_chargedHadrons_puppiNoMuonWeighted_R04)[muonRef] + (*isoValue_neutralHadrons_puppiNoMuonWeighted_R04)[muonRef] + (*isoValue_photons_puppiNoMuonWeighted_R04)[muonRef]) / muon.pt());

        isLoose_.push_back(muon::isLooseMuon(muon));
        isMedium_.push_back(muon::isMediumMuon(muon));
        isSoft_.push_back(muon::isSoftMuon(muon, primaryVertex));
        isTight_.push_back(muon::isTightMuon(muon, primaryVertex));
        isHighPt_.push_back(muon::isHighPtMuon(muon, primaryVertex));

        // IDs
        std::map<std::string, bool> ids;
        for (auto& idHandle: idHandles) {
            ids[idHandle.first] = (*(idHandle.second))[muonRef];
        }
        ids_.push_back(ids);

    }

    tree.fill();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MuonAnalyzer);
