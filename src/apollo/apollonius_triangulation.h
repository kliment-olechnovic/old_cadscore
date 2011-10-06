#ifndef APOLLONIUS_TRIANGULATION_H_
#define APOLLONIUS_TRIANGULATION_H_

#include <vector>
#include <deque>
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

	ApolloniusTriangulation(const Hierarchy& spheres_hierarchy) :
		spheres_hierarchy_(spheres_hierarchy),
		spheres_(spheres_hierarchy_.spheres())
	{
	}

	QuadruplesMap find_quadruples() const
	{
		QuadruplesMap quadruples_map;

		std::deque<Face> stack=find_first_faces();
		if(!stack.empty())
		{
			quadruples_map[Quadruple(stack.front().abc_ids(), stack.front().d1_id())].push_back(stack.front().d1_tangent_sphere());

			TriplesMap triples_map;
			for(std::size_t i=0;i<stack.size();i++)
			{
				triples_map[stack[i].abc_ids()].push_back(stack[i].d1_id());
			}

			while(!stack.empty())
			{
				const Face pre_face=stack.back();
				stack.pop_back();
				if(triples_map.find(pre_face.abc_ids())->second.size()==1)
				{
					Face face=search_for_valid_d2(pre_face);
					if(face.d2_id()!=Face::npos)
					{
						const Quadruple quadruple(face.abc_ids(), face.d2_id());
						quadruples_map[quadruple].push_back(face.d2_tangent_sphere());
						for(int i=0;i<quadruple.size();i++)
						{
							triples_map[quadruple.exclude(i)].push_back(quadruple.get(i));
						}
						for(std::size_t i=0;i<3;i++)
						{
							stack.push_back(face.get_face_for_d2(i));
						}
					}
				}
			}
		}

		return quadruples_map;
	}

private:
	typedef ApolloniusFace<Sphere> Face;
	typedef std::tr1::unordered_map<Triple, std::vector<std::size_t>, Triple::HashFunctor> TriplesMap;

	struct simple_intersection_checkers
	{
		struct NodeChecker
		{
			const SimpleSphere& target;

			NodeChecker(const SimpleSphere& target) : target(target) {}

			bool operator()(const SimpleSphere& sphere) const
			{
				return sphere_intersects_sphere(sphere, target);
			}
		};

		struct LeafChecker
		{
			const SimpleSphere& target;

			LeafChecker(const SimpleSphere& target) : target(target) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere) const
			{
				if(sphere_intersects_sphere(sphere, target))
				{
					return std::make_pair(true, true);
				}
				return std::make_pair(false, false);
			}
		};
	};

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
					face.set_d2(id, check_result.second);
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

			const std::tr1::unordered_set<std::size_t>& visited;

			LeafChecker(Face& target, const std::tr1::unordered_set<std::size_t>& visited) : face(target), visited(visited) {}

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
						std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d2(id);
						if(check_result.first)
						{
							face.set_d2(id, check_result.second);
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

	bool simple_intersection_check(const SimpleSphere& target) const
	{
		typename simple_intersection_checkers::NodeChecker node_checker(target);
		typename simple_intersection_checkers::LeafChecker leaf_checker(target);
		return spheres_hierarchy_.search(node_checker, leaf_checker).empty();
	}

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
					if(tangents.size()==1 && simple_intersection_check(tangents.front()))
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
		face.unset_d2();
		typename simple_d2_checkers::NodeChecker node_checker(face);
		typename simple_d2_checkers::LeafChecker leaf_checker(face);
		spheres_hierarchy_.search(node_checker, leaf_checker);
		return face;
	}

	Face search_for_valid_d2(const Face& candidate_face) const
	{
		Face face=search_for_any_d2(candidate_face);
		std::tr1::unordered_set<std::size_t> visited;
		while(face.d2_id()!=Face::npos)
		{
			typename conflict_d2_checkers::NodeChecker node_checker(face);
			typename conflict_d2_checkers::LeafChecker leaf_checker(face, visited);
			const std::vector<std::size_t> results=spheres_hierarchy_.search(node_checker, leaf_checker);
			if(results.empty())
			{
				return face;
			}
			else
			{
				if(face.d2_id()==results.back())
				{
					visited.insert(results.back());
				}
				else
				{
					face.unset_d2();
				}
			}
		}
		return face;
	}

	const Hierarchy& spheres_hierarchy_;
	const std::vector<Sphere>& spheres_;
};

}

#endif /* APOLLONIUS_TRIANGULATION_H_ */
