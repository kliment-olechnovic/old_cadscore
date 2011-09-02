#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
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
		for(std::size_t i=0;i<graph.size();i++)
		{
			int count=0;
			gt.start(i);
			std::size_t id=gt.bfs_next();
			while(id!=gt.npos && gt.level(id)<3)
			{
				count++;
				id=gt.bfs_next();
			}
		}
	}

	{
		utils::BlockTimer bt("Bucketing time");

		std::vector<std::size_t> permutation;
		{
			permutation.reserve(graph.size());
			gt.start(0);
			std::size_t id=gt.bfs_next();
			while(id!=gt.npos)
			{
				permutation.push_back(id);
				id=gt.bfs_next();
			}
		}

		std::vector<int> labels(graph.size(), -1);
		std::vector< std::vector<std::size_t> > parents(graph.size());
		std::tr1::unordered_map< std::size_t, std::vector<std::size_t> > buckets;
		std::tr1::unordered_set<std::size_t> included;
		std::size_t count=0;
		for(std::size_t j=0;j<graph.size();j++)
		{
			const std::size_t i=permutation[j];
			if(labels[i]==-1)
			{
				std::vector<std::size_t>& neighbours=buckets[i];
				gt.start(i);
				std::size_t id=gt.bfs_next();
				while(id!=gt.npos && gt.level(id)<=2)
				{
					if(gt.level(id)<=1)
					{
						neighbours.push_back(id);
						included.insert(id);
					}
					if(labels[id]!=0 && labels[id]!=1)
					{
						labels[id]=gt.level(id);
					}
					parents[id].push_back(i);
					id=gt.bfs_next();
				}
				count+=neighbours.size();
			}
		}
		std::cout << included.size() << " " << count << " buckets contents count\n";
		std::cout << buckets.size() << " buckets\n";

//		std::vector<std::size_t> deserted;
//		for(std::size_t j=0;j<graph.size();j++)
//		{
//			if(labels[j]==2)
//			{
//				deserted.push_back(j);
//				std::cout << parents[j].size() << " parents\n";
//				for(std::size_t k=0;k<parents[j].size();k++)
//				{
//					std::cout << " " << DistanceBetweenSpheres<Sphere>()(spheres[j], spheres[parents[j][k]]) << " distance\n";
//				}
//			}
//		}
//		std::cout << deserted.size() << " deserted\n";
//		std::cout << (included.size()+deserted.size()) << " included+deserted\n";
	}

	return 0;
}
