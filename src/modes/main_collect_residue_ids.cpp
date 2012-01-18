#include "protein/atom.h"
#include "protein/residue_ids_collection.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/set_io.h"

void main_collect_residue_ids(const auxiliaries::CommandLineOptions& clo)
{
	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>();
	const std::set<protein::ResidueID> residue_ids=protein::collect_residue_ids_from_atoms(atoms);
	auxiliaries::print_file_header("residue_ids");
	auxiliaries::print_set(residue_ids);
}