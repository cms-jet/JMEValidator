File structure
==============

This page document the ROOT file produced by the framework.

Structure
---------

The framework is built around several *analyzers*. Each analyzer is responsible for the extraction of information of one particular high-level object (muons, vertex, photons, ...). These information are stored in plain ROOT format, using *TTrees*. The structure of this file is the following::

    ├── [*] analyzer1
    │       └── t
    ├── [*] analyzer2
    │       └── t
    ├── [*] analyzer3
    │       └── t
    ├── [*] analyzer4
    │       └── t
    
where [*] denotes a folder inside the file, and ``t`` is the tree associated with the analyzer. Each analyzer has its own folder inside the file. Inside this folder, a tree named ``t`` contains the products created by the analyzer.

Trees structure
---------------

The tree structure share a common basis for all the analyzer. When relevant, this basis includes information about the impulsion of the object, its PDG id, as well as information about the matched generator particle.

Unless stated otherwise, all branches are ``std::vector<>`` of the type stated in the branches list, containing multiple entries per event.

Common branches
_______________

All the following branches are ``std::vector<>`` of the type stated in the list, containing multiple entries per event.

.. cpp:member:: ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double>> p4
    
    the impulsion of the object

.. cpp:member:: float y

    rapidity of the object

.. cpp:member:: int8_t charge

.. cpp:member:: bool is_matched

    If ``true``, a generator particle has been matched to this object. ``gen_*`` branches are filled for this object. If ``false``, no generator particle has been matched to this object, and ``gen_*`` branches are filled with default value for this object.

.. cpp:member:: ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double>> gen_p4
    
    the impulsion of the generator object matched to the object

.. cpp:member:: float gen_y
.. cpp:member:: int8_t gen_charge

Analyzers and objects
_____________________

.. toctree::
   :glob:

   format/*
