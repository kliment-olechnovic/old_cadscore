#include <iostream>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/apollonius_graph.h"

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
		utils::BlockTimer bt("Clustering time");

		std::vector<apollo::spheres_clustering<Sphere>::ClustersLayer> clusters_layers=apollo::spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, 1.4*5, 100);
		std::clog << clusters_layers.size() << " clusters layers\n";
		for(std::size_t i=0;i<clusters_layers.size();i++)
		{
			std::clog << clusters_layers[i].size() << " clusters in layer " << i << "\n";
		}

		std::vector<Sphere> clusters_spheres;
		for(std::size_t i=0;i<clusters_layers.back().size();i++)
		{
			clusters_spheres.push_back(clusters_layers.back()[i].first);
		}

//		std::cout << "LIST\n";
//		print_spheres(clusters_spheres.begin(), clusters_spheres.end());
	}

	{
		utils::BlockTimer bt("Apollo time");
		std::pair< apollo::Quadruple, Sphere > first_quadruple=apollo::apollonius_graph<Sphere>::find_first_quadruple(spheres, 1.4*5, 100);

		std::cout << "LIST\n";
		print_sphere(first_quadruple.second);
		for(int i=0;i<4;i++)
		{
			print_sphere(spheres[first_quadruple.first.get(i)]);
		}
	}

	return 0;
}
