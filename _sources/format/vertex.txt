Vertex
......

Store general quality information about primary vertices. All the branches for this analyzer are stored inside ``std::vector<>`` of the corresping type listed below.

.. cpp:member:: float normalizedChi2

    :math:`\chi^2 / \text{ndof}`

.. cpp:member:: float ndof
.. cpp:member:: float isValid
.. cpp:member:: float isFake

.. cpp:member:: ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3>> covariance

    Use the diagonals of this 3x3 matrix to get the error on the position

.. cpp:member:: ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double>> position

    The position of the vertex
