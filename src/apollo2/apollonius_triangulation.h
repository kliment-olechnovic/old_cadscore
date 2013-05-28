#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <algorithm>

#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include "tuple.h"
#include "bounding_spheres_hierarchy.h"
#include "search_for_spherical_collisions.h"
#include "tangent_plane_of_three_spheres.h"
#include "tangent_sphere_of_four_spheres.h"
#include "tangent_sphere_of_three_spheres.h"

#include "apollonius_triangulation/search_for_valid_quadruples.h"
#include "apollonius_triangulation/search_for_surplus_valid_quadruples.h"

namespace apollo2
{

class ApolloniusTriangulation
{
public:
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;
	typedef std::tr1::unordered_map<std::size_t, std::tr1::unordered_set<std::size_t> > NeighborsMap;
	typedef std::vector< std::vector<std::size_t> > NeighborsGraph;
	typedef std::tr1::unordered_map<Pair, std::tr1::unordered_set<std::size_t>, Pair::HashFunctor> PairsNeighborsMap;
	typedef std::tr1::unordered_map<Triple, std::tr1::unordered_set<std::size_t>, Triple::HashFunctor> TriplesNeighborsMap;

	struct Result
	{
		QuadruplesMap quadruples_map;
		apollonius_triangulation::SearchForValidQuadruples::Log quadruples_log;
		apollonius_triangulation::SearchForSurplusValidQuadruples::Log surplus_quadruples_log;
		std::set<std::size_t> hidden_spheres_ids;
		std::set<std::size_t> ignored_spheres_ids;
	};

	template<typename SphereType>
	static Result construct(const std::vector<SphereType>& spheres, const double initial_radius_for_spheres_bucketing, const bool perform_augmentation)
	{
		Result result;
		BoundingSpheresHierarchy<SphereType> bsh(spheres, initial_radius_for_spheres_bucketing, 1);
		result.hidden_spheres_ids=SearchForSphericalCollisions::find_all_hidden_spheres(bsh);
		for(std::set<std::size_t>::const_iterator it=result.hidden_spheres_ids.begin();it!=result.hidden_spheres_ids.end();++it)
		{
			bsh.ignore_leaf_sphere(*it);
		}
		result.quadruples_map=apollonius_triangulation::SearchForValidQuadruples::find_valid_quadruples(bsh, result.quadruples_log);
		if(perform_augmentation)
		{
			result.quadruples_map=apollonius_triangulation::SearchForSurplusValidQuadruples::find_surplus_valid_quadruples(bsh, result.quadruples_map, result.surplus_quadruples_log);
		}
		std::vector<int> spheres_inclusion_map(spheres.size(), 0);
		for(QuadruplesMap::const_iterator it=result.quadruples_map.begin();it!=result.quadruples_map.end();++it)
		{
			const Quadruple& q=it->first;
			for(int i=0;i<4;i++)
			{
				spheres_inclusion_map[q.get(i)]=1;
			}
		}
		for(std::size_t i=0;i<spheres_inclusion_map.size();i++)
		{
			if(spheres_inclusion_map[i]==0 && result.hidden_spheres_ids.count(i)==0)
			{
				result.ignored_spheres_ids.insert(i);
			}
		}
		return result;
	}

	static NeighborsMap collect_neighbors_map_from_quadruples_map(const QuadruplesMap& quadruples_map)
	{
		NeighborsMap neighbors_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int a=0;a<4;a++)
			{
				for(int b=a+1;b<4;b++)
				{
					neighbors_map[quadruple.get(a)].insert(quadruple.get(b));
					neighbors_map[quadruple.get(b)].insert(quadruple.get(a));
				}
			}
		}
		return neighbors_map;
	}

	static NeighborsGraph collect_neighbors_graph_from_neighbors_map(const NeighborsMap& neighbors_map, const std::size_t number_of_vertices)
	{
		NeighborsGraph neighbors_graph(number_of_vertices);
		for(NeighborsMap::const_iterator it=neighbors_map.begin();it!=neighbors_map.end();++it)
		{
			if((it->first)<neighbors_graph.size())
			{
				neighbors_graph[it->first].insert(neighbors_graph[it->first].end(), it->second.begin(), it->second.end());
			}
		}
		return neighbors_graph;
	}

	static PairsNeighborsMap collect_pairs_neighbors_map_from_quadruples_map(const QuadruplesMap& quadruples_map)
	{
		PairsNeighborsMap pairs_neighbors_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int a=0;a<4;a++)
			{
				for(int b=a+1;b<4;b++)
				{
					const Pair pair(quadruple.get(a), quadruple.get(b));
					for(int c=0;c<4;c++)
					{
						if(c!=a && c!=b)
						{
							pairs_neighbors_map[pair].insert(quadruple.get(c));
						}
					}
				}
			}
		}
		return pairs_neighbors_map;
	}

	static TriplesNeighborsMap collect_triples_neighbors_map_from_quadruples_map(const QuadruplesMap& quadruples_map)
	{
		TriplesNeighborsMap triples_neighbors_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int a=0;a<4;a++)
			{
				triples_neighbors_map[quadruple.exclude(a)].insert(quadruple.get(a));
			}
		}
		return triples_neighbors_map;
	}

	static void print_quadruples_map(const QuadruplesMap& quadruples_map, std::ostream& output)
	{
		output.precision(std::numeric_limits<double>::digits10);
		output << std::fixed;
		const QuadruplesOrderedMap quadruples_ordered_map=collect_ordered_map_of_quadruples(quadruples_map);
		for(QuadruplesOrderedMap::const_iterator it=quadruples_ordered_map.begin();it!=quadruples_ordered_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			const std::vector<SimpleSphere>& tangent_spheres=it->second;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				output << quadruple.get(0) << " " << quadruple.get(1) << " " << quadruple.get(2) << " " << quadruple.get(3) << " ";
				output << tangent_sphere.x << " " << tangent_sphere.y << " " << tangent_sphere.z << " " << tangent_sphere.r << "\n";
			}
		}
	}

	static QuadruplesMap read_quadruples_map(std::istream& input)
	{
		QuadruplesMap quadruples_map;
		bool valid=true;
		while(input.good() && valid)
		{
			std::string line;
			std::getline(input, line);
			if(line.empty())
			{
				valid=false;
			}
			else
			{
				std::istringstream line_stream(line);
				std::size_t q[4]={0, 0, 0, 0};
				double s[4]={0, 0, 0, 0};
				for(int i=0;i<4 && valid;i++)
				{
					line_stream >> q[i];
					valid=!line_stream.fail();
				}
				for(int i=0;i<4 && valid;i++)
				{
					line_stream >> s[i];
					valid=!line_stream.fail();
				}
				if(valid)
				{
					quadruples_map[Quadruple(q[0], q[1], q[2], q[3])].push_back(SimpleSphere(s[0], s[1], s[2], s[3]));
				}
			}
		}
		return quadruples_map;
	}

	static void print_result_log(const Result& result, std::ostream& output)
	{
		output << "quadruples " << result.quadruples_log.quadruples << "\n";
		output << "tangent_spheres " << result.quadruples_log.tangent_spheres << "\n";
		output << "processed_faces " << result.quadruples_log.processed_faces << "\n";
		output << "difficult_faces " << result.quadruples_log.difficult_faces << "\n";
		output << "first_faces_iterations " << result.quadruples_log.finding_first_faces_iterations << "\n";
		output << "surplus_quadruples " << result.surplus_quadruples_log.surplus_quadruples << "\n";
		output << "surplus_tangent_spheres " << result.surplus_quadruples_log.surplus_tangent_spheres << "\n";
		output << "hidden_spheres " << result.hidden_spheres_ids.size() << "\n";
		output << "ignored_spheres " << result.ignored_spheres_ids.size() << "\n";
		output << "used_epsilon " << comparison_epsilon() << "\n";
	}

	template<typename SphereType>
	static bool check_quadruples_map(const std::vector<SphereType>& spheres, const QuadruplesMap& quadruples_map)
	{
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const QuadruplesMap::key_type& q=it->first;
			const QuadruplesMap::mapped_type& ts=it->second;
			if(q.has_repetetions() || ts.empty() || ts.size()>2 || (ts.size()==2 && spheres_equal(ts.front(), ts.back())))
			{
				return false;
			}
			for(std::size_t i=0;i<ts.size();i++)
			{
				const SimpleSphere& t=ts[i];
				for(int j=0;j<4;j++)
				{
					if(!sphere_touches_sphere(t, spheres[q.get(j)]))
					{
						return false;
					}
				}
				for(std::size_t j=0;j<spheres.size();j++)
				{
					if(sphere_intersects_sphere(t, spheres[j]))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

private:
	typedef std::map<QuadruplesMap::key_type, QuadruplesMap::mapped_type> QuadruplesOrderedMap;

	static QuadruplesOrderedMap collect_ordered_map_of_quadruples(const QuadruplesMap& quadruples_map)
	{
		QuadruplesOrderedMap quadruples_ordered_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			quadruples_ordered_map.insert(*it);
		}
		return quadruples_ordered_map;
	}
};

class DifferenceBetweenSetsOfQuadruples
{
public:
	struct Result
	{
		std::vector<Quadruple> all_differences;
		std::vector<Quadruple> confirmed_differences;
	};

	template<typename SphereType, typename InputQuadruples>
	static Result calculate_directional_difference_between_two_sets_of_quadruples(const std::vector<SphereType>& spheres, const double initial_radius_for_spheres_bucketing, const InputQuadruples& input_quadruples1, const InputQuadruples& input_quadruples2)
	{
		Result result;
		if(!input_quadruples1.empty())
		{
			const std::vector<Quadruple> quadruples1=get_sorted_unique_quadruples(input_quadruples1);
			const std::vector<Quadruple> quadruples2=get_sorted_unique_quadruples(input_quadruples2);

			result.all_differences.resize(quadruples1.size());
			std::vector<Quadruple>::iterator it=std::set_difference(quadruples1.begin(), quadruples1.end(), quadruples2.begin(), quadruples2.end(), result.all_differences.begin());
			result.all_differences.resize(it-result.all_differences.begin());

			if(!result.all_differences.empty())
			{
				const BoundingSpheresHierarchy<SphereType> bsh(spheres, initial_radius_for_spheres_bucketing, 1);
				const std::vector<SimpleSphere> empty_tangent_spheres2=get_empty_tangent_spheres_from_quadruples(bsh, quadruples2);
				for(std::size_t i=0;i<result.all_differences.size();i++)
				{
					const Quadruple q=result.all_differences[i];
					if(q.get(0)<bsh.leaves_spheres().size() && q.get(1)<bsh.leaves_spheres().size() && q.get(2)<bsh.leaves_spheres().size() && q.get(3)<bsh.leaves_spheres().size())
					{
						const std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(bsh.leaves_spheres().at(q.get(0)), bsh.leaves_spheres().at(q.get(1)), bsh.leaves_spheres().at(q.get(2)), bsh.leaves_spheres().at(q.get(3)));
						if(
								(tangents.size()==1 && SearchForSphericalCollisions::find_any_collision(bsh, tangents.front()).empty())
								|| (tangents.size()==2 && (SearchForSphericalCollisions::find_any_collision(bsh, tangents.front()).empty() || SearchForSphericalCollisions::find_any_collision(bsh, tangents.back()).empty()))
							)
						{
							result.confirmed_differences.push_back(q);
						}
					}
				}
			}
		}
		return result;
	}

private:
	template<typename InputQuadruples>
	static std::vector<Quadruple> get_sorted_unique_quadruples(const InputQuadruples& input_quadruples)
	{
		std::vector<Quadruple> result(input_quadruples.begin(), input_quadruples.end());
		if(!result.empty())
		{
			std::sort(result.begin(), result.end());
			std::vector<Quadruple>::iterator it=std::unique(result.begin(), result.end());
			result.resize(it-result.begin());
		}
		return result;
	}

	template<typename SphereType, typename InputQuadruples>
	static std::vector<SimpleSphere> get_empty_tangent_spheres_from_quadruples(const BoundingSpheresHierarchy<SphereType>& bsh, const InputQuadruples& quadruples)
	{
		std::vector<SimpleSphere> result;
		result.reserve(quadruples.size()+(quadruples.size()/10));
		for(typename InputQuadruples::const_iterator it=quadruples.begin();it!=quadruples.end();++it)
		{
			const Quadruple& q=(*it);
			if(q.get(0)<bsh.leaves_spheres().size() && q.get(1)<bsh.leaves_spheres().size() && q.get(2)<bsh.leaves_spheres().size() && q.get(3)<bsh.leaves_spheres().size())
			{
				const std::vector<SimpleSphere> tangent_spheres=TangentSphereOfFourSpheres::calculate<SimpleSphere>(bsh.leaves_spheres().at(q.get(0)), bsh.leaves_spheres().at(q.get(1)), bsh.leaves_spheres().at(q.get(2)), bsh.leaves_spheres().at(q.get(3)));
				for(std::size_t j=0;j<tangent_spheres.size();j++)
				{
					const SimpleSphere& tangent_sphere=tangent_spheres[j];
					if(SearchForSphericalCollisions::find_any_collision(bsh, tangent_sphere).empty())
					{
						result.push_back(tangent_sphere);
					}
				}
			}
		}
		return result;
	}
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
