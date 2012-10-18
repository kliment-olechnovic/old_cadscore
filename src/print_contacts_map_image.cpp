#include <iostream>
#include <sstream>

#include "protein/residue_id.h"

#include "contacto/contact_id.h"
#include "contacto/inter_residue_contact_areas.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/xml_writer.h"
#include "auxiliaries/color.h"

void print_inter_residue_contacts_map_image(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > InterResidueContacts;

	clo.check_allowed_options("--contact-class: --color-by-area");

	const std::string contact_class=clo.arg<std::string>("--contact-class");
	const bool color_by_area=clo.isopt("--color-by-area");

	const InterResidueContacts inter_residue_contacts=auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	std::map<protein::ResidueID, std::size_t> axis;
	for(InterResidueContacts::const_iterator it=inter_residue_contacts.begin();it!=inter_residue_contacts.end();++it)
	{
		axis[it->first.a]=0;
		axis[it->first.b]=0;
	}
	{
		std::size_t i=0;
		for(std::map<protein::ResidueID, std::size_t>::iterator it=axis.begin();it!=axis.end();++it)
		{
			it->second=i++;
		}
	}

	double max_area=0.0;
	for(InterResidueContacts::const_iterator it=inter_residue_contacts.begin();it!=inter_residue_contacts.end();++it)
	{
		max_area=std::max(max_area, it->second.area(contact_class));
	}

	auxiliaries::XMLWriter svg("svg");
	svg.set("width", axis.size()).set("height", axis.size());
	svg.add_child(auxiliaries::XMLWriter("rect").set("x", 0).set("y", 0).set("width", axis.size()).set("height", axis.size()).set("fill", "black"));
	for(InterResidueContacts::const_iterator it=inter_residue_contacts.begin();it!=inter_residue_contacts.end();++it)
	{
		const std::size_t x=axis[it->first.a];
		const std::size_t y=axis[it->first.b];
		const double area=it->second.area(contact_class);
		if(area>0.0)
		{
			const auxiliaries::Color color=(color_by_area ? auxiliaries::Color::from_temperature_to_blue_white_red(max_area>0.0 ? area/max_area : 0.0) : auxiliaries::Color(255, 255, 255));
			std::ostringstream color_output;
			color_output << "rgb(" << static_cast<unsigned int>(color.r) << "," << static_cast<unsigned int>(color.g) << "," << static_cast<unsigned int>(color.b) << ")";
			svg.add_child(auxiliaries::XMLWriter("rect").set("x", x).set("y", y).set("width", 1).set("height", 1).set("fill", color_output.str()));
		}
	}
	svg.write(std::cout);
}
