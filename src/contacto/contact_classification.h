#ifndef CONTACTO_CONTACT_CLASSIFICATION_H_
#define CONTACTO_CONTACT_CLASSIFICATION_H_

#include <string>
#include <set>
#include <vector>
#include <cstdlib>

namespace contacto
{

class ContactClassification
{
public:
	template<typename AtomType, typename ResidueIDType>
	static std::vector<std::string> classify_atoms_contact(const AtomType& a, const AtomType& b)
	{
		if(a==b)
		{
			std::vector<std::string> atom_classes=classify_atom_by_name(a.atom_name);
			for(std::size_t i=0;i<atom_classes.size();i++)
			{
				atom_classes[i]+="W";
			}
			return atom_classes;
		}
		else
		{
			if(!(ResidueIDType::from_atom(a)==ResidueIDType::from_atom(b))
					&& !check_if_atoms_contact_is_covalent(a, b))
			{
				return classify_atoms_contact_by_names(a.atom_name, b.atom_name);
			}
		}
		return std::vector<std::string>();
	}

	static std::vector<std::string> get_all_classes_list()
	{
		std::vector<std::string> all_classes;
		std::string c1[3]={"A", "M", "S"};
		std::string c2[4]={"A", "M", "S", "W"};
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<4;j++)
			{
				all_classes.push_back(c1[i]+c2[j]);
			}
		}
		return all_classes;
	}

private:
	ContactClassification()
	{
	}

	static std::set<std::string> construct_main_chain_atom_names_set()
	{
		std::set<std::string> names;

		names.insert("C");
		names.insert("CA");
		names.insert("CO");//TODO check
		names.insert("N");
		names.insert("O");
		names.insert("OXT");

		names.insert("OP3");//TODO check
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
		names.insert("O4'");//TODO check
		names.insert("O4*");//TODO check
		names.insert("C3'");
		names.insert("C3*");
		names.insert("O3'");//TODO check
		names.insert("O3*");//TODO check
		names.insert("C2'");
		names.insert("C2*");
		names.insert("O2'");
		names.insert("O2*");
		names.insert("C1'");
		names.insert("C1*");

		return names;
	}

	static bool check_if_atom_belongs_to_main_chain_by_name(const std::string& name)
	{
		static const std::set<std::string> names(construct_main_chain_atom_names_set());
		return (names.find(name)!=names.end());
	}

	static std::vector<std::string> classify_atom_by_name(const std::string& name)
	{
		std::vector<std::string> classes;
		classes.push_back(check_if_atom_belongs_to_main_chain_by_name(name) ? "M" : "S");
		classes.push_back("A");
		return classes;
	}

	static std::vector<std::string> classify_atoms_contact_by_names(const std::string& a_name, const std::string& b_name)
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

	template<typename AtomType>
	static bool check_if_atoms_contact_is_covalent(const AtomType& a, const AtomType& b)
	{
		if(abs(a.residue_number-b.residue_number)==1 && a.chain_id==b.chain_id)
		{
			//TODO add stuff for RNA
			if((a.atom_name=="C" && b.atom_name=="N") || (a.atom_name=="N" && b.atom_name=="C"))
			{
				return true;
			}
		}
		return false;
	}
};

}

#endif /* CONTACTO_CONTACT_CLASSIFICATION_H_ */
