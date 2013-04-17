#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_

#include <vector>

#include <tr1/functional>

#include "../bounding_spheres_hierarchy.h"
#include "../collision_search.h"

#include "face.h"
#include "log.h"

namespace apollo2
{

namespace apollonius_triangulation
{

template<typename SphereType>
static std::vector< Face<SphereType> > find_first_faces(const BoundingSpheresHierarchy<SphereType>& bsh)
{
	typedef SphereType Sphere;

	const std::vector<Sphere>& spheres=bsh.leaves_spheres();
	std::vector< Face<Sphere> > result;
	if(!spheres.empty())
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres.front(), std::tr1::placeholders::_1));
		for(std::size_t u=4;u<traversal.size();u++)
		{
			for(std::size_t a=0;a<u;a++)
			{
				for(std::size_t b=a+1;b<u;b++)
				{
					for(std::size_t c=b+1;c<u;c++)
					{
						for(std::size_t d=((a+1<u && b+1<u && c+1<u) ? (u-1) : (c+1));d<u;d++)
						{
							log_ref().finding_first_faces_iterations++;
							Quadruple quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
							std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
							if(tangents.size()==1 && CollisionSearch::find_any_collision(bsh, tangents.front()).empty())
							{
								for(int i=0;i<4;i++)
								{
									result.push_back(Face<Sphere>(spheres, quadruple.exclude(i), bsh.min_input_radius()));
									result.back().set_d_with_d_number_selection(quadruple.get(i), tangents.front());
								}
								return result;
							}
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
