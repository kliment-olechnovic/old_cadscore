#ifndef PROTEIN_PDBPARSING_H_
#define PROTEIN_PDBPARSING_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace protein
{

class PDBParsing
{
public:
	struct AtomRecord
	{
		std::string label;
		int atom_serial_number;
		std::string name;
		std::string alternate_location_indicator;
		std::string residue_name;
		std::string chain_name;
		int residue_sequence_number;
		std::string insertion_code;
		double x;
		double y;
		double z;

		AtomRecord(const std::string& PDB_file_line) :
			label(substring_of_PDB_file_line(PDB_file_line, 1, 6)),
			atom_serial_number(convert_string<int>(substring_of_PDB_file_line(PDB_file_line, 7, 11))),
			name(substring_of_PDB_file_line(PDB_file_line, 13, 16)),
			alternate_location_indicator(substring_of_PDB_file_line(PDB_file_line, 17, 17)),
			residue_name(substring_of_PDB_file_line(PDB_file_line, 18, 20)),
			chain_name(substring_of_PDB_file_line(PDB_file_line, 22, 22)),
			residue_sequence_number(convert_string<int>(substring_of_PDB_file_line(PDB_file_line, 23, 26))),
			insertion_code(substring_of_PDB_file_line(PDB_file_line, 27, 27)),
			x(convert_string<double>(substring_of_PDB_file_line(PDB_file_line, 31, 38))),
			y(convert_string<double>(substring_of_PDB_file_line(PDB_file_line, 39, 46))),
			z(convert_string<double>(substring_of_PDB_file_line(PDB_file_line, 47, 54)))
		{
			if (label.empty() || name.empty() || residue_name.empty())
			{
				throw std::runtime_error("Atom record has not enough string data");
			}

		}
	};

	static std::vector<AtomRecord> read_PDB_atom_records_from_PDB_file_stream(std::istream& pdb_file_stream)
	{
		std::vector<AtomRecord> records;
		while(pdb_file_stream.good())
		{
			std::string line;
			std::getline(pdb_file_stream, line);
			const std::string label=substring_of_PDB_file_line(line, 1, 6);
			if(label=="ATOM" || label=="HETATM")
			{
				try
				{
					const AtomRecord record(line);
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

private:
	PDBParsing()
	{
	}

	template<typename T>
	static T convert_string(const std::string& str)
	{
		std::istringstream input(str);
		input.exceptions(std::istringstream::failbit | std::istringstream::badbit);
		T value;
		input >> value;
		return value;
	}

	static std::string substring_of_PDB_file_line(const std::string& line, int start, int end)
	{
		std::string extraction;
		int line_length=static_cast<int>(line.size());
		for(int i=start-1;i<end && i<line_length && i>=0;i++)
		{
			if(line[i]!=32) { extraction.push_back(line[i]); }
		}
		return extraction;
	}
};

}

#endif /* PROTEIN_PDBPARSING_H_ */
