#ifndef APOLLO2_POINTS_BASIC_OPERATIONS_H_
#define APOLLO2_POINTS_BASIC_OPERATIONS_H_

#include <cmath>

#include "safe_comparison.h"

namespace apollo2
{

struct PODPoint
{
	double x;
	double y;
	double z;
};

template<typename OutputPointType>
OutputPointType custom_point(const double x, const double y, const double z)
{
	OutputPointType result;
	result.x=x;
	result.y=y;
	result.z=z;
	return result;
}

template<typename OutputPointType, typename InputObjectType>
OutputPointType custom_point_from_object(const InputObjectType& o)
{
	return custom_point<OutputPointType>(o.x, o.y, o.z);
}

template<typename InputPointTypeA, typename InputPointTypeB>
bool points_equal(const InputPointTypeA& a, const InputPointTypeB& b)
{
	return (equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z));
}

template<typename OutputPointType, typename InputPointType>
OutputPointType inverted_point(const InputPointType& a)
{
	return custom_point<OutputPointType>(0-a.x, 0-a.y, 0-a.z);
}

template<typename InputPointTypeA, typename InputPointTypeB>
double distance_from_point_to_point(const InputPointTypeA& a, const InputPointTypeB& b)
{
	const double dx=(a.x-b.x);
	const double dy=(a.y-b.y);
	const double dz=(a.z-b.z);
	return sqrt(dx*dx+dy*dy+dz*dz);
}

template<typename InputPointType>
double point_squared_module(const InputPointType& a)
{
	return (a.x*a.x+a.y*a.y+a.z*a.z);
}

template<typename InputPointType>
double point_module(const InputPointType& a)
{
	return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
}

template<typename InputPointTypeA, typename InputPointTypeB>
double dot_product(const InputPointTypeA& a, const InputPointTypeB& b)
{
	return (a.x*b.x+a.y*b.y+a.z*b.z);
}

template<typename OutputPointType, typename InputPointTypeA, typename InputPointTypeB>
OutputPointType cross_product(const InputPointTypeA& a, const InputPointTypeB& b)
{
	return custom_point<OutputPointType>(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

template<typename OutputPointType, typename InputPointType>
OutputPointType point_and_number_product(const InputPointType& a, const double k)
{
	return custom_point<OutputPointType>(a.x*k, a.y*k, a.z*k);
}

template<typename OutputPointType, typename InputPointType>
OutputPointType unit_point(const InputPointType& a)
{
	return point_and_number_product<OutputPointType>(a, 1/point_module(a));
}

template<typename OutputPointType, typename InputPointTypeA, typename InputPointTypeB>
OutputPointType sum_of_points(const InputPointTypeA& a, const InputPointTypeB& b)
{
	return custom_point<OutputPointType>(a.x+b.x, a.y+b.y, a.z+b.z);
}

template<typename OutputPointType, typename InputPointTypeA, typename InputPointTypeB>
OutputPointType sub_of_points(const InputPointTypeA& a, const InputPointTypeB& b)
{
	return custom_point<OutputPointType>(a.x-b.x, a.y-b.y, a.z-b.z);
}

template<typename OutputPointType, typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
OutputPointType plane_normal_from_three_points(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c)
{
	return unit_point<OutputPointType>(cross_product<PODPoint>(sub_of_points<PODPoint>(b, a), sub_of_points<PODPoint>(c, a)));
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
double signed_distance_from_point_to_plane(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputPointTypeC& x)
{
	return dot_product(unit_point<PODPoint>(plane_normal), sub_of_points<PODPoint>(x, plane_point));
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
int halfspace_of_point(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputPointTypeC& x)
{
	const double sd=signed_distance_from_point_to_plane(plane_point, plane_normal, x);
	if(sd>0)
	{
		return 1;
	}
	else if(sd<0)
	{
		return -1;
	}
	return 0;
}

struct SimplePoint
{
	double x;
	double y;
	double z;

	SimplePoint() : x(0), y(0), z(0)
	{
	}

	SimplePoint(const double x, const double y, const double z) : x(x), y(y), z(z)
	{
	}

	template<typename InputPointType>
	SimplePoint(const InputPointType& input_point) : x(input_point.x), y(input_point.y), z(input_point.z)
	{
	}

	SimplePoint operator+(const SimplePoint& b) const
	{
		return sum_of_points<SimplePoint>(*this, b);
	}

	SimplePoint operator-(const SimplePoint& b) const
	{
		return sub_of_points<SimplePoint>(*this, b);
	}

	double operator*(const SimplePoint& b) const
	{
		return dot_product(*this, b);
	}

	SimplePoint operator*(const double k) const
	{
		return point_and_number_product<SimplePoint>(*this, k);
	}

	double module() const
	{
		return point_module(*this);
	}

	SimplePoint unit() const
	{
		return unit_point<SimplePoint>(*this);
	}

	SimplePoint inverted() const
	{
		return inverted_point<SimplePoint>(*this);
	}
};

}

#endif /* APOLLO2_POINTS_BASIC_OPERATIONS_H_ */
