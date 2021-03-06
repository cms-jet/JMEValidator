import FWCore.ParameterSet.Config as cms
 
def runMuonIsolation(process):
	
	process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
	process.load("Configuration.EventContent.EventContent_cff")
	process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
	process.load('Configuration.StandardSequences.MagneticField_38T_cff')
	process.GlobalTag.globaltag = "MCRUN2_74_V7::All"
	
	### load default PAT sequence
	process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
	process.load("PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff")
	process.load("JMEAnalysis.JMEValidator.convertPackedCandToRecoCand_cff")
	
	process.packedPFCandidatesWoMuon  = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV>=2 && abs(pdgId)!=13 " ) )
	convertedPackedPFCandidatesWoMuon = cms.EDProducer('convertCandToRecoCand',
							   src = cms.InputTag('packedPFCandidatesWoMuon')
							   )
	
	setattr( process, 'convertedPackedPFCandidatesWoMuon', convertedPackedPFCandidatesWoMuon )

	# presequences needed for PUPPI and PF-Weighting
	process.patseq = cms.Sequence(process.convertedPackedPFCandidates *
				      process.convertedPackedPFCandidatesWoMuon *
				      process.pfParticleSelectionForIsoSequence *
				      process.selectedPatMuons
				      )

	process.p = cms.Path(process.patseq)
	
	# # change the input collections
	process.particleFlowPtrs.src = 'convertedPackedPFCandidates'
	process.pfPileUpIsoPFBRECO.Vertices = 'offlineSlimmedPrimaryVertices'
	process.pfPileUpPFBRECO.Vertices    = 'offlineSlimmedPrimaryVertices'
	
	### muon selection
	process.selectedPatMuons.src = 'slimmedMuons'
	process.selectedPatMuons.cut = 'pt>10 && abs(eta)<2.4'
	
	# load user-defined particle collections (e.g. PUPPI)
	
	# -- PF-Weighted
	process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')
	process.pfWeightedPhotons.src = 'pfAllPhotonsPFBRECO'
	process.pfWeightedPhotons.chargedFromPV = 'pfAllChargedParticlesPFBRECO'
	process.pfWeightedPhotons.chargedFromPU = 'pfPileUpAllChargedParticlesPFBRECO'
	process.pfWeightedNeutralHadrons.src = 'pfAllNeutralHadronsPFBRECO'
	process.pfWeightedNeutralHadrons.chargedFromPV = 'pfAllChargedParticlesPFBRECO'
	process.pfWeightedNeutralHadrons.chargedFromPU = 'pfPileUpAllChargedParticlesPFBRECO'

	# -- PUPPI
	from JMEAnalysis.JMEValidator.pfPUPPISequence_cff import *
	load_pfPUPPI_sequence(process, 'pfPUPPISequence', algo = 'PUPPI',
	  src_puppi = 'pfAllHadronsAndPhotonsForPUPPI',
	  cone_puppi_central = 0.5
	)
	
	# change the input collections
	process.pfAllHadronsAndPhotonsForPUPPI.src = 'convertedPackedPFCandidates'
	process.particleFlowPUPPI.candName = 'packedPFCandidates'
	process.particleFlowPUPPI.vertexName = 'offlineSlimmedPrimaryVertices'
	
	# -- PUPPI isolation calculation without muon
	load_pfPUPPI_sequence(process, 'pfNoMuonPUPPISequence', algo = 'NoMuonPUPPI',
	  src_puppi = 'pfAllHadronsAndPhotonsForNoMuonPUPPI',
	  cone_puppi_central = 0.5
	)
	process.pfAllHadronsAndPhotonsForNoMuonPUPPI.src = 'convertedPackedPFCandidatesWoMuon'
	process.particleFlowNoMuonPUPPI.candName         = 'packedPFCandidatesWoMuon'
	process.particleFlowNoMuonPUPPI.vertexName       = 'offlineSlimmedPrimaryVertices'
	
	process.ParticleIsoSequences = cms.Sequence(process.pfDeltaBetaWeightingSequence * 
												process.pfPUPPISequence * 
												process.pfNoMuonPUPPISequence
												)
	
	from JMEAnalysis.JMEValidator.MuonPFIsolationSequence_cff import *
	muon_src, cone_size = 'selectedPatMuons', 0.4
	
	process.pfCHLVForIso = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV>=2 && abs(charge) > 0"))
	process.pfCHPUForIso = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV<=1 && abs(charge) > 0"))
	process.pfPhotonsForIso = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("pdgId==22"))
	process.pfNHForIso = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("pdgId!=22 && abs(charge) == 0" ))
	
	load_muonPFiso_sequence(process, 'MuonPFIsoSequenceSTAND', algo = 'R04STAND',
	  src = muon_src,
	  src_charged_hadron = 'pfCHLVForIso',
	  src_neutral_hadron = 'pfNHForIso',
	  src_photon         = 'pfPhotonsForIso',
	  src_charged_pileup = 'pfCHPUForIso',
	  coneR = cone_size
	)
	
	load_muonPFiso_sequence(process, 'MuonPFIsoSequencePFWGT', algo = 'R04PFWGT',
	  src = muon_src,
	  src_neutral_hadron = 'pfWeightedNeutralHadrons',
	  src_photon         = 'pfWeightedPhotons',
	  coneR = cone_size
	)
	
	load_muonPFiso_sequence(process, 'MuonPFIsoSequencePUPPI', algo = 'R04PUPPI',
	  src = muon_src,
	  src_charged_hadron = 'pfPUPPIChargedHadrons',
	  src_neutral_hadron = 'pfPUPPINeutralHadrons',
	  src_photon         = 'pfPUPPIPhotons',
	  coneR = cone_size
	)
	
	load_muonPFiso_sequence(process, 'MuonPFIsoSequenceNoMuonPUPPI', algo = 'R04NOMUONPUPPI',
	  src = muon_src,
	  src_charged_hadron = 'pfNoMuonPUPPIChargedHadrons',
	  src_neutral_hadron = 'pfNoMuonPUPPINeutralHadrons',
	  src_photon         = 'pfNoMuonPUPPIPhotons',
	  coneR = cone_size
	)
	
	# process.muPFIsoDepositCharged.src = 'slimmedMuons'
	process.muonMatch.src = 'slimmedMuons'
	process.muonMatch.matched = 'prunedGenParticles'
	process.patMuons.pvSrc = 'offlineSlimmedPrimaryVertices'
	process.p.remove(process.patMuons)
	
	process.MuonPFIsoSequences = cms.Sequence(
	  process.MuonPFIsoSequenceSTAND *
	  process.MuonPFIsoSequencePFWGT *
	  process.MuonPFIsoSequencePUPPI *
	  process.MuonPFIsoSequenceNoMuonPUPPI
	)
	
	process.p.replace(
	  process.selectedPatMuons,
	  process.selectedPatMuons *
	  process.ParticleIsoSequences *
	  process.MuonPFIsoSequences
	)
	
