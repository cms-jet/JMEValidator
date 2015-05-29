.. _ids:

IDs
###

Lepton and photon ID are computed using official selectors and stored inside a branch named ``ids`` of type :code:`std::vector<std::map<std::string, bool>>`. The ``key`` of the map is the name of official selector, and the ``value`` is ``true`` if the particle pass the id, or ``false`` otherwise.

The IDs used are summarized in the table below:

    ========   ============================================================================================================
    particle   link
    ========   ============================================================================================================
    photon     https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2#Recipe_for_regular_users_for_74X
    electron   https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_for_74X
    muon       No official selector for the moment
    ========   ============================================================================================================
