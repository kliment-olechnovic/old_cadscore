#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <map>
#include <cmath>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/graph_traversing.h"

template<typename ContainerType, typename OperatorType>
std::vector< std::vector<std::size_t> > construct_graph(const ContainerType& objects, const OperatorType& op)
{
	std::vector< std::vector<std::size_t> > graph(objects.size());
	for(std::size_t i=0;i<objects.size();i++)
	{
		for(std::size_t j=i+1;j<objects.size();j++)
		{
			if(op(objects[i], objects[j]))
			{
				graph[i].push_back(j);
				graph[j].push_back(i);
			}
		}
	}
	return graph;
}

template<typename SphereType>
struct DistanceBetweenSpheres
{
	typedef SphereType ObjectType;

	double operator()(const SphereType& a, const SphereType& b) const
	{
		const double dx=(a.x-b.x);
		const double dy=(a.y-b.y);
		const double dz=(a.z-b.z);
		return (sqrt(dx*dx+dy*dy+dz*dz)-a.r-b.r);
	}
};

template<typename OperatorType>
struct TresholdChecker
{
	typedef typename OperatorType::ObjectType ObjectType;

	TresholdChecker(const double treshold) : treshold(treshold)
	{
	}

	double operator()(const ObjectType& a, const ObjectType& b) const
	{
		return (OperatorType()(a, b)<=treshold);
	}

	double treshold;
};

template<typename Iterator>
void print_spheres(const Iterator first, const Iterator last)
{
	for(Iterator it=first;it!=last;++it)
	{
		std::cout << "SPHERE " << it->r << " " << it->x << " " << it->y << " " << it->z << "\n";
	}
}

template<typename SphereType>
double special_distance_between_spheres(const SphereType& a, const SphereType& b)
{
	const double dx=(a.x-b.x);
	const double dy=(a.y-b.y);
	const double dz=(a.z-b.z);
	return (sqrt(dx*dx+dy*dy+dz*dz)+b.r);
}

template<typename SphereType>
std::vector<SphereType> form_clusters(const std::vector<SphereType>& spheres, const std::vector<size_t> centers)
{
	std::vector<SphereType> clusters;
	for(std::size_t i=0;i<centers.size();i++)
	{
		clusters.push_back(spheres[centers[i]]);
	}
	for(std::size_t i=0;i<spheres.size();i++)
	{
		std::size_t min_dist_j=0;
		for(std::size_t j=1;j<clusters.size();j++)
		{
			if( special_distance_between_spheres(clusters[j], spheres[i]) <
					special_distance_between_spheres(clusters[min_dist_j], spheres[i]))
			{
				min_dist_j=j;
			}
		}
		clusters[min_dist_j].r=std::max(clusters[min_dist_j].r, special_distance_between_spheres(clusters[min_dist_j], spheres[i]));
	}
	return clusters;
}

int main()
{
	typedef Atom Sphere;
	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
	std::clog << spheres.size() << " spheres read\n";

	utils::BlockTimer* bt_ptr=new utils::BlockTimer("Preparation time");

	Graph graph=construct_graph(spheres, TresholdChecker< DistanceBetweenSpheres<Sphere> >(2.8));
	GraphTraverser gt(graph);

	delete bt_ptr;

	{
		utils::BlockTimer bt("Bucketing time");
		std::vector<std::size_t> buckets=subdivide_graph(graph, 2);
		std::clog << buckets.size() << " buckets\n";

		std::vector<Sphere> centers=form_clusters(spheres, buckets);
		std::cout << "LIST\n";
		print_spheres(centers.begin(), centers.end());
//		print_spheres(spheres.begin(), spheres.end());
	}

	return 0;
}
