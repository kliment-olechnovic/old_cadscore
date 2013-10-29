#include <iostream>
#include <map>

#include "protein/pdb_parsing.h"

#include "auxiliaries/command_line_options.h"

void x_chop_chain_ends(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map<int, std::vector<std::size_t> > ChainResiduesMap;
	typedef std::map<std::string, ChainResiduesMap> ChainsMap;

	clo.check_allowed_options("--chunk-size:");

	const std::size_t chunk_size=clo.arg<std::size_t>("--chunk-size");

	const std::vector<protein::PDBAtomRecord> records=protein::read_PDB_atom_records_from_PDB_file_stream(std::cin);

	ChainsMap chains_map;

	for(std::size_t i=0;i<records.size();i++)
	{
		if(records[i].label=="ATOM")
		{
			chains_map[records[i].chain_name][records[i].residue_sequence_number].push_back(i);
		}
	}

	for(ChainsMap::const_iterator it=chains_map.begin();it!=chains_map.end();++it)
	{
		const ChainResiduesMap& chain_residues_map=it->second;
		if(chunk_size<chain_residues_map.size())
		{
			std::size_t num=0;
			for(ChainResiduesMap::const_iterator jt=chain_residues_map.begin();jt!=chain_residues_map.end();++jt)
			{
				if((num>=chunk_size) && ((num+chunk_size)<chain_residues_map.size()))
				{
					const std::vector<std::size_t>& records_ids=jt->second;
					for(std::size_t i=0;i<records_ids.size();i++)
					{
						std::cout << records[records_ids[i]].generate_PDB_file_line() << "\n";
					}
				}
				num++;
			}
		}
	}

	std::cout << "END\n";
}
