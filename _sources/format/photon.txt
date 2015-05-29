Photon
######

All the branches for this analyzer are stored inside ``std::vector<>`` of the corresping type listed below.

.. cpp:member:: float supercluster_eta
                float supercluster_phi

.. cpp:member:: float hOverE

.. cpp:member:: bool has_pixel_seed

.. cpp:member:: bool has_matched_prompt_electron

.. cpp:member:: float full5x5_sigmaIetaIeta

.. cpp:member:: float chargedHadronIsoR03
                float neutralHadronIsoR03
                float photonIsoR03

   Charged hadron, neutral hadron and photon isolation in a cone of size :math:`R = 0.3`

.. cpp:member:: float chargedHadronIsoR03_withEA
                float neutralHadronIsoR03_withEA
                float photonIsoR03_withEA

   Charged hadron, neutral hadron and photon isolation in a cone of size :math:`R = 0.3`, with ``effective area`` correction. Effective area values are extracted from `this twiki page <https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2#Selection_implementation_details>`__

   .. math::

      I_\text{EA corrected} &= \max{\left( I - \rho \times \text{EA}, 0 \right)}

.. cpp:member:: std::map<std::string, bool> ids

   See the :ref:`ids` page
