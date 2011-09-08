#ifndef SPHERES_H_
#define SPHERES_H_

#include <vector>
#include <algorithm>

#include "safe_comparison.h"

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

template<typename PointType, typename SphereType>
double minimal_distance_from_point_to_sphere(const PointType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)-b.r);
}

template<typename PointType, typename SphereType>
double maximal_distance_from_point_to_sphere(const PointType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)+b.r);
}

template<typename SphereType>
double minimal_distance_from_sphere_to_sphere(const SphereType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)-a.r-b.r);
}

template<typename SphereType>
bool sphere_intersects_sphere(const SphereType& a, const SphereType& b)
{
	return less(minimal_distance_from_sphere_to_sphere(a,b), 0);
}

template<typename SphereType>
bool sphere_touches_sphere(const SphereType& a, const SphereType& b)
{
	return equal(minimal_distance_from_sphere_to_sphere(a,b), 0);
}

template<typename SphereType>
bool sphere_contains_sphere(const SphereType& a, const SphereType& b)
{
	return less(maximal_distance_from_point_to_sphere(a,b), a.r);
}

}

#endif /* SPHERES_H_ */
