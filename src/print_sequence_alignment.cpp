#include <iostream>
#include <algorithm>
#include <iomanip>

#include "protein/pairwise_sequence_alignment.h"
#include "protein/residue_id.h"
#include "protein/residue_summary.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"

void print_sequence_alignment(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	std::string seq1;
	std::string seq2;

	std::getline(std::cin, seq1);
	std::getline(std::cin, seq2);

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
