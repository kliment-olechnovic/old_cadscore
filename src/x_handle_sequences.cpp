#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "protein/residue_id.h"
#include "protein/residue_summary.h"
#include "protein/letters_coding.h"
#include "protein/pairwise_sequence_alignment.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"

void x_collect_residue_sequence(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "residue identifiers", "residue_ids", false);
	for(std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator it=residue_ids.begin();it!=residue_ids.end();++it)
	{
		std::cout << protein::LettersCoding::convert_residue_codes_big_to_small(it->second.name);
		std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator next_it=it;
		++next_it;
	}
	std::cout << "\n";
}

void x_construct_global_sequence_alignment(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--match: --mismatch: --gap-start: --gap-extension:");

	const int match_score=clo.arg_or_default_value<int>("--match", 10);
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
		protein::PairwiseSequenceAlignment::print_global_sequence_alignment(seq1, seq2, scorer, std::cout);
	}
}

void x_combine_three_global_sequence_alignments(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	std::pair<std::string, std::string> alignments[3];
	for(int i=0;i<3;i++)
	{
		std::getline(std::cin, alignments[i].first);
		std::getline(std::cin, alignments[i].second);
		if(alignments[i].first.empty() || alignments[i].first.size()!=alignments[i].second.size())
		{
			throw std::runtime_error("Invalid input alignments");
		}
	}

	std::pair<std::string, std::string> contents[3];
	for(int i=0;i<3;i++)
	{
		for(std::size_t j=0;j<alignments[i].first.size();j++)
		{
			if(alignments[i].first[j]!='-')
			{
				contents[i].first.push_back(alignments[i].first[j]);
			}
		}
		for(std::size_t j=0;j<alignments[i].second.size();j++)
		{
			if(alignments[i].second[j]!='-')
			{
				contents[i].second.push_back(alignments[i].second[j]);
			}
		}
	}

	if(contents[0].second!=contents[1].first || contents[1].second!=contents[2].first)
	{
		throw std::runtime_error("Second alignment is not consistent with others");
	}

	for(int j=0;j<2;j++)
	{
		for(std::size_t i=0;i<std::max(alignments[j].second.size(), alignments[j+1].first.size());i++)
		{
			if(i<alignments[j].second.size() && alignments[j].second[i]=='-')
			{
				alignments[j+1].first.insert(i, "-");
				alignments[j+1].second.insert(i, "-");
			}
			else if(i<alignments[j+1].first.size() && alignments[j+1].first[i]=='-')
			{
				for(int e=0;e<=j;e++)
				{
					alignments[e].first.insert(i, "-");
					alignments[e].second.insert(i, "-");
				}
			}
		}
	}

	std::cout << alignments[0].first << "\n";
	std::cout << alignments[1].first << "\n";
	std::cout << alignments[1].second << "\n";
	std::cout << alignments[2].second << "\n";
}
