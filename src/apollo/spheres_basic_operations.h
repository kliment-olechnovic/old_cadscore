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

template<typename InputPointTypeA, typename InputPointTypeB, typename InputSphereTypeC>
int halfspace_of_sphere(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputSphereTypeC& x)
{
	const SimplePoint sx=custom_point_from_object<SimplePoint>(x);
	const SimplePoint sn=custom_point_from_object<SimplePoint>(plane_normal).unit();
	const int halfspace_one=halfspace_of_point(plane_point, plane_normal, sx+(sn*x.r));
	const int halfspace_two=halfspace_of_point(plane_point, plane_normal, sx-(sn*x.r));
	if(halfspace_one!=halfspace_two)
	{
		return 0;
	}
	else
	{
		return halfspace_one;
	}
}

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_intersects_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	if(a.r>b.r)
	{
		return sphere_intersects_touching_cone(b, a, c);
	}

	if(sphere_intersects_sphere(a, c) || sphere_intersects_sphere(b, c))
	{
		return true;
	}
	else
	{
		const double stick_length=distance_from_point_to_point(a, b);
		const double distance_one=project_point_on_vector(a, b, c);
		if(greater(distance_one, 0) && less(distance_one, stick_length))
		{
			const double distance_two=distance_from_point_to_line(a, b, c)-c.r-a.r;
			return less(distance_two/distance_one, (b.r-a.r)/stick_length);
		}
		else
		{
			return false;
		}
	}
}

struct SimpleSphere
{
	double x;
	double y;
	double z;
	double r;

	SimpleSphere() : x(0), y(0), z(0), r(0) {}

	SimpleSphere(const double x, const double y, const double z, const double r) : x(x), y(y), z(z), r(r) {}
};

}

#endif /* SPHERES_BASIC_OPERATIONS_H_ */
