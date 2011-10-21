#ifndef APOLLO_SPHERES_TANGENT_STICK_H_
#define APOLLO_SPHERES_TANGENT_STICK_H_

#include <utility>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename InputSphereTypeA, typename InputSphereTypeB>
std::pair<bool, SimplePoint> construct_spheres_circles_tangent_line_normal(const InputSphereTypeA& a, const InputSphereTypeB& b)
{
	if(a.r>b.r)
	{
		return construct_spheres_circles_tangent_line_normal(b, a);
	}

	if(sphere_contains_sphere(b, a))
	{
		return std::make_pair(false, SimplePoint());
	}

	const double r=b.r-a.r;
	if(greater(r, 0))
	{
		const double l=distance_from_point_to_point(a, b);
		const double m=sqrt(l*l-r*r);
		const double sin_a=r/l;
		const double cos_a=sqrt(1-sin_a*sin_a);
		return std::make_pair(true, SimplePoint(m*cos_a-l, m*sin_a, 0).unit());
	}
	else
	{
		return std::make_pair(true, SimplePoint(0, 1, 0));
	}
}

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_intersects_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c, const bool fully)
{
	if(a.r>b.r)
	{
		return stick_intersects_sphere(b, a, c, fully);
	}

	if(sphere_contains_sphere(b, a))
	{
		if(fully)
		{
			return sphere_contains_sphere(b, c);
		}
		else
		{
			return sphere_intersects_sphere(b, c);
		}
	}

	if(fully)
	{
		if(sphere_contains_sphere(b, c) || sphere_contains_sphere(a, c))
		{
			return true;
		}
	}
	else
	{
		if(sphere_intersects_sphere(b, c) || sphere_intersects_sphere(a, c))
		{
			return true;
		}
	}

	std::pair<bool, SimplePoint> normal_result=construct_spheres_circles_tangent_line_normal(a, b);
	if(normal_result.first)
	{
		const SimplePoint& normal=normal_result.second;
		const SimplePoint c_location(project_point_on_vector(a, b, c), distance_from_point_to_line(a, b, c), 0);
		const SimplePoint a_touch=normal*a.r;
		const SimplePoint b_touch=SimplePoint(distance_from_point_to_point(a, b), 0, 0)+(normal*b.r);

		if(greater_or_equal(project_point_on_vector(b_touch, a_touch, c_location), 0)
				&& greater_or_equal(project_point_on_vector(a_touch, b_touch, c_location), 0))
		{
			if(fully)
			{
				if(less_or_equal(project_point_on_vector(a_touch, a_touch+normal, c_location), 0)
						&& greater_or_equal(distance_from_point_to_line(a_touch, b_touch, c_location), c.r))
				{
					return true;
				}
			}
			else
			{
				if(less_or_equal(project_point_on_vector(a_touch, a_touch+normal, c_location), 0)
						|| less_or_equal(distance_from_point_to_line(a_touch, b_touch, c_location), c.r))
				{
					return true;
				}
			}
		}
	}

	return false;
}

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_contains_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	return stick_intersects_sphere(b, a, c, true);
}

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_intersects_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	return stick_intersects_sphere(b, a, c, false);
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

#endif /* APOLLO_SPHERES_TANGENT_STICK_H_ */
