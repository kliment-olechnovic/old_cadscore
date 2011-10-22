#include <cmath>

#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_collection.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"
#include "contacto/residue_contact_area_difference_profile.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

struct DifferenceProducer
{
	double operator()(const double target_area, const double model_area) const
	{
		return std::min(fabs(target_area-model_area), target_area);
	}
};

struct ReferenceProducer
{
	double operator()(const double target_area, const double model_area) const
	{
		return target_area;
	}
};

void main_calculate_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo)
{
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

	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile=contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, DifferenceProducer, ReferenceProducer>(combined_inter_residue_contacts, residue_ids_1);

	auxiliaries::print_file_header("cad_profile");
	auxiliaries::print_map(residue_contact_area_difference_profile);
}
