#ifndef APOLLO2_ROTATION_H_
#define APOLLO2_ROTATION_H_

#include "basic_operations_on_points.h"
#include "quaternion.h"

namespace apollo2
{

struct Rotation
{
	SimplePoint axis;
	double angle;

	Rotation() : angle(0)
	{
	}

	template<typename InputPointType>
	Rotation(const InputPointType& axis, double angle) : axis(axis), angle(angle)
	{
	}

	static double pi()
	{
		return 3.14159265;
	}

	template<typename OutputPointType, typename InputPointType>
	OutputPointType rotate(const InputPointType& p) const
	{
		if(axis.module()>0)
		{
			const double radians_angle_half=angle*pi()/360.0;
			const Quaternion q1(cos(radians_angle_half), axis.unit()*sin(radians_angle_half));
			const Quaternion q2(0, p);
			const Quaternion q3=((q1*q2)*(!q1));
			return custom_point<OutputPointType>(q3.b, q3.c, q3.d);
		}
		else
		{
			return p;
		}
	}
};

}

#endif /* APOLLO2_ROTATION_H_ */
