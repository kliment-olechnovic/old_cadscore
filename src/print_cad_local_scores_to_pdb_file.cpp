#include <iostream>

#include "protein/residue_id.h"
#include "protein/pdb_parsing.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

void print_cad_local_scores_to_pdb_file(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode:");

	auxiliaries::assert_file_header(std::cin, "local_scores");
	const std::map<protein::ResidueID, double> local_scores=auxiliaries::read_map<protein::ResidueID, double>();

	std::vector<protein::PDBParsing::AtomRecord> atom_records=protein::PDBParsing::read_PDB_atom_records_from_PDB_file_stream(std::cin);

	if(!local_scores.empty() && !atom_records.empty())
	{
		std::cout << "MODEL        1\n";
		for(std::size_t i=0;i<atom_records.size();i++)
		{
			protein::PDBParsing::AtomRecord atom_record=atom_records[i];
			std::map<protein::ResidueID, double>::const_iterator it=local_scores.find(protein::ResidueID(atom_record.chain_name, atom_record.residue_sequence_number));
			if(it==local_scores.end() || it->second<0)
			{
				atom_record.temperature_factor=0;
			}
			else
			{
				atom_record.temperature_factor=std::min(it->second*100.0, 100.0);
			}
			std::cout << atom_record.generate_PDB_file_line() << "\n";

			if(i+1==atom_records.size() || (i+1<atom_records.size() && atom_records[i+1].chain_name!=atom_record.chain_name))
			{
				std::cout << atom_record.generate_TER_PDB_file_line() << "\n";
			}
		}
		std::cout << "ENDMDL\n";
		std::cout << "END   \n";
	}
}
