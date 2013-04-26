#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SHELL_FOR_TWO_SPHERES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SHELL_FOR_TWO_SPHERES_H_

#include "../basic_operations_on_spheres.h"

namespace apollo2
{

namespace apollonius_triangulation
{

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
bool sphere_is_inside_shell_of_two_spheres(const InputSphereTypeA& a, const InputSphereTypeB& b, const InputSphereTypeC& c)
{
	if(b.r<a.r)
	{
		return sphere_inside_shell_of_two_spheres(b, a, c);
	}

	if(c.r>b.r)
	{
		return false;
	}

	if(sphere_contains_sphere(b, a))
	{
		return sphere_contains_sphere(b, c);
	}

	if(sphere_contains_sphere(a, c) || sphere_contains_sphere(b, c))
	{
		return true;
	}

	const double distance_between_a_and_b_centers=distance_from_point_to_point(a, b);
	const double transformed_b_r=(b.r-a.r);
	const double tangent_line_length=sqrt(distance_between_a_and_b_centers*distance_between_a_and_b_centers-transformed_b_r*transformed_b_r);
	SimplePoint shell_normal(0, 1, 0);
	if(a.r!=b.r)
	{
		const double sin_value=(transformed_b_r/distance_between_a_and_b_centers);
		shell_normal.x=(0-(transformed_b_r*sin_value));
		shell_normal.y=(tangent_line_length*sin_value);
	}
	const SimplePoint tangent_line_unit_vector=(SimplePoint(distance_between_a_and_b_centers, 0, 0)+(shell_normal*transformed_b_r)).unit();
	const double transformed_c_x=(SimplePoint(c)-SimplePoint(a))*((SimplePoint(b)-SimplePoint(a)).unit());
	const double transformed_c_y=sqrt(squared_distance_from_point_to_point(a, c)-transformed_c_x*transformed_c_x);
	const SimplePoint transformed_c(transformed_c_x, transformed_c_y, 0);

	return (less((transformed_c*shell_normal)+(c.r-a.r), 0) && less(transformed_c*tangent_line_unit_vector, tangent_line_length));
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SHELL_FOR_TWO_SPHERES_H_ */
