#ifndef INTER_RESIDUE_CONTACTS_H_
#define INTER_RESIDUE_CONTACTS_H_

#include <vector>
#include <map>

#include "inter_atom_contact.h"
#include "contact_classification.h"

namespace contacto
{

template<typename Atom, typename ResidueID>
std::map< std::pair<ResidueID, ResidueID>, std::map<std::string, double> > collect_contacts_between_residues(const std::vector<Atom>& atoms, const std::vector<InterAtomContact>& contacts)
{
	std::map< std::pair<ResidueID, ResidueID>, std::map<std::string, double> > contacts_between_residues;
	for(std::size_t i=0;i<contacts.size();i++)
	{
		const InterAtomContact& contact=contacts[i];
		const Atom& a1=atoms[contact.a];
		const Atom& a2=atoms[contact.b];
		const std::vector<std::string> contact_classes=ContactClassification::classify_atoms_contact<Atom, ResidueID>(a1, a2);
		if(!contact_classes.empty())
		{
			std::map<std::string, double>& map_by_classes=contacts_between_residues[std::make_pair(ResidueID::from_atom(a1), ResidueID::from_atom(a2))];
			for(std::size_t j=0;j<contact_classes.size();j++)
			{
				map_by_classes[contact_classes[j]]+=contact.area;
			}
		}
	}
	return contacts_between_residues;
}

}

#endif /* INTER_RESIDUE_CONTACTS_H_ */
