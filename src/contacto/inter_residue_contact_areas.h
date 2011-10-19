#ifndef INTER_RESIDUE_CONTACT_AREAS_H_
#define INTER_RESIDUE_CONTACT_AREAS_H_

#include <map>
#include <string>
#include <iostream>

namespace contacto
{

struct InterResidueContactAreas
{
	typedef std::map<std::string, double> AreasMap;

	AreasMap areas;

	InterResidueContactAreas()
	{
	}

	friend std::ostream& operator<<(std::ostream& output, const InterResidueContactAreas& contact)
	{
		output << contact.areas.size() << "\n";
		for(AreasMap::const_iterator it=contact.areas.begin();it!=contact.areas.end();++it)
		{
			output << it->first << " " << it->second << "\n";
		}
		return output;
	}

	friend std::istream& operator>>(std::istream& input, InterResidueContactAreas& contact)
	{
		contact.areas.clear();
		std::size_t n=0;
		input >> n;
		for(std::size_t i=0;i<n;i++)
		{
			std::string key;
			double value=0;
			input >> key;
			input >> value;
			contact.areas[key]=value;
		}
		return input;
	}
};

}

#endif /* INTER_RESIDUE_CONTACT_AREAS_H_ */
