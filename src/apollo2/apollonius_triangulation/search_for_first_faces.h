#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_

#include <vector>

#include <tr1/functional>

#include "../bounding_spheres_hierarchy.h"

#include "face.h"
#include "search_for_collisions.h"

namespace apollo2
{

namespace apollonius_triangulation
{

template<typename SphereType>
std::vector< Face<SphereType> > find_first_faces(const BoundingSpheresHierarchy<SphereType>& bsh, const std::size_t starting_sphere_id, const bool fix_starting_sphere_id, std::size_t& iterations_count)
{
	const std::vector<SphereType>& spheres=bsh.leaves_spheres();
	iterations_count=0;
	std::vector< Face<SphereType> > result;
	if(spheres.size()>=4 && starting_sphere_id<spheres.size())
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<SphereType, SphereType>, spheres[starting_sphere_id], std::tr1::placeholders::_1));
		for(std::size_t u=4;u<traversal.size();u++)
		{
			for(std::size_t a=0;a<(fix_starting_sphere_id ? 1 : u);a++)
			{
				for(std::size_t b=a+1;b+1<u;b++)
				{
					for(std::size_t c=b+1;c+1<u;c++)
					{
						iterations_count++;
						Quadruple quadruple(traversal[a], traversal[b], traversal[c], traversal[u-1]);
						const std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
						if(tangents.size()==1 && find_any_collision(bsh, tangents.front()).empty())
						{
							for(int i=0;i<4;i++)
							{
								result.push_back(Face<SphereType>(bsh.leaves_spheres(), quadruple.exclude(i), bsh.min_input_radius()));
							}
							return result;
						}
					}
				}
			}
		}
	}
	return result;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_ */
