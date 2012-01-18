#include <limits>

#include "../protein/atom.h"
#include "../protein/residue_ids_collection.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"

void main_filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo)
{
	const std::size_t portion=clo.arg_or_default_value("--portion", std::numeric_limits<std::size_t>::max());

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>();
	std::set<protein::ResidueID> residue_ids_1=protein::collect_residue_ids_from_atoms(atoms_1);

	if(residue_ids_1.size()>portion)
	{
		std::set<protein::ResidueID> residue_ids_1_portion;
		std::set<protein::ResidueID>::const_iterator it=residue_ids_1.begin();
		std::set<protein::ResidueID>::iterator prev=residue_ids_1_portion.begin();
		for(std::size_t i=0;i<portion;i++)
		{
			prev=residue_ids_1_portion.insert(prev, (*it));
			++it;
		}
		residue_ids_1=residue_ids_1_portion;
	}

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>();

	std::vector<protein::Atom> result;
	result.reserve(atoms_2.size());
	for(std::size_t i=0;i<atoms_2.size();i++)
	{
		const protein::Atom& atom=atoms_2[i];
		if(residue_ids_1.count(protein::ResidueID::from_atom(atom))>0)
		{
			result.push_back(atom);
		}
	}

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(result);
}
