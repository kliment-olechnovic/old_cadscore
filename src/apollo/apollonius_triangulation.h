#ifndef APOLLO_APOLLONIUS_TRIANGULATION_H_
#define APOLLO_APOLLONIUS_TRIANGULATION_H_

#include <vector>
#include <deque>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "apollonius_face.h"

namespace apollo
{

template<typename SpheresHierarchyType, int MonitoringLevel=0>
class ApolloniusTriangulation
{
public:
	typedef SpheresHierarchyType Hierarchy;
	typedef typename Hierarchy::Sphere Sphere;
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;
	typedef std::vector< std::vector<std::size_t> > Graph;
	typedef std::tr1::unordered_map<Pair, std::tr1::unordered_set<std::size_t>, Pair::HashFunctor> PairsNeighboursMap;
	typedef std::tr1::unordered_map<Triple, std::tr1::unordered_set<std::size_t>, Triple::HashFunctor> TriplesNeighboursMap;

	static QuadruplesMap find_quadruples(const Hierarchy& hierarchy, bool enable_searching_for_d3)
	{
		QuadruplesMap quadruples_map;

		std::deque<Face> stack=find_first_faces(hierarchy);
		std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> stack_map;
		for(std::size_t i=0;i<stack.size();i++)
		{
			stack_map[stack[i].abc_ids()]=i;
		}
		if(!stack.empty())
		{
			std::tr1::unordered_set<Triple, Triple::HashFunctor> processed_triples;

			quadruples_map[stack.front().quadruple_with_d1()].push_back(stack.front().d1_tangent_sphere());

			while(!stack.empty())
			{
				Face face=stack.back();
				stack.pop_back();
				stack_map.erase(face.abc_ids());
				{
					const bool found_d2=(face.d2_id()==Face::npos) && face.can_have_d2() && find_valid_d2(hierarchy, face);
					const bool found_d3=enable_searching_for_d3 && face.can_have_d3() && find_valid_d3(hierarchy, face);
					if(found_d2 || found_d3)
					{
						const std::vector< std::pair<Quadruple, SimpleSphere> > produced_quadruples=face.produce_quadruples();
						for(std::size_t i=0;i<produced_quadruples.size();i++)
						{
							const Quadruple& quadruple=produced_quadruples[i].first;
							const SimpleSphere& quadruple_tangent_sphere=produced_quadruples[i].second;
							QuadruplesMap::iterator qm_it=quadruples_map.find(quadruple);
							if(qm_it==quadruples_map.end())
							{
								quadruples_map[quadruple].push_back(quadruple_tangent_sphere);
							}
							else
							{
								std::vector<SimpleSphere>& quadruple_tangent_spheres_list=qm_it->second;
								if(quadruple_tangent_spheres_list.size()==1 && !spheres_equal(quadruple_tangent_spheres_list.front(), quadruple_tangent_sphere))
								{
									quadruple_tangent_spheres_list.push_back(quadruple_tangent_sphere);
								}
							}
						}
						const std::vector<Face> produced_faces=face.produce_faces();
						for(std::size_t i=0;i<produced_faces.size();i++)
						{
							const Face& produced_face=produced_faces[i];
							if(processed_triples.count(produced_face.abc_ids())==0)
							{
								std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor>::const_iterator sm_it=stack_map.find(produced_faces[i].abc_ids());
								if(sm_it==stack_map.end())
								{
									stack_map[produced_face.abc_ids()]=stack.size();
									stack.push_back(produced_face);
								}
								else
								{
									const std::size_t candidate_id=produced_face.d1_id();
									Face& stacked_face=stack[sm_it->second];
									if(candidate_id!=stacked_face.d1_id() && stacked_face.d2_id()==Face::npos)
									{
										const std::pair<bool, SimpleSphere> candidate_tangent_sphere=stacked_face.check_candidate_for_d2(candidate_id);
										if(candidate_tangent_sphere.first)
										{
											stacked_face.set_d2_and_unset_d3(candidate_id, candidate_tangent_sphere.second);
										}
									}
								}
							}
						}
					}
					processed_triples.insert(face.abc_ids());
				}
			}

			if(MonitoringLevel==1)
			{
				std::clog << "quadruples " << quadruples_map.size() << "\n";
				std::clog << "triples " << processed_triples.size() << "\n";
			}
		}

		return quadruples_map;
	}

	static Graph construct_graph_from_quadruples(const QuadruplesMap& quadruples_map, const std::size_t graph_size)
	{
		typedef std::tr1::unordered_map<std::size_t, std::tr1::unordered_set<std::size_t> > GraphMap;
		GraphMap graph_map;
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int i=0;i<4;i++)
			{
				for(int j=0;j<4;j++)
				{
					if(i!=j)
					{
						graph_map[quadruple.get(i)].insert(quadruple.get(j));
					}
				}
			}
		}
		Graph graph(std::max(graph_map.size(), graph_size));
		for(GraphMap::const_iterator it=graph_map.begin();it!=graph_map.end();it++)
		{
			graph[it->first].insert(graph[it->first].end(), it->second.begin(), it->second.end());
		}
		return graph;
	}

	static PairsNeighboursMap collect_pairs_neighbours_from_quadruples(const QuadruplesMap& quadruples_map)
	{
		PairsNeighboursMap sets_map;
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int i=0;i<4;i++)
			{
				for(int j=i+1;j<4;j++)
				{
					const Pair pair=make_pair(quadruple.get(i), quadruple.get(j));
					for(int k=0;k<4;k++)
					{
						if(k!=i && k!=j)
						{
							sets_map[pair].insert(quadruple.get(k));
						}
					}
				}
			}
		}
		return sets_map;
	}

	static TriplesNeighboursMap collect_triples_neighbours_from_quadruples(const QuadruplesMap& quadruples_map)
	{
		TriplesNeighboursMap sets_map;
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& quadruple=it->first;
			for(int i=0;i<4;i++)
			{
				const Triple triple=quadruple.exclude(i);
				sets_map[triple].insert(quadruple.get(i));
			}
		}
		return sets_map;
	}

	static bool check_quadruples(const QuadruplesMap& quadruples_map, const std::vector<Sphere>& spheres)
	{
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const Quadruple& q=it->first;
			if(q.has_repetetions())
			{
				return false;
			}
			const std::vector<SimpleSphere>& ts=it->second;
			if(ts.empty() || ts.size()>2)
			{
				return false;
			}
			if(ts.size()==2 && spheres_equal(ts.front(), ts.back()))
			{
				return false;
			}
			for(std::size_t i=0;i<ts.size();i++)
			{
				const SimpleSphere& t=ts[i];
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

private:
	typedef ApolloniusFace<Sphere> Face;

	struct checkers_for_any_d2
	{
		struct NodeChecker
		{
			const Face& face;

			NodeChecker(const Face& target) : face(target) {}

			bool operator()(const SimpleSphere& sphere) const
			{
				return face.sphere_may_contain_candidate_for_d2(sphere);
			}
		};

		struct LeafChecker
		{
			Face& face;

			LeafChecker(Face& target) : face(target) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d2(id);
				if(check_result.first)
				{
					face.set_d2_and_unset_d3(id, check_result.second);
					return std::make_pair(true, true);
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_d2
	{
		struct NodeChecker
		{
			const Face& face;

			NodeChecker(const Face& target) : face(target) {}

			bool operator()(const SimpleSphere& sphere) const
			{
				return sphere_intersects_sphere(sphere, face.d2_tangent_sphere());
			}
		};

		struct LeafChecker
		{
			Face& face;

			std::tr1::unordered_set<std::size_t> visited;

			LeafChecker(Face& target) : face(target) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				if(sphere_intersects_sphere(sphere, face.d2_tangent_sphere()))
				{
					if(visited.find(id)!=visited.end())
					{
						return std::make_pair(true, false);
					}
					else
					{
						visited.insert(id);

						std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d2(id);
						if(check_result.first)
						{
							face.set_d2_and_unset_d3(id, check_result.second);
							return std::make_pair(true, true);
						}
						else
						{
							return std::make_pair(true, false);
						}
					}
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_d3
	{
		struct NodeChecker
		{
			const Face& face;

			NodeChecker(const Face& target) : face(target) {}

			bool operator()(const SimpleSphere& sphere) const
			{
				return face.sphere_may_contain_candidate_for_d3(sphere);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const Hierarchy& hierarchy;

			LeafChecker(Face& target, const Hierarchy& hierarchy) : face(target), hierarchy(hierarchy) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				std::pair<bool, std::vector<SimpleSphere> > check_result=face.check_candidate_for_d3(id);
				if(check_result.first)
				{
					std::vector<SimpleSphere> valid_tangents;
					for(std::size_t i=0;i<check_result.second.size();i++)
					{
						if(hierarchy.find_any_collision(check_result.second[i]).empty())
						{
							valid_tangents.push_back(check_result.second[i]);
						}
					}
					if(!valid_tangents.empty())
					{
						face.add_d3(id, valid_tangents);
						return std::make_pair(true, false);
					}
				}
				return std::make_pair(false, false);
			}
		};
	};

	static std::deque<Face> find_first_faces(const Hierarchy& hierarchy)
	{
		const std::vector<Sphere>& spheres=hierarchy.spheres();
		std::deque<Face> result;
		int tries_before_success=0;
		if(!spheres.empty())
		{
			const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres.front(), std::tr1::placeholders::_1));
			const std::size_t a=0;
			for(std::size_t b=a+1;b<traversal.size();b++)
			{
				for(std::size_t c=b+1;c<traversal.size();c++)
				{
					for(std::size_t d=c+1;d<traversal.size();d++)
					{
						Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
						std::vector<SimpleSphere> tangents=construct_spheres_tangent<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
						if(tangents.size()==1 && hierarchy.find_any_collision(tangents.front()).empty())
						{
							for(int i=0;i<4;i++)
							{
								const Face face(spheres, quadruple.exclude(i), quadruple.get(i), tangents.front());
								if(face.valid())
								{
									result.push_back(face);
								}
							}
							if(!result.empty())
							{
								if(MonitoringLevel==1)
								{
									std::clog << "brute " << tries_before_success << "\n";
								}
								return result;
							}
						}
						else
						{
							tries_before_success++;
						}
					}
				}
			}
		}
		return result;
	}

	static bool find_any_d2(const Hierarchy& hierarchy, Face& face)
	{
		if(face.d2_id()==Face::npos)
		{
			typename checkers_for_any_d2::NodeChecker node_checker(face);
			typename checkers_for_any_d2::LeafChecker leaf_checker(face);
			hierarchy.search(node_checker, leaf_checker);
		}
		return (face.d2_id()!=Face::npos);
	}

	static bool find_valid_d2(const Hierarchy& hierarchy, Face& face)
	{
		if(find_any_d2(hierarchy, face))
		{
			typename checkers_for_valid_d2::NodeChecker node_checker(face);
			typename checkers_for_valid_d2::LeafChecker leaf_checker(face);
			while(face.d2_id()!=Face::npos)
			{
				const std::vector<std::size_t> results=hierarchy.search(node_checker, leaf_checker);
				if(results.empty())
				{
					return true;
				}
				else
				{
					if(face.d2_id()!=results.back())
					{
						face.unset_d2_and_d3();
					}
				}
			}
		}
		return false;
	}

	static bool find_valid_d3(const Hierarchy& hierarchy, Face& face)
	{
		typename checkers_for_valid_d3::NodeChecker node_checker(face);
		typename checkers_for_valid_d3::LeafChecker leaf_checker(face, hierarchy);
		return !hierarchy.search(node_checker, leaf_checker).empty();
	}

	ApolloniusTriangulation()
	{
	}
};

}

#endif /* APOLLO_APOLLONIUS_TRIANGULATION_H_ */
