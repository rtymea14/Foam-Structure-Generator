# Foam-Structure-Generator
Generates 3D Foam Structure using Voronoi tessellation

## Overview
A 3D representative volume elements (RVEs) is generated using a combination of open-source software: [LIGGGHTS](https://www.cfdem.com/liggghtsr-open-source-discrete-element-method-particle-simulation-code), [voro++](http://math.lbl.gov/voro++/about.html), and [Blender](https://www.blender.org/). The figure below shows an example of the probability distribution for spheres that can fill up the foam space. 

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/SphereDist.jpg" width="196" height="196" />

Imitating this distribution will be the first step towards generating a foam RVE. The figure below shows the procedure used for generating such RVE. 

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/process.jpg" width="450" height="400" />

A bounding box volume is generated using the open-source granular simulation program [LIGGGHTS](https://www.cfdem.com/liggghtsr-open-source-discrete-element-method-particle-simulation-code). The bounding box is packed with polydisperse spheres whose size distribution matches the actual foam pore-size distribution. This ensures better pore-size distribution for foam geometry [[1]](#References). The coordinates of the spheres is then used to generate the Voronoi structure by the software [voro++](http://math.lbl.gov/voro++/about.html). The edges of the Voronoi structure then represent the struts of the foam filter geometry. A Python script is used to remove unnecessary edges and convert the output file to a format readable in Blender. The skeleton of the foam geometry comprising of the network of struts is subsequently exported to Blender, where the filter surface topology – such as circular for high-density foams or circular horn triangular [[2, 3]](#References) for relatively low-density polymer foams is applied using built-in functionalities therein. The resulting geometry is finally saved as a .stl file readable by various meshing software.

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/mesh.jpg" width="650" height="400" />

## Installation
The current version of the code uses the [OpenFOAM 2.3.1 libraries](http://www.openfoam.org/archive/2.3.1/download/source.php). It uses [isoadvector](https://github.com/isoAdvector/isoAdvector) library for interface tarcking and adection. It also calculates the thermodynamics and transport properties of gases (diffusion coefficients, thermal conductivity, heat capacities, and viscosity) based on the correlations available in the [OpenSMOKE++](https://www.opensmokepp.polimi.it) library. The coupling with the particle solver which is [LIGGGHTS](https://www.cfdem.com/liggghtsr-open-source-discrete-element-method-particle-simulation-code) is done using [CFDEM](https://www.cfdem.com/cfdemrcoupling-open-source-cfd-dem-framework) library. Once all the libraries are installed, you can proceed to install the CVOFLSDPD solver. 

**To run the code in example/basic folder and generate the figures:**
Navigate to a working folder in a shell terminal, clone the git code repository, and build.
```
$ git clone https://github.com/rtyme14/CVOFLS-DPD.git CVOFLS-DPD
$ cd CVOFLS-DPD/CVOFLS-DPD
$ source Allwmake.sh
```

The solver can be validated using the case in the [`validation_case`](validation_case). The nanoparticle-fluid case is in the [`colloid_case`](colloid_case).  

## References
1.  [Nie, Z., Lin, Y., and Tong, Q., 2017, "Modeling structures of open cell foams," Computational Materials Science, 131, pp. 160-169.](http://dx.doi.org/10.1016/j.commatsci.2017.01.029)
2.  [Kasner, E., and Kalish, A., 1944, "The Geometry of the Circular Horn Triangle," National Mathematics Magazine, 18(8), pp. 299-304.](https://doi.org/10.2307/3030080)
3.  [Schmierer, E. N., and Razani, A., 2006, "Self-Consistent Open-Celled Metal Foam Model for Thermal Applications," Journal of Heat Transfer, 128(11), pp. 1194-1203.](https://doi.org/10.1115/1.2352787)
4.  [Rycroft, C. H., 2009, "VORO++: A three-dimensional Voronoi cell library in C++," Chaos: An Interdisciplinary Journal of Nonlinear Science, 19(4), p. 041111.](https://doi.org/10.1063/1.3215722)
