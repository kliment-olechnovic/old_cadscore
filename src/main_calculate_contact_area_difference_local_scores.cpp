#include <iostream>
#include <limits>

#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/residue_contact_area_difference_local_scores.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void main_calculate_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo)
{
	const std::string category=clo.arg<std::string>("--category");
	const int window_size=clo.arg<int>("--window", 0, 1000);

	auxiliaries::assert_file_header("cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const std::map<protein::ResidueID, double> local_scores=contacto::blur_local_scores(contacto::construct_local_scores_from_profile(profile, category), window_size);

	auxiliaries::print_file_header("local_scores");
	auxiliaries::print_map(local_scores, false);
}
