#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/spheres_tangent_disk.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/opengl_printer.h"

void x_print_tunnels_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	clo.check_allowed_options("");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::TriplesNeighboursMap triples_neighbours_map=Apollo::collect_triples_neighbours_from_quadruples(quadruples_map);

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	const auxiliaries::OpenGLPrinter opengl_printer("obj_tunnels", "cgo_tunnels");

	for(Apollo::TriplesNeighboursMap::const_iterator it=triples_neighbours_map.begin();it!=triples_neighbours_map.end();++it)
	{
		const std::vector<apollo::SimpleSphere> tangent_disks=apollo::construct_spheres_tangent_disk<apollo::SimpleSphere>(atoms[it->first.get(0)], atoms[it->first.get(1)], atoms[it->first.get(2)]);
		for(std::size_t i=0;i<tangent_disks.size();i++)
		{
			const apollo::SimpleSphere& tangent_disk=tangent_disks[i];
			if(tangent_disk.r>0 && tangent_disk.r<1.4)
			{
				const apollo::SimplePoint normal=apollo::plane_normal_from_three_points<apollo::SimplePoint>(atoms[it->first.get(0)], atoms[it->first.get(1)], atoms[it->first.get(2)]);
				const apollo::SimplePoint center=apollo::custom_point_from_object<apollo::SimplePoint>(tangent_disk);
				opengl_printer.print_cylinder(center-(normal*0.1), center+(normal*0.1), tangent_disk.r, auxiliaries::Color::from_code(0xFFFF00), auxiliaries::Color::from_code(0xFFFF00));
			}
		}
	}
}
