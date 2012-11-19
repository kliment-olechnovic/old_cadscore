#include <iostream>
#include <map>

#include "protein/pdb_parsing.h"

#include "apollo/points_basic_operations.h"

#include "auxiliaries/command_line_options.h"

struct FullResidueID
{
	std::string chain_name;
	int residue_sequence_number;
	std::string insertion_code;

	FullResidueID(const protein::PDBAtomRecord& record) :
		chain_name(record.chain_name),
		residue_sequence_number(record.residue_sequence_number),
		insertion_code(record.insertion_code)
	{
	}

	bool operator==(const FullResidueID& rid) const
	{
		return (residue_sequence_number==rid.residue_sequence_number && chain_name==rid.chain_name && insertion_code==rid.insertion_code);
	}

	bool operator< (const FullResidueID& rid) const
	{
		return (chain_name<rid.chain_name
				|| (chain_name==rid.chain_name && residue_sequence_number<rid.residue_sequence_number)
				|| (chain_name==rid.chain_name && residue_sequence_number==rid.residue_sequence_number && insertion_code<rid.insertion_code));
	}

	friend std::ostream& operator<<(std::ostream &output, const FullResidueID &rid)
	{
		output << (rid.chain_name.empty() ? std::string("?") : rid.chain_name) << " ";
		output << rid.residue_sequence_number << " ";
		output << (rid.insertion_code.empty() ? std::string("?") : rid.insertion_code) << " ";
		return output;
	}
};

struct RecordsPair
{
	const protein::PDBAtomRecord* left;
	const protein::PDBAtomRecord* right;

	RecordsPair() : left(0), right(0)
	{
	}
};

void print_topological_ordering_of_residues(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--bond-distance:");

	const double bond_distance=clo.isopt("--bond-distance") ? clo.arg<double>("--bond-distance") : 1.5;

	const std::vector<protein::PDBAtomRecord> records=protein::read_PDB_atom_records_from_PDB_file_stream(std::cin);

	std::map<FullResidueID, RecordsPair> nodes_map;
	for(std::size_t i=0;i<records.size();i++)
	{
		const protein::PDBAtomRecord& record=records[i];
		if((record.alternate_location_indicator.empty() || record.alternate_location_indicator=="A") && record.residue_name!="HOH")
		{
			if(record.name=="N")
			{
				nodes_map[FullResidueID(record)].left=&record;
			}
			else if(record.name=="C")
			{
				nodes_map[FullResidueID(record)].right=&record;
			}
		}
	}

	std::vector< std::pair<FullResidueID, RecordsPair> > nodes_vector;
	nodes_vector.reserve(nodes_map.size());
	for(std::map<FullResidueID, RecordsPair>::const_iterator it=nodes_map.begin();it!=nodes_map.end();++it)
	{
		if(it->second.left!=0 && it->second.right!=0)
		{
			nodes_vector.push_back(*it);
		}
	}

	std::vector< std::pair<int, int> > bonds(nodes_vector.size(), std::make_pair(-1, -1));
	for(std::size_t i=0;i<nodes_vector.size();i++)
	{
		bool found=false;
		for(std::size_t l=0;l<nodes_vector.size() && !found;l++)
		{
			const std::size_t j=(i+l<nodes_vector.size() ? (i+l) : (i+l-nodes_vector.size()));
			if(j!=i && apollo::distance_from_point_to_point(*(nodes_vector[i].second.right), *(nodes_vector[j].second.left))<bond_distance)
			{
				found=true;
				bonds[i].second=j;
				bonds[j].first=i;
			}
		}
	}

	std::vector<bool> visited(nodes_vector.size(), false);
	for(std::size_t i=0;i<bonds.size();i++)
	{
		if(bonds[i].first<0 && !visited[i])
		{
			std::cout << nodes_vector[i].first << "\n";
			visited[i]=true;
			int next=bonds[i].second;
			while(next>=0 && !visited[next])
			{
				std::cout << nodes_vector[next].first << "\n";
				visited[next]=true;
				next=bonds[next].second;
			}
			std::cout << "ter\n";
		}
	}
}
