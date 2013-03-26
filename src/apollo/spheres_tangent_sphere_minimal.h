#ifndef APOLLO_SPHERES_TANGENT_SPHERE_MINIMAL_H_
#define APOLLO_SPHERES_TANGENT_SPHERE_MINIMAL_H_

#include <vector>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC, typename InputSphereTypeD>
bool check_spheres_tangent_sphere_minimal(const InputSphereTypeA& s1, const InputSphereTypeB& s2, const InputSphereTypeC& s3, const InputSphereTypeD& tangent_sphere_minimal)
{
	return (sphere_touches_sphere(tangent_sphere_minimal, s1) &&
			sphere_touches_sphere(tangent_sphere_minimal, s2) &&
			sphere_touches_sphere(tangent_sphere_minimal, s3) &&
			equal(tetrahedron_volume(s1, s2, s3, tangent_sphere_minimal), 0));
}

template<typename OutputSphereType, typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
inline std::vector<OutputSphereType> construct_spheres_tangent_sphere_minimal(const InputSphereTypeA& sm, const InputSphereTypeB& s1, const InputSphereTypeC& s2)
{
	{
		const double min_r=std::min(sm.r, std::min(s1.r, s2.r));
		if(sm.r!=min_r)
		{
			if(s1.r==min_r) return construct_spheres_tangent_sphere_minimal<OutputSphereType>(s1, sm, s2);
			if(s2.r==min_r) return construct_spheres_tangent_sphere_minimal<OutputSphereType>(s2, sm, s1);
		}
	}

	const double x1=distance_from_point_to_point(sm, s1);
	const double y1=0;
	const double r1=s1.r-sm.r;

	const double x2=project_point_on_vector(sm, s1, s2);
	const double y2=sqrt(point_squared_module(sub_of_points<SimplePoint>(s2, sm))-x2*x2);
	const double r2=s2.r-sm.r;

	const double a1=2*x1;
	const double b1=2*y1;
	const double d1=2*r1;
	const double o1=(r1*r1-x1*x1-y1*y1);

	const double a2=2*x2;
	const double b2=2*y2;
	const double d2=2*r2;
	const double o2=(r2*r2-x2*x2-y2*y2);

	const double w  = a2*b1-a1*b2;

	const double u1 = (b2*d1-b1*d2) / w;
	const double v1 = (b2*o1-b1*o2) / w;

	const double u2 = -(a2*d1-a1*d2) / w;
	const double v2 = -(a2*o1-a1*o2) / w;

	const double a = u1*u1+u2*u2-1;
	const double b = 2*(u1*v1+u2*v2);
	const double c = v1*v1+v2*v2;

	const double D = b*b-4*a*c;

	std::vector<double> radiuses;
	if(greater_or_equal(D, 0)) {radiuses.push_back((-b-sqrt(fabs(D)))/(2*a));}
	if(greater(D, 0))  {radiuses.push_back((-b+sqrt(fabs(D)))/(2*a));}

	std::vector<OutputSphereType> results;
	results.reserve(radiuses.size());
	for(std::size_t i=0;i<radiuses.size();i++)
	{
		const double r=radiuses[i];
		if(r>0)
		{
			const double virtual_x=u1*r+v1;
			const double virtual_y=u2*r+v2;
			const double real_l1_offset=virtual_y*x2/y2;
			const double real_l2=sqrt(real_l1_offset*real_l1_offset+virtual_y*virtual_y);
			const double real_l1=virtual_x-real_l1_offset;
			const OutputSphereType candidate=custom_sphere_from_point<OutputSphereType>(
					custom_point_from_object<SimplePoint>(sm)+(sub_of_points<SimplePoint>(s1, sm).unit()*real_l1)+(sub_of_points<SimplePoint>(s2, sm).unit()*real_l2),
					(r-sm.r));
			if(check_spheres_tangent_sphere_minimal(sm, s1, s2, candidate))
			{
				results.push_back(candidate);
			}
		}
	}

	return results;
}

}

#endif /* APOLLO_SPHERES_TANGENT_SPHERE_MINIMAL_H_ */
