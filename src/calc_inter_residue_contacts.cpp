#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_intervals.h"
#include "protein/residue_ids_collection.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"
#include "contacto/residue_contact_area_difference_profile.h"
#include "contacto/residue_contact_area_difference_basic_scoring_functors.h"

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

void print_combined_inter_residue_contact_file_comments()
{
	auxiliaries::print_file_comment(std::cout, "This file contains combined inter-residue contact areas");
	auxiliaries::print_file_comment(std::cout, "calculated by accumulating inter-atom contact areas.");
	auxiliaries::print_file_comment(std::cout, "");
	auxiliaries::print_file_comment(std::cout, "The file is structured as follows:");
	auxiliaries::print_file_comment(std::cout, "  file_header (always equals 'combined_residue_contacts')");
	auxiliaries::print_file_comment(std::cout, "  N (the number of inter-residue contacts)");
	auxiliaries::print_file_comment(std::cout, "  contact_record[1]");
	auxiliaries::print_file_comment(std::cout, "  contact_record[2]");
	auxiliaries::print_file_comment(std::cout, "  ...");
	auxiliaries::print_file_comment(std::cout, "  contact_record[N]");
	auxiliaries::print_file_comment(std::cout, "");
	auxiliaries::print_file_comment(std::cout, "Each contact record has the following format:");
	auxiliaries::print_file_comment(std::cout, "  first_residue_chain_name first_residue_number second_residue_chain_name second_residue_number");
	auxiliaries::print_file_comment(std::cout, "  M (the number of contact types)");
	auxiliaries::print_file_comment(std::cout, "  contact_type[1] corresponding_area_in_target corresponding_area_in_model");
	auxiliaries::print_file_comment(std::cout, "  contact_type[2] corresponding_area_in_target corresponding_area_in_model");
	auxiliaries::print_file_comment(std::cout, "  ...");
	auxiliaries::print_file_comment(std::cout, "  contact_type[M] corresponding_area_in_target corresponding_area_in_model");
	auxiliaries::print_file_comment(std::cout, "");
	auxiliaries::print_file_comment(std::cout, "Contact types are two-letter strings indicating what residue parts are in contact.");
	auxiliaries::print_file_comment(std::cout, "Each residue part is coded as a single letter:");
	auxiliaries::print_file_comment(std::cout, "  A - all residue atoms");
	auxiliaries::print_file_comment(std::cout, "  M - main chain");
	auxiliaries::print_file_comment(std::cout, "  S - side chain");
	auxiliaries::print_file_comment(std::cout, "Contact types ending with 'W' denote solvent-accessible surface areas");
	std::cout << "\n";
}

void calc_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--inter-interval: --inter-chain");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::vector<contacto::InterAtomContact> inter_atom_contacts=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin, "inter-atom contacts", "contacts", false);

	std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms, inter_atom_contacts);

	if(clo.isopt("--inter-chain"))
	{
		inter_residue_contacts=filter_inter_chain_contacts(inter_residue_contacts);
	}

	if(clo.isopt("--inter-interval"))
	{
		inter_residue_contacts=filter_inter_interval_contacts(inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
	}

	if(inter_residue_contacts.empty())
	{
		throw std::runtime_error("No inter-residue contacts constructed");
	}
	else
	{
		auxiliaries::print_map(std::cout, "residue_contacts", inter_residue_contacts, true);
	}
}

void calc_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--inter-interval: --inter-chain");

	const std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts_1=
			auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	const std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts_2=
			auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(inter_residue_contacts_1, inter_residue_contacts_2);

	if(clo.isopt("--inter-chain"))
	{
		combined_inter_residue_contacts=filter_inter_chain_contacts(combined_inter_residue_contacts);
	}

	if(clo.isopt("--inter-interval"))
	{
		combined_inter_residue_contacts=filter_inter_interval_contacts(combined_inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
	}

	if(combined_inter_residue_contacts.empty())
	{
		throw std::runtime_error("No combined inter-residue contacts constructed");
	}
	else
	{
		print_combined_inter_residue_contact_file_comments();
		auxiliaries::print_map(std::cout, "combined_residue_contacts", combined_inter_residue_contacts, true);
	}
}

template<typename ContactsMap>
std::vector<std::string> collect_chain_names_from_contacts_map(const ContactsMap& contacts)
{
	std::set<std::string> set_of_names;
	for(typename ContactsMap::const_iterator it=contacts.begin();it!=contacts.end();++it)
	{
		set_of_names.insert(it->first.a.chain_id);
		set_of_names.insert(it->first.b.chain_id);
	}
	std::vector<std::string> vector_of_names;
	vector_of_names.insert(vector_of_names.end(), set_of_names.begin(), set_of_names.end());
	return vector_of_names;
}

void calc_combined_inter_residue_contacts_with_chains_optimally_renamed(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > CombinedInterResidueContacts;

	clo.check_allowed_options("--inter-interval: --inter-chain");

	const std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts_1=
			auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	const std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts_2=
			auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids_1=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "target residue identifiers", "residue_ids", false);;

	const std::vector<std::string> chain_names_1=collect_chain_names_from_contacts_map(inter_residue_contacts_1);
	const std::vector<std::string> chain_names_2=collect_chain_names_from_contacts_map(inter_residue_contacts_2);

	bool renaming_allowed=(chain_names_1.size()>1 && chain_names_1.size()==chain_names_2.size());
	for(std::size_t j=0;j<chain_names_1.size() && renaming_allowed;j++)
	{
		renaming_allowed=(chain_names_1[j]==chain_names_2[j]);
	}

	if(renaming_allowed)
	{
		std::map<double, CombinedInterResidueContacts> variations;

		std::vector<std::string> chain_names_permutation=chain_names_1;

		do
		{
			std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts_2_with_renamed_chains;
			for(typename std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >::const_iterator it=inter_residue_contacts_2.begin();it!=inter_residue_contacts_2.end();++it)
			{
				contacto::ContactID<protein::ResidueID> cid=it->first;
				bool a_renamed=false;
				bool b_renamed=false;
				for(std::size_t j=0;j<chain_names_2.size() && !(a_renamed && b_renamed);j++)
				{
					if(!a_renamed && cid.a.chain_id==chain_names_2[j])
					{
						cid.a.chain_id=chain_names_permutation[j];
						a_renamed=true;
					}
					if(!b_renamed && cid.b.chain_id==chain_names_2[j])
					{
						cid.b.chain_id=chain_names_permutation[j];
						b_renamed=true;
					}
				}
				inter_residue_contacts_2_with_renamed_chains[cid]=it->second;
			}

			CombinedInterResidueContacts combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(inter_residue_contacts_1, inter_residue_contacts_2_with_renamed_chains);

			if(clo.isopt("--inter-chain"))
			{
				combined_inter_residue_contacts=filter_inter_chain_contacts(combined_inter_residue_contacts);
			}

			if(clo.isopt("--inter-interval"))
			{
				combined_inter_residue_contacts=filter_inter_interval_contacts(combined_inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
			}

			const std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> residue_contact_area_difference_profile=contacto::construct_residue_contact_area_difference_profile<protein::ResidueID, protein::ResidueSummary, contacto::BoundedDifferenceProducer, contacto::SimpleReferenceProducer>(combined_inter_residue_contacts, residue_ids_1);
			const contacto::ResidueContactAreaDifferenceScore global_score=contacto::calculate_global_contact_area_difference_score_from_profile(residue_contact_area_difference_profile, false);
			const contacto::Ratio ratio=global_score.ratio("AA");
			if(ratio.reference>0.0)
			{
				variations[(1-(ratio.difference/ratio.reference))]=combined_inter_residue_contacts;
			}

			std::clog << "Tried renaming chains: ( ";
			for(std::size_t j=0;j<chain_names_2.size();j++)
			{
				std::clog << chain_names_2[j] << " ";
			}
			std::clog << ") -> ( ";
			for(std::size_t j=0;j<chain_names_2.size();j++)
			{
				std::clog << chain_names_permutation[j] << " ";
			}
			std::clog << "), got AA-score == " << (ratio.reference>0.0 ? (1-(ratio.difference/ratio.reference)) : 0.0) << "\n";
		}
		while(std::next_permutation(chain_names_permutation.begin(), chain_names_permutation.end()));

		if(variations.empty() || variations.rbegin()->second.empty())
		{
			throw std::runtime_error("No combined inter-residue contacts constructed");
		}
		else
		{
			print_combined_inter_residue_contact_file_comments();
			auxiliaries::print_map(std::cout, "combined_residue_contacts", variations.rbegin()->second, true);
		}
	}
	else
	{
		std::clog << "Chains renaming was not performed\n";

		CombinedInterResidueContacts combined_inter_residue_contacts=contacto::combine_two_inter_residue_contact_maps<protein::ResidueID>(inter_residue_contacts_1, inter_residue_contacts_2);

		if(clo.isopt("--inter-chain"))
		{
			combined_inter_residue_contacts=filter_inter_chain_contacts(combined_inter_residue_contacts);
		}

		if(clo.isopt("--inter-interval"))
		{
			combined_inter_residue_contacts=filter_inter_interval_contacts(combined_inter_residue_contacts, clo.arg<std::string>("--inter-interval"));
		}

		if(combined_inter_residue_contacts.empty())
		{
			throw std::runtime_error("No combined inter-residue contacts constructed");
		}
		else
		{
			print_combined_inter_residue_contact_file_comments();
			auxiliaries::print_map(std::cout, "combined_residue_contacts", combined_inter_residue_contacts, true);
		}
	}
}
