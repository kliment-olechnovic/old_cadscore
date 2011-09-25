#ifndef POINTS_BASIC_OPERATIONS_H_
#define POINTS_BASIC_OPERATIONS_H_

#include <cmath>

#include "safe_comparison.h"

namespace apollo
{

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

template<typename OutputPointType>
OutputPointType zero_point()
{
	return custom_point<OutputPointType>(0, 0, 0);
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

struct SimplePoint
{
	double x;
	double y;
	double z;

	SimplePoint() : x(0), y(0), z(0) {}

	SimplePoint(const double x, const double y, const double z) : x(x), y(y), z(z) {}

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

	SimplePoint operator/(const double k) const
	{
		return point_and_number_product<SimplePoint>(*this, 1/k);
	}

	SimplePoint operator&(const SimplePoint& b) const
	{
		return cross_product<SimplePoint>(*this, b);
	}

	double module() const
	{
		return point_module(*this);
	}

	SimplePoint unit() const
	{
		return unit_point<SimplePoint>(*this);
	}
};

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC, typename InputPointTypeD>
double signed_tetrahedron_volume(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c, const InputPointTypeD& d)
{
	const SimplePoint da=sub_of_points<SimplePoint>(a, d);
	const SimplePoint db=sub_of_points<SimplePoint>(b, d);
	const SimplePoint dc=sub_of_points<SimplePoint>(c, d);
	return ((da*(db&dc))/6);
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC, typename InputPointTypeD>
double tetrahedron_volume(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c, const InputPointTypeD& d)
{
	return fabs(signed_tetrahedron_volume(a, b, c, d));
}

template<typename OutputPointType, typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
OutputPointType plane_normal_from_three_points(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c)
{
	const SimplePoint ab=sub_of_points<SimplePoint>(b, a);
	const SimplePoint ac=sub_of_points<SimplePoint>(c, a);
	return custom_point_from_object<OutputPointType>((ab&ac).unit());
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
double signed_distance_from_point_to_plane(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputPointTypeC& x)
{
	const SimplePoint n=unit_point<SimplePoint>(plane_normal);
	const SimplePoint v=sub_of_points<SimplePoint>(x, plane_point);
	return (n*v);
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
double distance_from_point_to_plane(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputPointTypeC& x)
{
	return signed_distance_from_point_to_plane(plane_point, plane_normal, x);
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
int halfspace(const InputPointTypeA& plane_point, const InputPointTypeB& plane_normal, const InputPointTypeC& x)
{
	const double sd=signed_distance_from_point_to_plane(plane_point, plane_normal, x);
	if(equal(sd, 0))
	{
		return 0;
	}
	else if(sd<0)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC, typename InputPointTypeD>
int halfspace(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c, const InputPointTypeD& d)
{
	return halfspace(a, plane_normal_from_three_points<SimplePoint>(a, b, c), d);
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
double project_point_on_vector(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c)
{
	const SimplePoint ab=sub_of_points<SimplePoint>(b, a);
	const SimplePoint ac=sub_of_points<SimplePoint>(c, a);
	return ab.unit()*ac;
}

template<typename InputPointTypeA, typename InputPointTypeB, typename InputPointTypeC>
double distance_from_point_to_line(const InputPointTypeA& a, const InputPointTypeB& b, const InputPointTypeC& c)
{
	const double vl=distance_from_point_to_point(a, c);
	const double pl=project_point_on_vector(a, b, c);
	return sqrt(vl*vl-pl*pl);
}

}

#endif /* POINTS_BASIC_OPERATIONS_H_ */
