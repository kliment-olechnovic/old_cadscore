#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_TANGENT_SPHERE_OF_THREE_SPHERES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_TANGENT_SPHERE_OF_THREE_SPHERES_H_

#include <vector>

#include "../basic_operations_on_spheres.h"

namespace apollo2
{

namespace apollonius_triangulation
{

class TangentSphereOfThreeSpheres
{
public:
	template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
	static inline std::vector<SimpleSphere> calculate(const InputSphereTypeA& sm, const InputSphereTypeB& s1, const InputSphereTypeC& s2)
	{
		{
			const double min_r=std::min(sm.r, std::min(s1.r, s2.r));
			if(sm.r!=min_r)
			{
				if(s1.r==min_r) return calculate(s1, sm, s2);
				if(s2.r==min_r) return calculate(s2, sm, s1);
			}
		}

		const double x1=distance_from_point_to_point(sm, s1);
		const double y1=0;
		const double r1=s1.r-sm.r;

		const double x2=dot_product(sub_of_points<SimplePoint>(s1, sm).unit(), sub_of_points<PODPoint>(s2, sm));
		const double y2=sqrt(squared_point_module(sub_of_points<PODPoint>(s2, sm))-x2*x2);
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
		if(greater_or_equal(D,0))
		{
			if(equal(D,0))
			{
				radiuses.push_back((-b)/(2*a));
			}
			else
			{
				radiuses.push_back((-b-sqrt(fabs(D)))/(2*a));
				radiuses.push_back((-b+sqrt(fabs(D)))/(2*a));
			}
		}

		std::vector<SimpleSphere> results;
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
				const SimpleSphere candidate(SimplePoint(sm)+(sub_of_points<SimplePoint>(s1, sm).unit()*real_l1)+(sub_of_points<SimplePoint>(s2, sm).unit()*real_l2), (r-sm.r));
				if(check_tangent_sphere(sm, s1, s2, candidate) && equal(fabs(signed_volume_of_tetrahedron(sm, s1, s2, candidate)), 0))
				{
					results.push_back(candidate);
				}
			}
		}

		return results;
	}

	template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC>
	static inline std::vector<SimpleSphere> calculate(const InputSphereTypeA& sm, const InputSphereTypeB& s1, const InputSphereTypeC& s2, const double custom_tangent_sphere_radius)
	{
		{
			const double min_r=std::min(sm.r, std::min(s1.r, s2.r));
			if(sm.r!=min_r)
			{
				if(s1.r==min_r) return calculate(s1, sm, s2, custom_tangent_sphere_radius);
				if(s2.r==min_r) return calculate(s2, sm, s1, custom_tangent_sphere_radius);
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
		if(greater_or_equal(D, 0)) {zs.push_back((-b-sqrt(fabs(D)))/(2*a));}
		if(greater(D, 0))  {zs.push_back((-b+sqrt(fabs(D)))/(2*a));}

		std::vector<SimpleSphere> results;
		results.reserve(zs.size());
		for(std::size_t i=0;i<zs.size();i++)
		{
			const double z=zs[i];
			const SimpleSphere candidate((ux*z+vx+sm.x), (uy*z+vy+sm.y), (z+sm.z), (r-sm.r));
			if(equal(candidate.r, custom_tangent_sphere_radius) && check_tangent_sphere(sm, s1, s2, candidate))
			{
				results.push_back(candidate);
			}
		}

		return results;
	}

private:
	template<typename InputSphereTypeA, typename InputSphereTypeB, typename InputSphereTypeC, typename InputSphereTypeD>
	static bool check_tangent_sphere(const InputSphereTypeA& s1, const InputSphereTypeB& s2, const InputSphereTypeC& s3, const InputSphereTypeD& tangent_sphere)
	{
		return (sphere_touches_sphere(tangent_sphere, s1) &&
				sphere_touches_sphere(tangent_sphere, s2) &&
				sphere_touches_sphere(tangent_sphere, s3));
	}
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_TANGENT_SPHERE_OF_THREE_SPHERES_H_ */
