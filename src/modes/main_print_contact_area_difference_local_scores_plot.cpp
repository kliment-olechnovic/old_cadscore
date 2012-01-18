#include <iostream>

#include "../protein/residue_id.h"

#include "../contacto/residue_contact_area_difference_local_scores.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/map_io.h"
#include "../auxiliaries/ppm_image_writer.h"

void main_print_contact_area_difference_local_scores_plot(const auxiliaries::CommandLineOptions& clo)
{
	const std::string category=clo.arg<std::string>("--category");
	const int max_window_size=clo.arg<int>("--max-window", 0, 1000);

	auxiliaries::assert_file_header("cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const std::map<protein::ResidueID, double> local_scores=contacto::construct_local_scores_from_profile(profile, category);
	auxiliaries::PPMImageWriter image(static_cast<std::size_t>(max_window_size+1), local_scores.size());
	for(int window_size=0;window_size<=max_window_size;window_size++)
	{
		const std::map<protein::ResidueID, double> blured_local_scores=contacto::blur_local_scores(local_scores, window_size);
		std::size_t i=0;
		for(std::map<protein::ResidueID, double>::const_iterator it=blured_local_scores.begin();it!=blured_local_scores.end();++it)
		{
			auxiliaries::Color& color=image.color(window_size, i);
			const double score=it->second;
			if(score>=0)
			{
				color=auxiliaries::Color::from_temperature(score);
			}
			else
			{
				color=auxiliaries::Color(255, 255, 0);
			}
			i++;
		}
	}
	image.write(std::cout);

	auxiliaries::print_file_header("local_scores");
	auxiliaries::print_map(local_scores, false);
}
