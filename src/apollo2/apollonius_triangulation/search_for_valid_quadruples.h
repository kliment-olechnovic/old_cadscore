#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_

#include <vector>

#include <tr1/unordered_set>
#include <tr1/unordered_map>

#include "search_for_first_faces.h"
#include "search_for_any_d.h"
#include "search_for_valid_d.h"
#include "search_for_valid_e.h"

namespace apollo2
{

namespace apollonius_triangulation
{

typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

struct QuadruplesLog
{
	std::size_t quadruples;
	std::size_t tangent_spheres;
	std::size_t difficult_faces;
	std::size_t produced_faces;
	std::size_t updated_faces;
	std::size_t triples_repetitions;
	std::size_t finding_first_faces_iterations;
	std::set<std::size_t> hidden_spheres_ids;
	std::set<std::size_t> ignored_spheres_ids;
};

template<typename SphereType>
static QuadruplesMap find_valid_quadruples(const BoundingSpheresHierarchy<SphereType>& bsh, QuadruplesLog& log)
{
	typedef SphereType Sphere;
	typedef std::tr1::unordered_set<Triple, Triple::HashFunctor> TriplesSet;
	typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;
	typedef std::vector< std::tr1::unordered_set<std::size_t> > Graph;

	const std::set<std::size_t> hidden_spheres_ids=find_all_hidden_spheres(bsh);
	log=QuadruplesLog();
	log.hidden_spheres_ids=hidden_spheres_ids;
	QuadruplesMap quadruples_map;
	TriplesSet processed_triples_set;
	Graph graph(bsh.leaves_spheres().size());
	std::vector< Face<Sphere> > stack=find_first_faces(bsh, 0, log.finding_first_faces_iterations);
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
			if(!face.can_have_d())
			{
				log.difficult_faces++;
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
						log.quadruples++;
						log.tangent_spheres++;
						quadruples_map[quadruple].push_back(quadruple_tangent_sphere);
						for(int a=0;a<4;a++)
						{
							for(int b=0;b<4;b++)
							{
								if(a!=b)
								{
									graph[quadruple.get(a)].insert(quadruple.get(b));
								}
							}
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
			processed_triples_set.insert(face.abc_ids());
		}
		std::set<std::size_t> ignored_spheres_ids;
		for(std::size_t i=0;i<graph.size();i++)
		{
			if(graph[i].empty() && hidden_spheres_ids.count(i)==0)
			{
				ignored_spheres_ids.insert(i);
			}
		}
		log.ignored_spheres_ids=ignored_spheres_ids;

		if(!ignored_spheres_ids.empty())
		{
			std::vector< Face<Sphere> > additional_faces;
			for(std::set<std::size_t>::const_iterator it=ignored_spheres_ids.begin();it!=ignored_spheres_ids.end() && additional_faces.empty();++it)
			{
				const std::size_t ignored_sphere_id=(*it);
				const Sphere& ignored_sphere=bsh.leaves_spheres()[ignored_sphere_id];
				std::size_t nearest_id=graph.size();
				double nearest_id_distance=0;
				for(std::size_t i=0;i<bsh.leaves_spheres().size();i++)
				{
					if(ignored_sphere_id!=i && !graph[i].empty())
					{
						const double distance=minimal_distance_from_sphere_to_sphere(ignored_sphere, bsh.leaves_spheres()[i]);
						if(distance<nearest_id_distance || nearest_id==graph.size())
						{
							nearest_id=i;
							nearest_id_distance=distance;
						}
					}
				}
				if(nearest_id<graph.size())
				{
					for(std::tr1::unordered_set<std::size_t>::const_iterator jt=graph[nearest_id].begin();jt!=graph[nearest_id].end() && additional_faces.empty();++jt)
					{
						std::set<std::size_t> candidate_ids_for_additional_faces=ignored_spheres_ids;
						candidate_ids_for_additional_faces.insert(nearest_id);
						candidate_ids_for_additional_faces.insert(*jt);
						std::size_t iterations_count=0;
						additional_faces=find_first_faces_for_subset_of_spheres(bsh, candidate_ids_for_additional_faces, nearest_id, iterations_count);
						log.finding_first_faces_iterations+=iterations_count;
					}
				}
			}
			stack=additional_faces;
		}
	}
	return quadruples_map;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_VALID_QUADRUPLES_H_ */
