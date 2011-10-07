#ifndef SPHERES_TANGENT_STICK_H_
#define SPHERES_TANGENT_STICK_H_

#include <utility>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_contains_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	if(a.r>b.r)
	{
		return stick_contains_sphere(b, a, c);
	}

	if(sphere_contains_sphere(b, a))
	{
		return sphere_contains_sphere(b, c);
	}
	else if(sphere_contains_sphere(a, c) || sphere_contains_sphere(b, c))
	{
		return true;
	}
	else
	{
		const double length=distance_from_point_to_point(a, b);
		const double distance_x=project_point_on_vector(a, b, c);
		if(greater(distance_x, 0) && less(distance_x, length))
		{
			const double distance_y=distance_from_point_to_line(a, b, c);
			const SimplePoint c_location(distance_x, distance_y, 0);
			const double r=b.r-a.r;
			if(greater(r, 0))
			{
				const double m=sqrt(length*length-r*r);
				const double sin_a=r/length;
				const double ry=m*sin_a;
				const double cos_a=sqrt(1-sin_a*sin_a);
				const double rx=m*cos_a-length;
				const SimplePoint normal=SimplePoint(rx, ry, 0).unit();
				return greater_or_equal(distance_from_point_to_line(normal*a.r, SimplePoint(length, 0, 0)+(normal*b.r), c_location), c.r);
			}
			else
			{
				const SimplePoint normal(0, 1, 0);
				return greater_or_equal(distance_from_point_to_line(normal*a.r, SimplePoint(length, 0, 0)+(normal*b.r), c_location), c.r);
			}
		}
		else
		{
			return false;
		}
	}
}

template<typename InputSphereType>
std::pair<const InputSphereType*, const InputSphereType*> select_tangent_stick(const InputSphereType& a, const InputSphereType& b, const InputSphereType& c)
{
	if(stick_contains_sphere(a, b, c))
	{
		return std::make_pair(&a, &b);
	}
	else if(stick_contains_sphere(a, c, b))
	{
		return std::make_pair(&a, &c);
	}
	else if(stick_contains_sphere(b, c, a))
	{
		return std::make_pair(&b, &c);
	}
	else
	{
		return std::pair<const InputSphereType*, const InputSphereType*>(NULL, NULL);
	}
}

}

#endif /* SPHERES_TANGENT_STICK_H_ */
