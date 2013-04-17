#ifndef APOLLO2_BASIC_OPERATIONS_ON_SPHERES_H_
#define APOLLO2_BASIC_OPERATIONS_ON_SPHERES_H_

#include "basic_operations_on_points.h"

namespace apollo2
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
	return custom_sphere<OutputSphereType>(o.x, o.y, o.z, o.r);
}

template<typename OutputSphereType, typename InputPointType>
OutputSphereType custom_sphere_from_point(const InputPointType& p, const double r)
{
	return custom_sphere<OutputSphereType>(p.x, p.y, p.z, r);
}

template<typename InputSphereTypeA, typename InputSphereTypeB>
bool spheres_equal(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z) && equal(a.r, b.r));
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

template<typename InputPointTypeA, typename InputPointTypeB, typename InputSphereTypeC>
int halfspace_of_sphere(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputSphereTypeC& x)
{
	const double dc=signed_distance_from_point_to_plane(plane_point, plane_normal, x);
	if(dc>0 && (dc-x.r>0))
	{
		return 1;
	}
	else if(dc<0 && (dc+x.r<0))
	{
		return -1;
	}
	return 0;
}

struct SimpleSphere
{
	double x;
	double y;
	double z;
	double r;

	SimpleSphere() : x(0), y(0), z(0), r(0)
	{
	}

	SimpleSphere(const double x, const double y, const double z, const double r) : x(x), y(y), z(z), r(r)
	{
	}

	template<typename InputSphereType>
	SimpleSphere(const InputSphereType& input_sphere) : x(input_sphere.x), y(input_sphere.y), z(input_sphere.z), r(input_sphere.r)
	{
	}

	template<typename InputPointType>
	SimpleSphere(const InputPointType& input_point, const double r) : x(input_point.x), y(input_point.y), z(input_point.z), r(r)
	{
	}

	bool operator==(const SimpleSphere& b) const
	{
		return spheres_equal(*this, b);
	}
};

}

#endif /* APOLLO2_BASIC_OPERATIONS_ON_SPHERES_H_ */