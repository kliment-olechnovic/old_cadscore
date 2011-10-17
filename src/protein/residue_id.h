#ifndef RESIDUE_ID_H_
#define RESIDUE_ID_H_

#include <string>

namespace protein
{

struct ResidueID
{
	std::string chain_id;
	int residue_number;

	ResidueID() : residue_number(0)
	{
	}

	ResidueID(const std::string& chain_id, int residue_number) : chain_id(chain_id), residue_number(residue_number)
	{
	}

	bool operator==(const ResidueID& rid) const
	{
		return (residue_number==rid.residue_number && chain_id==rid.chain_id);
	}

	bool operator< (const ResidueID& rid) const
	{
		return (chain_id<rid.chain_id || (chain_id==rid.chain_id && residue_number<rid.residue_number));
	}
};

}

#endif /* RESIDUE_ID_H_ */
