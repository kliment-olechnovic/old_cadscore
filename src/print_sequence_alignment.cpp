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
	{
		int i=static_cast<int>(seq1.size());
		int j=static_cast<int>(seq2.size());
		if(local)
		{
			i=static_cast<int>(overall_max_score_position.first);
			j=static_cast<int>(overall_max_score_position.second);
		}
		while(i>0 && j>0 && (!local || scores_matrix[i][j]>0))
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
		std::cout << "\nglobal:\n";
		print_sequence_alignment(seq1, seq2, create_sequence_alignment(seq1, seq2, PrimitiveScorer<std::string::value_type>(), false));
		std::cout << "\nlocal:\n";
		print_sequence_alignment(seq1, seq2, create_sequence_alignment(seq1, seq2, PrimitiveScorer<std::string::value_type>(), true));
		std::cout << "\n";
	}
}
