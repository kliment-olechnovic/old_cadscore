#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <sstream>
#include <limits>

#include "apollonius_triangulation/search_for_valid_quadruples.h"
#include "bounding_spheres_hierarchy.h"

namespace apollo2
{

class ApolloniusTriangulation
{
public:
	typedef apollonius_triangulation::QuadruplesMap QuadruplesMap;
	typedef apollonius_triangulation::QuadruplesLog QuadruplesLog;

	struct Result
	{
		QuadruplesMap quadruples_map;
		QuadruplesLog quadruples_log;
		std::set<std::size_t> hidden_spheres_ids;
		std::set<std::size_t> ignored_spheres_ids;
	};

	template<typename SphereType>
	static Result construct(const std::vector<SphereType>& spheres, const double initial_radius_for_spheres_bucketing)
	{
		Result result;
		BoundingSpheresHierarchy<SphereType> bsh(spheres, initial_radius_for_spheres_bucketing, 1);
		result.hidden_spheres_ids=apollonius_triangulation::find_all_hidden_spheres(bsh);
		for(std::set<std::size_t>::const_iterator it=result.hidden_spheres_ids.begin();it!=result.hidden_spheres_ids.end();++it)
		{
			bsh.ignore_leaf_sphere(*it);
		}
		result.quadruples_map=apollonius_triangulation::find_valid_quadruples(bsh, result.quadruples_log);
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
		output << "quadruples                      " << result.quadruples_log.quadruples << "\n";
		output << "tangent_spheres                 " << result.quadruples_log.tangent_spheres << "\n";
		output << "difficult_faces                 " << result.quadruples_log.difficult_faces << "\n";
		output << "triples_repetitions             " << result.quadruples_log.triples_repetitions << "\n";
		output << "finding_first_faces_iterations  " << result.quadruples_log.finding_first_faces_iterations << "\n";
		output << "hidden_spheres                  " << result.hidden_spheres_ids.size() << "\n";
		output << "ignored_spheres                 " << result.ignored_spheres_ids.size() << "\n";
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
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
