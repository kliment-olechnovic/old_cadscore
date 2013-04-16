#ifndef APOLLO2_COLLISION_SEARCH_H_
#define APOLLO2_COLLISION_SEARCH_H_

#include <vector>
#include <set>

#include "bounding_spheres_hierarchy.h"

namespace apollo2
{

class CollisionSearch
{
public:
	template<typename T>
	static std::vector<std::size_t> find_all_collisions(const BoundingSpheresHierarchy<T>& bsh, const SimpleSphere& target)
	{
		return find_collisions(bsh, target, false);
	}

	template<typename T>
	static std::vector<std::size_t> find_any_collision(const BoundingSpheresHierarchy<T>& bsh, const SimpleSphere& target)
	{
		return find_collisions(bsh, target, true);
	}

	template<typename T>
	static std::set<std::size_t> find_all_hidden_spheres(const BoundingSpheresHierarchy<T>& bsh)
	{
		std::set<std::size_t> result;
		for(std::size_t i=0;i<bsh.spheres().size();i++)
		{
			std::vector<std::size_t> candidates=find_all_collisions(bsh, custom_sphere_from_object<SimpleSphere>(bsh.spheres()[i]));
			for(std::size_t j=0;j<candidates.size();j++)
			{
				if(i!=candidates[j])
				{
					if(sphere_contains_sphere(bsh.spheres()[i], bsh.spheres()[candidates[j]]))
					{
						result.insert(candidates[j]);
					}
					if(sphere_contains_sphere(bsh.spheres()[candidates[j]], bsh.spheres()[i]))
					{
						result.insert(i);
					}
				}
			}
		}
		return result;
	}

private:
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
		const bool one_hit_is_enough;

		LeafChecker(const SimpleSphere& target) : target(target), one_hit_is_enough(true) {}

		LeafChecker(const SimpleSphere& target, const bool one_hit_is_enough) : target(target), one_hit_is_enough(one_hit_is_enough) {}

		template<typename Sphere>
		std::pair<bool, bool> operator()(const std::size_t /*id*/, const Sphere& sphere) const
		{
			if(sphere_intersects_sphere(sphere, target))
			{
				return std::make_pair(true, one_hit_is_enough);
			}
			return std::make_pair(false, false);
		}
	};

	template<typename T>
	static std::vector<std::size_t> find_collisions(const BoundingSpheresHierarchy<T>& bsh, const SimpleSphere& target, const bool one_hit_is_enough)
	{
		NodeChecker node_checker(target);
		LeafChecker leaf_checker(target, one_hit_is_enough);
		return bsh.search(node_checker, leaf_checker);
	}
};

}

#endif /* APOLLO2_COLLISION_SEARCH_H_ */
