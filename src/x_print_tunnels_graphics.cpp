#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/spheres_tangent_disk.h"
#include "apollo/spheres_tangent_sphere_by_radius.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/opengl_printer.h"

void x_print_tunnels_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	clo.check_allowed_options("--min-tangent-radius: --max-tangent-radius: --step:");

	const double min_tangent_radius=clo.isopt("--min-tangent-radius") ? clo.arg_with_min_value<double>("--min-tangent-radius", 0) : 0.1;
	const double max_tangent_radius=clo.isopt("--max-tangent-radius") ? clo.arg_with_min_value<double>("--max-tangent-radius", min_tangent_radius) : 2.8;
	const double step=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0) : 0.1;

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
		for(std::size_t e=0;e<tangent_disks.size();e++)
		{
			const apollo::SimpleSphere& tangent_disk=tangent_disks[e];
			if(tangent_disk.r>=min_tangent_radius && tangent_disk.r<max_tangent_radius/4)
			{
				opengl_printer.print_sphere(atoms[it->first.get(0)], auxiliaries::Color::from_code(0x00FF00));
				opengl_printer.print_sphere(atoms[it->first.get(1)], auxiliaries::Color::from_code(0x00FF00));
				opengl_printer.print_sphere(atoms[it->first.get(2)], auxiliaries::Color::from_code(0x00FF00));

				for(double r=tangent_disk.r;r<=max_tangent_radius;r+=step)
				{
					const std::vector<apollo::SimpleSphere> tangent_spheres=apollo::construct_spheres_tangent_sphere_by_radius<apollo::SimpleSphere>(atoms[it->first.get(0)], atoms[it->first.get(1)], atoms[it->first.get(2)], r);
					for(std::size_t i=0;i<tangent_spheres.size();i++)
					{
						const apollo::SimpleSphere& tangent_sphere=tangent_spheres[i];
//						opengl_printer.print_sphere(tangent_sphere, auxiliaries::Color::from_code(0x0000FF));
						const apollo::SimplePoint tanget_sphere_center=apollo::custom_point_from_object<apollo::SimplePoint>(tangent_sphere);
						std::vector<apollo::SimpleSphere> circle_points(3);
						for(int j=0;j<3;j++)
						{
							circle_points[j]=apollo::custom_sphere_from_point<apollo::SimpleSphere>(tanget_sphere_center+((apollo::custom_point_from_object<apollo::SimplePoint>(atoms[it->first.get(j)])-tanget_sphere_center).unit()*tangent_sphere.r), 0);
						}
						const std::vector<apollo::SimpleSphere> circles=apollo::construct_spheres_tangent_disk<apollo::SimpleSphere>(circle_points[0], circle_points[1], circle_points[2]);
						for(std::size_t j=0;j<circles.size();j++)
						{
							const apollo::SimpleSphere& circle=circles[j];
							const apollo::SimplePoint normal=apollo::plane_normal_from_three_points<apollo::SimplePoint>(circle_points[0], circle_points[1], circle_points[2]);
							const apollo::SimplePoint center=apollo::custom_point_from_object<apollo::SimplePoint>(circle);
							opengl_printer.print_cylinder(center-(normal*0.02), center+(normal*0.02), circle.r, auxiliaries::Color::from_code(0xFFFF00), auxiliaries::Color::from_code(0xFFFF00));
						}
					}
				}

				return;
			}
		}
	}
}
