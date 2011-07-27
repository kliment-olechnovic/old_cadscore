#include <iostream>
#include "SpheresMap.h"

template<typename FloatingPointNumberType>
struct TestSphere
{
	typedef FloatingPointNumberType FloatType;

	double x() const { return 1; }
	double y() const { return 1; }
	double z() const { return 1; }
	double r() const { return 1; }
};

int main()
{
	SpheresMap< TestSphere<double> > sm(1.0, 1.0, 10.0);
	TestSphere<double> ts;
	sm.add(&ts);
	sm.find_neighbours(ts);
	sm.remove(&ts);
	std::cout << "Hi!\n";
	return 0;
}
