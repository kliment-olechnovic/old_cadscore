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
static std::vector< Face<SphereType> > find_first_faces(const BoundingSpheresHierarchy<SphereType>& bsh, const std::size_t starting_face_id, std::size_t& iterations_count)
{
	typedef SphereType Sphere;

	iterations_count=0;
	const std::vector<Sphere>& spheres=bsh.leaves_spheres();
	std::vector< Face<Sphere> > result;
	if(starting_face_id<spheres.size())
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres[starting_face_id], std::tr1::placeholders::_1));
		for(std::size_t u=4;u<traversal.size();u++)
		{
			for(std::size_t a=0;a<u;a++)
			{
				for(std::size_t b=a+1;b+1<u;b++)
				{
					for(std::size_t c=b+1;c+1<u;c++)
					{
						const std::size_t d=u-1;
						iterations_count++;
						Quadruple quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
						const std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
						if(!tangents.empty() && (find_any_collision(bsh, tangents.front()).empty() || (tangents.size()==2 && find_any_collision(bsh, tangents.back()).empty())))
						{
							for(int i=0;i<4;i++)
							{
								result.push_back(Face<Sphere>(spheres, quadruple.exclude(i), bsh.min_input_radius()));
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
