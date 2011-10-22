#ifndef CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_
#define CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_

#include <map>
#include <stdexcept>

#include "inter_residue_contact_id.h"
#include "inter_residue_contact_areas.h"
#include "inter_residue_contact_dual_areas.h"
#include "utilities.h"

namespace contacto
{


template<typename ResidueID>
std::map< InterResidueContactID<ResidueID>, InterResidueContactDualAreas > combine_two_inter_residue_contact_maps(const std::map< InterResidueContactID<ResidueID>, InterResidueContactAreas >& contact_map_1, const std::map< InterResidueContactID<ResidueID>, InterResidueContactAreas >& contact_map_2)
{
	typedef std::map< InterResidueContactID<ResidueID>, std::pair<InterResidueContactAreas, InterResidueContactAreas> > Combination;
	const Combination combination=combine_two_maps(contact_map_1, contact_map_2);
	typedef std::map< InterResidueContactID<ResidueID>, InterResidueContactDualAreas > Result;
	Result result;
	typename Result::iterator prev=result.begin();
	for(typename Combination::const_iterator it=combination.begin();it!=combination.end();++it)
	{
		prev=result.insert(prev, std::make_pair(it->first, combine_two_contacts(it->second.first, it->second.second)));
	}
	return result;
}

InterResidueContactDualAreas combine_two_contacts(const InterResidueContactAreas& contact_1, const InterResidueContactAreas& contact_2)
{
	InterResidueContactDualAreas combination;
	combination.areas=combine_two_maps(contact_1.areas, contact_2.areas);
	return combination;
}

}

#endif /* CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_ */
