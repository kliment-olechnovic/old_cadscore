#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <map>
#include <cmath>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/GraphTraverser.h"

template<typename ContainerType, typename OperatorType>
std::vector< std::vector<std::size_t> > construct_graph(const ContainerType& objects, const OperatorType& op)
{
	std::vector< std::vector<std::size_t> > graph(objects.size());
	for(std::size_t i=0;i<objects.size();i++)
	{
		for(std::size_t j=i+1;j<objects.size();j++)
		{
			if(op(objects[i], objects[j]))
			{
				graph[i].push_back(j);
				graph[j].push_back(i);
			}
		}
	}
	return graph;
}

template<typename SphereType>
struct DistanceBetweenSpheres
{
	typedef SphereType ObjectType;

	double operator()(const SphereType& a, const SphereType& b) const
	{
		const double dx=(a.x-b.x);
		const double dy=(a.y-b.y);
		const double dz=(a.z-b.z);
		return (sqrt(dx*dx+dy*dy+dz*dz)-a.r-b.r);
	}
};

template<typename OperatorType>
struct TresholdChecker
{
	typedef typename OperatorType::ObjectType ObjectType;

	TresholdChecker(const double treshold) : treshold(treshold)
	{
	}

	double operator()(const ObjectType& a, const ObjectType& b) const
	{
		return (OperatorType()(a, b)<=treshold);
	}

	double treshold;
};

int main()
{
	typedef Atom Sphere;
	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
	std::cout << spheres.size() << " spheres read\n";

	utils::BlockTimer* bt_ptr=new utils::BlockTimer("Preparation time");

	std::vector< std::vector<std::size_t> > graph=construct_graph(spheres, TresholdChecker< DistanceBetweenSpheres<Sphere> >(2.8));
	GraphTraverser gt(graph);

	delete bt_ptr;

	{
		utils::BlockTimer bt("All-to-all traverse time");

		for(std::size_t i=0;i<graph.size();i++)
		{
			int count=0;
			gt.start(i);
			std::size_t id=gt.bfs_next();
			while(id!=gt.npos && gt.level(id)<4)
			{
				count++;
				id=gt.bfs_next();
			}
//			std::cout << count << "\n";
		}
	}

	return 0;
}
