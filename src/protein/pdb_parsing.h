#ifndef PROTEIN_PDBPARSING_H_
#define PROTEIN_PDBPARSING_H_

#include <vector>

#include "basic_parsing.h"
#include "pdb_atom_record.h"

namespace protein
{

std::vector<PDBAtomRecord> read_PDB_atom_records_from_PDB_file_stream(std::istream& pdb_file_stream)
{
	std::vector<PDBAtomRecord> records;
	while(pdb_file_stream.good())
	{
		std::string line;
		std::getline(pdb_file_stream, line);
		const std::string label=basic_parsing::substring_of_columned_file_line(line, 1, 6);
		if(label=="ATOM" || label=="HETATM")
		{
			try
			{
				const PDBAtomRecord record(line);
				records.push_back(record);
			}
			catch(const std::exception& e)
			{
				std::cerr << "Invalid atom record in line: " << line << "\n";
			}
		}
		else if(label=="END" || label=="ENDMDL")
		{
			return records;
		}
	}
	return records;
}

}

#endif /* PROTEIN_PDBPARSING_H_ */
