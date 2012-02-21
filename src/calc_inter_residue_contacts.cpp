#include "protein/atom.h"
#include "protein/residue_id.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

template<typename ContactsMap>
ContactsMap filter_interface_contacts(const ContactsMap& all_contacts)
{
	ContactsMap interface_contacts;
	typename ContactsMap::iterator prev=interface_contacts.begin();
	for(typename ContactsMap::const_iterator it=all_contacts.begin();it!=all_contacts.end();++it)
	{
		if(it->first.a.chain_id!=it->first.b.chain_id)
		{
			prev=interface_contacts.insert(prev, *it);
		}
	}
	return interface_contacts;
}

void calc_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --interface-only");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms, inter_atom_contacts);

	if(clo.isopt("--interface-only"))
	{
		inter_residue_contacts=filter_interface_contacts(inter_residue_contacts);
	}

	auxiliaries::print_file_header(std::cout, "residue_contacts");
	auxiliaries::print_map(std::cout, inter_residue_contacts);
}

void calc_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --interface-only");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_1, inter_atom_contacts_1),
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_2, inter_atom_contacts_2));

	if(clo.isopt("--interface-only"))
	{
		combined_inter_residue_contacts=filter_interface_contacts(combined_inter_residue_contacts);
	}

	auxiliaries::print_file_header(std::cout, "combined_residue_contacts");
	auxiliaries::print_map(std::cout, combined_inter_residue_contacts);
}