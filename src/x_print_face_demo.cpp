#include <deque>

#include "apollo2/apollonius_triangulation/tangent_sphere_of_three_spheres.h"

#include "apollo/rotation.h"

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
		std::deque<double> radii;
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
							radii.push_front(r);
						}
						else
						{
							curve.push_back(apollo2::SimplePoint(tangent_spheres[i]));
							radii.push_back(r);
						}
					}
				}
			}
		}

		opengl_printer.print_line_strip(std::vector<apollo2::SimplePoint>(curve.begin(), curve.end()));

		std::vector< std::vector<apollo2::SimplePoint> > circles_vertices;
		std::vector< std::vector<apollo2::SimplePoint> > circles_normals;
		for(std::size_t i=0;i<curve.size();i++)
		{
			const apollo2::SimpleSphere tangent(curve[i], radii[i]);
			std::vector<apollo2::SimpleSphere> touches;
			for(std::size_t j=0;j<generators.size();j++)
			{
				apollo2::SimplePoint a(tangent);
				apollo2::SimplePoint b(generators[j]);
				apollo2::SimplePoint ab=((b-a).unit())*tangent.r;
				touches.push_back(apollo2::SimpleSphere(a+ab, 0.0));
			}
			const std::vector<apollo2::SimpleSphere> circles=apollo2::apollonius_triangulation::TangentSphereOfThreeSpheres::calculate(touches[0], touches[1], touches[2]);
			if(circles.size()==1)
			{
				const apollo2::SimpleSphere& circle=circles.front();
				const apollo2::SimplePoint orientation=apollo2::plane_normal_from_three_points<apollo2::SimplePoint>(touches[0], touches[1], touches[2]);
				const apollo2::SimplePoint first_point(apollo2::SimplePoint(circle)-apollo2::SimplePoint(touches[0]));
				apollo::Rotation rotation(apollo2::custom_point_from_object<apollo::SimplePoint>(orientation), 0);
				const double angle_step=10;
				std::vector<apollo2::SimplePoint> circle_vertices;
				for(rotation.angle=0;rotation.angle<=360;rotation.angle+=angle_step)
				{
					circle_vertices.push_back(apollo2::SimplePoint(circle)+rotation.rotate<apollo2::SimplePoint>(first_point));
				}
				std::vector<apollo2::SimplePoint> circle_normals;
				for(std::size_t j=0;j<circle_vertices.size();j++)
				{
					circle_normals.push_back((circle_vertices[j]-apollo2::SimplePoint(circle)).unit());
				}
				circles_vertices.push_back(circle_vertices);
				circles_normals.push_back(circle_normals);
			}
		}

		opengl_printer.print_color(auxiliaries::Color::from_code(0xFF5A40));
		for(std::size_t i=0;i+1<circles_vertices.size();i++)
		{
			std::size_t j=i+1;
			std::vector<apollo2::SimplePoint> vertices;
			std::vector<apollo2::SimplePoint> normals;
			if(circles_vertices[i].size()==circles_vertices[j].size())
			{
				for(std::size_t e=0;e<circles_vertices[i].size();e++)
				{
					vertices.push_back(circles_vertices[i][e]-(circles_normals[i][e]*0.01));
					vertices.push_back(circles_vertices[j][e]-(circles_normals[j][e]*0.01));
					normals.push_back(circles_normals[i][e]);
					normals.push_back(circles_normals[j][e]);
				}
				opengl_printer.print_triangle_strip(vertices, normals);
			}
		}
	}
}
