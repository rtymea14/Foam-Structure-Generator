#version 3.6;

// Right-handed coordinate system in which the z axis points upwards
camera {
	location <2,5,3>
	sky z
	right -0.4*x*image_width/image_height
	up 0.4*z
	look_at <0,0,1.25>
}

// White background
background{rgb 1}

// Two lights with slightly different colors
light_source{<8,2,3> color rgb <0.77,0.75,0.75>}
light_source{<2,1,5> color rgb <0.38,0.40,0.40>}

// Radius of the Voronoi cell network, and the particle radius
#declare rr=0.08;
#declare r=0.08;
#declare s=0.5;
#declare ss=0.5;

// Particles
//union{
//#include "cylinder_p.pov"
//scale <1,-1,1>	
//	pigment{rgb <1,0.95,0.5>} finish{reflection 0.1 specular 0.3 ambient 0.42}
//}

// Voronoi cells
union{
#include "coord_v.pov"
scale <1,1,1>	
pigment{rgb <0.5,0.8,1>} finish{specular 0.5 ambient 0.42}
}
union{
#include "domain.pov"
scale <1,1,1>	
pigment{rgb <0.5,0.8,1>} finish{specular 0.5 ambient 0.42}
}
