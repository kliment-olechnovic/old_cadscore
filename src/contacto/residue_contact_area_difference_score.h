#ifndef CONTACTO_RESIDUE_CONTACT_AREA_DIFFERENCE_SCORE_H_
#define CONTACTO_RESIDUE_CONTACT_AREA_DIFFERENCE_SCORE_H_

#include <map>
#include <string>
#include <iostream>

namespace contacto
{

struct ResidueContactAreaDifferenceScore
{
	struct Ratio
	{
		double difference;
		double reference;

		Ratio() : difference(0), reference(0)
		{
		}

		friend std::ostream& operator<<(std::ostream &output, const Ratio &ratio)
		{
			output << ratio.difference << " ";
			output << ratio.reference;
			return output;
		}

		friend std::istream& operator>>(std::istream &input, Ratio &ratio)
		{
			input >> ratio.difference;
			input >> ratio.reference;
			return input;
		}
	};

	typedef std::map<std::string, Ratio> RatiosMap;

	RatiosMap ratios;

	ResidueContactAreaDifferenceScore()
	{
	}

	Ratio ratio(const std::string& area_class) const
	{
		RatiosMap::const_iterator it=ratios.find(area_class);
		return (it!=ratios.end() ? it->second : Ratio());
	}

	friend std::ostream& operator<<(std::ostream& output, const ResidueContactAreaDifferenceScore& score)
	{
		output << score.ratios.size() << "\n";
		for(RatiosMap::const_iterator it=score.ratios.begin();it!=score.ratios.end();++it)
		{
			output << it->first << " " << it->second << "\n";
		}
		return output;
	}

	friend std::istream& operator>>(std::istream& input, ResidueContactAreaDifferenceScore& score)
	{
		score.ratios.clear();
		std::size_t n=0;
		input >> n;
		for(std::size_t i=0;i<n;i++)
		{
			std::string key;
			input >> key;
			Ratio value;
			input >> value;
			score.ratios[key]=value;
		}
		return input;
	}
};

}

#endif /* CONTACTO_RESIDUE_CONTACT_AREA_DIFFERENCE_SCORE_H_ */
