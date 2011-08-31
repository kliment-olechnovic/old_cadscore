#include <iostream>
#include <cstdlib>

#include "atoms/atoms.h"
#include "standard_container_types_proxies.h"
#include "Emmental.h"
#include "Octree.h"

#include "../../contactus/src/baltymus/utils/BlockTimer.h"

int main()
{
	typedef Atom Sphere;

	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
	std::cout << spheres.size() << " spheres\n";

	utils::BlockTimer* bt_ec=new utils::BlockTimer("Grid construction");
	typedef Emmental<Sphere, double, OrderedMapTypeProxy, VectorTypeProxy> TheEmmental;
	TheEmmental emmental(1.0/20.0);
	for(std::size_t i=0;i<spheres.size();i++)
	{
		emmental.add(spheres[i]);
	}
	delete bt_ec;
	const TheEmmental::Statistics stat=emmental.collect_statistics();
	std::cout << stat.cells_count << " " << stat.all_records_count << " " << stat.min_records_count << " " << stat.max_records_count << " emmental stat\n";


	utils::BlockTimer* bt_oc=new utils::BlockTimer("Octree construction");
	Octree octree(Octree::Box::from_spheres(spheres));
	for(std::size_t i=0;i<spheres.size();i++)
	{
		octree.insert(i, Octree::Box::from_sphere(spheres[i]), 20);
	}
	delete bt_oc;

	const double dist=5;

	{
		utils::BlockTimer bt("Simple");
		for(std::size_t i=0;i<spheres.size();i++)
		{
			Sphere s=spheres[i%spheres.size()];
			s.r+=dist;

			std::vector<int> result_set;
			for(std::size_t j=0;j<spheres.size();j++)
			{
				const Sphere& a=s;
				const Sphere& b=spheres[j];
				if((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z)<(a.r+b.r)*(a.r+b.r))
				{
					result_set.push_back(j);
				}
			}
		}
	}

	{
		utils::BlockTimer bt("Grid");
		for(std::size_t i=0;i<spheres.size();i++)
		{
			Sphere s=spheres[i%spheres.size()];
			s.r+=dist;

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
		}
	}

	utils::BlockTimer bt("Octree");
	for(std::size_t i=0;i<spheres.size();i++)
	{
		Sphere s=spheres[i%spheres.size()];
		s.r+=dist;

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
	}

	return 0;
}
