#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include "FloatingPointHandler.h"

class Point
{
public:
	Point() : x_(0), y_(0), z_(0) {}
	Point(const double x, const double y, const double z) : x_(x), y_(y), z_(z) {}
	Point(const Point& p) : x_(p.x()), y_(p.y()), z_(p.z()) {}

	inline double x() const { return x_; }
	inline double y() const { return y_; }
	inline double z() const { return z_; }

	inline void set_x(const double x) { x_=x; }
	inline void set_y(const double y) { y_=y; }
	inline void set_z(const double z) { z_=z; }

	inline Point with_x(const double b, const double a=0) const { return Point(a*x_+b, y_, z_); }
	inline Point with_y(const double b, const double a=0) const { return Point(x_, a*y_+b, z_); }
	inline Point with_z(const double b, const double a=0) const { return Point(x_, y_, a*z_+b); }

	static inline bool equal(const Point& a, const Point& b) { return ( fph::equal(a.x(),b.x()) && fph::equal(a.y(),b.y()) && fph::equal(a.z(),b.z()) ); }

	static inline double dot_product(const Point& a, const Point& b) { return (a.x()*b.x()+a.y()*b.y()+a.z()*b.z()); }

	static inline Point cross_product(const Point& a, const Point& b) { return Point(a.y()*b.z()-a.z()*b.y(), a.z()*b.x()-a.x()*b.z(), a.x()*b.y()-y()*b.x()); }

	inline double squared_module() const { return dot_product(*this, *this); }
	inline double module() const { return sqrt(squared_module()); }

	static inline double distance(const Point& a, const Point& b) { return (a-b).module(); }
	inline double distance_to(const Point& p) { return distance(*this, p); }

	bool operator==(const Point& p) const { return equal(*this, p); }
	bool operator!=(const Point& p) const { return !equal(*this, p); }

	Point operator+(double d) const { return Point(x_+d, y_+d, z_+d); }
	Point operator+(const Point& p) const { return Point(x_+p.x(), y_+p.y(), z_+p.z()); }

	Point operator-(double d) const { return Point(x_-d, y_-d, z_-d); }
	Point operator-(const Point& p) const { return Point(x_-p.x(), y_-p.y(), z_-p.z()); }
	Point operator-() const { return Point(0-x_, 0-y_, 0-z_); }

	Point operator*(double k) const { return Point(x_*k, y_*k, z_*k); }

	Point operator/(double k) const { return Point(x_/k, y_/k, z_/k); }

	double operator^(const Point& p) const { return dot_product(*this, p); }

	Point operator&(const Point& p) const { return cross_product(*this, p); }

private:
	double x_;
	double y_;
	double z_;
};

#endif /* POINT_H_ */
