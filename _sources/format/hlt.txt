HLT
...

General informations about the High Level Trigger. All the branches for this analyzer are stored inside ``std::vector<>`` of the corresping type listed below.

.. cpp:member:: std::string paths

    The list of all HLT paths accepted by the event.

    .. note::

       ALCA triggers are not stored

.. cpp:member:: int prescales

    The prescale of the HLT path

    .. note::

       Use :c:member:`paths` to get the corresponding HLT name


.. cpp:member:: float objects_pt
              float objects_eta
              float objects_phi
              float objects_e

   Store impulsion of all HLT objects. For each object, you can find in :c:member:`objects_paths` which paths this object has triggered.

.. cpp:member:: std::vector<int> objects_paths

    For each HLT object, contains a list of all HLT paths triggered by this object.

    .. note::

       Only indexes are stored in this list for disk-space reasons. You can use theses indexes in the :c:member:`paths` list to retrieve the path name

.. cpp:member:: std::vector<bool> objects_paths_isl3
.. cpp:member:: std::vector<bool> objects_paths_islast


