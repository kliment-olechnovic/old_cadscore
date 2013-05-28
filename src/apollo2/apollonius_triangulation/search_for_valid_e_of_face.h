#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_OF_FACE_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_OF_FACE_H_

#include "face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

class SearchForValidEOfFace
{
public:
	template<typename SphereType>
	static bool find_valid_e(const BoundingSpheresHierarchy<SphereType>& bsh, Face<SphereType>& face)
	{
		NodeCheckerForValidE<SphereType> node_checker(face);
		LeafCheckerForValidE<SphereType> leaf_checker(face, bsh);
		return !bsh.search(node_checker, leaf_checker).empty();
	}

private:
	template<typename SphereType>
	struct NodeCheckerForValidE
	{
		const Face<SphereType>& face;

		NodeCheckerForValidE(const Face<SphereType>& target) : face(target)
		{
		}

		bool operator()(const SimpleSphere& sphere) const
		{
			return face.sphere_may_contain_candidate_for_e(sphere);
		}
	};

	template<typename SphereType>
	struct LeafCheckerForValidE
	{
		Face<SphereType>& face;
		const BoundingSpheresHierarchy<SphereType>& bsh;

		LeafCheckerForValidE(Face<SphereType>& target, const BoundingSpheresHierarchy<SphereType>& bsh) : face(target), bsh(bsh)
		{
		}

		std::pair<bool, bool> operator()(const std::size_t id, const SphereType&)
		{
			const std::vector<SimpleSphere> check_result=face.check_candidate_for_e(id);
			bool e_added=false;
			for(std::size_t i=0;i<check_result.size();i++)
			{
				if(SearchForSphericalCollisions::find_any_collision(bsh, check_result[i]).empty())
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

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_E_OF_FACE_H_ */
