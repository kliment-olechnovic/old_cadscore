#ifndef HYPERBOLOIDS_BASIC_OPERATIONS_H_
#define HYPERBOLOIDS_BASIC_OPERATIONS_H_

#include <cmath>

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
SimplePoint intersect_vector_with_hyperboloid(const SimplePoint& a, const SimplePoint& b, const SphereType& s1, const SphereType& s2)
{
	if(s1.r>s2.r)
	{
		return intersect_vector_with_hyperboloid(a, b, s2, s1);
	}
	else
	{
		const SimplePoint dv=(custom_point_from_object<SimplePoint>(s1)-custom_point_from_object<SimplePoint>(s2))/2;
		const double d=dv.module();
		const SimplePoint c=custom_point_from_object<SimplePoint>(s2)+dv;

		const SimplePoint ca=a-c;
		const double maz=dv.unit()*ca;
		const SimplePoint cax=(a-(dv.unit()*maz))-c;
		const double max=cax.unit()*ca;
		const double may=0;
		const SimplePoint ma(max, may, maz);

		const SimplePoint cb=b-c;
		const double mbz=dv.unit()*cb;
		const double mbx=cax.unit()*cb;
		const double mby=sqrt(point_squared_module(cb)-mbz*mbz-mbx*mbx);
		const SimplePoint mb(mbx, mby, mbz);

		const double t=intersect_vector_with_simple_hyperboloid(ma, mb, d, s1.r, s2.r);

		return (a+((b-a).unit()*t));
	}
}

}

#endif /* HYPERBOLOIDS_BASIC_OPERATIONS_H_ */
