#include <iostream>
#include <algorithm>

#include "auxiliaries/command_line_options.h"

template<typename T>
struct PrimitiveScorer
{
	static int match(const T& v1, const T& v2)
	{
		return (v1==v2 ? 2 : -1);
	}

	static int deletion(const T&)
	{
		return -1;
	}

	static int insertion(const T&)
	{
		return -1;
	}
};

template<typename T, typename Scorer>
std::vector< std::pair<int, int> > create_sequence_alignment(const T& seq1, const T& seq2, const Scorer& scorer, const bool local)
{
	std::vector< std::vector<int> > scores_matrix(seq1.size()+1, std::vector<int>(seq2.size()+1, 0));
	std::vector< std::vector<int> > directions_matrix(seq1.size()+1, std::vector<int>(seq2.size()+1, 0));
	int overall_max_score=0;
	std::pair<std::size_t, std::size_t> overall_max_score_position(0, 0);
	for(std::size_t i=1;i<=seq1.size();i++)
	{
		for(std::size_t j=1;j<=seq2.size();j++)
		{
			const typename T::value_type& v1=seq1[i-1];
			const typename T::value_type& v2=seq2[j-1];
			const int match_score=scorer.match(v1, v2)+scores_matrix[i-1][j-1];
			const int deletion_score=scorer.deletion(v1)+scores_matrix[i-1][j];
			const int insertion_score=scorer.insertion(v2)+scores_matrix[i][j-1];
			const int max_score=std::max(match_score, std::max(deletion_score, insertion_score));
			const int current_score=(local ? std::max(0, max_score) : max_score);
			scores_matrix[i][j]=current_score;
			directions_matrix[i][j]=(max_score==match_score ? 0 : (max_score==deletion_score ? 1 : 2));
			if(overall_max_score<current_score)
			{
				overall_max_score=current_score;
				overall_max_score_position=std::make_pair(i, j);
			}
		}
	}

	std::vector< std::pair<int, int> > alignment;
	if(local)
	{
		int i=static_cast<int>(overall_max_score_position.first);
		int j=static_cast<int>(overall_max_score_position.second);
		while(i>0 && j>0 && scores_matrix[i][j]>0)
		{
			const int dir=directions_matrix[i][j];
			if(dir==0)
			{
				i--;
				j--;
				alignment.push_back(std::make_pair(i, j));
			}
			else if(dir==1)
			{
				i--;
				alignment.push_back(std::make_pair(i, -1));
			}
			else
			{
				j--;
				alignment.push_back(std::make_pair(-1, j));
			}
		}
	}
	else
	{
		int i=static_cast<int>(seq1.size());
		int j=static_cast<int>(seq2.size());
		while(i>0 && j>0)
		{
			const int dir=directions_matrix[i][j];
			if(dir==0)
			{
				i--;
				j--;
				alignment.push_back(std::make_pair(i, j));
			}
			else if(dir==1)
			{
				i--;
				alignment.push_back(std::make_pair(i, -1));
			}
			else
			{
				j--;
				alignment.push_back(std::make_pair(-1, j));
			}
		}
		if(!local)
		{
			while(i>0)
			{
				i--;
				alignment.push_back(std::make_pair(i, -1));
			}
			while(j>0)
			{
				j--;
				alignment.push_back(std::make_pair(-1, j));
			}
		}
	}
	std::reverse(alignment.begin(), alignment.end());

	return alignment;
}

template<typename T>
void print_sequence_alignment(const T& seq1, const T& seq2, const std::vector< std::pair<int, int> >& alignment)
{
	for(std::size_t i=0;i<alignment.size();i++)
	{
		const int j=alignment[i].first;
		if(j<static_cast<int>(seq1.size()))
		{
			std::cout << (j>=0 ? seq1[j] : '-') << " ";
		}
	}
	std::cout << "\n";
	for(std::size_t i=0;i<alignment.size();i++)
	{
		const int j=alignment[i].second;
		if(j<static_cast<int>(seq2.size()))
		{
			std::cout << (j>=0 ? seq2[j] : '-') << " ";
		}
	}
	std::cout << "\n";
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
		throw std::runtime_error("Two lines not found in standard input ");
	}
	else
	{
		std::cout << "\nglobal:\n";
		print_sequence_alignment(seq1, seq2, create_sequence_alignment(seq1, seq2, PrimitiveScorer<std::string::value_type>(), false));
		std::cout << "\nlocal:\n";
		print_sequence_alignment(seq1, seq2, create_sequence_alignment(seq1, seq2, PrimitiveScorer<std::string::value_type>(), true));
		std::cout << "\n";
	}
}
