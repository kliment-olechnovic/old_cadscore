#ifndef SPHERES_BASIC_OPERATIONS_H_
#define SPHERES_BASIC_OPERATIONS_H_

#include "points_basic_operations.h"

namespace apollo
{

template<typename SphereType>
SphereType custom_sphere(const double x, const double y, const double z, const double r)
{
	SphereType result;
	result.x=x;
	result.y=y;
	result.z=z;
	result.r=r;
	return result;
}

template<typename SphereType, typename ObjectType>
SphereType custom_sphere_from_object(const ObjectType& o)
{
	return custom_point(o.x, o.y, o.z, o.r);
}

template<typename PointType, typename SphereType>
SphereType sphere_from_point_and_radius(const PointType& a, const double r)
{
	return custom_sphere(a.x, a.y, a.z, r);
}

template<typename SphereType>
bool spheres_equal(const SphereType& a, const SphereType& b)
{
	return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z) && equal(a.r, b.r));
}

template<typename SphereType>
SphereType zero_sphere()
{
	return custom_sphere<SphereType>(0, 0, 0, 0);
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

#endif /* SPHERES_BASIC_OPERATIONS_H_ */
