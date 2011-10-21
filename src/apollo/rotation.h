#ifndef APOLLO_ROTATION_H_
#define APOLLO_ROTATION_H_

#include "points_basic_operations.h"
#include "quaternion.h"

namespace apollo
{

const double PI=3.14159265;

struct Rotation
{
	SimplePoint axis;
	double angle;

	Rotation() : angle(0)
	{
	}

	Rotation(const SimplePoint& axis, double angle) : axis(axis), angle(angle)
	{
	}

	template<typename InputPointType>
	static Rotation from_axis_and_angle(const InputPointType& axis, double angle)
	{
		return Rotation(custom_point_from_object<SimplePoint>(axis), angle);
	}

	template<typename InputPointTypeA, typename InputPointTypeB>
	static Rotation from_two_points(const InputPointTypeA& a, const InputPointTypeB& a)
	{
		const SimplePoint axis=cross_product<SimplePoint>(a, b);
		const double angle=0;
		const double am=point_module(a);
		const double bm=point_module(b);
		if(!equal(am*bm, 0))
		{
			angle=(180/PI)*asin(axis.module()/(am*bm));
			const double abm=sub_of_points<SimplePoint>(b, a).module();
			if(point_squared_module(sub_of_points<SimplePoint>(b, a))>(am*am+bm*bm))
			{
				angle=180-angle;
			}
		}
		return Rotation(axis, angle);
	}

	template<typename OutputPointType, typename InputPointType>
	OutputPointType rotate(const InputPointType& p) const
	{
		if(axis.module()>0)
		{
			const double radians_angle_half=angle*PI/180/2;
			const Quaternion q1=Quaternion::from_point(cos(radians_angle_half), axis.unit()*sin(radians_angle_half));
			const Quaternion q2=Quaternion::from_point(0, p);
			return ((q1*q2)*(!q1)).point<OutputPointType>();
		}
		else
		{
			return p;
		}
	}
};

}


#endif /* APOLLO_ROTATION_H_ */
