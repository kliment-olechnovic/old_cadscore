#ifndef APOLLO_SPHERES_TANGENT_SPHERE_BY_RADIUS_H_
#define APOLLO_SPHERES_TANGENT_SPHERE_BY_RADIUS_H_

#include <vector>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC, typename InputSphereTypeD>
bool check_spheres_tangent_sphere_by_radius(const InputSphereTypeA& s1, const InputSphereTypeB& s2, const InputSphereTypeC& s3, const double custom_tangent_sphere_radius, const InputSphereTypeD& tangent_sphere)
{
	return (equal(tangent_sphere.r, custom_tangent_sphere_radius) &&
			sphere_touches_sphere(tangent_sphere, s1) &&
			sphere_touches_sphere(tangent_sphere, s2) &&
			sphere_touches_sphere(tangent_sphere, s3));
}

template<typename OutputSphereType, typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
inline std::vector<OutputSphereType> construct_spheres_tangent_sphere_by_radius(const InputSphereTypeA& sm, const InputSphereTypeB& s1, const InputSphereTypeC& s2, const double custom_tangent_sphere_radius)
{
	{
		const double min_r=std::min(sm.r, std::min(s1.r, s2.r));
		if(sm.r!=min_r)
		{
			if(s1.r==min_r) return construct_spheres_tangent_sphere_by_radius<OutputSphereType>(s1, sm, s2, custom_tangent_sphere_radius);
			if(s2.r==min_r) return construct_spheres_tangent_sphere_by_radius<OutputSphereType>(s2, sm, s1, custom_tangent_sphere_radius);
		}
	}

	const double r=custom_tangent_sphere_radius+sm.r;

	const double x1=s1.x-sm.x;
	const double y1=s1.y-sm.y;
	const double z1=s1.z-sm.z;
	const double r1=s1.r-sm.r;

	const double x2=s2.x-sm.x;
	const double y2=s2.y-sm.y;
	const double z2=s2.z-sm.z;
	const double r2=s2.r-sm.r;

	const double a1=2*x1;
	const double b1=2*y1;
	const double c1=2*z1;
	const double o1=(r+r1)*(r+r1)-(r*r)-(x1*x1+y1*y1+z1*z1);

	const double a2=2*x2;
	const double b2=2*y2;
	const double c2=2*z2;
	const double o2=(r+r2)*(r+r2)-(r*r)-(x2*x2+y2*y2+z2*z2);

	const double ux=(c2*b1-c1*b2)/(a1*b2-a2*b1);
	const double vx=(o2*b1-o1*b2)/(a1*b2-a2*b1);

	const double uy=(c2*a1-c1*a2)/(b1*a2-b2*a1);
	const double vy=(o2*a1-o1*a2)/(b1*a2-b2*a1);

	const double a=(ux*ux+uy*uy+1);
	const double b=2*(ux*vx+uy*vy);
	const double c=(vx*vx+vy*vy-r*r);

	const double D = b*b-4*a*c;

	std::vector<double> zs;
	if(D>=0) {zs.push_back((-b-sqrt(D))/(2*a));}
	if(D>0)  {zs.push_back((-b+sqrt(D))/(2*a));}

	std::vector<OutputSphereType> results;
	results.reserve(zs.size());
	for(std::size_t i=0;i<zs.size();i++)
	{
		const double z=zs[i];
		const OutputSphereType candidate=custom_sphere<OutputSphereType>((ux*z+vx+sm.x), (uy*z+vy+sm.y), (z+sm.z), (r-sm.r));
		//if(check_spheres_tangent_sphere_by_radius(sm, s1, s2, custom_tangent_sphere_radius, candidate))
		{
			results.push_back(candidate);
		}
	}

	return results;
}

}

#endif /* APOLLO_SPHERES_TANGENT_SPHERE_BY_RADIUS_H_ */
