#ifndef APOLLONIUS_TRIANGULATION_H_
#define APOLLONIUS_TRIANGULATION_H_

#include <vector>
#include <deque>
#include <tr1/unordered_set>
#include <tr1/unordered_map>

#include "spheres_hierarchy.h"
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

	ApolloniusTriangulation(const Hierarchy& hierarchy) :
		hierarchy_(hierarchy),
		spheres_(hierarchy_.spheres())
	{
	}

	QuadruplesMap find_quadruples() const
	{
		QuadruplesMap quadruples_map;

		std::deque<Face> stack=find_first_faces();
		if(!stack.empty())
		{
			quadruples_map[Quadruple(stack.front().abc_ids(), stack.front().d1_id())].push_back(stack.front().d1_tangent_sphere());

			std::tr1::unordered_map<Triple, int, Triple::HashFunctor> triples_map;
			for(std::size_t i=0;i<stack.size();i++)
			{
				triples_map[stack[i].abc_ids()]++;
			}

			while(!stack.empty())
			{
				Face face=stack.back();
				stack.pop_back();
				if(triples_map.find(face.abc_ids())->second==1)
				{
					if(search_for_valid_d2(face))
					{
						const Quadruple quadruple(face.abc_ids(), face.d2_id());
						quadruples_map[quadruple].push_back(face.d2_tangent_sphere());
						for(int i=0;i<quadruple.size();i++)
						{
							triples_map[quadruple.exclude(i)]++;
						}
						for(std::size_t i=0;i<3;i++)
						{
							stack.push_back(face.get_face_for_d2(i));
						}
					}

					search_for_candidates_for_d3(face);
				}
			}
		}

		return quadruples_map;
	}

private:
	typedef ApolloniusFace<Sphere> Face;

	struct simple_d2_checkers
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

	struct conflict_d2_checkers
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

	struct simple_d3_checkers
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

	std::deque<Face> find_first_faces() const
	{
		std::deque<Face> result;
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres_, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres_.front(), std::tr1::placeholders::_1));
		const std::size_t a=0;
		for(std::size_t b=a+1;b<traversal.size();b++)
		{
			for(std::size_t c=b+1;c<traversal.size();c++)
			{
				for(std::size_t d=c+1;d<traversal.size();d++)
				{
					Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
					std::vector<SimpleSphere> tangents=construct_spheres_tangent<SimpleSphere>(spheres_[quadruple.get(0)], spheres_[quadruple.get(1)], spheres_[quadruple.get(2)], spheres_[quadruple.get(3)]);
					if(tangents.size()==1 && hierarchy_.find_any_collision(tangents.front()).empty())
					{
						for(int i=0;i<4;i++)
						{
							result.push_back(Face(spheres_, quadruple.exclude(i), quadruple.get(i), tangents.front()));
						}
						return result;
					}
				}
			}
		}
		return result;
	}

	Face search_for_any_d2(const Face& candidate_face) const
	{
		Face face=candidate_face;
		face.unset_d2_and_d3();
		typename simple_d2_checkers::NodeChecker node_checker(face);
		typename simple_d2_checkers::LeafChecker leaf_checker(face);
		hierarchy_.search(node_checker, leaf_checker);
		return face;
	}

	bool search_for_any_d2(Face& face) const
	{
		if(face.d2_id()==Face::npos)
		{
			typename simple_d2_checkers::NodeChecker node_checker(face);
			typename simple_d2_checkers::LeafChecker leaf_checker(face);
			hierarchy_.search(node_checker, leaf_checker);
		}
		return (face.d2_id()!=Face::npos);
	}

	bool search_for_valid_d2(Face& face) const
	{
		if(search_for_any_d2(face))
		{
			typename conflict_d2_checkers::NodeChecker node_checker(face);
			typename conflict_d2_checkers::LeafChecker leaf_checker(face);
			while(face.d2_id()!=Face::npos)
			{
				const std::vector<std::size_t> results=hierarchy_.search(node_checker, leaf_checker);
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

	bool search_for_candidates_for_d3(Face& face) const
	{
		typename simple_d3_checkers::NodeChecker node_checker(face);
		typename simple_d3_checkers::LeafChecker leaf_checker(face, hierarchy_);
		return !hierarchy_.search(node_checker, leaf_checker).empty();
	}

	const Hierarchy& hierarchy_;
	const std::vector<Sphere>& spheres_;
};

}

#endif /* APOLLONIUS_TRIANGULATION_H_ */
