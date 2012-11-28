#ifndef PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_
#define PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_

#include <algorithm>
#include <vector>

namespace protein
{

class PairwiseSequenceAlignment
{
public:
	struct SimpleScorer
	{
		int match_score;
		int mismatch_score;
		int gap_score;

		SimpleScorer(int match_score, int mismatch_score, int gap_score) :
			match_score(match_score),
			mismatch_score(mismatch_score),
			gap_score(gap_score)
		{
		}

		template<typename T>
		static int match(const T& v1, const T& v2)
		{
			return (v1==v2 ? 2 : -1);
		}

		template<typename T>
		static int deletion(const T&)
		{
			return -1;
		}

		template<typename T>
		static int insertion(const T&)
		{
			return -1;
		}
	};

	struct AlignmentResult
	{
		std::vector< std::pair<int, int> > alignment;
		int score;

		AlignmentResult() : score(0)
		{
		}
	};

	template<typename T, typename Scorer>
	static AlignmentResult align_two_sequences(const T& seq1, const T& seq2, const Scorer& scorer, const bool local)
	{
		AlignmentResult result;
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
					const int match_score=scorer.match(v1, v2)+scores_matrix[i-1][j-1];
					const int deletion_score=scorer.deletion(v1)+scores_matrix[i-1][j];
					const int insertion_score=scorer.insertion(v2)+scores_matrix[i][j-1];
					const int max_score=std::max(match_score, std::max(deletion_score, insertion_score));
					const int current_score=(local ? std::max(0, max_score) : max_score);
					scores_matrix[i][j]=current_score;
					directions_matrix[i][j]=(max_score==match_score ? 0 : (max_score==deletion_score ? 1 : 2));
					if(overall_max_score<=current_score)
					{
						overall_max_score=current_score;
						overall_max_score_position=std::make_pair(i, j);
					}
				}
			}

			int i=static_cast<int>(seq1.size());
			int j=static_cast<int>(seq2.size());
			if(local)
			{
				i=static_cast<int>(overall_max_score_position.first);
				j=static_cast<int>(overall_max_score_position.second);
			}
			result.score=scores_matrix[i][j];
			while(i>0 && j>0 && (!local || scores_matrix[i][j]>0))
			{
				const int dir=directions_matrix[i][j];
				if(dir==0)
				{
					i--;
					j--;
					result.alignment.push_back(std::make_pair(i, j));
				}
				else if(dir==1)
				{
					i--;
					result.alignment.push_back(std::make_pair(i, -1));
				}
				else
				{
					j--;
					result.alignment.push_back(std::make_pair(-1, j));
				}
			}
			if(!local)
			{
				while(i>0)
				{
					i--;
					result.alignment.push_back(std::make_pair(i, -1));
				}
				while(j>0)
				{
					j--;
					result.alignment.push_back(std::make_pair(-1, j));
				}
			}
			std::reverse(result.alignment.begin(), result.alignment.end());
		}
		return result;
	}
};

}

#endif /* PROTEIN_PAIRWISE_SEQUENCE_ALIGNMENT_H_ */
