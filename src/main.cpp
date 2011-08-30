#include <iostream>
#include <cstdlib>

#include "standard_container_types_proxies.h"

#include "Emmental.h"

#include "Octree.h"

struct Sphere
{
	double x;
	double y;
	double z;
	double r;
};

int main()
{
	std::vector<Sphere> spheres;
	const int N=10;
	spheres.reserve(N);
	for(int i=0;i<N;i++)
	{
		Sphere s;
		s.x=(rand()%100);
		s.y=(rand()%100);
		s.z=(rand()%100);
		s.r=rand()%3+1;
		spheres.push_back(s);
	}

//	typedef Emmental<Sphere, double, OrderedMapTypeProxy, VectorTypeProxy> TheEmmental;
//	TheEmmental emmental(1.0/10.0);
//	for(int i=0;i<N;i++)
//	{
//		emmental.add(spheres[i]);
//	}
//	const TheEmmental::Statistics stat=emmental.collect_statistics();
//	std::cout << stat.cells_count << " " << stat.all_records_count << " " << stat.min_records_count << " " << stat.max_records_count << "\n";
//	for(int i=0;i<N*10;i++)
//	{
//		Sphere s=spheres[i%N];
//		s.r=5;
//		emmental.intersect(s);
//	}

	Octree octree(Octree::Box(-200, 200, -200, 200, -200, 200));
	for(int i=0;i<N;i++)
	{
		octree.insert(i, Octree::Box::from_sphere(spheres[i]), 10);
	}
//	for(int i=0;i<10;i++)
//	{
//		Sphere s=spheres[i%N];
//		s.r=5;
//		octree.intersect(Octree::Box::from_sphere(s)).size();
//	}

	return 0;
}
