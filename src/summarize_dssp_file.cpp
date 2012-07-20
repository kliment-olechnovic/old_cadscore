#include <iostream>

#include "protein/dssp_parsing.h"

#include "auxiliaries/command_line_options.h"

void summarize_dssp_file(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --use-linux-console-colors");

	std::vector<protein::DSSPResidueRecord> dssp_records=protein::read_DSSP_atom_records_from_DSSP_file_stream(std::cin);

	if(clo.isopt("--use-linux-console-colors"))
	{
		for(std::size_t i=0;i<dssp_records.size();i++)
		{
			const protein::DSSPResidueRecord& dr=dssp_records[i];
			std::cout << "\033[30;";
			if(dr.secondary_structure_summary=="H"
					|| dr.secondary_structure_summary=="G"
					|| dr.secondary_structure_summary=="I")
			{
				std::cout << "41m";
			}
			else if(dr.secondary_structure_summary=="B"
					|| dr.secondary_structure_summary=="E")
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
}
