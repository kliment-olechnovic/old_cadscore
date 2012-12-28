#include <iostream>

#include "protein/atom.h"
#include "protein/residue_ids_collection.h"
#include "protein/pairwise_sequence_alignment.h"
#include "protein/letters_coding.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void collect_residue_sequence(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--chain-separator:");

	const std::string chain_separator=clo.arg_or_default_value<std::string>("--chain-separator", "");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=protein::collect_residue_ids_from_atoms(atoms);

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
	const std::string result=output.str();

	if(result.empty())
	{
		throw std::runtime_error("No residue sequence was collected from the provided atoms stream");
	}
	else
	{
		std::cout << result << "\n";
	}
}

void construct_primitive_global_sequence_alignment(const auxiliaries::CommandLineOptions& clo)
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
