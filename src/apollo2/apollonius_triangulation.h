#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <sstream>
#include <limits>
#include <map>

#include "apollonius_triangulation/search_for_valid_quadruples.h"
#include "bounding_spheres_hierarchy.h"

namespace apollo2
{

class ApolloniusTriangulation
{
public:
	typedef apollonius_triangulation::QuadruplesMap QuadruplesMap;
	typedef apollonius_triangulation::QuadruplesLog QuadruplesLog;
	typedef std::tr1::unordered_set<apollonius_triangulation::Pair, apollonius_triangulation::Pair::HashFunctor> PairsSet;
	typedef std::tr1::unordered_set<apollonius_triangulation::Triple, apollonius_triangulation::Triple::HashFunctor> TriplesSet;

	struct Result
	{
		QuadruplesMap quadruples_map;
		QuadruplesLog quadruples_log;
		std::set<std::size_t> hidden_spheres_ids;
		std::set<std::size_t> ignored_spheres_ids;
	};

	template<typename SphereType>
	static Result construct(const std::vector<SphereType>& spheres, const double initial_radius_for_spheres_bucketing, const bool perform_augmentation)
	{
		Result result;
		BoundingSpheresHierarchy<SphereType> bsh(spheres, initial_radius_for_spheres_bucketing, 1);
		result.hidden_spheres_ids=apollonius_triangulation::find_all_hidden_spheres(bsh);
		for(std::set<std::size_t>::const_iterator it=result.hidden_spheres_ids.begin();it!=result.hidden_spheres_ids.end();++it)
		{
			bsh.ignore_leaf_sphere(*it);
		}
		result.quadruples_map=apollonius_triangulation::find_valid_quadruples(bsh, result.quadruples_log);
		if(perform_augmentation)
		{
			result.quadruples_map=augment_quadruples_map(bsh, result.quadruples_map, result.quadruples_log);
		}
		std::vector<int> spheres_inclusion_map(spheres.size(), 0);
		for(QuadruplesMap::const_iterator it=result.quadruples_map.begin();it!=result.quadruples_map.end();++it)
		{
			const apollonius_triangulation::Quadruple& q=it->first;
			for(int i=0;i<4;i++)
			{
				spheres_inclusion_map[q.get(i)]=1;
			}
		}
		for(std::size_t i=0;i<spheres_inclusion_map.size();i++)
		{
			if(spheres_inclusion_map[i]==0 && result.hidden_spheres_ids.count(i)==0)
			{
				result.ignored_spheres_ids.insert(i);
			}
		}
		return result;
	}

	static PairsSet collect_pairs_set_from_quadruples_map(const QuadruplesMap& quadruples_map)
	{
		PairsSet result;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const apollonius_triangulation::Quadruple& quadruple=it->first;
			for(int a=0;a<4;a++)
			{
				for(int b=a+1;b<4;b++)
				{
					result.insert(apollonius_triangulation::Pair(quadruple.get(a), quadruple.get(b)));
				}
			}
		}
		return result;
	}

	static TriplesSet collect_triples_set_from_quadruples_map(const QuadruplesMap& quadruples_map)
	{
		TriplesSet result;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const apollonius_triangulation::Quadruple& quadruple=it->first;
			for(int a=0;a<4;a++)
			{
				for(int b=a+1;b<4;b++)
				{
					for(int c=b+1;c<4;c++)
					{
						result.insert(apollonius_triangulation::Triple(quadruple.get(a), quadruple.get(b), quadruple.get(c)));
					}
				}
			}
		}
		return result;
	}

	static void print_quadruples_map(const QuadruplesMap& quadruples_map, std::ostream& output)
	{
		output.precision(std::numeric_limits<double>::digits10);
		output << std::fixed;
		const QuadruplesOrderedMap quadruples_ordered_map=collect_ordered_map_of_quadruples(quadruples_map);
		for(QuadruplesOrderedMap::const_iterator it=quadruples_ordered_map.begin();it!=quadruples_ordered_map.end();++it)
		{
			const apollonius_triangulation::Quadruple& quadruple=it->first;
			const std::vector<SimpleSphere>& tangent_spheres=it->second;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				output << quadruple.get(0) << " " << quadruple.get(1) << " " << quadruple.get(2) << " " << quadruple.get(3) << " ";
				output << tangent_sphere.x << " " << tangent_sphere.y << " " << tangent_sphere.z << " " << tangent_sphere.r << "\n";
			}
		}
	}

	static QuadruplesMap read_quadruples_map(std::istream& input)
	{
		QuadruplesMap quadruples_map;
		bool valid=true;
		while(input.good() && valid)
		{
			std::string line;
			std::getline(input, line);
			if(line.empty())
			{
				valid=false;
			}
			else
			{
				std::istringstream line_stream(line);
				std::size_t q[4]={0, 0, 0, 0};
				double s[4]={0, 0, 0, 0};
				for(int i=0;i<4 && valid;i++)
				{
					line_stream >> q[i];
					valid=!line_stream.fail();
				}
				for(int i=0;i<4 && valid;i++)
				{
					line_stream >> s[i];
					valid=!line_stream.fail();
				}
				if(valid)
				{
					quadruples_map[apollonius_triangulation::Quadruple(q[0], q[1], q[2], q[3])].push_back(SimpleSphere(s[0], s[1], s[2], s[3]));
				}
			}
		}
		return quadruples_map;
	}

	static void print_result_log(const Result& result, std::ostream& output)
	{
		output << "quadruples " << result.quadruples_map.size() << "\n";
		output << "tangent_spheres " << result.quadruples_log.tangent_spheres << "\n";
		output << "processed_faces " << result.quadruples_log.processed_faces << "\n";
		output << "difficult_faces " << result.quadruples_log.difficult_faces << "\n";
		output << "first_faces_iterations " << result.quadruples_log.finding_first_faces_iterations << "\n";
		output << "hidden_spheres " << result.hidden_spheres_ids.size() << "\n";
		output << "ignored_spheres " << result.ignored_spheres_ids.size() << "\n";
	}

	template<typename SphereType>
	static bool check_quadruples_map(const std::vector<SphereType>& spheres, const QuadruplesMap& quadruples_map)
	{
		for(typename QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const QuadruplesMap::key_type& q=it->first;
			const QuadruplesMap::mapped_type& ts=it->second;
			if(q.has_repetetions() || ts.empty() || ts.size()>2 || (ts.size()==2 && spheres_equal(ts.front(), ts.back())))
			{
				return false;
			}
			for(std::size_t i=0;i<ts.size();i++)
			{
				const SimpleSphere& t=ts[i];
				for(int j=0;j<4;j++)
				{
					if(!sphere_touches_sphere(t, spheres[q.get(j)]))
					{
						return false;
					}
				}
				for(std::size_t j=0;j<spheres.size();j++)
				{
					if(sphere_intersects_sphere(t, spheres[j]))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

private:
	typedef std::map<QuadruplesMap::key_type, QuadruplesMap::mapped_type> QuadruplesOrderedMap;

	static QuadruplesOrderedMap collect_ordered_map_of_quadruples(const QuadruplesMap& quadruples_map)
	{
		QuadruplesOrderedMap quadruples_ordered_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			quadruples_ordered_map.insert(*it);
		}
		return quadruples_ordered_map;
	}

	template<typename SphereType>
	static QuadruplesMap augment_quadruples_map(const BoundingSpheresHierarchy<SphereType>& bsh, const QuadruplesMap& quadruples_map, QuadruplesLog& quadruples_log)
	{
		QuadruplesMap augmented_quadruples_map=quadruples_map;
		for(QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			const std::vector<SimpleSphere>& tangent_spheres=it->second;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				const SimpleSphere expanded_tangent_sphere(tangent_sphere, tangent_sphere.r+(3*comparison_epsilon()));
				const std::vector<std::size_t> expanded_collisions=apollonius_triangulation::find_all_collisions(bsh, expanded_tangent_sphere);
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
									const apollonius_triangulation::Quadruple quadruple(refined_collisions[a], refined_collisions[b], refined_collisions[c], refined_collisions[d]);
									QuadruplesMap::iterator aqm_it=augmented_quadruples_map.find(quadruple);
									if(aqm_it==augmented_quadruples_map.end())
									{
										quadruples_log.quadruples++;
										quadruples_log.tangent_spheres++;
										augmented_quadruples_map[quadruple].push_back(tangent_sphere);
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
											quadruples_log.tangent_spheres++;
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
		return augmented_quadruples_map;
	}
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
