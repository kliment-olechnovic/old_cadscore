#include <iostream>
#include "SpheresMap.h"
#include "SpheresCollisionDetection.h"

struct TestSphere
{
	double x() const { return 1; }
	double y() const { return 1; }
	double z() const { return 1; }
	double r() const { return 1; }
	bool intersects_with(const TestSphere& s) const { return true; }
	bool contains(const TestSphere& s) const { return true; }
};

int main()
{
	SpheresMap<TestSphere> sm(1.0, 1.0, 10.0);
	TestSphere ts;
	sm.add(&ts);
	sm.find_potential_intersections(ts);
	SpheresCollisionDetection< SpheresMap<TestSphere> >::check_for_collision(sm, ts, false);
	sm.remove(&ts);
	std::cout << "Hi!\n";
	return 0;
}
