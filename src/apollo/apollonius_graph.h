#ifndef APOLLONIUS_GRAPH_H_
#define APOLLONIUS_GRAPH_H_

#include <vector>
#include <deque>
#include <set>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include "points_basic_operations.h"
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
	typedef std::tr1::unordered_map<Triple, std::set<std::size_t>, Triple::HashFunctor> TriplesMap;
	typedef std::tr1::unordered_map<Quadruple, std::vector<Sphere>, Quadruple::HashFunctor> QuadruplesMap;
	typedef std::vector< std::vector<std::size_t> > Graph;

	apollonius_graph(
			const std::vector<Sphere>& spheres,
			const double clustering_r,
			const std::size_t clustering_low_count) :
				spheres_(spheres),
				randomized_search_list_(generate_random_permutation(spheres.size())),
				clusters_layers_(spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count))
	{
	}

	QuadruplesMap find_quadruples() const
	{
		QuadruplesMap quadruples_map;
		TriplesMap triples_map;
		std::deque<Triple> triples_stack;

		{
			const Exposition exposition=find_first_exposition();
			if(!exposition.tangents.empty())
			{
				const Quadruple found_quadruple(exposition.triple, exposition.protagonist);
				quadruples_map.insert(std::make_pair(found_quadruple, exposition.tangents));
				for(int i=0;i<found_quadruple.size();i++)
				{
					const Triple found_triple=found_quadruple.exclude(i);
					const std::size_t found_protagonist=found_quadruple.get(i);
					triples_map[found_triple].insert(found_protagonist);
					triples_stack.push_back(found_triple);
				}
			}
		}

		while(!triples_stack.empty())
		{
			const Triple triple=triples_stack.back();
			triples_stack.pop_back();
			TriplesMap::const_iterator it=triples_map.find(triple);
			if(it->second.size()==1)
			{
				const Exposition exposition=find_valid_protagonistic_exposition(triple, it->second);
				if(!exposition.tangents.empty())
				{
					const Quadruple found_quadruple(triple, exposition.protagonist);
					quadruples_map.insert(std::make_pair(found_quadruple, exposition.tangents));
					for(int i=0;i<found_quadruple.size();i++)
					{
						const Triple found_triple=found_quadruple.exclude(i);
						const std::size_t found_protagonist=found_quadruple.get(i);
						triples_map[found_triple].insert(found_protagonist);
						triples_stack.push_back(found_triple);
					}
				}
			}
		}
		return quadruples_map;
	}

	static bool check_quadruples_map(const QuadruplesMap& quadruples_map, const std::vector<Sphere>& spheres)
	{
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple q=it->first;
			if(q==Quadruple())
			{
				return false;
			}
			const std::vector<Sphere> ts=it->second;
			if(ts.empty())
			{
				return false;
			}
			for(std::size_t i=0;i<ts.size();i++)
			{
				const Sphere& t=ts[i];
				for(std::size_t j=0;j<spheres.size();j++)
				{
					if(sphere_intersects_sphere(t, spheres[j]))
					{
						return false;
					}
				}
				for(int j=0;j<4;j++)
				{
					if(!sphere_touches_sphere(t, spheres[q.get(j)]))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	static Graph graph_from_quadruples(const QuadruplesMap& quadruples_map, const std::vector<Sphere>& spheres)
	{
		typedef std::tr1::unordered_map<std::size_t, std::tr1::unordered_set<std::size_t> > GraphMap;
		GraphMap graph_map;
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple q=it->first;
			for(int i=0;i<4;i++)
			{
				for(int j=i+1;j<4;j++)
				{
					graph_map[q.get(i)].insert(q.get(j));
					graph_map[q.get(j)].insert(q.get(i));
				}
			}
		}
		Graph graph(spheres.size(), std::vector<std::size_t>());
		for(GraphMap::const_iterator it=graph_map.begin();it!=graph_map.end();++it)
		{
			for(GraphMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
			{
				graph[it->first].push_back(*jt);
			}
		}
		return graph;
	}

private:
	struct Exposition
	{
		Triple triple;
		std::size_t protagonist;
		std::set<std::size_t> antagonists;
		std::vector<Sphere> tangents;

		Exposition() : protagonist(npos()), antagonists(std::set<std::size_t>())
		{
		}

		Exposition(
				const Triple& triple,
				const std::size_t protagonist,
				const std::set<std::size_t>& antagonists,
				const std::vector<Sphere>& spheres) :
					triple(triple),
					protagonist(protagonist),
					antagonists(antagonists),
					tangents(construct_tangents(triple, protagonist, antagonists, spheres))
		{
		}

		static std::vector<Sphere> construct_tangents(
				const Triple& triple,
				const std::size_t protagonist,
				const std::set<std::size_t>& antagonists,
				const std::vector<Sphere>& spheres)
		{
			if(protagonist>=spheres.size() || antagonists.find(protagonist)!=antagonists.end() || triple.contains(protagonist))
			{
				return std::vector<Sphere>();
			}
			else
			{
				const Sphere& protagonist_sphere=spheres[protagonist];
				const std::vector<Sphere> all_tangents=construct_spheres_tangent(spheres[triple.get(0)], spheres[triple.get(1)], spheres[triple.get(2)], protagonist_sphere);
				if(antagonists.empty())
				{
					return all_tangents;
				}
				else
				{
					std::vector<Sphere> checked_tangents;
					for(std::size_t j=0;j<all_tangents.size();j++)
					{
						const Sphere& tangent=all_tangents[j];
						bool tangent_valid=true;
						for(std::set<std::size_t>::const_iterator antagonist_it=antagonists.begin();antagonist_it!=antagonists.end() && tangent_valid;antagonist_it++)
						{
							const Sphere& antagonist_sphere=spheres[*antagonist_it];
							if(sphere_intersects_sphere(tangent, antagonist_sphere))
							{
								tangent_valid=false;
							}
							else if(sphere_touches_sphere(tangent, antagonist_sphere))
							{
								const SimplePoint tp0=spheres_touching_point(tangent, spheres[triple.get(0)]);
								const SimplePoint tp1=spheres_touching_point(tangent, spheres[triple.get(1)]);
								const SimplePoint tp2=spheres_touching_point(tangent, spheres[triple.get(2)]);
								const int protagonist_halfspace=halfspace(tp0, tp1, tp2,
										spheres_touching_point(tangent, protagonist_sphere));
								const int antagonist_halfspace=halfspace(tp0, tp1, tp2,
										spheres_touching_point(tangent, antagonist_sphere));
								if(protagonist_halfspace==antagonist_halfspace)
								{
									tangent_valid=false;
								}
							}
						}
						if(tangent_valid)
						{
							checked_tangents.push_back(tangent);
						}
					}
					return checked_tangents;
				}
			}
		}

		void remove_tangents(const std::set<std::size_t>& ids_to_remove)
		{
			if(!ids_to_remove.empty())
			{
				std::vector<Sphere> refined_tangents;
				for(std::size_t i=0;i<tangents.size();i++)
				{
					if(ids_to_remove.find(i)==ids_to_remove.end())
					{
						refined_tangents.push_back(tangents[i]);
					}
				}
				tangents=refined_tangents;
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
			const std::tr1::unordered_set<std::size_t>& visited;
			std::set<std::size_t> failed_tangents;

			LeafChecker(
					const Exposition& exposition,
					const std::vector<Sphere>& spheres,
					const std::tr1::unordered_set<std::size_t>& visited) :
						exposition(exposition),
						spheres(spheres),
						visited(visited)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id)
			{
				if(id<spheres.size() && id!=exposition.protagonist && !exposition.triple.contains(id))
				{
					for(std::size_t j=0;j<exposition.tangents.size();j++)
					{
						if(sphere_intersects_sphere(exposition.tangents[j], spheres[id]))
						{
							failed_tangents.insert(j);
						}
					}
					if(failed_tangents.size()==exposition.tangents.size())
					{
						return std::make_pair(true, visited.find(id)==visited.end());
					}
				}
				return std::make_pair(false, false);
			}
		};
	};

	bool check_exposition(const Exposition& exposition) const
	{
		if(exposition.tangents.empty())
		{
			return false;
		}
		typename intersection_search_operators::NodeChecker node_checker(exposition);
		typename intersection_search_operators::LeafChecker leaf_checker(exposition, spheres_, std::tr1::unordered_set<std::size_t>());
		return spheres_clustering<Sphere>::search_in_clusters_layers_narrow(
				clusters_layers_,
				node_checker,
				leaf_checker).empty();
	}

	Exposition find_first_exposition() const
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres_, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere>, spheres_.front(), std::tr1::placeholders::_1));
		const std::size_t a=0;
		for(std::size_t b=a+1;b<traversal.size();b++)
		{
			for(std::size_t c=b+1;c<traversal.size();c++)
			{
				for(std::size_t d=c+1;d<traversal.size();d++)
				{
					const Exposition exposition=Exposition(
							make_triple(traversal[a], traversal[b], traversal[c]),
							traversal[d],
							std::set<std::size_t>(),
							spheres_);
					if(exposition.tangents.size()==1 && check_exposition(exposition))
					{
						return exposition;
					}
				}
			}
		}
		return Exposition();
	}

	Exposition find_any_protagonistic_exposition(const Triple& triple, const std::set<std::size_t>& antagonists) const
	{
		for(std::size_t i=0;i<randomized_search_list_.size();i++)
		{
			const std::size_t protagonist=randomized_search_list_[i];
			if(antagonists.find(protagonist)==antagonists.end() && !triple.contains(protagonist))
			{
				const Exposition protagonistic_exposition(triple, protagonist, antagonists, spheres_);
				if(!protagonistic_exposition.tangents.empty())
				{
					return protagonistic_exposition;
				}
			}
		}
		return Exposition();
	}

	Exposition find_valid_protagonistic_exposition(const Triple& triple, const std::set<std::size_t>& antagonists) const
	{
		Exposition exposition=find_any_protagonistic_exposition(triple, antagonists);
		std::tr1::unordered_set<std::size_t> visited;
		while(!exposition.tangents.empty())
		{
			typename intersection_search_operators::NodeChecker node_checker(exposition);
			typename intersection_search_operators::LeafChecker leaf_checker(exposition, spheres_, visited);

			const std::vector<std::size_t> result=spheres_clustering<Sphere>::search_in_clusters_layers_narrow(
					clusters_layers_,
					node_checker,
					leaf_checker);

			if(result.empty())
			{
				exposition.remove_tangents(leaf_checker.failed_tangents);
				return exposition;
			}
			else
			{
				const std::size_t last_id=result.back();
				if(visited.find(last_id)==visited.end())
				{
					visited.insert(last_id);
					exposition=Exposition(triple, last_id, antagonists, spheres_);
				}
				else
				{
					return Exposition();
				}
			}
		}
		return Exposition();
	}

	const std::vector<Sphere>& spheres_;
	const std::vector<std::size_t> randomized_search_list_;
	const ClustersLayers clusters_layers_;
};

}

#endif /* APOLLONIUS_GRAPH_H_ */
