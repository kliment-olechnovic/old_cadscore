#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_profile.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void main_calculate_contact_area_difference_global_score(const auxiliaries::CommandLineOptions& clo)
{
	const bool use_min=clo.isopt("--use-min");

	auxiliaries::assert_file_header("cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const contacto::ResidueContactAreaDifferenceScore global_score=contacto::calculate_global_contact_area_difference_score_from_profile(residue_contact_area_difference_profile, use_min);

	auxiliaries::print_file_header("global_score");
	std::cout << global_score << "\n";
}
