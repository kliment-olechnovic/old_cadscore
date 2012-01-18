#include <set>

#include "../protein/residue_id.h"

#include "../contacto/inter_residue_contact_id.h"
#include "../contacto/inter_residue_contact_dual_areas.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/map_io.h"

void main_print_combined_inter_residue_contacts_sequence_map(const auxiliaries::CommandLineOptions&/* clo*/)
{
	auxiliaries::assert_file_header("combined_residue_contacts");
	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
			auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >();

	std::set<protein::ResidueID> sequence_set;
	for(std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts.begin();it!=combined_inter_residue_contacts.end();++it)
	{
		sequence_set.insert(it->first.a);
		sequence_set.insert(it->first.b);
	}

	auxiliaries::print_file_header("sequence_map");
	std::cout << sequence_set.size() << "\n";
	std::size_t i=0;
	for(std::set<protein::ResidueID>::iterator it=sequence_set.begin();it!=sequence_set.end();++it)
	{
		std::cout << i << " " << (*it) << "\n";
		i++;
	}
}
