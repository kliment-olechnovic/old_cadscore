#include <iostream>
#include <exception>

#include "protein_input.h"
#include "protein_graph.h"
#include "protein_contacts.h"

#include "auxiliaries/vector_io.h"

int main()
{
	std::cin.exceptions(std::istream::failbit | std::istream::badbit);
	std::cout.exceptions(std::ostream::failbit | std::ostream::badbit);

	try
	{
		const std::vector<protein::Atom> unrefined_atoms=read_protein_atoms(false, false);
		const std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > protein_graph=construct_protein_graph(unrefined_atoms);
		const std::vector<protein::Atom>& atoms=protein_graph.first;
		const std::vector< std::vector<std::size_t> >& graph=protein_graph.second;
		const std::vector<contacto::InterAtomContact> inter_atom_contacts=construct_inter_atom_contacts_from_atoms_graph(atoms, graph, 3, 1.4);

		auxiliaries::print_vector(atoms);
		auxiliaries::print_vector(inter_atom_contacts);
	}
	catch(const std::exception& e)
	{
		std::cerr << "STD exception caught: " << (e.what()) << std::endl;
		return -1;
	}
	catch(...)
	{
		std::cerr << "Unknown exception caught" << std::endl;
		return -1;
	}

	return 0;
}
