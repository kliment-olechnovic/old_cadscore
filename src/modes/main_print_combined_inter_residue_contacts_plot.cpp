#include "../protein/residue_id.h"

#include "../contacto/inter_residue_contact_id.h"
#include "../contacto/inter_residue_contact_dual_areas.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/map_io.h"
#include "../auxiliaries/ppm_image_writer.h"

void main_print_combined_inter_residue_contacts_plot(const auxiliaries::CommandLineOptions& clo)
{
	const std::string category=clo.arg<std::string>("--category");

	auxiliaries::assert_file_header("combined_residue_contacts");
	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
			auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >();

	std::map<protein::ResidueID, std::size_t> sequence_map;
	for(std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts.begin();it!=combined_inter_residue_contacts.end();++it)
	{
		sequence_map[it->first.a]=0;
		sequence_map[it->first.b]=0;
	}
	std::size_t i=0;
	for(std::map<protein::ResidueID, std::size_t>::iterator it=sequence_map.begin();it!=sequence_map.end();++it)
	{
		it->second=i++;
	}

	auxiliaries::PPMImageWriter image(sequence_map.size(), sequence_map.size());
	for(std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts.begin();it!=combined_inter_residue_contacts.end();++it)
	{
		const std::pair<double, double> area=it->second.area(category);
		if(area.first>0 || area.second>0)
		{
			const double max_area=std::max(area.first, area.second);
			auxiliaries::Color& color=image.color(sequence_map[it->first.a], sequence_map[it->first.b]);
			color.r=(area.first/max_area)*255;
			color.g=(area.second/max_area)*255;
		}
	}
	image.write(std::cout);
}
