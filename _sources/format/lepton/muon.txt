.. _muon:

Muon
####

All the branches for this analyzer are stored inside ``std::vector<>`` of the corresping type listed below.

.. cpp:member:: bool isLoose
                bool isSoft
                bool isMedium
                bool isTight
                bool isHighPt

   Official muon ID selector. See description of `this twiki page <https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonId2015>`__.

.. cpp:member:: float neutralHadronIsoR04_pfWeighted
                float photonIsoR04_pfWeighted
                float relativeIsoR04_pfWeighted

   Neutral hadrons, photon and relative isolation using particle flow reweighting. See `this twiki page <https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonIsolationForRun2#Particle_Flow_reweighting>`__ for a detailled description.

.. cpp:member:: float chargedHadronIsoR04_puppiWeighted
                float neutralHadronIsoR04_puppiWeighted
                float photonIsoR04_puppiWeighted
                float relativeIsoR04_puppiWeighted

   Charged hadrons, neutral hadrons, photon and relative isolation using PUPPI reweighting. See `this twiki page <https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonIsolationForRun2#PUPPI>`__ for a detailled description.

.. cpp:member:: float chargedHadronIsoR04_puppiNoMuonWeighted
                float neutralHadronIsoR04_puppiNoMuonWeighted
                float photonIsoR04_puppiNoMuonWeighted
                float relativeIsoR04_puppiNoMuonWeighted

   Charged hadrons, neutral hadrons, photon and relative isolation using PUPPI reweighting, excluding all the muons from the particles contributing to the isolation. See `this twiki page <https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonIsolationForRun2#PUPPI>`__ for a detailled description.

.. cpp:member:: std::map<std::string, bool> ids

   See the :ref:`ids` page
