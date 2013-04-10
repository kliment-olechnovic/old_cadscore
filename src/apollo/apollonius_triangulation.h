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

template<typename SpheresHierarchyType>
class ApolloniusTriangulation
{
public:
	typedef SpheresHierarchyType Hierarchy;
	typedef typename Hierarchy::Sphere Sphere;
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;
	typedef std::vector< std::vector<std::size_t> > Graph;
	typedef std::tr1::unordered_map<Pair, std::tr1::unordered_set<std::size_t>, Pair::HashFunctor> PairsNeighboursMap;
	typedef std::tr1::unordered_map<Triple, std::tr1::unordered_set<std::size_t>, Triple::HashFunctor> TriplesNeighboursMap;

	struct Log
	{
		std::size_t quadruples;
		std::size_t tangent_spheres;
		std::size_t difficult_faces;
		std::size_t d_searches;
		std::size_t e_searches;
		std::size_t produced_faces;
		std::size_t updated_faces;
		std::size_t triples_repetitions;
		std::size_t finding_first_faces_iterations;
		std::size_t finding_any_d_node_checks;
		std::size_t finding_any_d_leaf_checks;
		std::size_t finding_valid_d_node_checks;
		std::size_t finding_valid_d_leaf_checks;
		std::size_t finding_valid_e_node_checks;
		std::size_t finding_valid_e_leaf_checks;

		void print(std::ostream& output) const
		{
			output << "quadruples                      " << quadruples << "\n";
			output << "tangent_spheres                 " << tangent_spheres << "\n";
			output << "difficult_faces                 " << difficult_faces << "\n";
			output << "d_searches                      " << d_searches << "\n";
			output << "e_searches                      " << e_searches << "\n";
			output << "produced_faces                  " << produced_faces << "\n";
			output << "updated_faces                   " << updated_faces << "\n";
			output << "triples_repetitions             " << triples_repetitions << "\n";
			output << "finding_first_faces_iterations  " << finding_first_faces_iterations << "\n";
			output << "finding_any_d_node_checks       " <<  finding_any_d_node_checks<< "\n";
			output << "finding_any_d_leaf_checks       " << finding_any_d_leaf_checks << "\n";
			output << "finding_valid_d_node_checks     " << finding_valid_d_node_checks << "\n";
			output << "finding_valid_d_leaf_checks     " << finding_valid_d_leaf_checks << "\n";
			output << "finding_valid_e_node_checks     " << finding_valid_e_node_checks << "\n";
			output << "finding_valid_e_leaf_checks     " << finding_valid_e_leaf_checks << "\n";
		}
	};

	static QuadruplesMap find_quadruples(const Hierarchy& hierarchy, bool enable_searching_for_d3)
	{
		log_ref()=Log();
		QuadruplesMap quadruples_map;
		std::tr1::unordered_set<Triple, Triple::HashFunctor> processed_triples;
		std::size_t difficult_faces_count=0;

		std::deque<Face> stack=find_first_faces(hierarchy);
		std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> stack_map;
		for(std::size_t i=0;i<stack.size();i++)
		{
			stack_map[stack[i].abc_ids()]=i;
		}
		if(!stack.empty())
		{
			quadruples_map[stack.front().quadruple_with_d1()].push_back(stack.front().d1_tangent_sphere());
			while(!stack.empty())
			{
				Face face=stack.back();
				stack.pop_back();
				stack_map.erase(face.abc_ids());
				{
					if(!face.can_have_d2())
					{
						difficult_faces_count++;
						log_ref().difficult_faces++;
					}
					const bool found_d2=(face.d2_id()==Face::npos) && face.can_have_d2() && find_valid_d2(hierarchy, face);
					const bool found_d3=enable_searching_for_d3 && face.d3_ids_and_tangent_spheres().empty() && face.can_have_d3() && find_valid_d3(hierarchy, face);
					if(found_d2 || found_d3)
					{
						std::vector< std::vector< std::pair<Quadruple, SimpleSphere> > > all_produced_quadruples;
						if(found_d2)
						{
							all_produced_quadruples.push_back(face.produce_quadruple_with_d2());
						}
						if(found_d3)
						{
							all_produced_quadruples.push_back(face.produce_quadruples_with_d3());
						}
						for(std::size_t l=0;l<all_produced_quadruples.size();l++)
						{
							const std::vector< std::pair<Quadruple, SimpleSphere> >& produced_quadruples=all_produced_quadruples[l];
							for(std::size_t i=0;i<produced_quadruples.size();i++)
							{
								const Quadruple& quadruple=produced_quadruples[i].first;
								const SimpleSphere& quadruple_tangent_sphere=produced_quadruples[i].second;
								QuadruplesMap::iterator qm_it=quadruples_map.find(quadruple);
								if(qm_it==quadruples_map.end())
								{
									log_ref().quadruples++;
									log_ref().tangent_spheres++;
									quadruples_map[quadruple].push_back(quadruple_tangent_sphere);
								}
								else
								{
									std::vector<SimpleSphere>& quadruple_tangent_spheres_list=qm_it->second;
									if(quadruple_tangent_spheres_list.size()==1 && !spheres_equal(quadruple_tangent_spheres_list.front(), quadruple_tangent_sphere))
									{
										log_ref().tangent_spheres++;
										quadruple_tangent_spheres_list.push_back(quadruple_tangent_sphere);
									}
								}
							}
						}

						std::vector< std::vector<Face> > all_produced_faces;
						if(found_d2)
						{
							all_produced_faces.push_back(face.produce_faces_with_d2());
						}
						if(found_d3)
						{
							all_produced_faces.push_back(face.produce_faces_with_d3());
						}
						for(std::size_t l=0;l<all_produced_faces.size();l++)
						{
							const std::vector<Face>& produced_faces=all_produced_faces[l];
							for(std::size_t i=0;i<produced_faces.size();i++)
							{
								const Face& produced_face=produced_faces[i];
								if(processed_triples.count(produced_face.abc_ids())==0)
								{
									std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor>::const_iterator sm_it=stack_map.find(produced_faces[i].abc_ids());
									if(sm_it==stack_map.end())
									{
										log_ref().produced_faces++;
										stack_map[produced_face.abc_ids()]=stack.size();
										stack.push_back(produced_face);
									}
									else
									{
										log_ref().updated_faces++;
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
								else
								{
									log_ref().triples_repetitions++;
								}
							}
						}
					}
					processed_triples.insert(face.abc_ids());
				}
			}
		}

		if(monitoring_level()>0)
		{
			std::clog << "spheres " << hierarchy.spheres().size() << "\n";
			std::clog << "quadruples " << quadruples_map.size() << "\n";
			std::clog << "triples " << processed_triples.size() << "\n";
			std::clog << "difficulties " << difficult_faces_count << "\n";
			if(monitoring_level()>1)
			{
				std::size_t tangent_spheres_count=0;
				for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
				{
					tangent_spheres_count+=it->second.size();
				}
				std::clog << "tangents " << tangent_spheres_count << "\n";
			}
			if(monitoring_level()>2)
			{
				std::vector<int> spheres_inclusion_map(hierarchy.spheres().size(), 0);
				for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
				{
					const Quadruple& q=it->first;
					for(int i=0;i<4;i++)
					{
						spheres_inclusion_map[q.get(i)]=1;
					}
				}
				std::size_t ignored_spheres_count=0;
				for(std::size_t i=0;i<spheres_inclusion_map.size();i++)
				{
					if(spheres_inclusion_map[i]==0)
					{
						ignored_spheres_count++;
					}
				}
				std::clog << "ignored " << ignored_spheres_count;
				for(std::size_t i=0;i<spheres_inclusion_map.size();i++)
				{
					if(spheres_inclusion_map[i]==0)
					{
						std::clog << " " << i;
					}
				}
				std::clog << "\n";
			}
			if(monitoring_level()>3)
			{
				const std::set<std::size_t> hidden_spheres_ids=hierarchy.find_all_hidden_spheres();
				std::clog << "hidden " << hidden_spheres_ids.size();
				for(std::set<std::size_t>::const_iterator it=hidden_spheres_ids.begin();it!=hidden_spheres_ids.end();++it)
				{
					std::clog << " " << (*it);
				}
				std::clog << "\n";
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

	static int& monitoring_level_reference()
	{
		static int monitoring_level=0;
		return monitoring_level;
	}

	static int monitoring_level()
	{
		return monitoring_level_reference();
	}

	static Log log()
	{
		return log_ref();
	}

private:
	typedef ApolloniusFace<Sphere> Face;

	struct checkers_for_any_d2
	{
		struct NodeChecker
		{
			const Face& face;
			const bool constrained;
			const SimpleSphere constraint_sphere;

			NodeChecker(const Face& target) :
				face(target),
				constrained(false)
			{
			}

			NodeChecker(const Face& target, const double constraint_radius) :
				face(target),
				constrained(true),
				constraint_sphere(constrained ? custom_sphere_from_point<SimpleSphere>(face.d1_tangent_sphere(), constraint_radius) : SimpleSphere())
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				log_ref().finding_any_d_node_checks++;
				return ((!constrained || sphere_intersects_sphere(constraint_sphere, sphere)) && face.sphere_may_contain_candidate_for_d2(sphere));
			}
		};

		struct LeafChecker
		{
			Face& face;

			LeafChecker(Face& target) : face(target) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				log_ref().finding_any_d_leaf_checks++;
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
				log_ref().finding_valid_d_node_checks++;
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
				log_ref().finding_valid_d_leaf_checks++;
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
				log_ref().finding_valid_e_node_checks++;
				return face.sphere_may_contain_candidate_for_d3(sphere);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const Hierarchy& hierarchy;

			LeafChecker(Face& target, const Hierarchy& hierarchy) : face(target), hierarchy(hierarchy) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				log_ref().finding_valid_e_leaf_checks++;
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

	static Log& log_ref()
	{
		static Log log=Log();
		return log;
	}

	static std::deque<Face> find_first_faces(const Hierarchy& hierarchy)
	{
		const std::vector<Sphere>& spheres=hierarchy.spheres();
		std::deque<Face> result;
		int tries_before_success=0;
		if(!spheres.empty())
		{
			const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres.front(), std::tr1::placeholders::_1));
			for(std::size_t u=4;u<traversal.size();u++)
			{
				for(std::size_t a=0;a<u;a++)
				{
					for(std::size_t b=a+1;b<u;b++)
					{
						for(std::size_t c=b+1;c<u;c++)
						{
							for(std::size_t d=((a+1<u && b+1<u && c+1<u) ? (u-1) : (c+1));d<u;d++)
							{
								tries_before_success++;
								log_ref().finding_first_faces_iterations++;
								Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
								std::vector<SimpleSphere> tangents=construct_spheres_tangent_sphere<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
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
										if(monitoring_level()>0)
										{
											std::clog << "brute " << tries_before_success << "\n";
										}
										return result;
									}
								}
							}
						}
					}
				}
			}
		}
		if(monitoring_level()>0)
		{
			std::clog << "brute " << tries_before_success << "\n";
		}
		return result;
	}

	static bool find_any_d2(const Hierarchy& hierarchy, Face& face)
	{
		if(face.d2_id()==Face::npos)
		{
			typename checkers_for_any_d2::LeafChecker leaf_checker(face);
			typename checkers_for_any_d2::NodeChecker node_checker_with_constraint(face, 0-face.d2_tangent_sphere().r);
			hierarchy.search(node_checker_with_constraint, leaf_checker);
			if(node_checker_with_constraint.constrained && face.d2_id()==Face::npos)
			{
				typename checkers_for_any_d2::NodeChecker node_checker_without_constraint(face);
				hierarchy.search(node_checker_without_constraint, leaf_checker);
			}
		}
		return (face.d2_id()!=Face::npos);
	}

	static bool find_valid_d2(const Hierarchy& hierarchy, Face& face)
	{
		log_ref().d_searches++;
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
		log_ref().e_searches++;
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
