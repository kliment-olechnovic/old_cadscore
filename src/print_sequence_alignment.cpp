#include <iostream>
#include <algorithm>
#include <iomanip>

#include "protein/pairwise_sequence_alignment.h"

#include "auxiliaries/command_line_options.h"

template<typename T>
void print_sequence_alignment(const T& seq1, const T& seq2, const protein::PairwiseSequenceAlignment::AlignmentResult& alignment_result)
{
	std::cout << "score = " << alignment_result.score << "\n";
	const std::vector< std::pair<int, int> >& alignment=alignment_result.alignment;

	std::pair<int, int> start(-1, -1);
	std::pair<int, int> end(-1, -1);
	for(std::size_t i=0;i<alignment.size() && (start.first<0 || start.second<0 || end.first<0 || end.second<0);i++)
	{
		if(start.first<0)
		{
			start.first=alignment[i].first;
		}
		if(start.second<0)
		{
			start.second=alignment[i].second;
		}
		if(end.first<0)
		{
			end.first=alignment[alignment.size()-1-i].first;
		}
		if(end.second<0)
		{
			end.second=alignment[alignment.size()-1-i].second;
		}
	}

	if(start.first>=0 && start.second>=0)
	{
		const int offset=std::max(start.first, start.second);

		for(int i=0;i<(offset-start.first);i++)
		{
			std::cout << " ";
		}
		for(int i=0;i<start.first;i++)
		{
			std::cout << seq1[i];
		}
		for(std::size_t i=0;i<alignment.size();i++)
		{
			const int j=alignment[i].first;
			std::cout << (j>=0 ? seq1[j] : '-');
		}
		for(int i=end.first+1;i<static_cast<int>(seq1.size());i++)
		{
			std::cout << seq1[i];
		}
		std::cout << "\n";

		for(int i=0;i<offset;i++)
		{
			std::cout << " ";
		}
		for(std::size_t i=0;i<alignment.size();i++)
		{
			if(alignment[i].first>=0 && alignment[i].second>=0 && (seq1[alignment[i].first]==seq2[alignment[i].second]))
			{
				std::cout << "+";
			}
			else
			{
				std::cout << "|";
			}
		}
		std::cout << "\n";

		for(int i=0;i<(offset-start.second);i++)
		{
			std::cout << " ";
		}
		for(int i=0;i<start.second;i++)
		{
			std::cout << seq2[i];
		}
		for(std::size_t i=0;i<alignment.size();i++)
		{
			const int j=alignment[i].second;
			std::cout << (j>=0 ? seq2[j] : '-');
		}
		for(int i=end.second+1;i<static_cast<int>(seq2.size());i++)
		{
			std::cout << seq2[i];
		}
		std::cout << "\n";
	}
}

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
		protein::PairwiseSequenceAlignment::SimpleScorer scorer(2, -1, -1);
		std::cout << "\nglobal:\n";
		print_sequence_alignment(seq1, seq2, protein::PairwiseSequenceAlignment::align_two_sequences(seq1, seq2, scorer, false));
		std::cout << "\nlocal:\n";
		print_sequence_alignment(seq1, seq2, protein::PairwiseSequenceAlignment::align_two_sequences(seq1, seq2, scorer, true));
		std::cout << "\n";
	}
}