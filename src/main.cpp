#include <iostream>
#include <cstdlib>

#include "standard_container_types_proxies.h"
#include "Emmental.h"

struct Sphere
{
	double x;
	double y;
	double z;
	double r;
};

int main()
{
	Emmental<Sphere, double, OrderedMapTypeProxy, VectorTypeProxy> emmental(0.5/1.4);
	std::vector<Sphere> spheres;
	const int N=10000;
	spheres.reserve(N);
	for(int i=0;i<N;i++)
	{
		Sphere s;
		s.x=(rand()%100);
		s.y=(rand()%100);
		s.z=(rand()%100);
		s.r=1.4;
		spheres.push_back(s);
		emmental.add(&spheres[i]);
	}
	for(int i=0;i<N*10;i++)
	{
		Sphere s=spheres[i%N];
		s.r=5;
		emmental.intersect(s);
	}
	return 0;
}
