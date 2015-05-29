Jet
###

Store information about jets. The following selection is applied:

    * :math:`p_T \geq 5` GeV

All the branches for this analyzer are stored inside ``std::vector<>`` of the corresping type listed below.

.. cpp:member:: int refpdgid

    The PDG id of the gen jet associated to the jet

.. cpp:member:: float refdrjt

    :math:`\Delta R` between the jet and its gen jet

.. cpp:member:: float refarea

    Jet area of the gen jet

.. cpp:member:: int8_t partonFlavor
                int8_t hadronFlavor

    Flavor of the jet. See `this twiki page <https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools#Hadron_based_jet_flavour_definit>`_ for a detailed explanation

.. cpp:member:: float jtarea

    Jet area of the get

.. cpp:member:: float jtjec

    Jet Energy Correction factor applied to this jet. Scale the impulsion of the jet with the inverse of this value to get the raw jet

.. cpp:member:: float beta
                float betaStar
                float betaClassic
                float betaStarClassic

   Compute :math:`\beta` and :math:`\beta^\star`, given by:

   .. math::

      \beta &= \frac{\sum{p_T^{\text{CH, PV}}}}{\sum{p_T^{\text{CH}}}} \\
      \beta^\star &= \frac{\sum{p_T^{\text{CH, PU}}}}{\sum{p_T^{\text{CH}}}}

   where the sum is over all the charged (:math:`\text{CH}`) constituents of the jet, :math:`\text{PV}` stands for constituents coming from the primary vertex and :math:`\text{PU}` for constituents coming from another vertex.

   ``Classic`` values are computed using ``MiniAOD`` :code:`fromPV()` function (See `here <https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2015#PV_Assignment>`_ for more details):
   
   * :math:`\text{PV}` are constituents with :code:`fromPV() == 3`
   * :math:`\text{PU}` are constituents with :code:`fromPV() == 0`

   Other values are computed by iterating over all the vertices and finding which one the constituent comes from, requiring :code:`dZ() < 0.2`.

.. cpp:member:: float dZ

    ``dZ`` of the constituent with the biggest :math:`p_T`

.. cpp:member:: float DRweighted

    .. math::

       &= \frac{\sum{p_T^2} \Delta R^2}{\sum{p_T^2}}

    where the sum if over all the constituents of the jet

.. cpp:member:: float fRing0
                float fRing1
                float fRing2
                float fRing3
                float fRing4
                float fRing5
                float fRing6
                float fRing7
                float fRing8

   Fraction of transverse momentum of the jet in a annulus in :math:`\Delta R` plane between the jet and the constituents. The annulus is given by the number at the end of the branch name. For ``fRing6``, it means :math:`0.6 < \Delta R < 0.7`, etc.

   .. note::
      
      ``fRing8`` contains the fraction of transverse momentum for :math:`\Delta R \geq 0.8`

.. cpp:member:: uint16_t nCh
                uint16_t nNeutrals

   Number of charged and neutrals constituents of the jet

.. cpp:member:: float ptD

    .. math::

       &= \frac{\sqrt{\sum{p_T^2}}}{\sum{p_T}}

    where the sum if over all the constituents of the jet
   

.. cpp:member:: float PUJetId_fullDiscriminant
                int PUJetId_cutBasedId
                int PUJetId_fullId

   Store information about PU jet ID. See `this twiki page <https://twiki.cern.ch/twiki/bin/view/CMS/PileupJetID>`__ for more detail.

.. cpp:member:: float QGTagger_qgLikelihood

   Quark / Gluon tagger discriminant

b-tagging
---------

Additionnaly, a set of branches are created dynamically, one for each b-tagging algorithm available. The name of these branches are by definition not known in advance because the available b-tagging algorithm may change. However, all these branches name ends with ``BJetsTag`` so they are easily identifiable.

.. cpp:member:: float *BJetsTag

   b-tagging algorithm discriminant. The name of the branch is the actual name of the b-tagging algorithm.
