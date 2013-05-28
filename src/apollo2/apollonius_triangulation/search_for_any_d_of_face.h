#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_ANY_D_OF_FACE_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_ANY_D_OF_FACE_H_

#include "../bounding_spheres_hierarchy.h"

#include "face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

class SearchForAnyDOfFace
{
public:
	template<typename SphereType>
	static bool find_any_d(const BoundingSpheresHierarchy<SphereType>& bsh, Face<SphereType>& face, const std::size_t d_number)
	{
		if(!face.has_d(d_number))
		{
			NodeCheckerForAnyD<SphereType> node_checker(face, d_number);
			LeafCheckerForAnyD<SphereType> leaf_checker(face, d_number);
			node_checker.constrain();
			bsh.search(node_checker, leaf_checker);
			if(node_checker.constrained && !face.has_d(d_number))
			{
				node_checker.unconstrain();
				bsh.search(node_checker, leaf_checker);
			}
			return face.has_d(d_number);
		}
		return false;
	}

private:
	template<typename SphereType>
	struct NodeCheckerForAnyD
	{
		const Face<SphereType>& face;
		const std::size_t d_number;
		bool constrained;
		SimpleSphere constraint_sphere;

		NodeCheckerForAnyD(const Face<SphereType>& target, const std::size_t d_number) : face(target), d_number(d_number), constrained(false)
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
			return (!constrained || sphere_intersects_sphere(constraint_sphere, sphere)) && face.sphere_may_contain_candidate_for_d(sphere, d_number);
		}
	};

	template<typename SphereType>
	struct LeafCheckerForAnyD
	{
		Face<SphereType>& face;
		const std::size_t d_number;

		LeafCheckerForAnyD(Face<SphereType>& target, const std::size_t d_number) : face(target), d_number(d_number)
		{
		}

		std::pair<bool, bool> operator()(const std::size_t id, const SphereType&)
		{
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

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_ANY_D_OF_FACE_H_ */
