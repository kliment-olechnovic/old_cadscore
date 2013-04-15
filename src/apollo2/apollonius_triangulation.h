#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <vector>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "apollonius_face.h"
#include "spheres_collision_detector.h"

namespace apollo2
{

template<typename SpheresHierarchyType>
class ApolloniusTriangulation
{
public:
	typedef SpheresHierarchyType Hierarchy;
	typedef typename Hierarchy::Sphere Sphere;
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

	struct Log
	{
		std::size_t quadruples;
		std::size_t tangent_spheres;
		std::size_t difficult_faces;
		std::size_t d0_searches;
		std::size_t d1_searches;
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
			output << "d0_searches                     " << d0_searches << "\n";
			output << "d1_searches                     " << d1_searches << "\n";
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

	static QuadruplesMap find_quadruples(const Hierarchy& hierarchy, bool enable_searching_for_e)
	{
		typedef std::tr1::unordered_set<Triple, Triple::HashFunctor> TriplesSet;
		typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;
		log_ref()=Log();
		QuadruplesMap quadruples_map;
		TriplesSet processed_triples_set;
		std::vector<Face> stack=find_first_faces(hierarchy);
		TriplesMap stack_map;
		for(std::size_t i=0;i<stack.size();i++)
		{
			stack_map[stack[i].abc_ids()]=i;
		}
		while(!stack.empty())
		{
			Face face=stack.back();
			stack.pop_back();
			stack_map.erase(face.abc_ids());
			if(!face.can_have_d())
			{
				log_ref().difficult_faces++;
			}
			const bool found_d0=face.can_have_d() && !face.has_d(0) && find_valid_d(hierarchy, face, 0);
			const bool found_d1=face.can_have_d() && !face.has_d(1) && find_valid_d(hierarchy, face, 1);
			const bool found_e=enable_searching_for_e && face.can_have_e() && find_valid_e(hierarchy, face);
			if(found_d0 || found_d1 || found_e)
			{
				const std::vector< std::pair<Quadruple, SimpleSphere> > produced_quadruples=face.produce_quadruples(found_d0, found_d1, found_e);
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
				const std::vector< std::pair<Triple, std::pair<std::size_t, SimpleSphere> > > produced_prefaces=face.produce_prefaces(found_d0, found_d1, found_e);
				for(std::size_t i=0;i<produced_prefaces.size();i++)
				{
					const std::pair<Triple, std::pair<std::size_t, SimpleSphere> >& produced_preface=produced_prefaces[i];
					if(processed_triples_set.count(produced_preface.first)==0)
					{
						TriplesMap::const_iterator sm_it=stack_map.find(produced_preface.first);
						if(sm_it==stack_map.end())
						{
							stack_map[produced_preface.first]=stack.size();
							stack.push_back(Face(hierarchy.spheres(), produced_preface.first, hierarchy.min_input_radius()));
							stack.back().set_d_with_d_number_selection(produced_preface.second.first, produced_preface.second.second);
							log_ref().produced_faces++;
						}
						else
						{
							stack.at(sm_it->second).set_d_with_d_number_selection(produced_preface.second.first, produced_preface.second.second);
							log_ref().updated_faces++;
						}
					}
					else
					{
						log_ref().triples_repetitions++;
					}
				}
			}
			processed_triples_set.insert(face.abc_ids());
		}
		return quadruples_map;
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

	static Log log()
	{
		return log_ref();
	}

private:
	typedef ApolloniusFace<Sphere> Face;

	struct checkers_for_any_d
	{
		struct NodeChecker
		{
			const Face& face;
			const std::size_t d_number;
			bool constrained;
			SimpleSphere constraint_sphere;

			NodeChecker(const Face& target, const std::size_t d_number) : face(target), d_number(d_number), constrained(false)
			{
			}

			bool constrain()
			{
				if(face.has_d(d_number==0 ? 1 : 0))
				{
					constraint_sphere=face.get_d_tangent_sphere(d_number==0 ? 1 : 0);
					constrained=true;
				}
				else
				{
					constrained=false;
				}
				return constrained;
			}

			void unconstrain()
			{
				constrained=false;
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				log_ref().finding_any_d_node_checks++;
				return (!constrained || sphere_intersects_sphere(constraint_sphere, sphere)) && face.sphere_may_contain_candidate_for_d(sphere, d_number);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const std::size_t d_number;

			LeafChecker(Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				log_ref().finding_any_d_leaf_checks++;
				const std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d(id, d_number);
				if(check_result.first)
				{
					face.set_d(id, d_number, check_result.second);
					return std::make_pair(true, true);
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_d
	{
		struct NodeChecker
		{
			const Face& face;
			const std::size_t d_number;

			NodeChecker(const Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				log_ref().finding_valid_d_node_checks++;
				return (face.has_d(d_number) && sphere_intersects_sphere(sphere, face.get_d_tangent_sphere(d_number)));
			}
		};

		struct LeafChecker
		{
			Face& face;
			const std::size_t d_number;

			LeafChecker(Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				log_ref().finding_valid_d_leaf_checks++;
				if(face.has_d(d_number) && sphere_intersects_sphere(sphere, face.get_d_tangent_sphere(d_number)))
				{
					const std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d(id, d_number);
					if(check_result.first)
					{
						face.set_d(id, d_number, check_result.second);
						return std::make_pair(true, true);
					}
					else
					{
						return std::make_pair(true, false);
					}
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_e
	{
		struct NodeChecker
		{
			const Face& face;

			NodeChecker(const Face& target) : face(target)
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				log_ref().finding_valid_e_node_checks++;
				return face.sphere_may_contain_candidate_for_e(sphere);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const Hierarchy& hierarchy;

			LeafChecker(Face& target, const Hierarchy& hierarchy) : face(target), hierarchy(hierarchy)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				log_ref().finding_valid_e_leaf_checks++;
				const std::vector<SimpleSphere> check_result=face.check_candidate_for_e(id);
				bool e_added=false;
				for(std::size_t i=0;i<check_result.size();i++)
				{
					if(SpheresCollisionDetector::find_any_collision(hierarchy, check_result[i]).empty())
					{
						face.add_e(id, check_result[i]);
						e_added=true;
					}
				}
				return std::make_pair(e_added, false);
			}
		};
	};

	static Log& log_ref()
	{
		static Log log=Log();
		return log;
	}

	static std::vector<Face> find_first_faces(const Hierarchy& hierarchy)
	{
		const std::vector<Sphere>& spheres=hierarchy.spheres();
		std::vector<Face> result;
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
								log_ref().finding_first_faces_iterations++;
								Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
								std::vector<SimpleSphere> tangents=construct_spheres_tangent_sphere<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
								if(tangents.size()==1 && SpheresCollisionDetector::find_any_collision(hierarchy, tangents.front()).empty())
								{
									for(int i=0;i<4;i++)
									{
										result.push_back(Face(spheres, quadruple.exclude(i), hierarchy.min_input_radius()));
										result.back().set_d_with_d_number_selection(quadruple.get(i), tangents.front());
									}
									return result;
								}
							}
						}
					}
				}
			}
		}
		return result;
	}

	static bool find_valid_d(const Hierarchy& hierarchy, Face& face, const std::size_t d_number)
	{
		if(d_number==0)
		{
			log_ref().d0_searches++;
		}
		else
		{
			log_ref().d1_searches++;
		}
		if(!face.has_d(d_number))
		{
			typename checkers_for_any_d::NodeChecker node_checker(face, d_number);
			typename checkers_for_any_d::LeafChecker leaf_checker(face, d_number);
			node_checker.constrain();
			hierarchy.search(node_checker, leaf_checker);
			if(node_checker.constrained && !face.has_d(d_number))
			{
				node_checker.unconstrain();
				hierarchy.search(node_checker, leaf_checker);
			}
			if(face.has_d(d_number))
			{
				typename checkers_for_valid_d::NodeChecker node_checker(face, d_number);
				typename checkers_for_valid_d::LeafChecker leaf_checker(face, d_number);
				while(face.has_d(d_number))
				{
					const std::vector<std::size_t> results=hierarchy.search(node_checker, leaf_checker);
					if(results.empty())
					{
						return true;
					}
					else if(face.get_d_id(d_number)!=results.back())
					{
						face.unset_d(d_number);
					}
				}
			}
		}
		return false;
	}

	static bool find_valid_e(const Hierarchy& hierarchy, Face& face)
	{
		log_ref().e_searches++;
		typename checkers_for_valid_e::NodeChecker node_checker(face);
		typename checkers_for_valid_e::LeafChecker leaf_checker(face, hierarchy);
		return !hierarchy.search(node_checker, leaf_checker).empty();
	}
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
