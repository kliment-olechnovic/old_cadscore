#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_

#include <vector>

#include <tr1/unordered_set>
#include <tr1/unordered_map>

#include "../bounding_spheres_hierarchy.h"

#include "search_for_first_faces.h"
#include "search_for_any_d_of_face.h"
#include "search_for_valid_d_of_face.h"
#include "search_for_valid_e_of_face.h"

namespace apollo2
{

namespace apollonius_triangulation
{

class SearchForValidQuadruples
{
public:
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

	struct Log
	{
		std::size_t quadruples;
		std::size_t processed_faces;
		std::size_t tangent_spheres;
		std::size_t difficult_faces;
		std::size_t produced_faces;
		std::size_t updated_faces;
		std::size_t triples_repetitions;
		std::size_t finding_first_faces_iterations;
	};

	template<typename SphereType>
	static QuadruplesMap find_valid_quadruples(const BoundingSpheresHierarchy<SphereType>& bsh, Log& log)
	{
		typedef SphereType Sphere;
		typedef std::tr1::unordered_set<Triple, Triple::HashFunctor> TriplesSet;
		typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;

		log=Log();
		QuadruplesMap quadruples_map;
		TriplesSet processed_triples_set;
		std::vector<int> spheres_usage_mapping(bsh.leaves_spheres().size(), 0);
		std::set<std::size_t> ignorable_spheres_ids;
		std::vector< Face<Sphere> > stack=SearchForFirstFaces::find_first_faces(bsh, 0, log.finding_first_faces_iterations);
		if(stack.empty())
		{
			stack=SearchForFirstFaces::find_first_faces(bsh, 0, log.finding_first_faces_iterations, false, true);
		}
		while(!stack.empty())
		{
			TriplesMap stack_map;
			for(std::size_t i=0;i<stack.size();i++)
			{
				stack_map[stack[i].abc_ids()]=i;
			}
			while(!stack.empty())
			{
				Face<Sphere> face=stack.back();
				stack.pop_back();
				stack_map.erase(face.abc_ids());
				processed_triples_set.insert(face.abc_ids());
				log.processed_faces++;
				if(!face.can_have_d())
				{
					log.difficult_faces++;
				}
				const bool found_d0=face.can_have_d() && !face.has_d(0) && SearchForAnyDOfFace::find_any_d<Sphere>(bsh, face, 0) && SearchForValidDOfFace::find_valid_d<Sphere>(bsh, face, 0);
				const bool found_d1=face.can_have_d() && !face.has_d(1) && SearchForAnyDOfFace::find_any_d<Sphere>(bsh, face, 1) && SearchForValidDOfFace::find_valid_d<Sphere>(bsh, face, 1);
				const bool found_e=face.can_have_e() && SearchForValidEOfFace::find_valid_e<Sphere>(bsh, face);
				if(found_d0 || found_d1 || found_e)
				{
					const std::vector< std::pair<Quadruple, SimpleSphere> > produced_quadruples=face.produce_quadruples(found_d0, found_d1, found_e);
					for(std::size_t i=0;i<produced_quadruples.size();i++)
					{
						const Quadruple& quadruple=produced_quadruples[i].first;
						const SimpleSphere& quadruple_tangent_sphere=produced_quadruples[i].second;
						QuadruplesMap::iterator qm_it=quadruples_map.find(quadruple);
						if(qm_it==quadruples_map.end())
						{
							log.quadruples++;
							log.tangent_spheres++;
							quadruples_map[quadruple].push_back(quadruple_tangent_sphere);
							for(int a=0;a<4;a++)
							{
								spheres_usage_mapping[quadruple.get(a)]++;
							}
						}
						else
						{
							std::vector<SimpleSphere>& quadruple_tangent_spheres_list=qm_it->second;
							if(quadruple_tangent_spheres_list.size()==1 && !spheres_equal(quadruple_tangent_spheres_list.front(), quadruple_tangent_sphere))
							{
								log.tangent_spheres++;
								quadruple_tangent_spheres_list.push_back(quadruple_tangent_sphere);
							}
						}
					}
					const std::vector< std::pair<Triple, std::pair<std::size_t, SimpleSphere> > > produced_prefaces=face.produce_prefaces(found_d0, found_d1, found_e);
					for(std::size_t i=0;i<produced_prefaces.size();i++)
					{
						const std::pair<Triple, std::pair<std::size_t, SimpleSphere> >& produced_preface=produced_prefaces[i];
						if(processed_triples_set.count(produced_preface.first)==0)
						{
							TriplesMap::const_iterator sm_it=stack_map.find(produced_preface.first);
							if(sm_it==stack_map.end())
							{
								stack_map[produced_preface.first]=stack.size();
								stack.push_back(Face<Sphere>(bsh.leaves_spheres(), produced_preface.first, bsh.min_input_radius()));
								stack.back().set_d_with_d_number_selection(produced_preface.second.first, produced_preface.second.second);
								log.produced_faces++;
							}
							else
							{
								stack.at(sm_it->second).set_d_with_d_number_selection(produced_preface.second.first, produced_preface.second.second);
								log.updated_faces++;
							}
						}
						else
						{
							log.triples_repetitions++;
						}
					}
				}
			}
			for(std::size_t i=0;i<spheres_usage_mapping.size() && stack.empty();i++)
			{
				if(spheres_usage_mapping[i]==0 && ignorable_spheres_ids.count(i)==0)
				{
					const std::vector<Sphere>& all_spheres=bsh.leaves_spheres();
					const std::vector<std::size_t> colliding_neighbours=SearchForSphericalCollisions::find_all_collisions(bsh, all_spheres[i]);
					bool hidden=false;
					for(std::size_t j=0;j<colliding_neighbours.size() && !hidden;j++)
					{
						if(colliding_neighbours[j]!=i && sphere_contains_sphere(all_spheres[colliding_neighbours[j]], all_spheres[i]))
						{
							hidden=true;
						}
					}
					if(!hidden)
					{
						stack=SearchForFirstFaces::find_first_faces(bsh, i, log.finding_first_faces_iterations, true, true, 50);
					}
					ignorable_spheres_ids.insert(i);
				}
			}
		}
		return quadruples_map;
	}
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_ */
