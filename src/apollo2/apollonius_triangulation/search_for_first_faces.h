#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_

#include "face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

class SearchForFirstFaces
{
public:
	template<typename SphereType>
	static std::vector< Face<SphereType> > find_first_faces(
			const BoundingSpheresHierarchy<SphereType>& bsh,
			const std::size_t starting_sphere_id,
			std::size_t& iterations_count,
			const bool fix_starting_sphere_id=false,
			const bool allow_quadruples_with_two_tangent_spheres=false,
			const std::size_t max_size_of_traversal=std::numeric_limits<std::size_t>::max(),
			const double max_distance=std::numeric_limits<double>::max())
	{
		const std::vector<SphereType>& spheres=bsh.leaves_spheres();
		std::vector< Face<SphereType> > result;
		if(spheres.size()>=4 && starting_sphere_id<spheres.size())
		{
			const std::vector<std::size_t> traversal=BoundingSpheresHierarchy<SphereType>::sort_objects_by_distance_to_one_of_them(spheres, starting_sphere_id, minimal_distance_from_sphere_to_sphere<SphereType, SphereType>, max_distance);
			for(std::size_t d=3;d<std::min(traversal.size(), max_size_of_traversal);d++)
			{
				for(std::size_t a=0;a<(fix_starting_sphere_id ? 1 : d);a++)
				{
					for(std::size_t b=a+1;b<d;b++)
					{
						for(std::size_t c=b+1;c<d;c++)
						{
							iterations_count++;
							const Triple triple(traversal[a], traversal[b], traversal[c]);
							const Quadruple quadruple(triple, traversal[d]);
							const std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
							if(
									(tangents.size()==1 && SearchForSphericalCollisions::find_any_collision(bsh, tangents.front()).empty())
									|| (allow_quadruples_with_two_tangent_spheres && tangents.size()==2 && (SearchForSphericalCollisions::find_any_collision(bsh, tangents.front()).empty() || SearchForSphericalCollisions::find_any_collision(bsh, tangents.back()).empty()))
								)
							{
								result.push_back(Face<SphereType>(bsh.leaves_spheres(), triple, bsh.min_input_radius()));
								return result;
							}
						}
					}
				}
			}
		}
		return result;
	}
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_ */
