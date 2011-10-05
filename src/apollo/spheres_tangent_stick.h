#ifndef SPHERES_TANGENT_STICK_H_
#define SPHERES_TANGENT_STICK_H_

#include <utility>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_intersects_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	if(a.r>b.r)
	{
		return stick_intersects_sphere(b, a, c);
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

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool stick_contains_sphere(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	if(a.r>b.r)
	{
		return stick_contains_sphere(b, a, c);
	}

	if(sphere_contains_sphere(a, c) || sphere_contains_sphere(b, c))
	{
		return true;
	}
	else
	{
		const double stick_length=distance_from_point_to_point(a, b);
		const double distance_one=project_point_on_vector(a, b, c);
		if(greater(distance_one, 0) && less(distance_one, stick_length))
		{
			const double distance_two=distance_from_point_to_line(a, b, c)+c.r-a.r;
			return less(distance_two/distance_one, (b.r-a.r)/stick_length);
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
