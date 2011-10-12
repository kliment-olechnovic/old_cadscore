#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "points_basic_operations.h"

namespace apollo
{

struct Quaternion
{
	double a;
	double b;
	double c;
	double d;

	Quaternion() : a(0), b(0), c(0), d(0)
	{
	}

	Quaternion(const double a, const double b, const double c, const double d) : a(a), b(b), c(c), d(d)
	{
	}

	template<typename InputPointType>
	static Quaternion from_point(const double a, const InputPointType& p)
	{
		return Quaternion(a, p.x, p.y, p.z);
	}

	template<typename OutputPointType>
	OutputPointType point() const
	{
		return custom_point<OutputPointType>(b, c, d);
	}

	Quaternion operator*(const Quaternion& q) const
	{
		return Quaternion(
				a*q.a - b*q.b - c*q.c - d*q.d,
				a*q.b + b*q.a + c*q.d - d*q.c,
				a*q.c - b*q.d + c*q.a + d*q.b,
				a*q.d + b*q.c - c*q.b + d*q.a);
	}

	Quaternion operator!() const
	{
		return Quaternion(a, 0-b, 0-c, 0-d);
	}
};

}

#endif /* QUATERNION_H_ */
