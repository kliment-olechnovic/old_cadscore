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
	Emmental<Sphere, double, UnorderedMapTypeProxy, ListTypeProxy, UnorderedSetTypeProxy> emmental(0.5);
	std::vector<Sphere> spheres;
	const int N=50000;
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
		s.r*=2;
		emmental.intersect(s);
	}
	return 0;
}
