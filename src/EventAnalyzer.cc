#include "JMEAnalysis/JMEValidator/interface/EventAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

EventAnalyzer::EventAnalyzer(const edm::ParameterSet& iConfig): JME::Analyzer(iConfig),
    rhoToken_        (consumes<double>(iConfig.getParameter<edm::InputTag>("rho"))),
    rhosToken_       (consumes<std::vector<double>>(iConfig.getParameter<edm::InputTag>("rhos"))),
    puInfoToken_     (consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("slimmedAddPileupInfo"))),
    genInfoToken_    (consumes<GenEventInfoProduct>(edm::InputTag("generator"))),
    verticesToken_   (consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertices")))
{
    // Empty
}

EventAnalyzer::~EventAnalyzer() {
    // Empty
}

void EventAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  edm::Handle<GenEventInfoProduct>               genInfo;
  edm::Handle<std::vector<PileupSummaryInfo>>    puInfos;  
  edm::Handle<double>                            rho;
  edm::Handle<std::vector<double>>               rhos;
  edm::Handle<std::vector<reco::Vertex>>         vtx;

  // GENERATOR INFORMATION
  pthat_  = 0.0;
  weight_ = 1.0;
  if (iEvent.getByToken(genInfoToken_, genInfo)) {
    if (genInfo->hasBinningValues())
        pthat_ = genInfo->binningValues()[0];

    weight_ = genInfo->weight();

    nMEPartons_ = genInfo->nMEPartons();
    nMEPartonsFiltered_ = genInfo->nMEPartonsFiltered();

    alphaQCD_ = genInfo->alphaQCD();
    alphaQED_ = genInfo->alphaQED();
    qScale_ = genInfo->qScale();

    if (genInfo->hasPDF()) {
        pdfID_ = genInfo->pdf()->id;
        pdfX_.first = genInfo->pdf()->x.first;
        pdfX_.second = genInfo->pdf()->x.second;
    }
  }

  //RHO INFORMATION
  rho_ = 0;
  if (iEvent.getByToken(rhoToken_, rho)) {
    rho_ = *rho;
  }

  //ETA DEPENDENT RHO INFORMATION
  if (iEvent.getByToken(rhosToken_, rhos)) {
    for (const double& r: *rhos)
      rhos_.push_back(r);
  }
 
  //NPV INFORMATION
  npv = 0;
  if (iEvent.getByToken(verticesToken_, vtx)) {
     const reco::VertexCollection::const_iterator vtxEnd = vtx->end();
     for (reco::VertexCollection::const_iterator vtxIter = vtx->begin(); vtxEnd != vtxIter; ++vtxIter) {
        if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24)
           npv++;
     }
  }
 
  //EVENT INFORMATION
  run = iEvent.id().run();
  lumiBlock = iEvent.id().luminosityBlock();
  event = iEvent.id().event();

  // MC PILEUP INFORMATION
  nTrueInt=-999;
  npuIT = 0;
  npuOOT = 0;
  int InTimePileUpInfo_index=-999;
  bool isInTimePileUpInfo=false;
  unsigned int i=0;
  if (iEvent.getByToken(puInfoToken_, puInfos)) {
     for (const auto& pu: *puInfos) {
        npus.push_back(pu.getPU_NumInteractions());
        tnpus.push_back(pu.getTrueNumInteractions());
        bxns.push_back(pu.getBunchCrossing());
        bunchSpacings_.push_back(pu.getBunchSpacing());

        float sumptlowpt = 0;
        float sumpthighpt = 0;
        int ntrkslowpt = 0;
        int ntrkshighpt = 0;
        for (const float& pt: pu.getPU_sumpT_lowpT())
          sumptlowpt += pt;

        for (const float& pt: pu.getPU_sumpT_highpT())
          sumpthighpt += pt;

        for (const int& pt: pu.getPU_ntrks_lowpT())
          ntrkslowpt += pt;

        for (const int& pt: pu.getPU_ntrks_highpT())
          ntrkshighpt += pt;
        
        sumpt_lowpt_.push_back(sumptlowpt);
        sumpt_highpt_.push_back(sumpthighpt);
        ntrks_lowpt_.push_back(ntrkslowpt);
        ntrks_highpt_.push_back(ntrkshighpt);

        pu_zpositions_.push_back(pu.getPU_zpositions());

       if(pu.getBunchCrossing()==0){
        	npuIT = pu.getPU_NumInteractions();
        	InTimePileUpInfo_index=i;
        	isInTimePileUpInfo = true;
        }
        else{
        	npuOOT += pu.getPU_NumInteractions();
        }
		i++;
     }
     if(!isInTimePileUpInfo){
         edm::LogError("NoInTimePileUpInfo") << "Cannot find the in-time pileup info ";
     }
     else{
         nTrueInt=(*puInfos)[InTimePileUpInfo_index].getPU_NumInteractions();
     }
  }

  tree.fill();
}
