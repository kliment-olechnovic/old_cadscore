#include "../protein/residue_id.h"

#include "../contacto/inter_residue_contact_id.h"
#include "../contacto/inter_residue_contact_dual_areas.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/map_io.h"
#include "../auxiliaries/set_io.h"

void main_filter_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	const int filter_mode=clo.arg_in_interval<int>("--filter-mode", 0, 1);

	typedef std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > CombinedContacts;

	auxiliaries::assert_file_header("combined_residue_contacts");
	CombinedContacts combined_inter_residue_contacts=auxiliaries::read_map< CombinedContacts::key_type, CombinedContacts::mapped_type >();

	while(auxiliaries::check_file_header("residue_ids"))
	{
		const std::set<protein::ResidueID> residue_ids=auxiliaries::read_set<protein::ResidueID>();
		CombinedContacts filtered_combined_inter_residue_contacts;
		CombinedContacts::iterator prev=filtered_combined_inter_residue_contacts.begin();
		for(CombinedContacts::const_iterator it=combined_inter_residue_contacts.begin();it!=combined_inter_residue_contacts.end();++it)
		{
			if((filter_mode==0 && (residue_ids.count(it->first.a)==0 || residue_ids.count(it->first.b)==0)) ||
					(filter_mode==1 && (residue_ids.count(it->first.a)==1 || residue_ids.count(it->first.b)==1)))
			{
				prev=filtered_combined_inter_residue_contacts.insert(prev, *it);
			}
		}
		combined_inter_residue_contacts=filtered_combined_inter_residue_contacts;
	}

	auxiliaries::print_file_header("combined_residue_contacts");
	auxiliaries::print_map(combined_inter_residue_contacts);
}
