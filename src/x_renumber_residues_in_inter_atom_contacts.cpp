#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "protein/atom.h"
#include "protein/residue_ids_collection.h"

#include "contacto/inter_atom_contact.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

namespace
{

class LettersCoding
{
public:
	inline static std::string convert_residue_codes_big_to_small(const std::string& aaa)
	{
		static const std::map<std::string, std::string> m=create_map_of_residue_codes_big_to_small();
		const std::map<std::string, std::string>::const_iterator it=m.find(aaa);
		return (it==m.end() ? std::string("?") : it->second);
	}

	inline static std::string convert_residue_codes_small_to_big(const std::string& a)
	{
		static const std::map<std::string, std::string> m=create_map_of_residue_codes_small_to_big();
		const std::map<std::string, std::string>::const_iterator it=m.find(a);
		return (it==m.end() ? std::string("???") : it->second);
	}
private:
	static std::map<std::string, std::string> create_map_of_residue_codes_big_to_small()
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
		m["???"]="?";
		return m;
	}

	static std::map<std::string, std::string> create_map_of_residue_codes_small_to_big()
	{
		const std::map<std::string, std::string> m_big_to_small;
		std::map<std::string, std::string> m;
		for(std::map<std::string, std::string>::const_iterator it=m_big_to_small.begin();it!=m_big_to_small.end();++it)
		{
			m[it->second]=it->first;
		}
		return m;
	}
};

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
			for(std::size_t i=1;i<=seq1.size();i++)
			{
				scores_matrix[i][0]=((i==1) ? scorer.gap_start() : (scores_matrix[i-1][0]+scorer.gap_extension()));
				directions_matrix[i][0]=1;
			}
			for(std::size_t j=1;j<=seq2.size();j++)
			{
				scores_matrix[0][j]=((j==1) ? scorer.gap_start() : (scores_matrix[0][j-1]+scorer.gap_extension()));
				directions_matrix[0][j]=2;
			}
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
					scores_matrix[i][j]=max_score;
					directions_matrix[i][j]=(max_score==insertion_score ? 2 : (max_score==deletion_score ? 1 : 0));
					overall_max_score=std::max(overall_max_score, max_score);
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
			output << (j>=0 ? seq1.at(j) : '-');
		}
		output << "\n";

		for(std::size_t i=0;i<alignment.size();i++)
		{
			const int j=alignment[i].second;
			output << (j>=0 ? seq2.at(j) : '-');
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
		int pos1=0;
		int pos2=0;
		for(std::size_t i=0;i<alignment_size;i++)
		{
			if(alignment_seq1.at(i)!='-')
			{
				alignment[i].first=pos1;
				pos1++;
			}
			if(alignment_seq2.at(i)!='-')
			{
				alignment[i].second=pos2;
				pos2++;
			}
		}
		return alignment;
	}
};

std::string collect_string_without_gaps(const std::string& input)
{
	std::string output;
	for(std::size_t i=0;i<input.size();i++)
	{
		if(input[i]!='-')
		{
			output.push_back(input[i]);
		}
	}
	return output;
}

template<typename T>
std::vector< std::pair<typename T::key_type, typename T::mapped_type> > filter_residue_ids_map_by_chain_id(const T& residue_ids_map, const std::string& chain_id)
{
	std::vector< std::pair<typename T::key_type, typename T::mapped_type> > filtered_residue_ids_map;
	for(typename T::const_iterator it=residue_ids_map.begin();it!=residue_ids_map.end();++it)
	{
		if(it->first.chain_id==chain_id)
		{
			filtered_residue_ids_map.push_back(*it);
		}
	}
	return filtered_residue_ids_map;
}

template<typename T>
std::string collect_sequence_string_from_residue_ids_map(const T& residue_ids_map)
{
	std::ostringstream output;
	for(typename T::const_iterator it=residue_ids_map.begin();it!=residue_ids_map.end();++it)
	{
		output << LettersCoding::convert_residue_codes_big_to_small(it->second.name);
	}
	return output.str();
}

}

void x_renumber_residues_in_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--target-chain-names: --model-chain-names: --match: --mismatch: --gap-start: --gap-extension:");

	const std::string chain_names_1=clo.arg<std::string>("--target-chain-names");
	const std::string chain_names_2=clo.arg<std::string>("--model-chain-names");
	const int match_score=clo.arg_or_default_value<int>("--match", 10);
	const int mismatch_score=clo.arg_or_default_value<int>("--mismatch", -10);
	const int gap_start_score=clo.arg_or_default_value<int>("--gap-start", -11);
	const int gap_extension_score=clo.arg_or_default_value<int>("--gap-extension", -1);

	std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>(std::cin, "target atoms", "atoms", false);
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_1=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin, "target inter-atom contacts", "contacts", false);
	std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>(std::cin, "model atoms", "atoms", false);
	const std::vector<contacto::InterAtomContact> inter_atom_contacts_2=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin, "model inter-atom contacts", "contacts", false);

	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids_1=protein::collect_residue_ids_from_atoms(atoms_1);
	const std::map<protein::ResidueID, std::vector<std::size_t> > residue_ids_indices_1=protein::group_atoms_indices_by_residue_ids(atoms_1);
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids_2=protein::collect_residue_ids_from_atoms(atoms_2);
	const std::map<protein::ResidueID, std::vector<std::size_t> > residue_ids_indices_2=protein::group_atoms_indices_by_residue_ids(atoms_2);

	for(std::size_t l=0;l<chain_names_1.size() && l<chain_names_2.size();l++)
	{
		const std::string chain_name_1=chain_names_1.substr(l, 1);
		const std::string chain_name_2=chain_names_2.substr(l, 1);

		std::clog << "Chain in target: " << chain_name_1 << "\n";
		std::clog << "Chain in model: " << chain_name_2 << "\n";

		const std::vector< std::pair<protein::ResidueID, protein::ResidueSummary> > filtered_residue_ids_1=filter_residue_ids_map_by_chain_id(residue_ids_1, chain_name_1);
		const std::vector< std::pair<protein::ResidueID, protein::ResidueSummary> > filtered_residue_ids_2=filter_residue_ids_map_by_chain_id(residue_ids_2, chain_name_2);

		if(filtered_residue_ids_1.empty() || filtered_residue_ids_2.empty())
		{
			throw std::runtime_error("No atoms for chain name");
		}

		const std::string sequence_string_from_atoms_1=collect_sequence_string_from_residue_ids_map(filtered_residue_ids_1);
		const std::string sequence_string_from_atoms_2=collect_sequence_string_from_residue_ids_map(filtered_residue_ids_2);

		if(sequence_string_from_atoms_1.size()!=filtered_residue_ids_1.size() || sequence_string_from_atoms_2.size()!=filtered_residue_ids_2.size())
		{
			throw std::runtime_error("Unconvertible sequence");
		}

		std::clog << "Sequences from atoms:\n";
		std::clog << sequence_string_from_atoms_1 << "\n";
		std::clog << sequence_string_from_atoms_2 << "\n";

		std::string input_alignment_string_1;
		std::cin >> input_alignment_string_1;
		std::string input_alignment_string_2;
		std::cin >> input_alignment_string_2;

		const std::string sequence_string_from_input_alignment_1=collect_string_without_gaps(input_alignment_string_1);
		const std::string sequence_string_from_input_alignment_2=collect_string_without_gaps(input_alignment_string_2);

		if(input_alignment_string_1.empty() || input_alignment_string_1.size()!=input_alignment_string_2.size() || sequence_string_from_input_alignment_1.empty() || sequence_string_from_input_alignment_2.empty())
		{
			throw std::runtime_error("Invalid input alignment");
		}

		std::clog << "Input alignment:\n";
		std::clog << input_alignment_string_1 << "\n";
		std::clog << input_alignment_string_2 << "\n";

		const PairwiseSequenceAlignment::SimpleScorer scorer(match_score, mismatch_score, gap_start_score, gap_extension_score);
		std::vector< std::pair<int, int> > alignments[3];
		alignments[0]=PairwiseSequenceAlignment::construct_global_sequence_alignment(sequence_string_from_atoms_1, sequence_string_from_input_alignment_1, scorer);
		alignments[1]=PairwiseSequenceAlignment::read_sequence_alignment(input_alignment_string_1, input_alignment_string_2);
		alignments[2]=PairwiseSequenceAlignment::construct_global_sequence_alignment(sequence_string_from_input_alignment_2, sequence_string_from_atoms_2, scorer);

		for(int j=0;j<2;j++)
		{
			for(std::size_t i=0;i<std::max(alignments[j].size(), alignments[j+1].size());i++)
			{
				if(i<alignments[j].size() && alignments[j][i].second<0 && (i>=alignments[j+1].size() || alignments[j+1][i].first>=0))
				{
					alignments[j+1].insert(alignments[j+1].begin()+i, std::pair<int, int>(-1, -1));
				}
				else if(i<alignments[j+1].size() && alignments[j+1][i].first<0 && (i>=alignments[j].size() || alignments[j][i].second>=0))
				{
					for(int e=0;e<=j;e++)
					{
						alignments[e].insert(alignments[e].begin()+i, std::pair<int, int>(-1, -1));
					}
				}
			}
		}

		if(alignments[0].size()!=alignments[1].size() || alignments[1].size()!=alignments[2].size())
		{
			throw std::runtime_error("Failed to align alignments");
		}

		std::clog << "Result alignment:\n";
		PairwiseSequenceAlignment::print_sequence_alignment(sequence_string_from_atoms_1, sequence_string_from_input_alignment_1, alignments[0], std::clog);
		PairwiseSequenceAlignment::print_sequence_alignment(sequence_string_from_input_alignment_2, sequence_string_from_atoms_2, alignments[2], std::clog);

		const std::vector< std::pair<protein::ResidueID, std::vector<std::size_t> > > filtered_residue_ids_indices_1=filter_residue_ids_map_by_chain_id(residue_ids_indices_1, chain_name_1);
		const std::vector< std::pair<protein::ResidueID, std::vector<std::size_t> > > filtered_residue_ids_indices_2=filter_residue_ids_map_by_chain_id(residue_ids_indices_2, chain_name_2);

		for(std::size_t i=0;i<alignments[0].size();i++)
		{
			if(alignments[0][i].first>=0)
			{
				const std::vector<std::size_t>& atoms_indices=filtered_residue_ids_indices_1.at(alignments[0][i].first).second;
				for(std::size_t j=0;j<atoms_indices.size();j++)
				{
					protein::Atom& atom=atoms_1[atoms_indices[j]];
					atom.chain_id=chain_name_1;
					atom.residue_number=i+1;
				}
			}
			if(alignments[2][i].second>=0)
			{
				const std::vector<std::size_t>& atoms_indices=filtered_residue_ids_indices_2.at(alignments[2][i].second).second;
				for(std::size_t j=0;j<atoms_indices.size();j++)
				{
					protein::Atom& atom=atoms_2[atoms_indices[j]];
					atom.chain_id=chain_name_2;
					atom.residue_number=i+1;
				}
			}
		}

		std::clog << "\n";
	}

	auxiliaries::print_vector(std::cout, "atoms", atoms_1);
	auxiliaries::print_vector(std::cout, "contacts", inter_atom_contacts_1);
	auxiliaries::print_vector(std::cout, "atoms", atoms_2);
	auxiliaries::print_vector(std::cout, "contacts", inter_atom_contacts_2);
}
