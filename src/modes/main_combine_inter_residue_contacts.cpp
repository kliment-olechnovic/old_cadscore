#include "../protein/atom.h"
#include "../protein/residue_id.h"

#include "../contacto/inter_residue_contacts_construction.h"
#include "../contacto/inter_residue_contacts_combination.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"
#include "../auxiliaries/map_io.h"

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

	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_1, inter_atom_contacts_1),
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_2, inter_atom_contacts_2));

	auxiliaries::print_file_header("combined_residue_contacts");
	auxiliaries::print_map(combined_inter_residue_contacts);
}
