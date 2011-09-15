#ifndef POINTS_BASIC_OPERATIONS_H_
#define POINTS_BASIC_OPERATIONS_H_

#include <cmath>

#include "safe_comparison.h"

namespace apollo
{

template<typename PointType>
PointType custom_point(const double x, const double y, const double z)
{
	PointType result;
	result.x=x;
	result.y=y;
	result.z=z;
	return result;
}

template<typename PointType, typename ObjectType>
PointType custom_point_from_object(const ObjectType& o)
{
	return custom_point<PointType>(o.x, o.y, o.z);
}

template<typename PointType>
bool points_equal(const PointType& a, const PointType& b)
{
	return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z));
}

template<typename PointType>
PointType zero_point()
{
	return custom_point<PointType>(0, 0, 0);
}

template<typename PointType>
PointType inverted_point(const PointType& a)
{
	return custom_point<PointType>(0-a.x, 0-a.y, 0-a.z);
}

template<typename PointType>
double distance_from_point_to_point(const PointType& a, const PointType& b)
{
	const double dx=(a.x-b.x);
	const double dy=(a.y-b.y);
	const double dz=(a.z-b.z);
	return sqrt(dx*dx+dy*dy+dz*dz);
}

template<typename PointType>
double point_module(const PointType& a)
{
	return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
}

template<typename PointType>
double dot_product(const PointType& a, const PointType& b)
{
	return (a.x*b.x+a.y*b.y+a.z*b.z);
}

template<typename PointType>
PointType cross_product(const PointType& a, const PointType& b)
{
	return custom_point<PointType>(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

template<typename PointType>
PointType point_and_number_product(const PointType& a, const double k)
{
	return custom_point<PointType>(a.x*k, a.y*k, a.z*k);
}

template<typename PointType>
PointType unit_point(const PointType& a)
{
	return point_and_number_product(point_module(a), 1/point_module(a));
}

template<typename PointType>
PointType sum_of_points(const PointType& a, const PointType& b)
{
	return custom_point<PointType>(a.x+b.x, a.y+b.y, a.z*+b.z);
}

template<typename PointType>
PointType sub_of_points(const PointType& a, const PointType& b)
{
	return custom_point<PointType>(a.x-b.x, a.y-b.y, a.z-b.z);
}

template<typename PointType>
PointType plane_normal_from_three_points(const PointType& a, const PointType& b, const PointType& c)
{
	return unit_point(cross_product(sub_of_points(b, a), sub_of_points(c, a)));
}

template<typename PointType>
double signed_volume(const PointType& a, const PointType& b, const PointType& c, const PointType& d)
{
	const PointType sa=sub_of_points(a, d);
	const PointType sb=sub_of_points(b, d);
	const PointType sc=sub_of_points(c, d);
	return (dot_product(sa, cross_product(sb, sc))/6);
}

template<typename PointType>
int halfspace(const PointType& a, const PointType& b, const PointType& c, const PointType& d)
{
	const double v=signed_volume(a, b, c, d);
	if(equal(v, 0))
	{
		return 0;
	}
	else if(v<0)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

}

#endif /* POINTS_BASIC_OPERATIONS_H_ */
