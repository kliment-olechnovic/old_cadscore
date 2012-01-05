#include <iostream>
#include <cstdlib>

#include "apollo/spheres_hierarchy.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

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
	for(int i=0;i<200;i++)
	{
		spheres.push_back(Sphere(rand()%100, rand()%100, 0, rand()%5));
	}

	const Hierarchy hierarchy(spheres, radius, 0);

	const std::vector< std::vector<apollo::SimpleSphere> > spheres_layers=hierarchy.get_spheres_layers();

	if(!spheres_layers.empty())
	{
		const apollo::SimpleSphere& s=spheres_layers.back().front();
		std::cout << "scale " << s.x << " " << s.y << " " << s.z << " " << s.r << "\n";
	}

	std::cout << "background 1 1 1\n";

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
		const std::vector<apollo::SimpleSphere>& spheres_layer=spheres_layers[i];
		for(std::size_t j=0;j<spheres_layer.size();j++)
		{
			apollo::SimpleSphere s=spheres_layer[j];
			s.z=(i+1)*50;
			draw_circle(s, 1, 120);
		}
		std::cout << "flush\n";
		std::cout.flush();
	}
}
