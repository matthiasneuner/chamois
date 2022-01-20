Chamois is a MOOSE application for simulation complex, cohesive-friction materials available in the [Marmot](https://github.com/MAteRialMOdelingToolbox/marmot) library.
It provides an interface to MARMOT materials, and it offers specialized Kernels for using those materials in parallel 3D finite simulations.

It is developed at the [Unit of Strength of Materials and Structural Analysis of the University of Innsbruck](https://www.uibk.ac.at/bft/), and at the [CM2 lab at Stanford University](https://cm2.stanford.edu/).

For building Chamois, the [Marmot](https://github.com/MAteRialMOdelingToolbox/marmot) shared library and header files must be compiled and installed.
Please read the Marmot documentation for instructions on this essential step.

Both Chamois and Marmot make use of the [Eigen](https://gitlab.com/libeigen/eigen) and [Fastor](https://github.com/romeric/Fastor) libraries for performant linear algebra and tensor contractions.
