#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_local_scores.h"
#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/contact_classification.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void calc_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --category: --window:");

	const std::string category=clo.arg<std::string>("--category");
	const int window_size=clo.arg_in_interval<int>("--window", 0, 1000);

	auxiliaries::assert_file_header(std::cin, "cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>(std::cin);

	const std::map<protein::ResidueID, double> local_scores=contacto::blur_local_scores(contacto::construct_local_scores_from_profile(profile, category), window_size);

	auxiliaries::print_file_header(std::cout, "local_scores");
	auxiliaries::print_map(std::cout, local_scores, false);
}

void calc_contact_area_difference_global_scores(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --use-min");

	const bool use_min=clo.isopt("--use-min");

	auxiliaries::assert_file_header(std::cin, "cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>(std::cin);

	const contacto::ResidueContactAreaDifferenceScore global_score=contacto::calculate_global_contact_area_difference_score_from_profile(residue_contact_area_difference_profile, use_min);

	const std::vector<std::string> all_classes=contacto::ContactClassification::get_all_classes_list();
	for(std::size_t i=0;i<all_classes.size();i++)
	{
		const std::string& the_class=all_classes[i];
		const contacto::ResidueContactAreaDifferenceScore::Ratio ratio=global_score.ratio(the_class);
		std::cout << the_class << "_diff " << ratio.difference << "\n";
		std::cout << the_class << "_ref " << ratio.reference << "\n";
		std::cout << the_class << " " << (ratio.reference>0.0 ? (1-(ratio.difference/ratio.reference)) : 0.0) << "\n";
	}
}
