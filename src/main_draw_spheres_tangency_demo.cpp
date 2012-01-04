#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_tangent_sphere.h"
#include "apollo/spheres_tangent_plane.h"
#include "apollo/spheres_tangent_stick.h"

#include "auxiliaries/command_line_options.h"

typedef apollo::SimpleSphere Sphere;
typedef apollo::SimplePoint Point;

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

std::pair< std::vector< std::vector<Point> >, std::vector< std::vector<Point> > > construct_Dupine_cyclide_circles(const Sphere& a, const Sphere& b, const Sphere& c)
{
	std::pair< std::vector< std::vector<Point> >, std::vector< std::vector<Point> > > result;
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

			result.first.push_back(std::vector<Point>());
			result.second.push_back(std::vector<Point>());
			{
				const double step=0.05;
				for(double l=step;l<=1.0;l+=step)
				{
					const Point rp=cc+(((t1+((t2-t1)*l))-cc).unit()*ccr);
					result.first.back().push_back(rp);
					result.second.back().push_back((rp-cc).unit());
				}
				for(double l=step;l<=1.0;l+=step)
				{
					const Point rp=cc+(((t2+((t3-t2)*l))-cc).unit()*ccr);
					result.first.back().push_back(rp);
					result.second.back().push_back((rp-cc).unit());
				}
				for(double l=step;l<=1.0;l+=step)
				{
					const Point rp=cc+(((t3+((t1-t3)*l))-cc).unit()*ccr);
					result.first.back().push_back(rp);
					result.second.back().push_back((rp-cc).unit());
				}
			}
		}
	}
	return result;
}

void main_draw_spheres_tangency_demo(const auxiliaries::CommandLineOptions& clo)
{
	std::cout << "background 1 1 1\n";

	const Sphere a( 0.0, 0.6, 0.0, 0.5);
	const Sphere b( 0.51,-0.3, 0.0, 0.4);
	const Sphere c(-0.5,-0.3, 0.0, 0.3);

	if(clo.isopt("--st"))
	{
		const int wired=clo.isopt("--st-wired") ? 1 : 0;
		std::cout << "$spheres_triple\n";
		std::cout << "color 0.7 0.8 1.0\n";
		draw_sphere(a, 3, wired);
		draw_sphere(b, 3, wired);
		draw_sphere(c, 3, wired);
	}

	if(clo.isopt("--ts1") || clo.isopt("--ts2"))
	{
		std::cout << "$tangent_sphere\n";
		Sphere d=clo.isopt("--ts1") ? Sphere(0.0, 0.0, 0.7, 0.1) : Sphere(0.0, 0.0, 0.1, 0.1);
		draw_sphere(d, 3, 0);
		std::cout << "color 1 1 0\n";
		const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
		for(std::size_t i=0;i<ts.size();i++)
		{
			draw_sphere(ts[i], 3, 0);
		}
	}

	if(clo.isopt("--trace"))
	{
		std::cout << "color 1 0 0\n";
		Sphere d(0.0, 0.0, 0.0, 0.15);
		for(d.z=-2.0;d.z<=2.0;d.z+=0.001)
		{
			const std::vector<Sphere> ts=apollo::construct_spheres_tangent<Sphere>(a, b, c, d);
			for(std::size_t i=0;i<ts.size();i++)
			{
				Sphere t=ts[i];
				t.r=0.02;
				draw_sphere(t, 3, 0);
			}
		}
	}

	if(clo.isopt("--dc"))
	{
		std::cout << "$Dupine_cyclide\n";
		std::cout << "color 1 0 0\n";
		const std::pair< std::vector< std::vector<Point> >, std::vector< std::vector<Point> > > cps=construct_Dupine_cyclide_circles(a, b, c);
		for(std::size_t i=0;i<cps.first.size();i++)
		{
			std::size_t u=i+1;
			for(std::size_t e=u+1;e<cps.first.size();e++)
			{
				if(e!=i && ((cps.first[e][0]-cps.first[i][0]).module()<(cps.first[u][0]-cps.first[i][0]).module()))
				{
					u=e;
				}
			}
			if(u<cps.first.size())
			{
				for(std::size_t j=0;j<cps.first[i].size();j++)
				{
					const std::size_t g=(j+1<cps.first[i].size()) ? j+1 : 0;

					std::cout << "triangle ";
					print_point(cps.first[i][j]);
					print_point(cps.first[i][g]);
					print_point(cps.first[u][g]);
					std::cout << "3 ";
					print_point(cps.second[i][j]);
					print_point(cps.second[i][g]);
					print_point(cps.second[u][g]);
					std::cout << "\n";

					std::cout << "triangle ";
					print_point(cps.first[i][j]);
					print_point(cps.first[u][j]);
					print_point(cps.first[u][g]);
					std::cout << "3 ";
					print_point(cps.second[i][j]);
					print_point(cps.second[u][j]);
					print_point(cps.second[u][g]);
					std::cout << "\n";

					std::cout << "triangle ";
					print_point(cps.first[i][j]-(cps.second[i][j]*0.0001));
					print_point(cps.first[i][g]-(cps.second[i][g]*0.0001));
					print_point(cps.first[u][g]-(cps.second[u][g]*0.0001));
					std::cout << "3 ";
					print_point(Point()-cps.second[i][j]);
					print_point(Point()-cps.second[i][g]);
					print_point(Point()-cps.second[u][g]);
					std::cout << "\n";

					std::cout << "triangle ";
					print_point(cps.first[i][j]-(cps.second[i][j]*0.0001));
					print_point(cps.first[u][j]-(cps.second[u][j]*0.0001));
					print_point(cps.first[u][g]-(cps.second[u][g]*0.0001));
					std::cout << "3 ";
					print_point(Point()-cps.second[i][j]);
					print_point(Point()-cps.second[u][j]);
					print_point(Point()-cps.second[u][g]);
					std::cout << "\n";
				}
			}
		}
	}

	if(clo.isopt("--tp"))
	{
		std::cout << "$tangent_planes\n";
		const std::vector<Point> ps=apollo::construct_spheres_tangent_planes_normals<Point>(a, b, c);
		for(std::size_t i=0;i<ps.size();i++)
		{
			const Point& n=ps[i];
			Point plane[3]={apollo::custom_point_from_object<Point>(a)+(n*a.r), apollo::custom_point_from_object<Point>(b)+(n*b.r), apollo::custom_point_from_object<Point>(c)+(n*c.r)};
			const Point center=(plane[0]+plane[1]+plane[2])/3.0;

			if(clo.isopt("--tp-points"))
			{
				std::cout << "color 0 1 0\n";
				for(int j=0;j<3;j++)
				{
					draw_sphere(apollo::custom_sphere_from_point<Sphere>(plane[j], 0.02), 3, 0);
				}
			}

			for(int j=0;j<3;j++)
			{
				plane[j]=center+((plane[j]-center)*3);
			}

			if(clo.isopt("--tp-blend"))
			{
				std::cout << "color 0 1 0 0.5\n";
				draw_triangle(plane[0], plane[1], plane[2], n);
			}
			else
			{
				std::cout << "color 0 1 0\n";
				draw_triangle(plane[0]+n*0.0001, plane[1]+n*0.0001, plane[2]+n*0.0001, n);
				const Point rn=Point()-n;
				draw_triangle(plane[0]+rn*0.0001, plane[1]+rn*0.0001, plane[2]+rn*0.0001, rn);
			}
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
