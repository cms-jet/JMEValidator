.. _electron:

Electron
########

.. cpp:member:: float supercluster_eta
                float supercluster_phi


.. cpp:member:: float dEtaIn
                float dPhiIn

.. cpp:member:: float hOverE

.. cpp:member:: float full5x5_sigmaIetaIeta

.. cpp:member:: float ooEmooP

.. cpp:member:: float d0
.. cpp:member:: float dz

.. cpp:member:: float expected_missing_inner_hits

   .. math::

      &= \left| \frac{1}{E} - \frac{1}{p} \right|

   where :math:`E` is the ECAL energy (:math:`\text{electron.ecalEnergy()}`), and :math:`p` is obtained with

   .. math::

      p &= \frac{\text{electron.ecalEnergy()}}{\text{electron.eSuperClusterOverP()}}

.. cpp:member:: bool pass_conversion_veto
