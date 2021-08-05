# Foam-Structure-Generator
Generates 3D Foam Structure using Voronoi tessellation

## Overview
A 3D representative volume elements (RVEs) is generated using a combination of open-source software: [LIGGGHTS](https://www.cfdem.com/liggghtsr-open-source-discrete-element-method-particle-simulation-code), [voro++](http://math.lbl.gov/voro++/about.html), and [Blender](https://www.blender.org/). The figure below shows an example of the probability distribution for spheres that can fill up the foam space. 

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/SphereDist.jpg" width="196" height="196" />

Imitating this distribution will be the first step towards generating a foam RVE. The figure below shows the procedure used for generating such RVE. 

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/process.jpg" width="650" height="300" />

A bounding box volume is generated using the open-source granular simulation program [LIGGGHTS](https://www.cfdem.com/liggghtsr-open-source-discrete-element-method-particle-simulation-code). The bounding box is packed with polydisperse spheres whose size distribution matches the actual foam pore-size distribution. This ensures better pore-size distribution for foam geometry [[1]](#References). The coordinates of the spheres is then used to generate the Voronoi structure by the software [voro++](http://math.lbl.gov/voro++/about.html). The edges of the Voronoi structure then represent the struts of the foam filter geometry. A Python script is used to remove unnecessary edges and convert the output file to a format readable in Blender. The skeleton of the foam geometry comprising of the network of struts is subsequently exported to Blender, where the filter surface topology – such as circular for high-density foams or circular horn triangular [[2, 3]](#References) for relatively low-density polymer foams is applied using built-in functionalities therein. The resulting geometry is finally saved as a .stl file readable by various meshing software.

<img src="https://github.com/rtymea14/Foam-Structure-Generator/blob/main/mesh.jpg" width="350" height="175" />

## Installation
The current version of the code uses the voro++0.4.6 and pyvoro-3.7-stable. It uses python for post-processing. Both programs are uploaded and need to be installed. To install please follow the README files in the program folders. In general the steps are: configure, make and sudo make install. After installation please navigate to the [basic](voro++-0.4.6/examples/basic) folder in voro++0.4.6. It contains a import.cc file which can be compiled by typing Make. However, before compiling you must change the path for header and library in the [config.mk file](voro++-0.4.6/config.mk) and also change the path to this config.mk file in the [Makefile](voro++-0.4.6/examples/basic) in the examples/basic folder.After that compile and run the import command. After that run the two python files. A LIGGGHTS input file used to generate the sphere coordinates is also included. You can change the number of particles in the box by running the LIGGGHTS input file with LIGGGHTS. Right now only the python files for cleaning up the geomery is included. The rest of the files will be uploaded in future. 

**To run the code in example/basic folder and generate the figures:**
Navigate to a working folder in a shell terminal, clone the git code repository, and build.
```
$ cd Foam-Structure-Generator/voro++-0.4.6/examples/basic/
$ make import
$ ./import
$ povray +W800 +H600 +A0.01 +Oimport.png import.pov
$ python Nofront.py
$ python Sphere.py
$ povray +W800 +H600 +A0.01 +Ofoam.png foam.pov
```

The solver can be validated using the case in the [`validation_case`](validation_case). The nanoparticle-fluid case is in the [`colloid_case`](colloid_case).  

## References
1.  [Nie, Z., Lin, Y., and Tong, Q., 2017, "Modeling structures of open cell foams," Computational Materials Science, 131, pp. 160-169.](http://dx.doi.org/10.1016/j.commatsci.2017.01.029)
2.  [Kasner, E., and Kalish, A., 1944, "The Geometry of the Circular Horn Triangle," National Mathematics Magazine, 18(8), pp. 299-304.](https://doi.org/10.2307/3030080)
3.  [Schmierer, E. N., and Razani, A., 2006, "Self-Consistent Open-Celled Metal Foam Model for Thermal Applications," Journal of Heat Transfer, 128(11), pp. 1194-1203.](https://doi.org/10.1115/1.2352787)
4.  [Rycroft, C. H., 2009, "VORO++: A three-dimensional Voronoi cell library in C++," Chaos: An Interdisciplinary Journal of Nonlinear Science, 19(4), p. 041111.](https://doi.org/10.1063/1.3215722)
