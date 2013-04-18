#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_

#include <vector>

#include <tr1/unordered_set>
#include <tr1/unordered_map>

#include "search_for_first_faces.h"
#include "search_for_any_d.h"
#include "search_for_valid_d.h"
#include "search_for_valid_e.h"
#include "log.h"

namespace apollo2
{

namespace apollonius_triangulation
{

typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

template<typename SphereType>
static QuadruplesMap find_valid_quadruples(const BoundingSpheresHierarchy<SphereType>& bsh)
{
	typedef SphereType Sphere;
	typedef std::tr1::unordered_set<Triple, Triple::HashFunctor> TriplesSet;
	typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;

	log_ref()=Log();
	QuadruplesMap quadruples_map;
	TriplesSet processed_triples_set;
	std::vector< Face<Sphere> > stack=find_first_faces(bsh);
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
		if(!face.can_have_d())
		{
			log_ref().difficult_faces++;
		}
		const bool found_d0=face.can_have_d() && !face.has_d(0) && find_any_d<Sphere>(bsh, face, 0) && find_valid_d<Sphere>(bsh, face, 0);
		const bool found_d1=face.can_have_d() && !face.has_d(1) && find_any_d<Sphere>(bsh, face, 1) && find_valid_d<Sphere>(bsh, face, 1);
		const bool found_e=face.can_have_e() && find_valid_e<Sphere>(bsh, face);
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
					log_ref().quadruples++;
					log_ref().tangent_spheres++;
					quadruples_map[quadruple].push_back(quadruple_tangent_sphere);
				}
				else
				{
					std::vector<SimpleSphere>& quadruple_tangent_spheres_list=qm_it->second;
					if(quadruple_tangent_spheres_list.size()==1 && !spheres_equal(quadruple_tangent_spheres_list.front(), quadruple_tangent_sphere))
					{
						log_ref().tangent_spheres++;
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
						log_ref().produced_faces++;
					}
					else
					{
						stack.at(sm_it->second).set_d_with_d_number_selection(produced_preface.second.first, produced_preface.second.second);
						log_ref().updated_faces++;
					}
				}
				else
				{
					log_ref().triples_repetitions++;
				}
			}
		}
		processed_triples_set.insert(face.abc_ids());
	}
	return quadruples_map;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_ */
