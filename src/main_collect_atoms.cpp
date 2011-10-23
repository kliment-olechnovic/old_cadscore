#include <string>
#include <iostream>
#include <fstream>

#include "protein/atoms_reading.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void main_collect_atoms(const auxiliaries::CommandLineOptions& clo)
{
	const std::string radius_classes_file_name=clo.arg<std::string>("--radius-classes");
	const std::string radius_members_file_name=clo.arg<std::string>("--radius-members");
	const int include_heteroatoms=clo.arg<int>("--include-heteroatoms", 0, 1);
	const int include_water=clo.arg<int>("--include-water", 0, 1);

	std::ifstream radius_classes_stream(radius_classes_file_name.c_str());
	std::ifstream radius_members_stream(radius_members_file_name.c_str());
	const protein::VanDerWaalsRadiusAssigner radius_assigner(radius_classes_stream, radius_members_stream);

	const std::vector<protein::Atom> atoms=protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, (include_heteroatoms==1), (include_water==1));

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(atoms);
}
