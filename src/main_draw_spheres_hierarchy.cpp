#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

template<typename T>
void draw_sphere(const T& s, const int quality, const int wired)
{
	std::cout << "sphere " << s.x << " " << s.y << " " << s.z << " " << s.r << " " << quality << " " << wired << "\n";
}

void main_draw_spheres_hierarchy(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;

	const double radius=clo.arg<double>("--radius", 1);
	const std::size_t low_count=clo.arg<double>("--low-count", 0);

	auxiliaries::assert_file_header("atoms");
	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>();

	const Hierarchy hierarchy(atoms, radius, low_count);

	const std::vector< std::vector<apollo::SimpleSphere> > spheres_layers=hierarchy.get_spheres_layers();

	if(!spheres_layers.empty())
	{
		const apollo::SimpleSphere& s=spheres_layers.back().front();
		std::cout << "scale " << s.x << " " << s.y << " " << s.z << " " << s.r << "\n";
	}

	std::cout << "color 1 0 0\n";
	for(std::size_t i=0;i<atoms.size();i++)
	{
		draw_sphere(atoms[i], 2, 0);
	}
	std::cout << "flush\n";
	std::cout << "sleep 1000\n";
	std::cout.flush();

	std::cout << "color 1 1 1\n";
	for(std::size_t i=0;i<spheres_layers.size();i++)
	{
		std::cout << "$layer" << i << "\n";
		const std::vector<apollo::SimpleSphere>& spheres_layer=spheres_layers[i];
		for(std::size_t j=0;j<spheres_layer.size();j++)
		{
			draw_sphere(spheres_layer[j], 2, 1);
		}
		std::cout << "flush\n";
		std::cout << "sleep 1000\n";
		std::cout.flush();
	}
}
