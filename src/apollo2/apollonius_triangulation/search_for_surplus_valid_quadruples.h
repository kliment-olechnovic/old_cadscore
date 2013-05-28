#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_SURPLUS_VALID_QUADRUPLES_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_SURPLUS_VALID_QUADRUPLES_H_

namespace apollo2
{

namespace apollonius_triangulation
{

class SearchForSurplusValidQuadruples
{
public:
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

	struct Log
	{
		std::size_t surplus_quadruples;
		std::size_t surplus_tangent_spheres;
	};

	template<typename SphereType>
	static QuadruplesMap find_surplus_valid_quadruples(const BoundingSpheresHierarchy<SphereType>& bsh, const QuadruplesMap& quadruples_map, Log& log)
	{
		log=Log();
		QuadruplesMap surplus_quadruples_map=quadruples_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const std::vector<SimpleSphere>& tangent_spheres=it->second;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				const SimpleSphere expanded_tangent_sphere(tangent_sphere, tangent_sphere.r+(3*comparison_epsilon()));
				const std::vector<std::size_t> expanded_collisions=SearchForSphericalCollisions::find_all_collisions(bsh, expanded_tangent_sphere);
				std::vector<std::size_t> refined_collisions;
				for(std::size_t j=0;j<expanded_collisions.size();j++)
				{
					if(sphere_touches_sphere(tangent_sphere, bsh.leaves_spheres().at(expanded_collisions[j])))
					{
						refined_collisions.push_back(expanded_collisions[j]);
					}
				}
				if(refined_collisions.size()>4)
				{
					for(std::size_t a=0;a<refined_collisions.size();a++)
					{
						for(std::size_t b=a+1;b<refined_collisions.size();b++)
						{
							for(std::size_t c=b+1;c<refined_collisions.size();c++)
							{
								for(std::size_t d=c+1;d<refined_collisions.size();d++)
								{
									const Quadruple quadruple(refined_collisions[a], refined_collisions[b], refined_collisions[c], refined_collisions[d]);
									QuadruplesMap::iterator aqm_it=surplus_quadruples_map.find(quadruple);
									if(aqm_it==surplus_quadruples_map.end())
									{
										log.surplus_quadruples++;
										log.surplus_tangent_spheres++;
										surplus_quadruples_map[quadruple].push_back(tangent_sphere);
									}
									else
									{
										std::vector<SimpleSphere>& quadruple_tangent_spheres_list=aqm_it->second;
										bool found=false;
										for(std::size_t e=0;e<quadruple_tangent_spheres_list.size() && !found;e++)
										{
											if(spheres_equal(quadruple_tangent_spheres_list[e], tangent_sphere))
											{
												found=true;
											}
										}
										if(!found)
										{
											log.surplus_tangent_spheres++;
											quadruple_tangent_spheres_list.push_back(tangent_sphere);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return surplus_quadruples_map;
	}
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_SEARCH_FOR_SURPLUS_VALID_QUADRUPLES_H_ */
