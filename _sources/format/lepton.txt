Lepton
######

Both electrons and muons share a common set of branches related to isolation computation. These branches are documented below, and specific branches are documented separately for :ref:`muon` and :ref:`electron`.

Common branches
---------------

.. cpp:member:: float chargedHadronIsoR03
                float chargedHadronIsoR04

   Isolation for charged hadrons in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`)

.. cpp:member:: float neutralHadronIsoR03
                float neutralHadronIsoR04

   Isolation for neutral hadrons in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`)

.. cpp:member:: float photonIsoR03
                float photonIsoR04

   Isolation for photons in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`)

.. cpp:member:: float puChargedHadronIsoR03
                float puCHargedHadronIsoR04

   Isolation for PU charged hadrons in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`)

.. cpp:member:: float relativeIsoR03
                float relativeIsoR04

   Relative isolation of the particle in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`)

   .. math::

      I_\text{rel} &= \frac{ I^\text{CH} + I^\text{NH} + I^\text{Ph} }{ p_T }

.. cpp:member:: float relativeIsoR03_deltaBeta
                float relativeIsoR04_deltaBeta

   Relative isolation of the particle in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`), with ``delta beta`` correction

   .. math::

      I_\text{rel} &= \frac{ I^\text{CH} + \max{\left(I^\text{NH} + I^\text{Ph} - 0.5 I^\text{CH, PU}, 0 \right) } }{ p_T }

.. cpp:member:: float relativeIsoR03_withEA
                float relativeIsoR04_withEA

   Relative isolation of the particle in various cone sizes (:math:`R = 0.3` or :math:`R = 0.4`), with ``effective area`` correction

   .. math::

      I_\text{rel} &= \frac{ I^\text{CH} + \max{\left(I^\text{NH} + I^\text{Ph} - \rho \times \text{EA}, 0 \right) } }{ p_T }

   where :math:`EA` is the effective area. The values used are extracted on PHYS14 samples and summarised `on these slides <https://indico.cern.ch/event/367861/contribution/2/material/slides/0.pdf>`__.

Specific branches
-----------------

.. toctree::
   :glob:

   lepton/*
