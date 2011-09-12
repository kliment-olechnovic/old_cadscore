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

	apollonius_graph(
			const std::vector<Sphere>& spheres,
			const double clustering_r,
			const std::size_t clustering_low_count) :
				spheres_(spheres),
				randomized_search_list_(generate_random_permutation(spheres.size())),
				clusters_layers_(spheres_clustering<Sphere>::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count))
	{
	}

	std::deque<Quadruple> find_quadruples() const
	{
		std::deque<Quadruple> quadruples;
		TriplesMap triples_map;
		std::deque<Triple> triples_stack;

		{
			const Quadruple found_quadruple=find_first_quadruple();
			if(!(found_quadruple==Quadruple()))
			{
				quadruples.push_back(found_quadruple);
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
			std::clog << quadruples.size() << " qs\n";
			const Triple triple=triples_stack.back();
			triples_stack.pop_back();
			TriplesMap::const_iterator it=triples_map.find(triple);
//			std::clog << it->second.size() << " ts\n";
			if(it->second.size()==1)
			{
				const std::size_t antagonist=*(it->second.begin());
				const Exposition exposition=find_perfect_exposition(triple, antagonist);
				if(!exposition.tangents.empty())
				{
//					std::clog << check_exposition(exposition) << " che\n";
					const Quadruple found_quadruple(triple, exposition.protagonist);
					quadruples.push_back(found_quadruple);
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
		return quadruples;
	}

private:
	struct Exposition
	{
		Triple triple;
		std::size_t protagonist;
		std::size_t antagonist;
		std::vector<Sphere> tangents;

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
					antagonist(antagonist),
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
				const Sphere& protagonist_sphere=spheres[protagonist];
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

	bool check_exposition(const Exposition& exposition) const
	{
		for(std::size_t j=0;j<exposition.tangents.size();j++)
		{
			for(std::size_t i=0;i<spheres_.size();i++)
			{
				if(sphere_intersects_sphere(exposition.tangents[j], spheres_[i]))
				{
					return false;
				}
			}
		}
		return true;
	}

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
					const Exposition exposition=Exposition::exposition_from_quadruple(quadruple, spheres_);
					if(exposition.tangents.size()==1 && check_exposition(exposition))
					{
						return quadruple;
					}
				}
			}
		}
		return Quadruple();
	}

//	Exposition find_any_protagonistic_exposition(const Triple& triple, const std::size_t antagonist) const
//	{
//		for(std::size_t i=0;i<randomized_search_list_.size();i++)
//		{
//			const std::size_t protagonist=randomized_search_list_[i];
//			if(protagonist!=antagonist && !triple.contains(protagonist))
//			{
//				const Exposition protagonistic_exposition(triple, protagonist, antagonist, spheres_);
//				if(!protagonistic_exposition.tangents.empty())
//				{
//					return protagonistic_exposition;
//				}
//			}
//		}
//		return Exposition();
//	}
//
//	Exposition find_any_better_protagonistic_exposition(const Exposition& old_protagonistic_exposition) const
//	{
//		const std::vector<std::size_t> result=spheres_clustering<Sphere>::search_in_clusters_layers(
//				clusters_layers_,
//				typename intersection_search_operators::NodeChecker(old_protagonistic_exposition),
//				typename intersection_search_operators::LeafChecker(old_protagonistic_exposition, spheres_),
//				1);
//		if(!result.empty())
//		{
//			std::clog << "detected: " << result.front() << "\n";
//			return Exposition(old_protagonistic_exposition.triple, result.front(), old_protagonistic_exposition.antagonist, spheres_);
//		}
//		std::clog << "undetected\n";
//		return Exposition();
//	}
//
//	Exposition find_perfect_exposition(const Triple& triple, const std::size_t antagonist) const
//	{
//		Exposition current=find_any_protagonistic_exposition(triple, antagonist);
//		Exposition last=current;
//		while(!current.tangents.empty())
//		{
////			for(int i=0;i<3;i++)
////			{
////				std::cout << "SPHERE " << spheres_[current.triple.get(i)].r << " " << spheres_[current.triple.get(i)].x << " " << spheres_[current.triple.get(i)].y << " " << spheres_[current.triple.get(i)].z << "\n";
////			}
////			std::cout << "SPHERE " << spheres_[current.protagonist].r << " " << spheres_[current.protagonist].x << " " << spheres_[current.protagonist].y << " " << spheres_[current.protagonist].z << "\n";
////			std::cout << "SPHERE " << current.tangents.front().r << " " << current.tangents.front().x << " " << current.tangents.front().y << " " << current.tangents.front().z << "\n\n";
//			last=current;
//			current=find_any_better_protagonistic_exposition(current);
//		}
//		return last;
//	}

	Exposition find_perfect_exposition(const Triple& triple, const std::size_t antagonist) const
	{
		for(std::size_t i=0;i<spheres_.size();i++)
		{
			const Exposition expo(triple, i, antagonist, spheres_);
			if(check_exposition(expo))
			{
				return expo;
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
