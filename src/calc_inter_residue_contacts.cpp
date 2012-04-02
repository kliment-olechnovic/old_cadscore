#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_intervals.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

template<typename ContactsMap>
ContactsMap filter_inter_chain_contacts(const ContactsMap& all_contacts)
{
	ContactsMap interface_contacts;
	typename ContactsMap::iterator prev=interface_contacts.begin();
	for(typename ContactsMap::const_iterator it=all_contacts.begin();it!=all_contacts.end();++it)
	{
		if(it->first.a.chain_id!=it->first.b.chain_id)
		{
			prev=interface_contacts.insert(prev, *it);
		}
	}
	return interface_contacts;
}

template<typename ContactsMap>
ContactsMap filter_inter_interval_contacts(const ContactsMap& all_contacts, const std::string& intervals_string)
{
	std::vector< std::vector< std::pair<protein::ResidueID, protein::ResidueID> > > intervals;
	if(!protein::ResidueIDsIntervalsReader::read_residue_ids_intervals(intervals_string, intervals) || intervals.size()<2)
	{
		throw std::runtime_error(std::string("Invalid intervals string: ")+intervals_string);
	}
	ContactsMap interface_contacts;
	typename ContactsMap::iterator prev=interface_contacts.begin();
	for(typename ContactsMap::const_iterator it=all_contacts.begin();it!=all_contacts.end();++it)
	{
		const protein::ResidueID& a=it->first.a;
		const protein::ResidueID& b=it->first.b;
		int a_group=-1;
		int b_group=-1;
		for(std::size_t i=0;i<intervals.size() && (a_group<0 || b_group<0);i++)
		{
			for(std::size_t j=0;j<intervals[i].size() && (a_group<0 || b_group<0);j++)
			{
				const protein::ResidueID& r1=intervals[i][j].first;
				const protein::ResidueID& r2=intervals[i][j].second;
				if(a_group<0 && a.chain_id==r1.chain_id && a.residue_number>=r1.residue_number && a.residue_number<=r2.residue_number)
				{
					a_group=i;
				}
				else if(b_group<0 && b.chain_id==r1.chain_id && b.residue_number>=r1.residue_number && b.residue_number<=r2.residue_number)
				{
					b_group=i;
				}
			}
		}
		if(a_group>=0 && b_group>=0 && a_group!=b_group)
		{
			prev=interface_contacts.insert(prev, *it);
		}
	}
	return interface_contacts;
}

void calc_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --inter-interval: --inter-chain");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms, inter_atom_contacts);

	if(clo.isopt("--inter-chain"))
	{
		inter_residue_contacts=filter_inter_chain_contacts(inter_residue_contacts);
	}

	if(clo.isopt("--inter-interval"))
	{
		inter_residue_contacts=filter_inter_interval_contacts(inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
	}

	auxiliaries::print_file_header(std::cout, "residue_contacts");
	auxiliaries::print_map(std::cout, inter_residue_contacts);
}

void calc_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --inter-interval: --inter-chain");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_1, inter_atom_contacts_1),
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_2, inter_atom_contacts_2));

	if(clo.isopt("--inter-chain"))
	{
		combined_inter_residue_contacts=filter_inter_chain_contacts(combined_inter_residue_contacts);
	}

	if(clo.isopt("--inter-interval"))
	{
		combined_inter_residue_contacts=filter_inter_interval_contacts(combined_inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
	}

	auxiliaries::print_file_comment(std::cout, "This file contains combined inter-residue contact areas.");
	auxiliaries::print_file_comment(std::cout, "These areas were calculated by accumulating inter-residue contact areas.");
	auxiliaries::print_file_comment(std::cout, "");
	auxiliaries::print_file_comment(std::cout, "Brief format description:");
	auxiliaries::print_file_comment(std::cout, "  'file header'");
	auxiliaries::print_file_comment(std::cout, "  'the number of inter-residue contacts (N)'");
	auxiliaries::print_file_comment(std::cout, "  'contact record 1'");
	auxiliaries::print_file_comment(std::cout, "  'contact record 2'");
	auxiliaries::print_file_comment(std::cout, "  ...");
	auxiliaries::print_file_comment(std::cout, "  'contact record N'");
	auxiliaries::print_file_comment(std::cout, "");
	auxiliaries::print_file_comment(std::cout, "Each contact record has the following format:");
	auxiliaries::print_file_comment(std::cout, "  chain_1 residue_number_1 chain_2 residue_number_2");
	auxiliaries::print_file_comment(std::cout, "  'the number of contact types (M)'");
	auxiliaries::print_file_comment(std::cout, "  type_1 area_in_target area_in_model");
	auxiliaries::print_file_comment(std::cout, "  type_2 area_in_target area_in_model");
	auxiliaries::print_file_comment(std::cout, "  ...");
	auxiliaries::print_file_comment(std::cout, "  type_M area_in_target area_in_model");
	auxiliaries::print_file_comment(std::cout, "Contact types ending with 'W' denote solvent-accessible surface areas");
	std::cout << "\n";

	auxiliaries::print_file_header(std::cout, "combined_residue_contacts");
	auxiliaries::print_map(std::cout, combined_inter_residue_contacts);
}
