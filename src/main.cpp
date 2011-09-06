#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <map>
#include <cmath>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/spheres_clustering.h"

template<typename Iterator>
void print_spheres(const Iterator first, const Iterator last)
{
	for(Iterator it=first;it!=last;++it)
	{
		std::cout << "SPHERE " << it->r << " " << it->x << " " << it->y << " " << it->z << "\n";
	}
}

int main()
{
	typedef Atom Sphere;
	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
	std::clog << spheres.size() << " spheres read\n";

	{
		utils::BlockTimer bt("Clustering time");

		std::vector< std::pair<Sphere, std::vector<std::size_t> > > clusters=apollo::cluster_spheres(spheres, 1.4*5);
		std::clog << clusters.size() << " clusters\n";

		std::vector<Sphere> clusters_spheres;
		for(std::size_t i=0;i<clusters.size();i++)
		{
			clusters_spheres.push_back(clusters[i].first);
		}

		std::cout << "LIST\n";
		print_spheres(clusters_spheres.begin(), clusters_spheres.end());
	}

//	print_spheres(spheres.begin(), spheres.end());

	return 0;
}
