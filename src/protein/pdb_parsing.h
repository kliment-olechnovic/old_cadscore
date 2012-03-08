#ifndef PROTEIN_PDBPARSING_H_
#define PROTEIN_PDBPARSING_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
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
		double temperature_factor;

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
			z(convert_string<double>(substring_of_PDB_file_line(PDB_file_line, 47, 54))),
			temperature_factor(safe_convert_string<double>(substring_of_PDB_file_line(PDB_file_line, 61, 66), 0))
		{
			if (label.empty() || name.empty() || residue_name.empty())
			{
				throw std::runtime_error("Atom record has not enough string data");
			}
			if(chain_name.empty())
			{
				chain_name="?";
			}
		}

		std::string generate_PDB_file_line() const
		{
			std::string line(80, ' ');
			insert_string_to_PDB_file_line(label, 1, 6, false, line);
			insert_string_to_PDB_file_line(convert_int_to_string(atom_serial_number), 7, 11, true, line);
			insert_string_to_PDB_file_line(name, 13, 16, false, line);
			insert_string_to_PDB_file_line(alternate_location_indicator, 17, 17, false, line);
			insert_string_to_PDB_file_line(residue_name, 18, 20, false, line);
			insert_string_to_PDB_file_line((chain_name=="?" ? std::string(" ") : chain_name), 22, 22, false, line);
			insert_string_to_PDB_file_line(convert_int_to_string(residue_sequence_number), 23, 26, true, line);
			insert_string_to_PDB_file_line(insertion_code, 27, 27, false, line);
			insert_string_to_PDB_file_line(convert_double_to_string(x, 3), 31, 38, true, line);
			insert_string_to_PDB_file_line(convert_double_to_string(y, 3), 39, 46, true, line);
			insert_string_to_PDB_file_line(convert_double_to_string(z, 3), 47, 54, true, line);
			insert_string_to_PDB_file_line(convert_double_to_string(temperature_factor, 2), 61, 66, true, line);
			return line;
		}

		std::string generate_TER_PDB_file_line() const
		{
			std::string line(80, ' ');
			insert_string_to_PDB_file_line("TER", 1, 6, false, line);
			insert_string_to_PDB_file_line(convert_int_to_string(atom_serial_number+1), 7, 11, true, line);
			insert_string_to_PDB_file_line(residue_name, 18, 20, false, line);
			insert_string_to_PDB_file_line((chain_name=="?" ? std::string(" ") : chain_name), 22, 22, false, line);
			insert_string_to_PDB_file_line(convert_int_to_string(residue_sequence_number), 23, 26, true, line);
			return line;
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

	template<typename T>
	static T safe_convert_string(const std::string& str, const T default_value)
	{
		try
		{
			return convert_string<T>(str);
		}
		catch(const std::exception& e)
		{
			return default_value;
		}
	}

	static std::string substring_of_PDB_file_line(const std::string& line, const int start, const int end)
	{
		std::string extraction;
		int line_length=static_cast<int>(line.size());
		for(int i=start-1;i<end && i<line_length && i>=0;i++)
		{
			if(line[i]!=32) { extraction.push_back(line[i]); }
		}
		return extraction;
	}

	static std::string convert_int_to_string(const int value)
	{
		std::ostringstream output;
		output << value;
		return output.str();
	}

	static std::string convert_double_to_string(const double value, const int precision)
	{
		std::ostringstream output;
		output << std::fixed << std::setprecision(precision) << value;
		return output.str();
	}

	static bool insert_string_to_PDB_file_line(const std::string& str, const std::size_t start, const std::size_t end, const bool shift_right, std::string& line)
	{
		if(!str.empty() && start>=1 && start<=end && end<=line.size())
		{
			const std::size_t interval_length=(end-start)+1;
			if(str.size()<=interval_length)
			{
				const std::string addition(interval_length-str.size(), ' ');
				line.replace(start-1, interval_length, (shift_right ? addition+str : str+addition));
			}
		}
		return false;
	}
};

}

#endif /* PROTEIN_PDBPARSING_H_ */
