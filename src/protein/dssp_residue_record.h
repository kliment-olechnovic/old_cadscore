#ifndef PROTEIN_DSSP_RESIDUE_RECORD_H_
#define PROTEIN_DSSP_RESIDUE_RECORD_H_

#include "basic_parsing.h"

namespace protein
{

struct DSSPResidueRecord
{
	int residue_sequence_number;
	std::string insertion_code;
	std::string chain_name;
	std::string short_residue_name;
	std::string secondary_structure_summary;

	DSSPResidueRecord(const std::string& DSSP_file_line) :
		residue_sequence_number(basic_parsing::convert_string<int>(basic_parsing::substring_of_columned_file_line(DSSP_file_line, 6, 10))),
		insertion_code(basic_parsing::substring_of_columned_file_line(DSSP_file_line, 11, 11)),
		chain_name(basic_parsing::substring_of_columned_file_line(DSSP_file_line, 12, 12)),
		short_residue_name(basic_parsing::substring_of_columned_file_line(DSSP_file_line, 14, 14)),
		secondary_structure_summary(basic_parsing::substring_of_columned_file_line(DSSP_file_line, 17, 17))
	{
		if(chain_name.empty())
		{
			chain_name="?";
		}
		if(short_residue_name.empty())
		{
			secondary_structure_summary=" ";
		}
		if(secondary_structure_summary.empty())
		{
			secondary_structure_summary=" ";
		}
	}
};

}

#endif /* PROTEIN_DSSP_RESIDUE_RECORD_H_ */
