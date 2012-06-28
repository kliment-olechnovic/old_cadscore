#ifndef PROTEIN_DSSP_PARSING_H_
#define PROTEIN_DSSP_PARSING_H_

#include <vector>

#include "basic_parsing.h"
#include "dssp_residue_record.h"

namespace protein
{

std::vector<DSSPResidueRecord> read_DSSP_atom_records_from_DSSP_file_stream(std::istream& dssp_file_stream)
{
	std::vector<DSSPResidueRecord> records;
	bool records_started=false;
	while(dssp_file_stream.good())
	{
		std::string line;
		std::getline(dssp_file_stream, line);
		if(!records_started)
		{
			if(line.find("  #  RESIDUE AA STRUCTURE")==0)
			{
				records_started=true;
			}
		}
		else
		{
			const std::string short_residue_name=basic_parsing::substring_of_columned_file_line(line, 14, 14);
			if(!short_residue_name.empty() && short_residue_name!="!")
			{
				try
				{
					const DSSPResidueRecord record(line);
					records.push_back(record);
				}
				catch(const std::exception& e)
				{
					std::cerr << "Invalid DSSP record in line: " << line << "\n";
				}
			}
		}
	}
	return records;
}

}

#endif /* PROTEIN_DSSP_PARSING_H_ */
