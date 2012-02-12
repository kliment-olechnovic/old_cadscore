#include <iostream>

#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_local_scores.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/ppm_image_writer.h"

void print_cad_local_scores_plot(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --category: --max-window:");

	const std::string category=clo.arg<std::string>("--category");
	const int max_window_size=clo.arg_in_interval<int>("--max-window", 0, 1000);

	auxiliaries::assert_file_header(std::cin, "cad_profile");
	const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>(std::cin);

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
				color=auxiliaries::Color::from_temperature_to_blue_white_red(score);
			}
			else
			{
				color=auxiliaries::Color(200, 200, 200);
			}
			i++;
		}
	}
	image.write(std::cout);
}
