#ifndef CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_
#define CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_

#include <map>
#include <stdexcept>

#include "inter_residue_contact_id.h"
#include "inter_residue_contact_areas.h"
#include "inter_residue_contact_dual_areas.h"

namespace contacto
{

class InterResidueContactsCombination
{
public:
	template<typename ResidueID>
	static std::map< InterResidueContactID<ResidueID>, InterResidueContactDualAreas > combine_two_inter_residue_contact_maps(const std::map< InterResidueContactID<ResidueID>, InterResidueContactAreas >& contact_map_1, const std::map< InterResidueContactID<ResidueID>, InterResidueContactAreas >& contact_map_2)
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

private:
	InterResidueContactsCombination()
	{
	}

	template<typename Key, typename Value1, typename Value2>
	static std::map< Key, std::pair<Value1, Value2> > combine_two_maps(const std::map<Key, Value1>& map_1, const std::map<Key, Value2>& map_2)
	{
		typedef std::map< Key, std::pair<Value1, Value2> > CombinedMap;
		CombinedMap combined_map;
		typename std::map<Key, Value1>::const_iterator it_1=map_1.begin();
		typename std::map<Key, Value2>::const_iterator it_2=map_2.begin();
		typename CombinedMap::iterator prev=combined_map.begin();
		while(it_1!=map_1.end() || it_2!=map_2.end())
		{
			if(it_1!=map_1.end() && it_2!=map_2.end() && it_1->first==it_2->first)
			{
				prev=combined_map.insert(prev, std::make_pair(it_1->first, std::make_pair(it_1->second, it_2->second)));
				++it_1;
				++it_2;
			}
			else if(it_1!=map_1.end() && (it_2==map_2.end() || it_1->first<it_2->first))
			{
				prev=combined_map.insert(prev, std::make_pair(it_1->first, std::make_pair(it_1->second, Value2())));
				++it_1;
			}
			else if(it_2!=map_2.end() && (it_1==map_1.end() || it_2->first<it_1->first))
			{
				prev=combined_map.insert(prev, std::make_pair(it_2->first, std::make_pair(Value1(), it_2->second)));
				++it_2;
			}
			else
			{
				throw std::logic_error("Two maps iteration error");
			}
		}
		return combined_map;
	}

	static InterResidueContactDualAreas combine_two_contacts(const InterResidueContactAreas& contact_1, const InterResidueContactAreas& contact_2)
	{
		InterResidueContactDualAreas combination;
		combination.areas=combine_two_maps(contact_1.areas, contact_2.areas);
		return combination;
	}
};

}

#endif /* CONTACTO_INTER_RESIDUE_CONTACTS_COMBINATION_H_ */
