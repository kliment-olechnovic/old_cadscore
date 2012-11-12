#include <iostream>
#include <sstream>
#include <limits>

#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_collection.h"

#include "apollo/spheres_basic_operations.h"

#include "contacto/contact_id.h"
#include "contacto/inter_residue_contact_areas.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/xml_writer.h"
#include "auxiliaries/color.h"
#include "auxiliaries/ppm_image_writer.h"

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

void print_inter_residue_distance_map_image(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--max-distance:");

	const double max_distance=clo.isopt("--max-distance") ? clo.arg_with_min_value<double>("--max-distance", 0.1) : -1.0;

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map<protein::ResidueID, std::vector<std::size_t> > residue_ids_atoms=protein::group_atoms_indices_by_residue_ids(atoms);

	if(residue_ids_atoms.size()<2)
	{
		throw std::runtime_error("Less than 2 residues provided");
	}

	std::vector< std::vector<double> > distance_map(residue_ids_atoms.size(), std::vector<double>(residue_ids_atoms.size(), 0.0));

	double observed_max_distance=-std::numeric_limits<double>::max();

	{
		std::size_t i=0;
		for(std::map<protein::ResidueID, std::vector<std::size_t> >::const_iterator it=residue_ids_atoms.begin();it!=residue_ids_atoms.end();++it)
		{
			std::size_t j=i+1;
			std::map<protein::ResidueID, std::vector<std::size_t> >::const_iterator jt=it;
			++jt;
			for(;jt!=residue_ids_atoms.end();++jt)
			{
				const std::vector<std::size_t>& atoms_i=it->second;
				const std::vector<std::size_t>& atoms_j=jt->second;
				double min_distance=std::numeric_limits<double>::max();
				for(std::size_t m=0;m<atoms_i.size();m++)
				{
					for(std::size_t n=0;n<atoms_j.size();n++)
					{
						min_distance=std::min(min_distance, apollo::minimal_distance_from_sphere_to_sphere(atoms[atoms_i[m]], atoms[atoms_j[n]]));
					}
				}
				distance_map[i][j]=min_distance;
				distance_map[j][i]=min_distance;
				observed_max_distance=std::max(observed_max_distance, min_distance);
				j++;
			}
			i++;
		}
	}

	const double usable_max_distance=(max_distance>0.0 ? max_distance : observed_max_distance);

	auxiliaries::PPMImageWriter image_writer(distance_map.size(), distance_map.size());
	for(std::size_t i=0;i<distance_map.size();i++)
	{
		for(std::size_t j=0;j<distance_map.size();j++)
		{
			const double dist=distance_map[i][j];
			if(dist<=usable_max_distance)
			{
				image_writer.color(i, j)=auxiliaries::Color::from_temperature_to_blue_white_red(1.0-dist/usable_max_distance);
			}
		}
	}
	image_writer.write(std::cout);
}
