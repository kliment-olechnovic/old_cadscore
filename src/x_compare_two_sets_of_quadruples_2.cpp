#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"

std::vector<apollo2::SimpleSphere> read_spheres_from_stream(std::istream& input);

std::vector<apollo2::apollonius_triangulation::Quadruple> read_quadruples_from_stream(std::istream& input)
{
	std::vector<apollo2::apollonius_triangulation::Quadruple> result;
	while(input.good())
	{
		std::string line;
		std::getline(input, line);
		if(!line.empty())
		{
			std::istringstream line_input(line);
			std::vector<std::size_t> numbers(4, 0);
			for(std::size_t i=0;i<numbers.size() && !line_input.fail();i++)
			{
				line_input >> numbers[i];
			}
			if(!line_input.fail())
			{
				result.push_back(apollo2::apollonius_triangulation::Quadruple(numbers));
			}
		}
	}
	if(!result.empty())
	{
		std::sort(result.begin(), result.end());
		std::vector<apollo2::apollonius_triangulation::Quadruple>::iterator it=std::unique(result.begin(), result.end());
		result.resize(it-result.begin());
	}
	return result;
}

std::vector<apollo2::SimpleSphere> get_empty_tangent_spheres_from_quadruples(const apollo2::BoundingSpheresHierarchy<apollo2::SimpleSphere>& bsh, const std::vector<apollo2::apollonius_triangulation::Quadruple>& quadruples)
{
	std::vector<apollo2::SimpleSphere> result;
	result.reserve(quadruples.size()+(quadruples.size()/10));
	for(std::size_t i=0;i<quadruples.size();i++)
	{
		const apollo2::apollonius_triangulation::Quadruple q=quadruples[i];
		if(q.get(0)<bsh.leaves_spheres().size() && q.get(1)<bsh.leaves_spheres().size() && q.get(2)<bsh.leaves_spheres().size() && q.get(3)<bsh.leaves_spheres().size())
		{
			const std::vector<apollo2::SimpleSphere> tangents=apollo2::apollonius_triangulation::TangentSphereOfFourSpheres::calculate<apollo2::SimpleSphere>(bsh.leaves_spheres().at(q.get(0)), bsh.leaves_spheres().at(q.get(1)), bsh.leaves_spheres().at(q.get(2)), bsh.leaves_spheres().at(q.get(3)));
			for(std::size_t j=0;j<tangents.size();j++)
			{
				const apollo2::SimpleSphere& tangent=tangents[j];
				if(apollo2::apollonius_triangulation::find_any_collision(bsh, tangent).empty())
				{
					result.push_back(tangent);
				}
			}
		}
	}
	return result;
}

std::tr1::unordered_set<apollo2::apollonius_triangulation::Triple, apollo2::apollonius_triangulation::Triple::HashFunctor> collect_triples_from_quadruples(const std::vector<apollo2::apollonius_triangulation::Quadruple>& quadruples)
{
	std::tr1::unordered_set<apollo2::apollonius_triangulation::Triple, apollo2::apollonius_triangulation::Triple::HashFunctor> result;
	for(std::size_t i=0;i<quadruples.size();i++)
	{
		const apollo2::apollonius_triangulation::Quadruple q=quadruples[i];
		for(int j=0;j<4;j++)
		{
			result.insert(q.exclude(j));
		}

	}
	return result;
}

void x_compare_two_sets_of_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--file1: --file2:");

	std::string file1=clo.arg<std::string>("--file1");
	std::string file2=clo.arg<std::string>("--file2");

	std::vector<apollo2::SimpleSphere> atoms=read_spheres_from_stream(std::cin);

	std::ifstream stream1(file1.c_str(), std::ifstream::in);
	std::ifstream stream2(file2.c_str(), std::ifstream::in);

	std::vector<apollo2::apollonius_triangulation::Quadruple> quadruples1=read_quadruples_from_stream(stream1);
	std::vector<apollo2::apollonius_triangulation::Quadruple> quadruples2=read_quadruples_from_stream(stream2);

	std::vector<apollo2::apollonius_triangulation::Quadruple> difference(quadruples1.size());
	std::vector<apollo2::apollonius_triangulation::Quadruple>::iterator it=std::set_difference(quadruples1.begin(), quadruples1.end(), quadruples2.begin(), quadruples2.end(), difference.begin());
	difference.resize(it-difference.begin());

	int confirmed_differences=0;
	int confirmed_differences_with_ball_between_planes=0;
	double max_min_perturbation=0.0;
	int confirmed_differences_with_found_triples=0;
	if(!difference.empty())
	{
		apollo2::BoundingSpheresHierarchy<apollo2::SimpleSphere> bsh(atoms, 3.5, 1);
		const std::vector<apollo2::SimpleSphere> empty_tangent_spheres2=get_empty_tangent_spheres_from_quadruples(bsh, quadruples2);
		const std::tr1::unordered_set<apollo2::apollonius_triangulation::Triple, apollo2::apollonius_triangulation::Triple::HashFunctor> triples2=collect_triples_from_quadruples(quadruples2);
		for(std::size_t i=0;i<difference.size();i++)
		{
			const apollo2::apollonius_triangulation::Quadruple q=difference[i];
			if(q.get(0)<atoms.size() && q.get(1)<atoms.size() && q.get(2)<atoms.size() && q.get(3)<atoms.size())
			{
				const std::vector<apollo2::SimpleSphere> tangents=apollo2::apollonius_triangulation::TangentSphereOfFourSpheres::calculate<apollo2::SimpleSphere>(atoms.at(q.get(0)), atoms.at(q.get(1)), atoms.at(q.get(2)), atoms.at(q.get(3)));
				bool some_tangents_are_empty=false;
				for(std::size_t j=0;j<tangents.size();j++)
				{
					const apollo2::SimpleSphere& tangent=tangents[j];
					if(apollo2::apollonius_triangulation::find_any_collision(bsh, tangent).empty())
					{
						some_tangents_are_empty=true;
						double min_perturbation=std::numeric_limits<double>::max();
						for(std::size_t e=0;e<empty_tangent_spheres2.size();e++)
						{
							const double candidate_perturbation=apollo2::distance_from_point_to_point(tangent, empty_tangent_spheres2[e]);
							if(candidate_perturbation<min_perturbation)
							{
								min_perturbation=candidate_perturbation;
							}
						}
						if(min_perturbation>max_min_perturbation)
						{
							max_min_perturbation=min_perturbation;
						}
					}
				}
				if(some_tangents_are_empty)
				{
					confirmed_differences++;
					if(tangents.size()==2)
					{
						confirmed_differences_with_ball_between_planes++;
					}
					bool found_triple=false;
					for(int j=0;j<4 && !found_triple;j++)
					{
						if(triples2.count(q.exclude(j))>0)
						{
							confirmed_differences_with_found_triples++;
							found_triple=true;
						}
					}
				}
			}
		}
	}

	std::cout.precision(std::numeric_limits<double>::digits10);
	std::cout << std::fixed;
	std::cout << "all_differences " << difference.size() << "\n";
	std::cout << "confirmed_differences " << confirmed_differences << "\n";
	std::cout << "confirmed_differences_with_ball_between_planes " << confirmed_differences_with_ball_between_planes << "\n";
	std::cout << "confirmed_differences_with_found_triples " << confirmed_differences_with_found_triples << "\n";
	std::cout << "max_min_perturbation " << max_min_perturbation << "\n";
}
