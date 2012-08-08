#include <iostream>

#include "protein/pdb_parsing.h"
#include "protein/residue_ids_intervals.h"

#include "auxiliaries/command_line_options.h"

void reassign_chain_names_by_residue_intervals(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--intervals:");

	const std::string intervals_string=clo.arg<std::string>("--intervals");

	std::vector< std::vector< std::pair<protein::ResidueID, protein::ResidueID> > > intervals;
	if(!protein::ResidueIDsIntervalsReader::read_residue_ids_intervals(intervals_string, intervals) || intervals.size()<2)
	{
		throw std::runtime_error(std::string("Invalid intervals string: ")+intervals_string);
	}

	const std::vector<protein::PDBAtomRecord> pdb_atom_records_records=protein::read_PDB_atom_records_from_PDB_file_stream(std::cin);

	for(std::size_t u=0;u<pdb_atom_records_records.size();u++)
	{
		protein::PDBAtomRecord record=pdb_atom_records_records[u];
		const protein::ResidueID rid(record.chain_name, record.residue_sequence_number);
		bool matched=false;
		for(std::size_t i=0;i<intervals.size() && !matched && i<26;i++)
		{
			for(std::size_t j=0;j<intervals[i].size() && !matched;j++)
			{
				const protein::ResidueID& r1=intervals[i][j].first;
				const protein::ResidueID& r2=intervals[i][j].second;
				matched=(rid.chain_id==r1.chain_id && rid.residue_number>=r1.residue_number && rid.residue_number<=r2.residue_number);
			}
			if(matched)
			{
				record.chain_name=std::string(1, 'A'+static_cast<char>(i));
			}
		}
		if(!matched)
		{
			record.chain_name="?";
		}
		std::cout << record.generate_PDB_file_line() << "\n";
	}
}
