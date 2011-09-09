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

	static TriplesMap construct_full_triples_map(
			const std::vector<Sphere>& spheres,
			const double clustering_r,
			const std::size_t clustering_low_count)
	{
		const std::vector<std::size_t> search_list=generate_random_permutation(spheres.size());
		const ClustersLayers clusters_layers=spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count);
		TriplesMap triples_map=init_triples_map(spheres, clusters_layers);
		return triples_map;
	}

	static std::size_t find_any_antagonist(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t old,
			const std::vector<std::size_t>& search_list)
	{
		//
	}

	static std::size_t find_any_replacement(
			const std::vector<Sphere>& spheres,
			const Triple& triple,
			const std::size_t current,
			const ClustersLayers& clusters_layers)
	{
		//
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
		std::size_t current=find_any_antagonist(spheres, triple, old, antagonist_search_list);
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
