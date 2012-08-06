#include <iostream>

#include "protein/dssp_parsing.h"

#include "auxiliaries/command_line_options.h"

inline bool is_in_helix(const protein::DSSPResidueRecord& dr)
{
	return (dr.secondary_structure_summary=="H" || dr.secondary_structure_summary=="G" || dr.secondary_structure_summary=="I");
}

inline bool is_in_strand(const protein::DSSPResidueRecord& dr)
{
	return (dr.secondary_structure_summary=="B" || dr.secondary_structure_summary=="E");
}

void summarize_dssp_file(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--print-sequence --print-sequence-colored");

	std::vector<protein::DSSPResidueRecord> dssp_records=protein::read_DSSP_atom_records_from_DSSP_file_stream(std::cin);

	if(clo.isopt("--print-sequence"))
	{
		for(std::size_t i=0;i<dssp_records.size();i++)
		{
			std::cout << dssp_records[i].short_residue_name;
		}
		std::cout << "\n";
		for(std::size_t i=0;i<dssp_records.size();i++)
		{
			std::cout << dssp_records[i].secondary_structure_summary;
		}
		std::cout << "\n";
	}
	else if(clo.isopt("--print-sequence-colored"))
	{
		for(std::size_t i=0;i<dssp_records.size();i++)
		{
			const protein::DSSPResidueRecord& dr=dssp_records[i];
			std::cout << "\033[30;";
			if(is_in_helix(dr))
			{
				std::cout << "41m";
			}
			else if(is_in_strand(dr))
			{
				std::cout << "42m";
			}
			else
			{
				std::cout << "47m";
			}
			std::cout << dr.short_residue_name;
			std::cout << "\033[0m";
		}
		std::cout << "\n";
	}
	else
	{
		std::size_t count_of_residues_in_helix=0;
		std::size_t count_of_residues_in_strand=0;
		for(std::size_t i=0;i<dssp_records.size();i++)
		{
			const protein::DSSPResidueRecord& dr=dssp_records[i];
			if(is_in_helix(dr))
			{
				count_of_residues_in_helix++;
			}
			else if(is_in_strand(dr))
			{
				count_of_residues_in_strand++;
			}
		}
		std::cout << "all_residues_count " << dssp_records.size() << "\n";
		std::cout << "helix_residues_count " << count_of_residues_in_helix << "\n";
		std::cout << "strand_residues_count " << count_of_residues_in_strand << "\n";
	}
}
