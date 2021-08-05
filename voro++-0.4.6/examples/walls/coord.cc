// Cylindrical wall example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include "voro++.hh"
using namespace voro;

// Set up constants for the container geometry
const double x_min=105,x_max=195;
const double y_min=105,y_max=195;
const double z_min=0,z_max=1;
//const double z_min=0,z_max=18.5;

// Set the computational grid size
const int n_x=7,n_y=7,n_z=1;
//const int n_x=7,n_y=7,n_z=14;

int main() {
	// Create a container with the geometry given above, and make it
	// non-periodic in each of the three coordinates. Allocate space for
	// eight particles within each computational block.
	container con(x_min,x_max,y_min,y_max,z_min,z_max,n_x,n_y,n_z,
			false,false,false,8);

	// Add a cylindrical wall to the container
	wall_cylinder cyl(150,150,0,150,150,1,45);
	//wall_cylinder cyl(0,0,0,0,0,1,6);
	con.add_wall(cyl);

	// Import the particles from a file
	con.import("Coord1200.csv");

	// Output the particle positions in POV-Ray format
	con.draw_particles_pov("coord_p.pov");
	con.draw_domain_pov("domain.pov");

	// Output the Voronoi cells in POV-Ray format
	con.draw_cells_pov("coord_v.pov");
	con.draw_cells_gnuplot("coord_v.gnu");
}
