#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "protein/residue_id.h"
#include "protein/residue_summary.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/map_io.h"

namespace
{

inline std::map<std::string, std::string> create_map_of_residue_codes_big_to_small()
{
	std::map<std::string, std::string> m;
	m["LEU"]="L";
	m["VAL"]="V";
	m["ILE"]="I";
	m["ALA"]="A";
	m["PHE"]="F";
	m["TRP"]="W";
	m["MET"]="M";
	m["PRO"]="P";
	m["ASP"]="D";
	m["GLU"]="E";
	m["LYS"]="K";
	m["ARG"]="R";
	m["HIS"]="H";
	m["CYS"]="C";
	m["SER"]="S";
	m["THR"]="T";
	m["TYR"]="Y";
	m["ASN"]="N";
	m["GLN"]="Q";
	m["GLY"]="G";
	m["ASX"]="B";
	m["GLX"]="Z";
	m["A"]="a";
	m["C"]="c";
	m["G"]="g";
	m["T"]="t";
	m["U"]="u";
	return m;
}

inline std::map<std::string, std::string> create_map_of_residue_codes_small_to_big()
{
	const std::map<std::string, std::string> m_big_to_small;
	std::map<std::string, std::string> m;
	for(std::map<std::string, std::string>::const_iterator it=m_big_to_small.begin();it!=m_big_to_small.end();++it)
	{
		m[it->second]=it->first;
	}
	return m;
}

inline std::string convert_residue_codes_big_to_small(const std::string& aaa)
{
	static const std::map<std::string, std::string> m=create_map_of_residue_codes_big_to_small();
	const std::map<std::string, std::string>::const_iterator it=m.find(aaa);
	return (it==m.end() ? std::string("*") : it->second);
}

inline std::string convert_residue_codes_small_to_big(const std::string& a)
{
	static const std::map<std::string, std::string> m=create_map_of_residue_codes_small_to_big();
	const std::map<std::string, std::string>::const_iterator it=m.find(a);
	return (it==m.end() ? std::string("XXX") : it->second);
}

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

static bool print_global_sequence_alignment(const std::string& seq1, const std::string& seq2, const std::vector< std::pair<int, int> >& alignment, std::ostream& output)
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
	return print_global_sequence_alignment(seq1, seq2, construct_global_sequence_alignment(seq1, seq2, scorer),	output);
}

//static std::vector< std::pair<int, int> > read_global_sequence_alignment(const std::string& alignment_seq1, const std::string& alignment_seq2)
//{
//	const std::size_t alignment_size=std::min(alignment_seq1.size(), alignment_seq2.size());
//
//	std::vector< std::pair<int, int> > alignment(alignment_size, std::pair<int, int>(-1, -1));
//
//	for(int i=0;i<static_cast<int>(alignment_size);i++)
//	{
//		if(alignment_seq1.at(i)!='-')
//		{
//			alignment[i].first=i;
//		}
//		if(alignment_seq2.at(i)!='-')
//		{
//			alignment[i].second=i;
//		}
//	}
//
//	return alignment;
//}

struct SimpleSequenceAlignmentScorer
{
	int match_score;
	int mismatch_score;
	int gap_start_score;
	int gap_extension_score;

	SimpleSequenceAlignmentScorer(int match_score, int mismatch_score, int gap_start_score, int gap_extension_score) :
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

}

void x_collect_residue_sequence(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=auxiliaries::read_map<protein::ResidueID, protein::ResidueSummary>(std::cin, "residue identifiers", "residue_ids", false);
	for(std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator it=residue_ids.begin();it!=residue_ids.end();++it)
	{
		std::cout << convert_residue_codes_big_to_small(it->second.name);
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
		const SimpleSequenceAlignmentScorer scorer(match_score, mismatch_score, gap_start_score, gap_extension_score);
		print_global_sequence_alignment(seq1, seq2, scorer, std::cout);
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
