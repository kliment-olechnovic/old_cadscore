#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_tangent_sphere.h"
#include "apollo/spheres_tangent_plane.h"

#include "auxiliaries/command_line_options.h"

template<typename T>
void draw_sphere(const T& s, const int quality, const int wired)
{
	std::cout << "sphere " << s.x << " " << s.y << " " << s.z << " " << s.r << " " << quality << " " << wired << "\n";
}

template<typename T>
void print_point(const T& p)
{
	std::cout << p.x << " " << p.y << " " << p.z << " ";
}

template<typename T>
void draw_triangle(const T& a, const T& b, const T& c, const T& n)
{
	std::cout << "triangle ";
	print_point(a);
	print_point(b);
	print_point(c);
	std::cout << "1 ";
	print_point(n);
	std::cout << "\n";
}

void main_draw_spheres_tangency_demo(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SimpleSphere Sphere;
	typedef apollo::SimplePoint Point;

	std::cout << "color 1 1 1\n";
	const Sphere a( 0.0, 0.6, 0.0, 0.5);
	const Sphere b( 0.5,-0.3, 0.0, 0.4);
	const Sphere c(-0.5,-0.3, 0.0, 0.1);
	draw_sphere(a, 3, 0);
	draw_sphere(b, 3, 0);
	draw_sphere(c, 3, 0);

	if(clo.isopt("--ts"))
	{
		Sphere d(0.0, 0.0, 0.7, 0.15);
		draw_sphere(d, 3, 0);
		std::cout << "color 1 1 0\n";
		const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
		for(std::size_t i=0;i<ts.size();i++)
		{
			draw_sphere(ts[0], 3, 0);
		}
	}

	if(clo.isopt("--dc"))
	{
		std::cout << "color 1 1 0\n";
		Sphere d(0.0, 0.0, 0.0, 0.15);
		for(d.z=-0.7;d.z<=0.7;d.z+=0.05)
		{
			const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
			for(std::size_t i=0;i<ts.size();i++)
			{
				draw_sphere(ts[0], 3, 0);
			}
		}
	}

	if(clo.isopt("--tp"))
	{
		std::cout << "color 0 1 0\n";
		const std::vector<Point> ps=apollo::construct_spheres_tangent_planes_normals<Point>(a, b, c);
		for(std::size_t i=0;i<ps.size();i++)
		{
			const Point& n=ps[i];
			Point plane[3]={apollo::custom_point_from_object<Point>(a)+(n*a.r), apollo::custom_point_from_object<Point>(b)+(n*b.r), apollo::custom_point_from_object<Point>(c)+(n*c.r)};
			const Point center=(plane[0]+plane[1]+plane[2])/3.0;
			for(int j=0;j<3;j++)
			{
				plane[j]=center+((plane[j]-center)*3);
			}
			draw_triangle(plane[0]+n*0.0001, plane[1]+n*0.0001, plane[2]+n*0.0001, n);
			const Point rn=Point()-n;
			draw_triangle(plane[0]+rn*0.0001, plane[1]+rn*0.0001, plane[2]+rn*0.0001, rn);
		}
	}

	std::cout << "flush\n";
	std::cout.flush();
}
