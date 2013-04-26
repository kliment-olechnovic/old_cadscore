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
		return sphere_is_inside_shell_of_two_spheres(b, a, c);
	}

	if(!less(c.r, b.r))
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

//	return (sqrt(squared_distance_from_point_to_point(a, b)-(b.r-a.r)*(b.r-a.r))>(sqrt(squared_distance_from_point_to_point(a, c)-(a.r-c.r)*(a.r-c.r))+sqrt(squared_distance_from_point_to_point(b, c)-(b.r-c.r)*(b.r-c.r))));

	const SimplePoint tb(distance_from_point_to_point(a, b), 0, 0);
	const double tb_r=(b.r-a.r);

	const double tangent_line_length=sqrt(tb.x*tb.x-tb_r*tb_r);

	SimplePoint shell_normal(0, 1, 0);
	if(greater(tb_r, 0))
	{
		const double sin_value=(tb_r/tb.x);
		shell_normal=SimplePoint((0-(tb_r*sin_value)), (tangent_line_length*sin_value), 0)*(1/tb_r);
	}

	const SimplePoint tangent_line_unit_vector=(tb+(shell_normal*tb_r)).unit();

	const double tc_x=(sub_of_points<SimplePoint>(c, a))*(sub_of_points<SimplePoint>(b, a).unit());
	const double tc_y=sqrt(squared_distance_from_point_to_point(a, c)-tc_x*tc_x);
	const SimplePoint tc(tc_x, tc_y, 0);
	const SimplePoint mtc=(tc-(shell_normal*a.r));

	const double distance_from_c_to_tangent_line=(mtc*shell_normal)+c.r;
	const double projection_of_c_on_tangent_line=mtc*tangent_line_unit_vector;

	return (less(distance_from_c_to_tangent_line, 0) && greater(projection_of_c_on_tangent_line, 0) && less(projection_of_c_on_tangent_line, tangent_line_length));
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SHELL_FOR_TWO_SPHERES_H_ */
