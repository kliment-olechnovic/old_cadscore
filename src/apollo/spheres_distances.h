#ifndef SPHERES_H_
#define SPHERES_H_

#include <vector>

template<typename PointType>
double distance_from_point_to_point(const PointType& a, const PointType& b)
{
	const double dx=(a.x-b.x);
	const double dy=(a.y-b.y);
	const double dz=(a.z-b.z);
	return sqrt(dx*dx+dy*dy+dz*dz);
}

template<typename SphereType>
double minimal_distance_from_sphere_to_sphere(const SphereType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)-a.r-b.r);
}

template<typename PointType, typename SphereType>
double maximal_distance_from_point_to_sphere(const PointType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)+b.r);
}

template<typename SpheresContainerType>
std::vector< std::vector<std::size_t> > construct_graph_from_spheres_by_distance_threshold(const SpheresContainerType& spheres, const double treshold)
{
	std::vector< std::vector<std::size_t> > graph(spheres.size());
	for(std::size_t i=0;i<spheres.size();i++)
	{
		for(std::size_t j=i+1;j<spheres.size();j++)
		{
			if(minimal_distance_from_sphere_to_sphere(spheres[i], spheres[j])<=treshold)
			{
				graph[i].push_back(j);
				graph[j].push_back(i);
			}
		}
	}
	return graph;
}

#endif /* SPHERES_H_ */
