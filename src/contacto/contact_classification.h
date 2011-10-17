#ifndef CONTACT_CLASSIFICATION_H_
#define CONTACT_CLASSIFICATION_H_

#include <string>
#include <set>
#include <vector>

#include "../protein/atom.h"
#include "../protein/residue_id.h"

namespace protein
{

inline std::set<std::string> construct_main_chain_atom_names_set()
{
	std::set<std::string> names;

	names.insert("C");
	names.insert("CA");
	names.insert("C0");
	names.insert("N");
	names.insert("O");
	names.insert("OXT");

	names.insert("0P3");
	names.insert("O3P");
	names.insert("P");
	names.insert("OP1");
	names.insert("O1P");
	names.insert("OP2");
	names.insert("O2P");
	names.insert("O5'");
	names.insert("O5*");
	names.insert("C5'");
	names.insert("C5*");
	names.insert("C4'");
	names.insert("C4*");
	names.insert("04'");
	names.insert("04*");
	names.insert("C3'");
	names.insert("C3*");
	names.insert("03'");
	names.insert("03*");
	names.insert("C2'");
	names.insert("C2*");
	names.insert("O2'");
	names.insert("O2*");
	names.insert("C1'");
	names.insert("C1*");

	return names;
}

inline bool check_if_atom_belongs_to_main_chain_by_name(const std::string& name)
{
	static const std::set<std::string> names(construct_main_chain_atom_names_set());
	return (names.find(name)!=names.end());
}

inline std::vector<std::string> classify_atom_by_name(const std::string& name)
{
	std::vector<std::string> classes;
	classes.push_back(check_if_atom_belongs_to_main_chain_by_name(name) ? "M" : "S");
	classes.push_back("A");
	return classes;
}

inline std::vector<std::string> classify_atoms_contact_by_names(const std::string& a_name, const std::string& b_name)
{
	std::vector<std::string> classes;
	const std::vector<std::string> a_classes=classify_atom_by_name(a_name);
	const std::vector<std::string> b_classes=classify_atom_by_name(b_name);
	for(std::size_t i=0;i<a_classes.size();i++)
	{
		for(std::size_t j=0;j<b_classes.size();j++)
		{
			classes.push_back(a_classes[i]+b_classes[j]);
		}
	}
	return classes;
}

std::vector<std::string> classify_atoms_contact(const Atom& a, const Atom& b)
{
	const bool same_object=(a==b);
	const bool same_residue=same_object || ResidueID(a.chain_id, a.residue_number)==ResidueID(b.chain_id, b.residue_number);

	if(same_object)
	{
		std::vector<std::string> atom_classes=classify_atom_by_name(a.atom_name);
		for(std::size_t i=0;i<atom_classes.size();i++)
		{
			atom_classes[i]+="W";
		}
		return atom_classes;
	}
	else if(!same_residue)
	{
		return classify_atoms_contact_by_names(a.atom_name, b.atom_name);
	}
	return std::vector<std::string>();
}

}

#endif /* CONTACT_CLASSIFICATION_H_ */
