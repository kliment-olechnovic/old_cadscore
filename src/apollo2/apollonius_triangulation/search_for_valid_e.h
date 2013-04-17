#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_H_

#include "../bounding_spheres_hierarchy.h"
#include "../collision_search.h"

#include "face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

template<typename SphereType>
class SearchForValidE
{
public:
	typedef SphereType Sphere;

	static bool find_valid_e(const BoundingSpheresHierarchy<Sphere>& bsh, Face<Sphere>& face)
	{
		NodeChecker node_checker(face);
		LeafChecker leaf_checker(face, bsh);
		return !bsh.search(node_checker, leaf_checker).empty();
	}

private:
	struct NodeChecker
	{
		const Face<Sphere>& face;

		NodeChecker(const Face<Sphere>& target) : face(target)
		{
		}

		bool operator()(const SimpleSphere& sphere) const
		{
			return face.sphere_may_contain_candidate_for_e(sphere);
		}
	};

	struct LeafChecker
	{
		Face<Sphere>& face;
		const BoundingSpheresHierarchy<Sphere>& bsh;

		LeafChecker(Face<Sphere>& target, const BoundingSpheresHierarchy<Sphere>& bsh) : face(target), bsh(bsh)
		{
		}

		std::pair<bool, bool> operator()(const std::size_t id, const Sphere&)
		{
			const std::vector<SimpleSphere> check_result=face.check_candidate_for_e(id);
			bool e_added=false;
			for(std::size_t i=0;i<check_result.size();i++)
			{
				if(CollisionSearch::find_any_collision(bsh, check_result[i]).empty())
				{
					face.add_e(id, check_result[i]);
					e_added=true;
				}
			}
			return std::make_pair(e_added, false);
		}
	};
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_H_ */
