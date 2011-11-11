#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/contact_classification.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void main_calculate_contact_area_difference_global_score(const auxiliaries::CommandLineOptions& clo)
{
	const int use_min=clo.arg<int>("--use-min", 0, 1);
	const std::string prefix=clo.arg_or_default_value<std::string>("--prefix", "");

	auxiliaries::assert_file_header("cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const contacto::ResidueContactAreaDifferenceScore global_score=contacto::calculate_global_contact_area_difference_score_from_profile(residue_contact_area_difference_profile, (use_min==1));

	const std::vector<std::string> all_classes=contacto::ContactClassification::get_all_classes_list();
	for(std::size_t i=0;i<all_classes.size();i++)
	{
		const std::string& the_class=all_classes[i];
		const contacto::ResidueContactAreaDifferenceScore::Ratio ratio=global_score.ratio(the_class);
		const std::string big_prefix=(!prefix.empty() ? (prefix+"_") : std::string(""))+the_class;
		std::cout << big_prefix << "_diff " << ratio.difference << "\n";
		std::cout << big_prefix << "_ref " << ratio.reference << "\n";
	}
}
