#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_RESULT_CONTAINERS_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_RESULT_CONTAINERS_H_

#include <vector>

#include <tr1/unordered_map>

#include "../tuple.h"
#include "../basic_operations_on_spheres.h"

namespace apollo2
{

namespace apollonius_triangulation
{

typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

struct QuadruplesLog
{
	std::size_t quadruples;
	std::size_t processed_faces;
	std::size_t tangent_spheres;
	std::size_t difficult_faces;
	std::size_t produced_faces;
	std::size_t updated_faces;
	std::size_t triples_repetitions;
	std::size_t finding_first_faces_iterations;
	std::size_t surplus_quadruples;
	std::size_t surplus_tangent_spheres;
};

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_RESULT_CONTAINERS_H_ */
