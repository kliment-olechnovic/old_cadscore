#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_D_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_D_H_

#include "../bounding_spheres_hierarchy.h"

#include "face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

template<typename SphereType>
struct NodeCheckerForValidD
{
	const Face<SphereType>& face;
	const std::size_t d_number;

	NodeCheckerForValidD(const Face<SphereType>& target, const std::size_t d_number) : face(target), d_number(d_number)
	{
	}

	bool operator()(const SimpleSphere& sphere) const
	{
		return (face.has_d(d_number) && sphere_intersects_sphere(sphere, face.get_d_tangent_sphere(d_number)));
	}
};

template<typename SphereType>
struct LeafCheckerForValidD
{
	Face<SphereType>& face;
	const std::size_t d_number;

	LeafCheckerForValidD(Face<SphereType>& target, const std::size_t d_number) : face(target), d_number(d_number)
	{
	}

	std::pair<bool, bool> operator()(const std::size_t id, const SphereType& sphere)
	{
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

template<typename SphereType>
bool find_valid_d(const BoundingSpheresHierarchy<SphereType>& bsh, Face<SphereType>& face, const std::size_t d_number)
{
	if(face.has_d(d_number))
	{
		NodeCheckerForValidD<SphereType> node_checker(face, d_number);
		LeafCheckerForValidD<SphereType> leaf_checker(face, d_number);
		while(face.has_d(d_number))
		{
			const std::vector<std::size_t> results=bsh.search(node_checker, leaf_checker);
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
	return false;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_D_H_ */
