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
	const int N=10000;
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

	typedef Emmental<Sphere, double, OrderedMapTypeProxy, VectorTypeProxy> TheEmmental;
	TheEmmental emmental(1.0/20.0);
	for(int i=0;i<N;i++)
	{
		emmental.add(spheres[i]);
	}
//	const TheEmmental::Statistics stat=emmental.collect_statistics();
//	std::cout << stat.cells_count << " " << stat.all_records_count << " " << stat.min_records_count << " " << stat.max_records_count << "\n";


	Octree octree(Octree::Box(-200, -200, -200, 200, 200, 200));
	for(int i=0;i<N;i++)
	{
		octree.insert(i, Octree::Box::from_sphere(spheres[i]), 20);
	}

	bool good=true;
	for(int i=0;i<N;i++)
	{
		Sphere s=spheres[i%N];
		s.r+=5;

		int count1=0;
		{
			const std::vector<const Sphere*> result=emmental.intersect(s);
			std::set<const Sphere*> result_set;
			for(std::size_t j=0;j<result.size();j++)
			{
				const Sphere& a=s;
				const Sphere& b=*result[j];
				if((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z)<(a.r+b.r)*(a.r+b.r))
				{
					result_set.insert(result[j]);
				}
			}
			count1=result_set.size();
		}

		int count2=0;
		{
			const std::vector<int> result=octree.intersect(Octree::Box::from_sphere(s));
			std::set<int> result_set;
			for(std::size_t j=0;j<result.size();j++)
			{
				const Sphere& a=s;
				const Sphere& b=spheres[result[j]];
				if((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z)<(a.r+b.r)*(a.r+b.r))
				{
					result_set.insert(result[j]);
				}
			}
			count2=result_set.size();
		}

		good=good && (count1==count2);
	}

	std::cout << "Status = " << good << "\n";

	return 0;
}
