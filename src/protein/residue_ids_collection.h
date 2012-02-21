#ifndef PROTEIN_RESIDUE_IDS_COLLECTION_H_
#define PROTEIN_RESIDUE_IDS_COLLECTION_H_

#include <vector>
#include <map>
#include <string>

#include "residue_id.h"

namespace protein
{

template<typename AtomType>
std::map<ResidueID, std::string> collect_residue_ids_from_atoms(const std::vector<AtomType>& atoms)
{
	std::map<ResidueID, std::string> result;
	std::map<ResidueID, std::string>::iterator prev=result.begin();
	for(std::size_t i=0;i<atoms.size();i++)
	{
		prev=result.insert(prev, std::make_pair(ResidueID::from_atom(atoms[i]), atoms[i].residue_name));
	}
	return result;
}

template<typename AtomType>
std::map<ResidueID, std::vector<std::size_t> > group_atoms_indices_by_residue_ids(const std::vector<AtomType>& atoms)
{
	std::map<ResidueID, std::vector<std::size_t> > result;
	for(std::size_t i=0;i<atoms.size();i++)
	{
		result[ResidueID::from_atom(atoms[i])].push_back(i);
	}
	return result;
}

}

#endif /* PROTEIN_RESIDUE_IDS_COLLECTION_H_ */
