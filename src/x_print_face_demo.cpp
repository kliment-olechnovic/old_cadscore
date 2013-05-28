#include <deque>

#include "apollo2/tangent_plane_of_three_spheres.h"
#include "apollo2/tangent_sphere_of_three_spheres.h"

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

	auxiliaries::OpenGLPrinter opengl_printer1("obj1", "cgo1");
	auxiliaries::OpenGLPrinter opengl_printer2("obj2", "cgo2");
	auxiliaries::OpenGLPrinter opengl_printer3("obj3", "cgo3");
	auxiliaries::OpenGLPrinter opengl_printer4("obj4", "cgo4");

	for(std::size_t i=0;i<generators.size();i++)
	{
		opengl_printer1.print_sphere(apollo2::SimpleSphere(generators[i], generators[i].r-0.01), auxiliaries::Color::from_code(0x36BBCE));
	}

	std::vector< std::pair<apollo2::SimplePoint, apollo2::SimplePoint> > tangent_planes=apollo2::TangentPlaneOfThreeSpheres::calculate(generators[0], generators[1], generators[2]);
	const std::vector<apollo2::SimpleSphere> min_tangents=apollo2::TangentSphereOfThreeSpheres::calculate(generators[0], generators[1], generators[2]);
	if(tangent_planes.size()==2 && min_tangents.size()==1)
	{
		if(apollo2::halfspace_of_point(generators[0], apollo2::plane_normal_from_three_points<apollo2::SimplePoint>(generators[0], generators[1], generators[2]), tangent_planes[0].first+tangent_planes[0].second)>0)
		{
			std::swap(tangent_planes[0], tangent_planes[1]);
		}

		const apollo2::SimpleSphere min_tangent=min_tangents.front();
		opengl_printer1.print_sphere(apollo2::SimpleSphere(min_tangent, 0.1), auxiliaries::Color::from_code(0xA61700));

		std::deque<apollo2::SimplePoint> curve;
		std::deque<double> radii;
		{
			const double r_mult=1.01;
			const double r_max=7;
			for(double r=min_tangent.r;r<r_max;r=r*r_mult)
			{
				const std::vector<apollo2::SimpleSphere> tangent_spheres=apollo2::TangentSphereOfThreeSpheres::calculate(generators[0], generators[1], generators[2], r);
				if(tangent_spheres.size()==2)
				{
					for(std::size_t i=0;i<tangent_spheres.size();i++)
					{
						if(apollo2::halfspace_of_point(generators[0], apollo2::plane_normal_from_three_points<apollo2::SimplePoint>(generators[0], generators[1], generators[2]), tangent_spheres[i])<0)
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

		opengl_printer1.print_line_strip(std::vector<apollo2::SimplePoint>(curve.begin(), curve.end()));

		std::vector< std::vector<apollo2::SimplePoint> > circles_vertices;
		std::vector< std::vector<apollo2::SimplePoint> > circles_normals;
		for(std::size_t i=0;i<curve.size();i++)
		{
			std::vector<apollo2::SimpleSphere> touches;
			if(i==0 || i+1==curve.size())
			{
				for(std::size_t j=0;j<generators.size();j++)
				{
					touches.push_back(apollo2::SimpleSphere(apollo2::SimplePoint(generators[j])+(tangent_planes[i==0 ? 0 : 1].second*generators[j].r), 0.0));
				}
			}
			else
			{
				const apollo2::SimpleSphere tangent(curve[i], radii[i]);
				for(std::size_t j=0;j<generators.size();j++)
				{
					apollo2::SimplePoint a(tangent);
					apollo2::SimplePoint b(generators[j]);
					apollo2::SimplePoint ab=((b-a).unit())*tangent.r;
					touches.push_back(apollo2::SimpleSphere(a+ab, 0.0));
				}
			}
			const std::vector<apollo2::SimpleSphere> circles=apollo2::TangentSphereOfThreeSpheres::calculate(touches[0], touches[1], touches[2]);
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

		opengl_printer2.print_color(auxiliaries::Color::from_code(0xFF5A40));
		for(std::size_t i=0;i+1<circles_vertices.size();i++)
		{
			std::size_t j=i+1;
			std::vector<apollo2::SimplePoint> vertices;
			std::vector<apollo2::SimplePoint> normals;
			if(circles_vertices[i].size()==circles_vertices[j].size())
			{
				for(std::size_t e=0;e<circles_vertices[i].size();e++)
				{
					vertices.push_back(circles_vertices[i][e]);
					vertices.push_back(circles_vertices[j][e]);
					normals.push_back(circles_normals[i][e]);
					normals.push_back(circles_normals[j][e]);
				}
				opengl_printer2.print_triangle_strip(vertices, normals);
			}
		}

		for(int i=0;i<2;i++)
		{
			std::vector<apollo2::SimplePoint> vertices_inner=circles_vertices[i==0 ? 0 : circles_vertices.size()-1];
			std::vector<apollo2::SimplePoint> normals_inner=circles_normals[i==0 ? 0 : circles_normals.size()-1];
			std::vector<apollo2::SimplePoint> vertices;
			std::vector<apollo2::SimplePoint> normals;
			for(std::size_t j=0;j<vertices_inner.size();j++)
			{
				vertices.push_back(vertices_inner[j]);
				vertices.push_back(vertices_inner[j]+(normals_inner[j]*2.5));
				normals.push_back(tangent_planes[i].second);
				normals.push_back(tangent_planes[i].second);
			}
			opengl_printer3.print_color(auxiliaries::Color::from_code(i==0 ? 0xFFB673 : 0x64DE89));
			opengl_printer3.print_triangle_strip(vertices, normals);
		}

		std::size_t i=0;
		while(i<(curve.size()/2))
		{
			opengl_printer4.print_alpha(0.2);
			opengl_printer4.print_sphere(apollo2::SimpleSphere(curve[i], radii[i]-0.01), auxiliaries::Color::from_code(0xFF9C40));
			opengl_printer4.print_sphere(apollo2::SimpleSphere(curve[curve.size()-1-i], radii[curve.size()-1-i]-0.01), auxiliaries::Color::from_code(0x37DE6A));
			i+=30;
		}
	}

	std::cout << "cmd.set('ray_shadows', 'off')\n\n";
	std::cout << "cmd.set('ray_shadow', 'off')\n\n";
	std::cout << "cmd.set('two_sided_lighting', 'on')\n\n";
	std::cout << "cmd.set('cgo_line_width', 3)\n\n";
	std::cout << "cmd.set('bg_rgb', [1,1,1])\n\n";
	std::cout << "cmd.set('ambient', 0.3)\n\n";
}
