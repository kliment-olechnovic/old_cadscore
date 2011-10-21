#ifndef PROTEIN_RESIDUE_IDS_COLLECTION_H_
#define PROTEIN_RESIDUE_IDS_COLLECTION_H_

#include <vector>
#include <set>

#include "residue_id.h"

namespace protein
{

template<typename AtomType>
std::set<ResidueID> collect_residue_ids_from_atoms(const std::vector<AtomType>& atoms)
{
	std::set<ResidueID> result;
	std::set<ResidueID>::iterator prev=result.begin();
	for(std::size_t i=0;i<atoms.size();i++)
	{
		prev=result.insert(prev, ResidueID::from_atom(atoms[i]));
	}
	return result;
}

}

#endif /* PROTEIN_RESIDUE_IDS_COLLECTION_H_ */
