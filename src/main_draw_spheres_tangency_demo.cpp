#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_tangent_sphere.h"

#include "auxiliaries/command_line_options.h"

template<typename T>
void draw_sphere(const T& s, const int quality, const int wired)
{
	std::cout << "sphere " << s.x << " " << s.y << " " << s.z << " " << s.r << " " << quality << " " << wired << "\n";
}

void main_draw_spheres_tangency_demo(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SimpleSphere Sphere;

	std::cout << "color 1 1 1\n";

	const Sphere a( 0.0, 0.6, 0.0, 0.5);
	const Sphere b( 0.5,-0.3, 0.0, 0.4);
	const Sphere c(-0.5,-0.3, 0.0, 0.1);
	draw_sphere(a, 3, 0);
	draw_sphere(b, 3, 0);
	draw_sphere(c, 3, 0);

	std::cout << "color 1 1 0\n";
	Sphere d( 0.0, 0.0, 0.0, 0.15);
	for(d.z=-0.7;d.z<=0.7;d.z+=0.05)
	{
		const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
		for(std::size_t i=0;i<ts.size();i++)
		{
			draw_sphere(ts[0], 3, 0);
		}
	}

	std::cout << "flush\n";
	std::cout.flush();
}
