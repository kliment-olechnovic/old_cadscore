#include <iostream>

#include "protein/dssp_parsing.h"

#include "auxiliaries/command_line_options.h"

void summarize_dssp_file(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode:");

	std::vector<protein::DSSPResidueRecord> dssp_records=protein::read_DSSP_atom_records_from_DSSP_file_stream(std::cin);

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
