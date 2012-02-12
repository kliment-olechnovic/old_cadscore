#include "protein/atom.h"
#include "protein/residue_id.h"

#include "contacto/inter_residue_contact_dual_areas.h"
#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/residue_contact_area_difference_basic_scoring_functors.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/set_io.h"

void calc_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --type:");

	const int scoring_mode=clo.isopt("--type") ? clo.arg_in_interval<int>("--type", 0, 2) : 0;

	auxiliaries::assert_file_header(std::cin, "combined_residue_contacts");
	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
			auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >();

	auxiliaries::assert_file_header(std::cin, "residue_ids");
	const std::set<protein::ResidueID> residue_ids_1=auxiliaries::read_set<protein::ResidueID>();

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
	else
	{
		throw std::runtime_error("Invalid profile type");
	}

	auxiliaries::print_file_header(std::cout, "cad_profile");
	auxiliaries::print_map(residue_contact_area_difference_profile);
}


