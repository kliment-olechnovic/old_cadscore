#include <iostream>
#include <sstream>

#include "protein/residue_id.h"
#include "protein/residue_summary.h"
#include "protein/pairwise_sequence_alignment.h"
#include "protein/letters_coding.h"
#include "protein/atom.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/vector_io.h"

std::string get_sequence_string_from_residue_ids(const std::map<protein::ResidueID, protein::ResidueSummary>& residue_ids, const std::string& chain_separator)
{
	std::ostringstream output;
	for(std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator it=residue_ids.begin();it!=residue_ids.end();++it)
	{
		output << protein::convert_residue_codes_big_to_small(it->second.name);
		std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator next_it=it;
		++next_it;
		if(next_it!=residue_ids.end() && next_it->first.chain_id!=it->first.chain_id)
		{
			output << chain_separator;
		}
	}
	return output.str();
}

void collect_residue_sequence(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--chain-separator:");

	const std::string chain_separator=clo.arg_or_default_value<std::string>("--chain-separator", "");

	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "residue identifiers", "residue_ids", false);

	const std::string result=get_sequence_string_from_residue_ids(residue_ids, chain_separator);

	if(result.empty())
	{
		throw std::runtime_error("No residue sequence was collected from the provided atoms stream");
	}
	else
	{
		std::cout << result << "\n";
	}
}

void construct_global_sequence_alignment(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--match: --mismatch: --gap-start: --gap-extension:");

	const int match_score=clo.arg_or_default_value<int>("--match", 0);
	const int mismatch_score=clo.arg_or_default_value<int>("--mismatch", -10);
	const int gap_start_score=clo.arg_or_default_value<int>("--gap-start", -11);
	const int gap_extension_score=clo.arg_or_default_value<int>("--gap-extension", -1);

	std::string seq1;
	std::string seq2;
	std::getline(std::cin, seq1);
	std::getline(std::cin, seq2);

	if(seq1.empty() || seq2.empty())
	{
		throw std::runtime_error("Two sequences not found in standard input");
	}
	else
	{
		const protein::PairwiseSequenceAlignment::SimpleScorer scorer(match_score, mismatch_score, gap_start_score, gap_extension_score);
		protein::PairwiseSequenceAlignment::print_global_alignment(seq1, seq2, scorer, std::cout);
	}
}

void construct_residue_renumbering_map(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map<protein::ResidueID, protein::ResidueSummary> MapOfResidueIDs;

	clo.check_allowed_options("");

	const MapOfResidueIDs residue_ids_1=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "target residue identifiers", "residue_ids", false);
	const MapOfResidueIDs residue_ids_2=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "model residue identifiers", "residue_ids", false);

	std::string alignment_seq1;
	std::string alignment_seq2;
	std::cin >> alignment_seq1 >> alignment_seq2;

	if(alignment_seq1.empty() || alignment_seq2.empty())
	{
		throw std::runtime_error("Two sequences not found in standard input");
	}

	if(alignment_seq1.size()!=alignment_seq2.size())
	{
		throw std::runtime_error("Two sequences differ in size");
	}

	const std::vector< std::pair<int, int> > alignment=protein::PairwiseSequenceAlignment::read_global_alignment(alignment_seq1, alignment_seq2);

	if(alignment.size()!=alignment_seq1.size())
	{
		throw std::runtime_error("Invalid alignment provided");
	}

	std::map<protein::ResidueID, protein::ResidueID> renumbering_map;

	std::size_t i=0;
	MapOfResidueIDs::const_iterator it1=residue_ids_1.begin();
	MapOfResidueIDs::const_iterator it2=residue_ids_2.begin();

	while(i<alignment.size() && it1!=residue_ids_1.end() && it2!=residue_ids_2.end())
	{
		const std::pair<int, int>& matching=alignment[i];

		if((matching.first>=0 && protein::convert_residue_codes_big_to_small(it1->second.name)!=alignment_seq1.substr(i, 1))
				|| (matching.second>=0 && protein::convert_residue_codes_big_to_small(it2->second.name)!=alignment_seq2.substr(i, 1)))
		{
			throw std::runtime_error("Alignment does not correspond to provided residue identifiers");
		}

		if(matching.first>=0 && matching.second>=0)
		{
			renumbering_map[it2->first]=it1->first;
		}

		if(matching.first>=0)
		{
			++it1;
		}

		if(matching.second>=0)
		{
			++it2;
		}

		i++;
	}

	auxiliaries::print_map<protein::ResidueID, protein::ResidueID>(std::cout, "residue_renumbering_map", renumbering_map, false);
}

void renumber_residues(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--remove-not-renumbered");

	const bool remove_not_renumbered=clo.isopt("--remove-not-renumbered");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map<protein::ResidueID, protein::ResidueID> renumbering_map=auxiliaries::read_map<protein::ResidueID, protein::ResidueID>(std::cin, "residue renumbering map", "residue_renumbering_map", false);

	std::vector<protein::Atom> result;
	result.reserve(atoms.size());

	for(std::size_t i=0;i<atoms.size();i++)
	{
		const protein::Atom& atom=atoms[i];
		std::map<protein::ResidueID, protein::ResidueID>::const_iterator it=renumbering_map.find(protein::ResidueID::from_atom(atom));
		if(it!=renumbering_map.end())
		{
			result.push_back(atom);
			protein::Atom& result_atom=result.back();
			result_atom.chain_id=it->second.chain_id;
			result_atom.residue_number=it->second.residue_number;
		}
		else
		{
			if(!remove_not_renumbered)
			{
				result.push_back(atom);
			}
		}
	}

	if(result.empty())
	{
		throw std::runtime_error("No atoms for the provided renumbering map");
	}
	else
	{
		auxiliaries::print_vector(std::cout, "atoms", result);
	}
}