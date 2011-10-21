#include "protein_input.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void main_collect_atoms(const auxiliaries::CommandLineOptions& clo)
{
	const std::string radius_classes_file_name=clo.arg<std::string>("--radius-classes");
	const std::string radius_members_file_name=clo.arg<std::string>("--radius-members");
	const bool include_heteroatoms=clo.isopt("--include-heteroatoms");
	const bool include_water=clo.isopt("--include-water");

	const std::vector<protein::Atom> atoms=read_protein_atoms(radius_classes_file_name, radius_members_file_name, include_heteroatoms, include_water);
	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(atoms);
}
