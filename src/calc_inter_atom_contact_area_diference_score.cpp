#include <iostream>
#include <map>
#include <cmath>

#include "protein/atom.h"
#include "protein/atom_id.h"

#include "contacto/contact_id.h"
#include "contacto/inter_atom_contact.h"
#include "contacto/utilities.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

std::map<contacto::ContactID<protein::AtomID>, double> construct_inter_atom_contacts_map(const std::vector<protein::Atom>& atoms, const std::vector<contacto::InterAtomContact>& inter_atom_contacts)
{
	std::map<contacto::ContactID<protein::AtomID>, double> contacts_map;
	for(std::size_t i=0;i<inter_atom_contacts.size();i++)
	{
		const contacto::InterAtomContact& contact=inter_atom_contacts[i];
		if(contact.a!=contact.b)
		{
			contacts_map[contacto::ContactID<protein::AtomID>(protein::AtomID::from_atom(atoms[contact.a]), protein::AtomID::from_atom(atoms[contact.b]))]=contact.area;
		}
	}
	return contacts_map;
}

void calc_inter_atom_contact_area_difference_score(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--local --global");

	const bool print_local=clo.isopt("--local");
	const bool print_global=clo.isopt("--global") || !print_local;

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	typedef std::map< contacto::ContactID<protein::AtomID>, std::pair<double, double> > CombinedContactsMap;

	const CombinedContactsMap combined_contacts=contacto::combine_two_maps(
			construct_inter_atom_contacts_map(atoms_1, inter_atom_contacts_1),
			construct_inter_atom_contacts_map(atoms_2, inter_atom_contacts_2));

	if(print_global)
	{
		std::pair<double, double> global_ratio;
		for(CombinedContactsMap::const_iterator it=combined_contacts.begin();it!=combined_contacts.end();++it)
		{
			const double t=it->second.first;
			const double m=it->second.second;
			global_ratio.first+=std::min(fabs(t-m), t);
			global_ratio.second+=t;
		}
		std::cout << "inter_atom_cad_global_score " << (global_ratio.second>0.0 ? (1-(global_ratio.first/global_ratio.second)) : 0.0) << "\n";
	}

	if(print_local)
	{
		typedef std::map< protein::AtomID, std::pair<double, double> > LocalRatiosMap;
		LocalRatiosMap local_ratios;
		for(CombinedContactsMap::const_iterator it=combined_contacts.begin();it!=combined_contacts.end();++it)
		{
			const double t=it->second.first;
			const double m=it->second.second;
			std::pair<double, double>& local_ratio=local_ratios[it->first.a];
			local_ratio.first+=std::min(fabs(t-m), t);
			local_ratio.second+=t;
		}
		auxiliaries::print_file_header(std::cout, "inter_atom_cad_local_scores");
		std::cout << local_ratios.size() << "\n";
		for(LocalRatiosMap::const_iterator it=local_ratios.begin();it!=local_ratios.end();++it)
		{
			const std::pair<double, double>& local_ratio=it->second;
			std::cout << it->first << " " << (local_ratio.second>0.0 ? (1-(local_ratio.first/local_ratio.second)) : 0.0) << "\n";
		}
	}
}
