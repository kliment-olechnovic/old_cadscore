#include <iostream>
#include <map>
#include <cmath>

#include "protein/atom.h"
#include "protein/residue_id.h"

#include "contacto/contact_id.h"
#include "contacto/inter_atom_contact.h"
#include "contacto/utilities.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

template<typename AtomID>
inline AtomID atom_id_from_atom(const protein::Atom& a)
{
	return AtomID(protein::ResidueID::from_atom(a), a.atom_name);
}

template<typename AtomID>
std::map<contacto::ContactID<AtomID>, double> construct_inter_atom_contacts_map(const std::vector<protein::Atom>& atoms, const std::vector<contacto::InterAtomContact>& inter_atom_contacts)
{
	std::map<contacto::ContactID<AtomID>, double> contacts_map;
	for(std::size_t i=0;i<inter_atom_contacts.size();i++)
	{
		const contacto::InterAtomContact& contact=inter_atom_contacts[i];
		if(contact.a!=contact.b)
		{
			contacts_map[contacto::ContactID<AtomID>(atom_id_from_atom<AtomID>(atoms[contact.a]), atom_id_from_atom<AtomID>(atoms[contact.b]))]=contact.area;
		}
	}
	return contacts_map;
}

void calc_inter_atom_contact_area_difference_score(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	typedef std::pair<protein::ResidueID, std::string> AtomID;
	typedef std::map< contacto::ContactID<AtomID>, std::pair<double, double> > CombinedContactsMap;

	const CombinedContactsMap combined_contacts=contacto::combine_two_maps(
			construct_inter_atom_contacts_map<AtomID>(atoms_1, inter_atom_contacts_1),
			construct_inter_atom_contacts_map<AtomID>(atoms_2, inter_atom_contacts_2));

	double difference=0.0;
	double reference=0.0;
	for(CombinedContactsMap::const_iterator it=combined_contacts.begin();it!=combined_contacts.end();++it)
	{
		const double t=it->second.first;
		const double m=it->second.second;
		difference+=std::min(fabs(t-m), t);
		reference+=t;
	}

	std::cout << "inter_atom_cad_score " << (reference>0.0 ? (1-(difference/reference)) : 0.0) << "\n";
}
