#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <map>
#include <cmath>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/graph_traversing.h"
#include "apollo/spheres_distances.h"

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

	utils::BlockTimer* bt_ptr=new utils::BlockTimer("Preparation time");

	Graph graph=construct_graph_from_spheres_by_distance_threshold(spheres, 2.8);
	GraphTraverser gt(graph);

	delete bt_ptr;

	{
		utils::BlockTimer bt("Bucketing time");
		std::vector<std::size_t> buckets=subdivide_graph(graph, 2);
		std::clog << buckets.size() << " buckets\n";

		std::vector<Sphere> centers;
		for(std::size_t i=0;i<buckets.size();i++)
		{
			centers.push_back(spheres[buckets[i]]);
		}

		std::vector< std::pair<Sphere, std::vector<std::size_t> > > clusters=form_clusters_from_spheres_using_centers(spheres, centers);

		std::vector<Sphere> clusters_spheres;
		for(std::size_t i=0;i<clusters.size();i++)
		{
			clusters_spheres.push_back(clusters[i].first);
		}

		std::cout << "LIST\n";
		print_spheres(clusters_spheres.begin(), clusters_spheres.end());
//		print_spheres(spheres.begin(), spheres.end());
	}

	return 0;
}
