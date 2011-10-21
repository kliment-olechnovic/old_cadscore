#ifndef APOLLO_HYPERBOLOIDS_BASIC_OPERATIONS_H_
#define APOLLO_HYPERBOLOIDS_BASIC_OPERATIONS_H_

#include <cmath>
#include <vector>

#include "points_basic_operations.h"

namespace apollo
{

inline double intersect_vector_with_simple_hyperboloid(const SimplePoint& a, const SimplePoint& b, const double d, const double r1, const double r2)
{
	if(r1>r2)
	{
		return intersect_vector_with_simple_hyperboloid(a, b, d, r2, r1);
	}
	else
	{
		const double r=r2-r1;
		SimplePoint ab=(b-a);
		SimplePoint v=ab.unit();
		const double k=(4*r*r/((0-4*r*r+16*d*d)*(0-4*r*r+16*d*d))) * (0-r*r+4*d*d) * 4;
		const double m=(4*d*d-r*r)*k/4;

		const double x0=a.x;
		const double y0=a.y;
		const double z0=a.z;
		const double vx=v.x;
		const double vy=v.y;
		const double vz=v.z;

		const double t1 =  (sqrt((k*vy*vy+k*vx*vx)*z0*z0+(-2*k*vy*vz*y0-2*k*vx*vz*x0)*z0+(k*vz*vz-k*k*vx*vx)*y0*y0+2*k*k*vx*vy*x0*y0+(k*vz*vz-k*k*vy*vy)*x0*x0+m*vz*vz-k*m*vy*vy-k*m*vx*vx)-vz*z0+k*vy*y0+k*vx*x0)/(vz*vz-k*vy*vy-k*vx*vx);

		const double t2 = -(sqrt((k*vy*vy+k*vx*vx)*z0*z0+(-2*k*vy*vz*y0-2*k*vx*vz*x0)*z0+(k*vz*vz-k*k*vx*vx)*y0*y0+2*k*k*vx*vy*x0*y0+(k*vz*vz-k*k*vy*vy)*x0*x0+m*vz*vz-k*m*vy*vy-k*m*vx*vx)+vz*z0-k*vy*y0-k*vx*x0)/(vz*vz-k*vy*vy-k*vx*vx);

		const SimplePoint tp1=a+(v*t1);
		const SimplePoint tp2=a+(v*t2);
		if(greater(t1, 0) && less(t1, ab.module()) && equal(tp1.z, sqrt(k*tp1.x*tp1.x+k*tp1.y*tp1.y+m)))
		{
			return t1;
		}
		else if(greater(t2, 0) && less(t2, ab.module()) && equal(tp2.z, sqrt(k*tp2.x*tp2.x+k*tp2.y*tp2.y+m)))
		{
			return t2;
		}
		else
		{
			return 0;
		}
	}
}

template<typename SphereType>
double intersect_vector_with_hyperboloid(const SimplePoint& a, const SimplePoint& b, const SphereType& s1, const SphereType& s2)
{
	if(s1.r>s2.r)
	{
		return intersect_vector_with_hyperboloid(a, b, s2, s1);
	}
	else
	{
		const SimplePoint dv=(custom_point_from_object<SimplePoint>(s1)-custom_point_from_object<SimplePoint>(s2))/2;
		const SimplePoint c=custom_point_from_object<SimplePoint>(s2)+dv;

		const SimplePoint ca=a-c;
		const double maz=dv.unit()*ca;
		const SimplePoint cax=(a-(dv.unit()*maz))-c;
		const double max=cax.unit()*ca;
		const double may=0;

		const SimplePoint cb=b-c;
		const double mbz=dv.unit()*cb;
		const double mbx=cax.unit()*cb;
		const double mby=sqrt(point_squared_module(cb)-mbz*mbz-mbx*mbx);

		return intersect_vector_with_simple_hyperboloid(SimplePoint(max, may, maz), SimplePoint(mbx, mby, mbz), dv.module(), s1.r, s2.r);
	}
}

template<typename SphereType>
std::vector<SimplePoint> intersect_triangle_with_hyperboloid(const SimplePoint& a, const SimplePoint& b, const SimplePoint& c, const SphereType& s1, const SphereType& s2)
{
	bool halfspaces[3]={false, false, false};
	halfspaces[0]=less(minimal_distance_from_point_to_sphere(a, s1), minimal_distance_from_point_to_sphere(a, s2));
	halfspaces[1]=less(minimal_distance_from_point_to_sphere(b, s1), minimal_distance_from_point_to_sphere(b, s2));
	halfspaces[2]=less(minimal_distance_from_point_to_sphere(c, s1), minimal_distance_from_point_to_sphere(c, s2));

	if(halfspaces[0] || halfspaces[1] || halfspaces[2])
	{
		if(halfspaces[0] && halfspaces[1] && halfspaces[2])
		{
			std::vector<SimplePoint> container(3);
			container[0]=(a);
			container[1]=(b);
			container[2]=(c);
			return container;
		}
		else
		{
			const SimplePoint* points[3]={&a, &b, &c};
			for(int i=0;i<3;i++)
			{
				const int j = (i==0 ? 1 : 0);
				const int k = (i==2 ? 1 : 2);
				const bool tff=(halfspaces[i] && !halfspaces[j] && !halfspaces[k]);
				const bool ftt=(!halfspaces[i] && halfspaces[j] && halfspaces[k]);
				if(tff || ftt)
				{
					const SimplePoint& p0=*(points[i]);
					const SimplePoint& p1=*(points[j]);
					const SimplePoint& p2=*(points[k]);
					const double t01=intersect_vector_with_hyperboloid(p0, p1, s1, s2);
					const double t02=intersect_vector_with_hyperboloid(p0, p2, s1, s2);
					if(greater(t01, 0) && greater(t02, 0))
					{
						if(tff)
						{
							std::vector<SimplePoint> container(3);
							container[0]=(p0);
							container[1]=(p0+((p1-p0).unit()*t01));
							container[2]=(p0+((p2-p0).unit()*t02));
							return container;
						}
						else if(ftt)
						{
							std::vector<SimplePoint> container(4);
							container[0]=(p1);
							container[1]=(p0+((p1-p0).unit()*t01));
							container[2]=(p0+((p2-p0).unit()*t02));
							container[3]=(p2);
							return container;
						}
					}
				}
			}
		}
	}

	return std::vector<SimplePoint>();
}

}

#endif /* APOLLO_HYPERBOLOIDS_BASIC_OPERATIONS_H_ */
