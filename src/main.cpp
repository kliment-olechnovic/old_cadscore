#include <iostream>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/apollonius_graph.h"

#include "apollo/apollonius_triangulation.h"

template<typename SphereType>
void print_sphere(const SphereType& sphere)
{
	std::cout << "SPHERE " << sphere.r << " " << sphere.x << " " << sphere.y << " " << sphere.z << "\n";
}

template<typename Iterator>
void print_spheres(const Iterator first, const Iterator last)
{
	for(Iterator it=first;it!=last;++it)
	{
		print_sphere(*it);
	}
}

int main()
{
	typedef Atom Sphere;
	std::vector<Sphere> spheres;

	{
		utils::BlockTimer bt("Reading time");
		spheres=read_atoms_from_PDB_file_stream(std::cin);
		std::clog << spheres.size() << " spheres read\n";
	}

	{
		utils::BlockTimer bt("First Apollo time");
		apollo::apollonius_graph<Sphere>::QuadruplesMap quadruples_map=apollo::apollonius_graph<Sphere>(spheres, 1.4*3, 50).find_quadruples();
		std::clog << quadruples_map.size() << " quadruples found\n";
	}

	{
		utils::BlockTimer bt("Second Apollo time");
		typedef apollo::SpheresHierarchy<Sphere> Hierarchy;
		const Hierarchy hierarchy(spheres, 1.4*3, 50);
		typedef apollo::ApolloniusTriangulation< apollo::SpheresHierarchy<Sphere> > Apollo;
		Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy);
		std::clog << quadruples_map.size() << " quadruples found\n";
	}

	return 0;
}
