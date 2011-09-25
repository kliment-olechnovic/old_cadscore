#ifndef SPHERES_BASIC_OPERATIONS_H_
#define SPHERES_BASIC_OPERATIONS_H_

#include <utility>

#include "points_basic_operations.h"

namespace apollo
{

template<typename OutputSphereType>
OutputSphereType custom_sphere(const double x, const double y, const double z, const double r)
{
	OutputSphereType result;
	result.x=x;
	result.y=y;
	result.z=z;
	result.r=r;
	return result;
}

template<typename OutputSphereType, typename InputObjectType>
OutputSphereType custom_sphere_from_object(const InputObjectType& o)
{
	return custom_point(o.x, o.y, o.z, o.r);
}

template<typename InputSphereTypeA, typename InputSphereTypeB>
bool spheres_equal(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z) && equal(a.r, b.r));
}

template<typename OutputSphereType>
OutputSphereType zero_sphere()
{
	return custom_sphere<OutputSphereType>(0, 0, 0, 0);
}

template<typename InputPointType, typename InputSphereType>
double minimal_distance_from_point_to_sphere(const InputPointType& a, const InputSphereType& b)
{
	return (distance_from_point_to_point(a, b)-b.r);
}

template<typename InputPointType, typename InputSphereType>
double maximal_distance_from_point_to_sphere(const InputPointType& a, const InputSphereType& b)
{
	return (distance_from_point_to_point(a, b)+b.r);
}

template<typename InputSphereTypeA,typename InputSphereTypeB>
double minimal_distance_from_sphere_to_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return (distance_from_point_to_point(a, b)-a.r-b.r);
}

template<typename InputSphereTypeA, typename InputSphereTypeB>
bool sphere_intersects_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return less(minimal_distance_from_sphere_to_sphere(a,b), 0);
}

template<typename InputSphereTypeA, typename InputSphereTypeB>
bool sphere_touches_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return equal(minimal_distance_from_sphere_to_sphere(a,b), 0);
}

template<typename InputSphereTypeA, typename InputSphereTypeB>
bool sphere_contains_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return less_or_equal(maximal_distance_from_point_to_sphere(a,b), a.r);
}

template<typename OutputPointType, typename InputSphereTypeA, typename InputSphereTypeB>
OutputPointType spheres_touching_point(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	const SimplePoint ap=custom_point_from_object<SimplePoint>(a);
	const SimplePoint bp=custom_point_from_object<SimplePoint>(b);
	return custom_point_from_object<OutputPointType>(ap+((bp-ap).unit()*a.r));
}

}

#endif /* SPHERES_BASIC_OPERATIONS_H_ */
