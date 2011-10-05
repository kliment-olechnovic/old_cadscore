#ifndef APOLLONIUS_TRIANGULATION_H_
#define APOLLONIUS_TRIANGULATION_H_

#include <deque>
#include <tr1/unordered_set>

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

	ApolloniusTriangulation(const Hierarchy& spheres_hierarchy) :
		spheres_hierarchy_(spheres_hierarchy),
		spheres_(spheres_hierarchy_.spheres())
	{
	}

private:
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
				else
				{
					return std::make_pair(false, false);
				}
			}
		};
	};

	struct simple_d2_checkers
	{
		struct NodeChecker
		{
			const ApolloniusFace& face;

			NodeChecker(const ApolloniusFace& target) : face(target) {}

			bool operator()(const SimpleSphere& sphere) const
			{
				return face.sphere_may_contain_candidate_for_d2(sphere);
			}
		};

		struct LeafChecker
		{
			ApolloniusFace& face;

			LeafChecker(const ApolloniusFace& target) : face(target) {}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				std::pair<bool, SimpleSphere> check_result=face.check_candidate_for_d2(id);
				if(check_result.first)
				{
					face.set_d2(id, check_result.second);
					return std::make_pair(true, true);
				}
				else
				{
					return std::make_pair(false, false);
				}
			}
		};
	};

	bool simple_intersection_check(const SimpleSphere& target) const
	{
		simple_intersection_checkers::NodeChecker node_checker;
		simple_intersection_checkers::LeafChecker leaf_checker;
		return spheres_hierarchy_.search(node_checker, leaf_checker).empty();
	}

	std::deque<ApolloniusFace> find_first_faces() const
	{
		std::deque<ApolloniusFace> result;
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres_, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres_.front(), std::tr1::placeholders::_1));
		const std::size_t a=0;
		for(std::size_t b=a+1;b<traversal.size();b++)
		{
			for(std::size_t c=b+1;c<traversal.size();c++)
			{
				for(std::size_t d=c+1;d<traversal.size();d++)
				{
					Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
					std::vector<SimpleSphere> tangents=construct_spheres_tangent(spheres_[quadruple.get(0)], spheres_[quadruple.get(1)], spheres_[tquadruple.get(2)], spheres_[quadruple.get(3)]);
					if(tangents.size()==1 && simple_intersection_check(tangents.front()))
					{
						for(int i=0;i<4;i++)
						{
							result.push_back(ApolloniusFace(spheres_, quadruple.exclude(i), quadruple.get(i), tangents.front()));
						}
						return result;
					}
				}
			}
		}
		return result;
	}

	const Hierarchy& spheres_hierarchy_;
	const std::vector<Sphere>& spheres_;
};

}

#endif /* APOLLONIUS_TRIANGULATION_H_ */
