#include <iostream>
#include <algorithm>
#include <iomanip>

#include "protein/pairwise_sequence_alignment.h"
#include "protein/residue_id.h"
#include "protein/residue_summary.h"
#include "protein/letters_coding.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"

std::string read_sequence_from_residue_ids()
{
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "residue identifiers", "residue_ids", false);
	std::ostringstream output;
	for(std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator it=residue_ids.begin();it!=residue_ids.end();++it)
	{
		output << protein::convert_residue_codes_big_to_small(it->second.name);
		std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator next_it=it;
		++next_it;
		if(next_it!=residue_ids.end() && next_it->first.chain_id!=it->first.chain_id)
		{
			output << "=";
		}
	}
	return output.str();
}

void print_sequence_alignment(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--read-residue-ids");

	std::string seq1;
	std::string seq2;

	if(clo.isopt("--read-residue-ids"))
	{
		seq1=read_sequence_from_residue_ids();
		seq2=read_sequence_from_residue_ids();
	}
	else
	{
		std::getline(std::cin, seq1);
		std::getline(std::cin, seq2);
	}

	if(seq1.empty() || seq2.empty())
	{
		throw std::runtime_error("Two lines not found in standard input");
	}
	else
	{
		const protein::PairwiseSequenceAlignment::SimpleScorer scorer(2, -1, -1, -1);
		protein::PairwiseSequenceAlignment::print_global_alignment(seq1, seq2, scorer, std::cout);
	}
}
