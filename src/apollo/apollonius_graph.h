#ifndef APOLLONIUS_GRAPH_H_
#define APOLLONIUS_GRAPH_H_

#include <tr1/unordered_map>

#include "spheres_basic_operations.h"
#include "spheres_clustering.h"
#include "spheres_tangent.h"
#include "tuples.h"
#include "utilities.h"

namespace apollo
{

template<typename SphereType>
class apollonius_graph
{
public:
	typedef SphereType Sphere;
	typedef std::vector<typename spheres_clustering<Sphere>::ClustersLayer> ClustersLayers;
	typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;

//	static TriplesMap construct_full_triples_map(
//			const std::vector<Sphere>& spheres,
//			const double clustering_r,
//			const std::size_t clustering_low_count)
//	{
//		const std::vector<std::size_t> search_list=generate_random_permutation(spheres.size());
//		const ClustersLayers clusters_layers=spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count);
//		TriplesMap triples_map=init_triples_map(spheres, clusters_layers);
//		return triples_map;
//	}

	static std::size_t find_any_antagonist(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t old,
			const std::vector<std::size_t>& search_list)
	{
		for(std::size_t i=0;i<search_list.size();i++)
		{
			const std::size_t candidate=search_list[i];
			if(candidate!=old && !triple.contains(candidate))
			{
				const std::vector<Sphere> tangents=construct_spheres_tangent(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], spheres[candidate]);
				if(!tangents.empty())
				{
					bool good=!(old<spheres.size());
					for(std::size_t j=0;j<tangents.size() && !good;j++)
					{
						good=good || (!sphere_intersects_sphere(tangents[j], spheres[old]));
					}
					if(good)
					{
						return candidate;
					}
				}
			}
		}
		return npos();
	}

	struct NodeChecker
	{
		const std::vector<Sphere> tangents;

		NodeChecker(const std::vector<Sphere>& tangents) : tangents(tangents)
		{
		}

		bool operator()(const Sphere& sphere) const
		{
			bool good=false;
			for(std::size_t j=0;j<tangents.size() && !good;j++)
			{
				good=good || sphere_intersects_sphere(tangents[j], sphere);
			}
			return good;
		}
	};

	struct LeafChecker
	{
		const std::vector<Sphere>& spheres;
		const Triple& triple;
		const std::size_t current;
		const std::vector<Sphere> tangents;

		LeafChecker(
				const std::vector<Sphere>& spheres,
				const Triple& triple,
				const std::size_t current,
				const std::vector<Sphere>& tangents) :
					spheres(spheres),
					triple(triple),
					current(current),
					tangents(tangents)
		{
		}

		bool operator()(const std::size_t id) const
		{
			if(id<spheres.size() && id!=current && !triple.contains(id))
			{
				bool good=false;
				for(std::size_t j=0;j<tangents.size() && !good;j++)
				{
					good=good || sphere_intersects_sphere(tangents[j], spheres[id]);
				}
				return good;
			}
			else
			{
				return false;
			}
		}
	};

	static std::size_t find_any_replacement(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t current,
			const ClustersLayers& clusters_layers)
	{
		const std::vector<Sphere> tangents=construct_spheres_tangent(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], spheres[current]);
		const std::vector<std::size_t> result=spheres_clustering<Sphere>::search_in_clusters_layers(clusters_layers, NodeChecker(tangents), LeafChecker(spheres, triple, current, tangents), 1);
		if(!result.empty())
		{
			return result.front();
		}
		else
		{
			return npos();
		}
	}

	static std::vector<std::size_t> find_all_equivalents(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t current,
			const ClustersLayers& clusters_layers)
	{
		std::vector<std::size_t> equivalents;
		equivalents.push_back(current);
		return equivalents;
	}

	static std::vector<std::size_t> find_all_expositions(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t old,
			const ClustersLayers& clusters_layers,
			const std::vector<std::size_t>& search_list)
	{
		std::size_t current=find_any_antagonist(spheres, triple, old, search_list);
		std::size_t last=current;
		while(current<spheres.size())
		{
			last=current;
			current=find_any_replacement(spheres, triple, current, clusters_layers);
		}
		if(last<spheres.size())
		{
			return find_all_equivalents(spheres, triple, last, clusters_layers);
		}
		else
		{
			return std::vector<std::size_t>();
		}
	}
};

}

#endif /* APOLLONIUS_GRAPH_H_ */
