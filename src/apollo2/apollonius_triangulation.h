#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <vector>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "apollonius_triangulation_search_for_any_d.h"
#include "apollonius_triangulation_search_for_valid_d.h"
#include "apollonius_triangulation_search_for_valid_e.h"

namespace apollo2
{

template<typename SpheresHierarchyType>
class ApolloniusTriangulation
{
public:
	typedef SpheresHierarchyType Hierarchy;
	typedef typename Hierarchy::LeafSphere Sphere;
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

	struct Log
	{
		std::size_t quadruples;
		std::size_t tangent_spheres;
		std::size_t difficult_faces;
		std::size_t produced_faces;
		std::size_t updated_faces;
		std::size_t triples_repetitions;
		std::size_t finding_first_faces_iterations;

		void print(std::ostream& output) const
		{
			output << "quadruples                      " << quadruples << "\n";
			output << "tangent_spheres                 " << tangent_spheres << "\n";
			output << "difficult_faces                 " << difficult_faces << "\n";
			output << "produced_faces                  " << produced_faces << "\n";
			output << "updated_faces                   " << updated_faces << "\n";
			output << "triples_repetitions             " << triples_repetitions << "\n";
			output << "finding_first_faces_iterations  " << finding_first_faces_iterations << "\n";
		}
	};

	static QuadruplesMap find_quadruples(const Hierarchy& hierarchy, bool enable_searching_for_e)
	{
		typedef ApolloniusTriangulationSearchForAnyD<Sphere> SearchForAnyD;
		typedef ApolloniusTriangulationSearchForValidD<Sphere> SearchForValidD;
		typedef ApolloniusTriangulationSearchForValidE<Sphere> SearchForValidE;
		typedef std::tr1::unordered_set<Triple, Triple::HashFunctor> TriplesSet;
		typedef std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> TriplesMap;

		log_ref()=Log();
		QuadruplesMap quadruples_map;
		TriplesSet processed_triples_set;
		std::vector<Face> stack=find_first_faces(hierarchy);
		TriplesMap stack_map;
		for(std::size_t i=0;i<stack.size();i++)
		{
			stack_map[stack[i].abc_ids()]=i;
		}
		while(!stack.empty())
		{
			Face face=stack.back();
			stack.pop_back();
			stack_map.erase(face.abc_ids());
			if(!face.can_have_d())
			{
				log_ref().difficult_faces++;
			}
			const bool found_d0=face.can_have_d() && !face.has_d(0) && SearchForAnyD::find_any_d(hierarchy, face, 0) && SearchForValidD::find_valid_d(hierarchy, face, 0);
			const bool found_d1=face.can_have_d() && !face.has_d(1) && SearchForAnyD::find_any_d(hierarchy, face, 1) && SearchForValidD::find_valid_d(hierarchy, face, 1);
			const bool found_e=enable_searching_for_e && face.can_have_e() && SearchForValidE::find_valid_e(hierarchy, face);
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
							stack.push_back(Face(hierarchy.leaves_spheres(), produced_preface.first, hierarchy.min_input_radius()));
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

	static Log log()
	{
		return log_ref();
	}

private:
	typedef ApolloniusTriangulationFace<Sphere> Face;

	static Log& log_ref()
	{
		static Log log=Log();
		return log;
	}

	static std::vector<Face> find_first_faces(const Hierarchy& hierarchy)
	{
		const std::vector<Sphere>& spheres=hierarchy.leaves_spheres();
		std::vector<Face> result;
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
								if(tangents.size()==1 && CollisionSearch::find_any_collision(hierarchy, tangents.front()).empty())
								{
									for(int i=0;i<4;i++)
									{
										result.push_back(Face(spheres, quadruple.exclude(i), hierarchy.min_input_radius()));
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
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
