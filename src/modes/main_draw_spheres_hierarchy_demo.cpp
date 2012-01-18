#include <iostream>
#include <cstdlib>

#include "../apollo/spheres_hierarchy.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"

template<typename T>
void draw_circle(const T& s, const double border, const int quality)
{
	std::cout << "circle " << s.x << " " << s.y << " " << s.z << " 0 0 1 " << std::max(0.0, (s.r-border)) << " " << s.r << " " << quality << "\n";
}

void main_draw_spheres_hierarchy_demo(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SimpleSphere Sphere;
	typedef apollo::SpheresHierarchy<apollo::SimpleSphere> Hierarchy;

	const std::size_t max_level=clo.arg<std::size_t>("--max-level", 1);

	const double radius=10;

	std::vector<Sphere> spheres;
	srand(1);
	for(int i=0;i<300;i++)
	{
		spheres.push_back(Sphere(rand()%100, rand()%100, 0, rand()%5));
	}

	const Hierarchy hierarchy(spheres, radius, 0);

	const std::vector< std::vector< std::pair<Sphere, std::vector<std::size_t> > > > spheres_layers=hierarchy.get_clusters_layers();

	if(!spheres_layers.empty())
	{
		const apollo::SimpleSphere& s=spheres_layers.back().front().first;
		std::cout << "scale " << s.x << " " << s.y << " " << s.z << " " << s.r << "\n";
	}

	std::cout << "background 1 1 1\n";

	if(clo.isopt("--single"))
	{
		const std::vector< std::pair<Sphere, std::vector<std::size_t> > >& spheres_layer=spheres_layers[0];
		for(std::size_t j=0;j<spheres_layer.size();j++)
		{
			apollo::SimpleSphere cs=spheres_layer[j].first;
			cs.z=0.0-(j+1)*3;
			std::cout << "color 0.8 1 0.8\n";
			draw_circle(cs, cs.r, 120);
			std::cout << "color 0.4 1 0.5\n";
			cs.z+=0.001;
			draw_circle(cs, 1, 120);
			for(std::size_t i=0;i<spheres_layer[j].second.size();i++)
			{
				Sphere s=spheres[spheres_layer[j].second[i]];
				s.z=cs.z+0.001;
				std::cout << "color 0.4 0.6 1.0\n";
				draw_circle(s, s.r, 36);
				std::cout << "color 0.2 0.2 0.2\n";
				s.z+=0.001;
				draw_circle(s, 0.5, 120);
			}
		}
	}
	else
	{
		for(std::size_t i=0;i<spheres.size();i++)
		{
			Sphere s=spheres[i];
			std::cout << "color 0.4 0.6 1.0\n";
			draw_circle(s, s.r, 36);
			std::cout << "color 0.2 0.2 0.2\n";
			s.z+=0.001;
			draw_circle(s, 0.5, 120);
		}
		std::cout << "flush\n";
		std::cout.flush();
		std::cout << "color 0 0 0\n";
		for(std::size_t i=0;i<spheres_layers.size() && i<max_level;i++)
		{
			std::cout << "$layer" << i << "\n";
			if(i==0)
			{
				std::cout << "color 0.4 1 0.5\n";
			}
			else
			{
				std::cout << "ncolor " << (i*10) << "\n";
			}
			const std::vector< std::pair<Sphere, std::vector<std::size_t> > >& spheres_layer=spheres_layers[i];
			for(std::size_t j=0;j<spheres_layer.size();j++)
			{
				apollo::SimpleSphere s=spheres_layer[j].first;
				s.z=(i+1)*70;
				draw_circle(s, 1, 120);
			}
		}
	}
	std::cout << "flush\n";
	std::cout.flush();
}
