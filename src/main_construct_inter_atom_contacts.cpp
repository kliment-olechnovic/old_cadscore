#include "protein_input.h"
#include "protein_graph.h"
#include "protein_contacts.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void main_construct_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo)
{
	const std::string radius_classes_file_name=clo.arg<std::string>("--radius-classes");
	const std::string radius_members_file_name=clo.arg<std::string>("--radius-members");
	const std::size_t subdivision_depth=clo.arg<std::size_t>("--depth", 1, 4);
	const double probe_radius=clo.arg<double>("--probe", 0);

	const std::vector<protein::Atom> unrefined_atoms=read_protein_atoms(radius_classes_file_name, radius_members_file_name, false, false);
	const std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > protein_graph=construct_protein_graph(unrefined_atoms);
	const std::vector<protein::Atom>& atoms=protein_graph.first;
	const std::vector< std::vector<std::size_t> >& graph=protein_graph.second;
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=construct_inter_atom_contacts_from_atoms_graph(atoms, graph, subdivision_depth, probe_radius);

	auxiliaries::print_vector(atoms);
	auxiliaries::print_vector(inter_atom_contacts);
}
