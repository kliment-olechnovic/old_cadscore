#ifndef PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_
#define PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_

#include <vector>
#include <algorithm>

namespace protein
{

class PairwiseSequenceAlignment
{
public:
	struct SimpleScorer
	{
		int match_score;
		int mismatch_score;
		int gap_start_score;
		int gap_extension_score;

		SimpleScorer(int match_score, int mismatch_score, int gap_start_score, int gap_extension_score) :
			match_score(match_score),
			mismatch_score(mismatch_score),
			gap_start_score(gap_start_score),
			gap_extension_score(gap_extension_score)
		{
		}

		template<typename T>
		int match(const T& v1, const T& v2) const
		{
			return (v1==v2 ? match_score : mismatch_score);
		}

		int gap_start() const
		{
			return gap_start_score;
		}

		int gap_extension() const
		{
			return gap_extension_score;
		}
	};

	template<typename T, typename Scorer>
	static std::vector< std::pair<int, int> > construct_global_sequence_alignment(const T& seq1, const T& seq2, const Scorer& scorer)
	{
		std::vector< std::pair<int, int> > alignment;
		if(!seq1.empty() && !seq2.empty())
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
					const int match_score=scores_matrix[i-1][j-1]+scorer.match(v1, v2);
					const int deletion_score=scores_matrix[i-1][j]+(directions_matrix[i-1][j]!=1 ? scorer.gap_start() : scorer.gap_extension());
					const int insertion_score=scores_matrix[i][j-1]+(directions_matrix[i][j-1]!=2 ? scorer.gap_start() : scorer.gap_extension());
					const int max_score=std::max(match_score, std::max(deletion_score, insertion_score));
					const int current_score=max_score;
					scores_matrix[i][j]=current_score;
					directions_matrix[i][j]=(max_score==insertion_score ? 2 : (max_score==deletion_score ? 1 : 0));
					if(overall_max_score<=current_score)
					{
						overall_max_score=current_score;
						overall_max_score_position=std::make_pair(i, j);
					}
				}
			}

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
			std::reverse(alignment.begin(), alignment.end());
		}
		return alignment;
	}

	static bool print_sequence_alignment(const std::string& seq1, const std::string& seq2, const std::vector< std::pair<int, int> >& alignment, std::ostream& output)
	{
		for(std::size_t i=0;i<alignment.size();i++)
		{
			if(alignment[i].first>=static_cast<int>(seq1.size()) || alignment[i].second>=static_cast<int>(seq2.size()))
			{
				return false;
			}
		}

		for(std::size_t i=0;i<alignment.size();i++)
		{
			const int j=alignment[i].first;
			std::cout << (j>=0 ? seq1.at(j) : '-');
		}
		output << "\n";

		for(std::size_t i=0;i<alignment.size();i++)
		{
			const int j=alignment[i].second;
			std::cout << (j>=0 ? seq2.at(j) : '-');
		}
		output << "\n";

		return true;
	}

	template<typename Scorer>
	static bool print_global_sequence_alignment(const std::string& seq1, const std::string& seq2, const Scorer& scorer, std::ostream& output)
	{
		return print_sequence_alignment(seq1, seq2, construct_global_sequence_alignment(seq1, seq2, scorer), output);
	}

	static std::vector< std::pair<int, int> > read_sequence_alignment(const std::string& alignment_seq1, const std::string& alignment_seq2)
	{
		const std::size_t alignment_size=std::min(alignment_seq1.size(), alignment_seq2.size());
		std::vector< std::pair<int, int> > alignment(alignment_size, std::pair<int, int>(-1, -1));
		for(int i=0;i<static_cast<int>(alignment_size);i++)
		{
			if(alignment_seq1.at(i)!='-')
			{
				alignment[i].first=i;
			}
			if(alignment_seq2.at(i)!='-')
			{
				alignment[i].second=i;
			}
		}
		return alignment;
	}
};

}

#endif /* PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_ */
