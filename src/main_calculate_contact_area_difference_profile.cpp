#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_collection.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"
#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/residue_contact_area_difference_basic_scoring_functors.h"
#include "contacto/utilities.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

void main_calculate_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo)
{
	const int scoring_mode=clo.arg<int>("--scoring-mode", 0, 3);

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>();

	auxiliaries::assert_file_header("contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>();

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>();

	auxiliaries::assert_file_header("contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>();

	const std::set<protein::ResidueID> residue_ids_1=protein::collect_residue_ids_from_atoms(atoms_1);

	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_1, inter_atom_contacts_1),
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_2, inter_atom_contacts_2));

	std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile;
	if(scoring_mode==0)
	{
		residue_contact_area_difference_profile=
				contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, contacto::BoundedDifferenceProducer, contacto::SimpleReferenceProducer>(combined_inter_residue_contacts, residue_ids_1);
	}
	else if(scoring_mode==1)
	{
		residue_contact_area_difference_profile=
				contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, contacto::SimpleDifferenceProducer, contacto::SimpleReferenceProducer>(combined_inter_residue_contacts, residue_ids_1);
	}
	else if(scoring_mode==2)
	{
		residue_contact_area_difference_profile=
				contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, contacto::SimpleDifferenceProducer, contacto::SummingReferenceProducer>(combined_inter_residue_contacts, residue_ids_1);
	}
	else if(scoring_mode==3)
	{
		residue_contact_area_difference_profile=
				contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, contacto::SimpleDifferenceProducer, contacto::SummingReferenceProducer>(combined_inter_residue_contacts, contacto::intersect_two_sets(residue_ids_1, protein::collect_residue_ids_from_atoms(atoms_2)));
	}
	else
	{
		throw std::runtime_error("Invalid scoring mode");
	}

	auxiliaries::print_file_header("cad_profile");
	auxiliaries::print_map(residue_contact_area_difference_profile);
}
