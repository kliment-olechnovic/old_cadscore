#include <string>
#include <iostream>
#include <fstream>

#include "protein/atoms_reading.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

std::vector<protein::Atom> read_protein_atoms(const std::string& radius_classes_file_name, const std::string& radius_members_file_name, const bool include_heteroatoms, const bool include_water)
{
	std::ifstream radius_classes_stream(radius_classes_file_name.c_str());
	std::ifstream radius_members_stream(radius_members_file_name.c_str());
	const protein::VanDerWaalsRadiusAssigner radius_assigner(radius_classes_stream, radius_members_stream);
	return protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, include_heteroatoms, include_water);
}

void main_collect_atoms(const auxiliaries::CommandLineOptions& clo)
{
	const std::string radius_classes_file_name=clo.arg<std::string>("--radius-classes");
	const std::string radius_members_file_name=clo.arg<std::string>("--radius-members");
	const bool include_heteroatoms=clo.isopt("--include-heteroatoms");
	const bool include_water=clo.isopt("--include-water");

	const std::vector<protein::Atom> atoms=read_protein_atoms(
			radius_classes_file_name,
			radius_members_file_name,
			include_heteroatoms, include_water);

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(atoms);
}
