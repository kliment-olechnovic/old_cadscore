#include "protein_inter_residue_contacts.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

void main_combine_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>();
	auxiliaries::assert_file_header("contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>();
	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>();
	auxiliaries::assert_file_header("contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>();
	auxiliaries::print_file_header("combined_residue_contacts");
	auxiliaries::print_map(construct_combined_inter_residue_contacts_from_inter_atom_contacts(atoms_1, inter_atom_contacts_1, atoms_2, inter_atom_contacts_2));
}
