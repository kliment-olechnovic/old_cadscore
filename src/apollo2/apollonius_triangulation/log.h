#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_LOG_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_LOG_H_

#include <iostream>

namespace apollo2
{

namespace apollonius_triangulation
{

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

Log& log_ref()
{
	static Log log=Log();
	return log;
}

}

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_LOG_H_ */
