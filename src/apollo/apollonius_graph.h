#ifndef APOLLONIUS_GRAPH_H_
#define APOLLONIUS_GRAPH_H_

#include <tr1/unordered_map>
#include <tr1/unordered_set>

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
	typedef std::tr1::unordered_map<Triple, std::vector<std::size_t>, Triple::HashFunctor> TriplesMap;
	typedef std::tr1::unordered_set<Quadruple, Quadruple::HashFunctor> QuadruplesSet;

	apollonius_graph(
			const std::vector<Sphere>& spheres,
			const double clustering_r,
			const std::size_t clustering_low_count) :
				spheres_(spheres),
				randomized_search_list_(generate_random_permutation(spheres.size())),
				clusters_layers_(spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count))
	{
	}

private:
	struct Exposition
	{
		const Triple triple;
		const std::size_t protagonist;
		const std::size_t antagonist;
		const std::vector<Sphere> tangents;

		Exposition() : protagonist(npos()), antagonist(npos())
		{
		}

		Exposition(const Triple& triple) : triple(triple), protagonist(npos()), antagonist(npos())
		{
		}

		Exposition(
				const Triple& triple,
				const std::size_t protagonist,
				const std::size_t antagonist,
				const std::vector<Sphere>& spheres) :
					triple(triple),
					protagonist(protagonist),
					protagonist(antagonist),
					tangents(construct_tangents(triple, protagonist, antagonist, spheres))
		{
		}

		static Exposition exposition_from_quadruple(const Quadruple& qudruple, const std::vector<Sphere>& spheres)
		{
			return Exposition(qudruple.exclude(3), qudruple.get(3), npos(), spheres);
		}

		static std::vector<Sphere> construct_tangents(
				const Triple& triple,
				const std::size_t protagonist,
				const std::size_t antagonist,
				const std::vector<Sphere>& spheres)
		{
			if(protagonist==antagonist || protagonist>=spheres.size() || triple.contains(protagonist))
			{
				return std::vector<Sphere>();
			}
			else
			{
				const Sphere& protagonist_sphere=spheres[protagonist_sphere];
				const std::vector<Sphere> all_tangents=construct_spheres_tangent(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], protagonist_sphere);
				if(antagonist>=spheres.size())
				{
					return all_tangents;
				}
				else
				{
					const Sphere& antagonist_sphere=spheres[antagonist];
					const int protagonist_halfspace=halfspace(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], protagonist_sphere);
					const int antagonist_halfspace=halfspace(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], antagonist_sphere);
					if(protagonist_halfspace*antagonist_halfspace==1)
					{
						return std::vector<Sphere>();
					}
					else
					{
						std::vector<Sphere> checked_tangents;
						for(std::size_t j=0;j<all_tangents.size();j++)
						{
							const Sphere& tangent=all_tangents[j];
							if(!sphere_intersects_sphere(tangent, antagonist_sphere))
							{
								checked_tangents.push_back(tangent);
							}
						}
						return checked_tangents;
					}
				}
			}
		}
	};

	struct intersection_search_operators
	{
		struct NodeChecker
		{
			const Exposition exposition;

			NodeChecker(const Exposition& exposition) : exposition(exposition)
			{
			}

			bool operator()(const Sphere& sphere) const
			{
				for(std::size_t j=0;j<exposition.tangents.size();j++)
				{
					if(sphere_intersects_sphere(exposition.tangents[j], sphere))
					{
						return true;
					}
				}
				return false;
			}
		};

		struct LeafChecker
		{
			const Exposition exposition;
			const std::vector<Sphere>& spheres;

			LeafChecker(
					const Exposition& exposition,
					const std::vector<Sphere>& spheres) :
						exposition(exposition),
						spheres(spheres)
			{
			}

			bool operator()(const std::size_t id) const
			{
				if(id<spheres.size() && id!=exposition.protagonist && !exposition.triple.contains(id))
				{
					for(std::size_t j=0;j<exposition.tangents.size();j++)
					{
						if(sphere_intersects_sphere(exposition.tangents[j], spheres[id]))
						{
							return true;
						}
					}
				}
				return false;
			}
		};
	};

	Quadruple find_first_quadruple() const
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres_, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere>, spheres_.front(), std::tr1::placeholders::_1));
		const std::size_t a=0;
		for(std::size_t b=a+1;b<traversal.size();b++)
		{
			for(std::size_t c=b+1;c<traversal.size();c++)
			{
				for(std::size_t d=c+1;d<traversal.size();d++)
				{
					const Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
					Exposition exposition=Exposition::exposition_from_quadruple(quadruple, spheres_);
					if(exposition.tangents.size()==1)
					{
						const Sphere& tangent=exposition.tangents.front();
						bool valid=true;
						for(std::size_t i=0;i<spheres_.size() && valid;i++)
						{
							valid=valid && !sphere_intersects_sphere(tangent, spheres_[i]);
						}
						if(valid)
						{
							return quadruple;
						}
					}
				}
			}
		}
		return Quadruple();
	}

	Exposition find_any_protagonistic_exposition(const Triple& triple, const std::size_t antagonist) const
	{
		for(std::size_t i=0;i<randomized_search_list_.size();i++)
		{
			const std::size_t protagonist=randomized_search_list_[i];
			if(protagonist!=antagonist && !triple.contains(protagonist))
			{
				const Exposition protagonistic_exposition(triple, protagonist, antagonist, spheres_);
				if(!protagonistic_exposition.tangents.empty())
				{
					return protagonistic_exposition;
				}
			}
		}
		return Exposition();
	}

	Exposition find_any_better_protagonistic_exposition(const Exposition& old_protagonistic_exposition) const
	{
		const std::vector<std::size_t> result=spheres_clustering<Sphere>::search_in_clusters_layers(
				clusters_layers,
				intersection_search_operators::NodeChecker(old_protagonistic_exposition),
				intersection_search_operators::LeafChecker(old_protagonistic_exposition, spheres),
				1);
		if(!result.empty())
		{
			return Exposition::construct_from_other_exposition(old_protagonistic_exposition.triple, result.front(), old_protagonistic_exposition.antagonist, spheres_, false);
		}
		return Exposition();
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

	static std::pair< Triple, std::vector<std::size_t> > find_first_triple_with_expositions(
			const std::vector<Sphere>& spheres,
			const ClustersLayers& clusters_layers,
			const std::vector<std::size_t>& search_list)
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere>, spheres.front(), std::tr1::placeholders::_1));
		for(std::size_t a=0;a<traversal.size();a++)
		{
			for(std::size_t b=a+1;b<traversal.size();b++)
			{
				for(std::size_t c=b+1;c<traversal.size();c++)
				{
					const Triple triple=make_triple(traversal[a], traversal[b], traversal[c]);
					const std::vector<std::size_t> expositions=find_all_expositions(spheres, triple, npos(), clusters_layers, search_list);
					if(!expositions.empty())
					{
						return std::make_pair(triple, expositions);
					}
				}
			}
		}
		return std::make_pair(make_triple(npos(), npos(), npos()), std::vector<std::size_t>());
	}

	static std::pair< Quadruple, Sphere > find_first_quadruple(const std::vector<Sphere>& spheres,
			const double clustering_r,
			const std::size_t clustering_low_count)
	{
		const std::vector<std::size_t> search_list=generate_random_permutation(spheres.size());
		const ClustersLayers clusters_layers=spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count);
		const std::pair< Triple, std::vector<std::size_t> > first_triple_with_expositions=find_first_triple_with_expositions(spheres, clusters_layers, search_list);
		const Quadruple q(first_triple_with_expositions.first, first_triple_with_expositions.second.front());
		const Sphere sphere=construct_spheres_tangent(spheres[q.get(0)], spheres[q.get(1)], spheres[q.get(2)], spheres[q.get(3)]).front();
		return std::make_pair(q, sphere);
	}

//	static TriplesMap construct_full_triples_map(
//			const std::vector<Sphere>& spheres,
//			const double clustering_r,
//			const std::size_t clustering_low_count)
//	{
//		const std::vector<std::size_t> search_list=generate_random_permutation(spheres.size());
//		const ClustersLayers clusters_layers=spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count);
//		TriplesMap triples_map;
//		std::deque< std::pair<Triple, std::size_t> > stack;
//		const std::pair< Triple, std::vector<std::size_t> > first_triple_with_expositions=find_first_triple_with_expositions(spheres, clusters_layers, search_list);
//		stack.push_back(std::make_pair(first_triple_with_expositions.first, first_triple_with_expositions.second.front()));
//		while(!stack.empty())
//		{
//			//
//		}
//		return triples_map;
//	}

private:
	const std::vector<Sphere>& spheres_;
	const std::vector<std::size_t> randomized_search_list_;
	const ClustersLayers clusters_layers_;
};

}

#endif /* APOLLONIUS_GRAPH_H_ */
