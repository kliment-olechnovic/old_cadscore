#ifndef SPHERES_H_
#define SPHERES_H_

#include <vector>
#include <algorithm>

namespace apollo
{

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

template<typename FirstType, typename SecondType, typename DistanceFunctor>
std::vector<std::size_t> sort_objects_by_distances(const FirstType& a, const SecondType& b, const DistanceFunctor& distance_functor)
{
	std::vector< std::pair<double, std::size_t> > distances;
	distances.reserve(b.size());
	for(std::size_t i=0;i<b.size();i++)
	{
		distances.push_back(std::make_pair(distance_functor(a, b[i]), i));
	}
	std::vector<std::size_t> result;
	result.reserve(distances.size());
	for(std::size_t i=0;i<distances.size();i++)
	{
		result.push_back(distances[i].second);
	}
	return result;
}

}

#endif /* SPHERES_H_ */
