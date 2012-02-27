#ifndef RESIDUE_IDS_INTERVALS_H_
#define RESIDUE_IDS_INTERVALS_H_

#include <sstream>
#include <vector>
#include <limits>

#include "residue_id.h"

namespace protein
{

bool read_residue_ids_intervals(std::istream& input, std::vector< std::vector< std::pair<ResidueID, ResidueID> > >& result)
{
	std::vector< std::vector< std::pair<ResidueID, ResidueID> > > intervals;
	intervals.push_back(std::vector< std::pair<ResidueID, ResidueID> >());
	while(input.good())
	{
		std::string command;
		input >> command;
		if(command=="range")
		{
			ResidueID a;
			int b;
			input >> a >> b;
			if(!input.fail() && a.chain_id.size()==1 && a.residue_number<=b)
			{
				intervals.back().push_back(std::make_pair(a, ResidueID(a.chain_id, b)));
			}
			else
			{
				return false;
			}
		}
		else if(command=="single")
		{
			ResidueID a;
			input >> a;
			if(!input.fail() && a.chain_id.size()==1)
			{
				intervals.back().push_back(std::make_pair(a, a));
			}
			else
			{
				return false;
			}
		}
		else if(command=="chain")
		{
			std::string chain_name;
			input >> chain_name;
			if(chain_name.size()==1)
			{
				intervals.back().push_back(std::make_pair(ResidueID(chain_name, std::numeric_limits<int>::min()), ResidueID(chain_name, std::numeric_limits<int>::max())));
			}
			else
			{
				return false;
			}
		}
		else if(command=="with")
		{
			if(!intervals.back().empty())
			{
				intervals.push_back(std::vector< std::pair<ResidueID, ResidueID> >());
			}
		}
		else if(!command.empty())
		{
			return false;
		}
	}
	result.clear();
	for(std::size_t i=0;i<intervals.size();i++)
	{
		if(!intervals[i].empty())
		{
			result.push_back(intervals[i]);
		}
	}
	return (!result.empty());
}

bool read_residue_ids_intervals(const std::string input_str, std::vector< std::vector< std::pair<ResidueID, ResidueID> > >& result)
{
	std::istringstream input(input_str);
	return read_residue_ids_intervals(input, result);
}

}

#endif /* RESIDUE_IDS_INTERVALS_H_ */
