Event
#####

General informations about the event.

.. cpp:member:: ULong64_t run

    Current run number

.. cpp:member:: ULong64_t lumi

    Current lumi section

.. cpp:member:: ULong64_t evt

    Current event number

.. cpp:member:: float rho

    ``rho``, extracted from ``fixedGridRhoFastjetAll``

.. cpp:member:: std::vector<float> rhos

    eta-dependant ``rho``. Binning is 5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5

.. cpp:member:: ULong64_t npv

    Number of primary vertices of the event, passing the following selection:

    * ``!isFake()``
    * ``ndof() >= 4``
    * ``fabs(z()) <= 24``

.. cpp:member:: std::vector<int> npus

    Number of PU interactions for a given bunch crossing (``getPU_NumInteractions()``).

    .. note::

       Use :c:member:`bxns` to get the corresponding bunch crossing

.. cpp:member:: std::vector<float> tnpus

    Number of **true** PU interactions for a given bunch crossing (``getTrueNumInteractions()``)

    .. note::

       Use :c:member:`bxns` to get the corresponding bunch crossing

.. cpp:member:: std::vector<int> bxns

    Vector of IDs of the bunch crossing. Can contains one of the following value:

    * ``-1``: previous bunch crossing
    * ``0``: current bunch crossing
    * ``1``: next bunch crossing

    .. note::

       For PU reweighting, you need to use the value of :c:member:`tnpus` corresponding to the bunch crossing with ID ``0``

.. cpp:member:: std::vector<int> bunch_spacings

    Spacing of the current bunch

    .. note::

       Use :c:member:`bxns` to get the corresponding bunch crossing

.. cpp:member:: float weight

    Generator weight

.. cpp:member:: float pthat

    Generator :math:`\hat{p}_T`

.. cpp:member:: std::vector<float> pu_sumpt_lowpt
.. cpp:member:: std::vector<float> pu_sumpt_highpt
.. cpp:member:: std::vector<int> pu_ntrks_lowpt
.. cpp:member:: std::vector<int> pu_ntrks_highpt
.. cpp:member:: std::vector<std::vector<float>> pu_zpositions

Generator information
_____________________

Information extracted from the `GenEventInfoProduct <https://cmssdt.cern.ch/SDT/doxygen/CMSSW_7_4_3_patch1/doc/html/d3/d77/classGenEventInfoProduct.html>`_ class.

.. cpp:member:: int nMEPartons
.. cpp:member:: int nMEPartonsFiltered
.. cpp:member:: float alphaQCD
.. cpp:member:: float alphaQED
.. cpp:member:: float qScale
.. cpp:member:: std::pair<int, int> pdfID
.. cpp:member:: std::pair<float, float> pdfX

