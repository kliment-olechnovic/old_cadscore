#include <iostream>
#include <fstream>

#include "protein_input.h"
#include "protein_graph.h"
#include "protein_contacts.h"

#include "auxiliaries/vector_io.h"
#include "auxiliaries/BlockTimer.h"

int main()
{
	auxiliaries::BlockTimer global_timer("Whole execution time");

	const std::vector<protein::Atom> unrefined_atoms=read_protein_atoms(false, false);
	const std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > protein_graph=construct_protein_graph(unrefined_atoms);
	const std::vector<protein::Atom>& atoms=protein_graph.first;
	const std::vector< std::vector<std::size_t> >& graph=protein_graph.second;
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=construct_inter_atom_contacts_from_atoms_graph(atoms, graph, 3, 1.4);
	const std::map< std::pair<protein::ResidueID, protein::ResidueID>, std::map<std::string, double> > contacts_between_residues=collect_contacts_between_residues(atoms, inter_atom_contacts);

	auxiliaries::print_vector(atoms);
	auxiliaries::print_vector(inter_atom_contacts);

	return 0;
}
