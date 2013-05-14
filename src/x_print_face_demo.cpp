#include <deque>

#include "apollo2/apollonius_triangulation/tangent_sphere_of_three_spheres.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/opengl_printer.h"

void x_print_face_demo(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	std::vector<apollo2::SimpleSphere> generators;
	generators.push_back(apollo2::SimpleSphere(2, 1, 0, 0.7));
	generators.push_back(apollo2::SimpleSphere(-1, 2, 0, 1.0));
	generators.push_back(apollo2::SimpleSphere(-1, -1, 0, 1.3));

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	auxiliaries::OpenGLPrinter opengl_printer("obj1", "cgo1");

	for(std::size_t i=0;i<generators.size();i++)
	{
		opengl_printer.print_sphere(generators[i], auxiliaries::Color::from_code(0x36BBCE));
	}

	const std::vector<apollo2::SimpleSphere> min_tangents=apollo2::apollonius_triangulation::TangentSphereOfThreeSpheres::calculate(generators[0], generators[1], generators[2]);
	if(min_tangents.size()==1)
	{
		const apollo2::SimpleSphere min_tangent=min_tangents.front();
		opengl_printer.print_sphere(apollo2::SimpleSphere(min_tangent, 0.1), auxiliaries::Color::from_code(0xA61700));

		std::deque<apollo2::SimplePoint> curve;
		{
			const double r_mult=1.01;
			const double r_max=5;
			for(double r=min_tangent.r;r<r_max;r=r*r_mult)
			{
				const std::vector<apollo2::SimpleSphere> tangent_spheres=apollo2::apollonius_triangulation::TangentSphereOfThreeSpheres::calculate(generators[0], generators[1], generators[2], r);
				if(tangent_spheres.size()==2)
				{
					for(std::size_t i=0;i<tangent_spheres.size();i++)
					{
						const double h=apollo2::halfspace_of_point(generators[0], apollo2::plane_normal_from_three_points<apollo2::SimplePoint>(generators[0], generators[1], generators[2]), tangent_spheres[i]);
						if(h<0)
						{
							curve.push_front(apollo2::SimplePoint(tangent_spheres[i]));
						}
						else
						{
							curve.push_back(apollo2::SimplePoint(tangent_spheres[i]));
						}
					}
				}
			}
		}
		opengl_printer.print_line_strip(std::vector<apollo2::SimplePoint>(curve.begin(), curve.end()));
	}
}
