#ifndef SPHERES_TANGENT_H_
#define SPHERES_TANGENT_H_

#include <vector>

#include "spheres_basic_operations.h"

namespace apollo
{

template<typename SphereType>
bool check_spheres_tangent(const SphereType& s1, const SphereType& s2, const SphereType& s3, const SphereType& s4, const SphereType& tangent)
{
	return (sphere_touches_sphere(tangent, s1) &&
			sphere_touches_sphere(tangent, s2) &&
			sphere_touches_sphere(tangent, s3) &&
			sphere_touches_sphere(tangent, s4));
}

template<typename SphereType>
inline std::vector<SphereType> construct_spheres_tangent(const SphereType& sm, const SphereType& s1, const SphereType& s2, const SphereType& s3)
{
	{
		const double min_r=std::min(sm.r, std::min(s1.r, std::min(s2.r, s3.r)));
		if(sm.r!=min_r)
		{
			if(s1.r==min_r) return construct_spheres_tangent(s1, sm, s2, s3);
			if(s2.r==min_r) return construct_spheres_tangent(s2, sm, s1, s3);
			if(s3.r==min_r) return construct_spheres_tangent(s3, sm, s1, s2);
		}
	}

	const double x1=s1.x-sm.x;
	const double y1=s1.y-sm.y;
	const double z1=s1.z-sm.z;
	const double r1=s1.r-sm.r;

	const double x2=s2.x-sm.x;
	const double y2=s2.y-sm.y;
	const double z2=s2.z-sm.z;
	const double r2=s2.r-sm.r;

	const double x3=s3.x-sm.x;
	const double y3=s3.y-sm.y;
	const double z3=s3.z-sm.z;
	const double r3=s3.r-sm.r;

	const double a1=2*x1;
	const double b1=2*y1;
	const double c1=2*z1;
	const double d1=2*r1;
	const double o1=(r1*r1-x1*x1-y1*y1-z1*z1);

	const double a2=2*x2;
	const double b2=2*y2;
	const double c2=2*z2;
	const double d2=2*r2;
	const double o2=(r2*r2-x2*x2-y2*y2-z2*z2);

	const double a3=2*x3;
	const double b3=2*y3;
	const double c3=2*z3;
	const double d3=2*r3;
	const double o3=(r3*r3-x3*x3-y3*y3-z3*z3);

	const double w  = (a1*(b3*c2-b2*c3) + b1*(a2*c3-a3*c2) + c1*(a3*b2-a2*b3));

	const double u1 = -( b1*(c3*d2-c2*d3) + c1*(b2*d3-b3*d2) + d1*(b3*c2-b2*c3) ) / w;
	const double v1 = -( b1*(c3*o2-c2*o3) + c1*(b2*o3-b3*o2) + o1*(b3*c2-b2*c3) ) / w;

	const double u2 =  ( a1*(c3*d2-c2*d3) + c1*(a2*d3-a3*d2) + d1*(a3*c2-a2*c3) ) / w;
	const double v2 =  ( a1*(c3*o2-c2*o3) + c1*(a2*o3-a3*o2) + o1*(a3*c2-a2*c3) ) / w;

	const double u3 = -( a1*(b3*d2-b2*d3) + b1*(a2*d3-a3*d2) + d1*(a3*b2-a2*b3) ) / w;
	const double v3 = -( a1*(b3*o2-b2*o3) + b1*(a2*o3-a3*o2) + o1*(a3*b2-a2*b3) ) / w;

	const double a = u1*u1+u2*u2+u3*u3-1;
	const double b = 2*(u1*v1+u2*v2+u3*v3);
	const double c = v1*v1+v2*v2+v3*v3;

	const double D = b*b-4*a*c;

	std::vector<double> radiuses;
	if(D>=0) {radiuses.push_back((-b-sqrt(D))/(2*a));}
	if(D>0)  {radiuses.push_back((-b+sqrt(D))/(2*a));}

	std::vector<SphereType> results;
	results.reserve(radiuses.size());
	for(std::size_t i=0;i<radiuses.size();i++)
	{
		const double r=radiuses[i];
		if(r>0)
		{
			const SphereType candidate=custom_sphere<SphereType>((u1*r+v1+sm.x), (u2*r+v2+sm.y), (u3*r+v3+sm.z), (r-sm.r));
			if(check_spheres_tangent(sm, s1, s2, s3, candidate))
			{
				results.push_back(candidate);
			}
		}
	}

	return results;
}

template<typename SphereType, typename PointType>
bool check_spheres_tangent_plane(const SphereType& s1, const SphereType& s2, const SphereType& s3, const PointType& tangent)
{
	const PointType p1=sum_of_points(custom_point_from_object<PointType>(s1), point_and_number_product(tangent, s1.r));
	const PointType p2=sum_of_points(custom_point_from_object<PointType>(s2), point_and_number_product(tangent, s2.r));
	const PointType p3=sum_of_points(custom_point_from_object<PointType>(s3), point_and_number_product(tangent, s3.r));
	const PointType p12=sub_of_points(p2, p1);
	const PointType p13=sub_of_points(p3, p1);
	return (equal(dot_product(sub_of_points(p2, p1), tangent), 0) &&
			equal(dot_product(sub_of_points(p3, p1), tangent), 0));
}


template<typename SphereType, typename PointType>
inline std::vector<PointType> construct_spheres_tangent_plane(const SphereType& sm, const SphereType& s1, const SphereType& s2)
{
	{
		const double min_r=std::min(sm.r, std::min(s1.r, s2.r));
		if(sm.r!=min_r)
		{
			if(s1.r==min_r) return construct_spheres_tangent_plane<SphereType, PointType>(s1, sm, s2);
			if(s2.r==min_r) return construct_spheres_tangent_plane<SphereType, PointType>(s2, sm, s1);
		}
	}

	const double x1=s1.x-sm.x;
	const double y1=s1.y-sm.y;
	const double z1=s1.z-sm.z;
	const double r1=s1.r-sm.r;

	const double x2=s2.x-sm.x;
	const double y2=s2.y-sm.y;
	const double z2=s2.z-sm.z;
	const double r2=s2.r-sm.r;

	const double ad=0-x1;
	const double a0=r1/ad;
	const double ay=y1/ad;
	const double az=z1/ad;

	const double bd=0-(y2+ay*x2);
	const double b0=(r2+a0*x2)/bd;
	const double bz=(z2+az*x2)/bd;

	const double c0=a0+ay*b0;
	const double cz=(ay*bz+az);

	const double a=(1+cz*cz+bz*bz);
	const double b=2*(c0*cz+b0*bz);
	const double c=(c0*c0+b0*b0-1);
	const double D=b*b-4*a*c;

	std::vector<double> zs;
	if(greater_or_equal(D,0)) {zs.push_back((-b-sqrt(D))/(2*a));}
	if(greater(D,0))  {zs.push_back((-b+sqrt(D))/(2*a));}

	std::vector<SphereType> results;
	results.reserve(zs.size());
	for(std::size_t i=0;i<zs.size();i++)
	{
		const double z=zs[i];
		const PointType candidate=custom_point<PointType>(c0+z*cz, b0+z*bz, z);
		if(check_spheres_tangent_plane(sm, s1, s2, candidate))
		{
			results.push_back(candidate);
		}
	}

	return results;
}

}

#endif /* SPHERES_TANGENT_H_ */
