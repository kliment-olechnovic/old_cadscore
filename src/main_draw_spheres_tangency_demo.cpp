#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_tangent_sphere.h"
#include "apollo/spheres_tangent_plane.h"
#include "apollo/spheres_tangent_stick.h"

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

void draw_cone(const apollo::SimpleSphere& a, const apollo::SimpleSphere& b)
{
	typedef apollo::SimpleSphere Sphere;
	typedef apollo::SimplePoint Point;

	const std::pair<bool, Point> descriptor=apollo::construct_spheres_circles_tangent_line_normal(a, b);
	if(descriptor.first)
	{
		Point start=apollo::custom_point_from_object<Point>(a);
		double start_r=a.r;
		Point end=apollo::custom_point_from_object<Point>(b);
		double end_r=b.r;
		if(a.r>b.r)
		{
			std::swap(start, end);
			std::swap(start_r, end_r);
		}
		const Point face_start=start+((end-start).unit()*(descriptor.second.x*start_r));
		const double face_start_r=descriptor.second.y*start_r;
		const Point face_end=end+((end-start).unit()*(descriptor.second.x*end_r));
		const double face_end_r=descriptor.second.y*end_r;
		std::cout << "tube ";
		print_point(face_start);
		print_point(face_end);
		std::cout << face_start_r << " " << face_end_r << " 108 1\n";
	}
	else
	{
		std::cerr << "err\n";
	}
}

void main_draw_spheres_tangency_demo(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SimpleSphere Sphere;
	typedef apollo::SimplePoint Point;

	std::cout << "color 1 1 1\n";
	const Sphere a( 0.0, 0.6, 0.0, 0.5);
	const Sphere b( 0.51,-0.3, 0.0, 0.4);
	const Sphere c(-0.5,-0.3, 0.0, 0.3);
	draw_sphere(a, 3, 0);
	draw_sphere(b, 3, 0);
	draw_sphere(c, 3, 0);

	if(clo.isopt("--ts1") || clo.isopt("--ts2"))
	{
		std::cout << "$tangent_sphere\n";
		Sphere d=clo.isopt("--ts1") ? Sphere(0.0, 0.0, 0.7, 0.1) : Sphere(0.0, 0.0, 0.001, 0.1);
		draw_sphere(d, 3, 0);
		std::cout << "color 1 1 0\n";
		const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
		for(std::size_t i=0;i<ts.size();i++)
		{
			draw_sphere(ts[i], 3, 0);
		}
	}

	if(clo.isopt("--dc"))
	{
		std::cout << "$Dupine_cyclide\n";
		Sphere d(0.0, 0.0, 0.0, 0.15);
		const Point ap=apollo::custom_point_from_object<Point>(a);
		const Point bp=apollo::custom_point_from_object<Point>(b);
		const Point cp=apollo::custom_point_from_object<Point>(c);
		for(d.z=-2.0;d.z<=2.0;d.z+=0.01)
		{
			const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
			for(std::size_t i=0;i<ts.size();i++)
			{
				const Sphere& t=ts[i];
				const Point tp=apollo::custom_point_from_object<Point>(t);
				const Point t1=(tp+((ap-tp).unit()*t.r));
				const Point t2=(tp+((bp-tp).unit()*t.r));
				const Point t3=(tp+((cp-tp).unit()*t.r));

//				std::cout << "color 1 1 0\n";
//				{
//					std::cout << "line ";
//					print_point(t1);
//					print_point(t2);
//					std::cout << "\n";
//					std::cout << "line ";
//					print_point(t2);
//					print_point(t3);
//					std::cout << "\n";
//					std::cout << "line ";
//					print_point(t3);
//					print_point(t1);
//					std::cout << "\n";
//				}

				Point cc;
				double err=((t1-cc).module()-(t2-cc).module())*((t1-cc).module()-(t2-cc).module())+
						((t1-cc).module()-(t3-cc).module())*((t1-cc).module()-(t3-cc).module())+
						((t2-cc).module()-(t3-cc).module())*((t2-cc).module()-(t3-cc).module());
				{
					const double step=0.005;
					for(double rnd1=0.0;rnd1<=1.0;rnd1+=step)
					{
						for(double rnd2=0.0;rnd2<=1.0;rnd2+=step)
						{
							const Point ncc=(t1+((t2-t1)*rnd1))+((t3-t2)*rnd2);
							double nerr=((t1-ncc).module()-(t2-ncc).module())*((t1-ncc).module()-(t2-ncc).module())+
									((t1-ncc).module()-(t3-ncc).module())*((t1-ncc).module()-(t3-ncc).module())+
									((t2-ncc).module()-(t3-ncc).module())*((t2-ncc).module()-(t3-ncc).module());
							if(nerr<err)
							{
								cc=ncc;
								err=nerr;
							}
						}
					}
				}
				const double ccr=((t1-cc).module()+(t2-cc).module()+(t3-cc).module())/3.0;

				std::cout << "color 1 0 0\n";
				{
					const double step=0.05;
					for(double l=step;l<1.0+step;l+=step)
					{
						std::cout << "line ";
						print_point(cc+(((t1+((t2-t1)*(l-step)))-cc).unit()*ccr));
						print_point(cc+(((t1+((t2-t1)*(l-0.0)))-cc).unit()*ccr));
						std::cout << "\n";

						std::cout << "line ";
						print_point(cc+(((t2+((t3-t2)*(l-step)))-cc).unit()*ccr));
						print_point(cc+(((t2+((t3-t2)*(l-0.0)))-cc).unit()*ccr));
						std::cout << "\n";

						std::cout << "line ";
						print_point(cc+(((t1+((t3-t1)*(l-step)))-cc).unit()*ccr));
						print_point(cc+(((t1+((t3-t1)*(l-0.0)))-cc).unit()*ccr));
						std::cout << "\n";
					}
				}
			}
		}
	}

	if(clo.isopt("--tp"))
	{
		std::cout << "$tangent_planes\n";
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

	if(clo.isopt("--tc"))
	{
		std::cout << "$tangent_cones\n";
		std::cout << "color 0 1 1\n";
		draw_cone(a, b);
		draw_cone(a, c);
		draw_cone(b, c);
	}

	std::cout << "flush\n";
	std::cout.flush();
}
