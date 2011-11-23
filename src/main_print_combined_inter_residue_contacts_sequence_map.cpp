#include "protein/residue_id.h"

#include "contacto/inter_residue_contact_id.h"
#include "contacto/inter_residue_contact_dual_areas.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void main_print_combined_inter_residue_contacts_sequence_map(const auxiliaries::CommandLineOptions&/* clo*/)
{
	auxiliaries::assert_file_header("combined_residue_contacts");
	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
			auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >();

	std::map<protein::ResidueID, std::size_t> sequence_map;
	for(std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts.begin();it!=combined_inter_residue_contacts.end();++it)
	{
		sequence_map[it->first.a]=0;
		sequence_map[it->first.b]=0;
	}

	auxiliaries::print_file_header("sequence_map");
	std::cout << sequence_map.size() << "\n";
	std::size_t i=0;
	for(std::map<protein::ResidueID, std::size_t>::iterator it=sequence_map.begin();it!=sequence_map.end();++it)
	{
		std::cout << i << " " << it->first << "\n";
		i++;
	}
}
