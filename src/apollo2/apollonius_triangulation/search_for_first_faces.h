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
static std::vector<Triple> find_first_faces_triples(const BoundingSpheresHierarchy<SphereType>& bsh, const std::vector<SphereType>& spheres, const std::size_t starting_sphere_id, std::size_t& iterations_count)
{
	iterations_count=0;
	std::vector<Triple> result;
	if(spheres.size()>=4 && starting_sphere_id<spheres.size())
	{
		const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<SphereType, SphereType>, spheres[starting_sphere_id], std::tr1::placeholders::_1));
		for(std::size_t u=4;u<traversal.size();u++)
		{
			for(std::size_t a=0;a<u;a++)
			{
				for(std::size_t b=a+1;b+1<u;b++)
				{
					for(std::size_t c=b+1;c+1<u;c++)
					{
						iterations_count++;
						Quadruple quadruple(traversal[a], traversal[b], traversal[c], traversal[u-1]);
						const std::vector<SimpleSphere> tangents=TangentSphereOfFourSpheres::calculate<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
						if(!tangents.empty() && (find_any_collision(bsh, tangents.front()).empty() || (tangents.size()==2 && find_any_collision(bsh, tangents.back()).empty())))
						{
							for(int i=0;i<4;i++)
							{
								result.push_back(quadruple.exclude(i));
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

template<typename SphereType>
std::vector< Face<SphereType> > find_first_faces(const BoundingSpheresHierarchy<SphereType>& bsh, const std::size_t starting_sphere_id, std::size_t& iterations_count)
{
	std::vector< Face<SphereType> > result;
	const std::vector<Triple> triples=find_first_faces_triples(bsh, bsh.leaves_spheres(), starting_sphere_id, iterations_count);
	for(std::size_t i=0;i<triples.size();i++)
	{
		result.push_back(Face<SphereType>(bsh.leaves_spheres(), triples[i], bsh.min_input_radius()));
	}
	return result;
}

template<typename SphereType>
std::vector< Face<SphereType> > find_first_faces_for_subset_of_spheres(const BoundingSpheresHierarchy<SphereType>& bsh, const std::set<std::size_t>& set_of_spheres_ids, const std::size_t starting_sphere_id, std::size_t& iterations_count)
{
	std::vector< Face<SphereType> > result;
	if(set_of_spheres_ids.count(starting_sphere_id)==1)
	{
		std::vector<SimpleSphere> local_spheres;
		std::map<std::size_t, std::size_t> local_to_global_ids_map;
		std::size_t local_starting_sphere_id=0;
		for(std::set<std::size_t>::const_iterator it=set_of_spheres_ids.begin();it!=set_of_spheres_ids.end();++it)
		{
			const std::size_t global_id=(*it);
			local_spheres.push_back(SimpleSphere(bsh.leaves_spheres().at(global_id)));
			local_to_global_ids_map[local_spheres.size()-1]=global_id;
			if(global_id==starting_sphere_id)
			{
				local_starting_sphere_id=local_spheres.size()-1;
			}
		}
		const std::vector<Triple> triples=find_first_faces_triples(bsh, local_spheres, local_starting_sphere_id, iterations_count);
		for(std::size_t i=0;i<triples.size();i++)
		{
			const Triple& t=triples[i];
			result.push_back(Face<SphereType>(bsh.leaves_spheres(), Triple(local_to_global_ids_map[t.get(0)], local_to_global_ids_map[t.get(1)], local_to_global_ids_map[t.get(2)]), bsh.min_input_radius()));
		}
	}
	return result;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_FIRST_FACES_H_ */
