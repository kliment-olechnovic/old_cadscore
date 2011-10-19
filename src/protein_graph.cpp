#include "protein_graph.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"

void report_hidden_atoms(const std::vector<protein::Atom>& atoms, const std::set<std::size_t>& hidden_spheres)
{
	for(std::set<std::size_t>::const_iterator it=hidden_spheres.begin();it!=hidden_spheres.end();++it)
	{
		std::clog << "Atom is fully inside some other atom: " << atoms[*it] << "\n";
	}
}

void report_atoms_without_neighbours(const std::vector<protein::Atom>& atoms, const std::vector< std::vector<std::size_t> >& graph)
{
	for(std::size_t i=0;i<graph.size();i++)
	{
		if(graph[i].empty())
		{
			std::clog << "Atom has no neighbours in the Apollonius graph: " << atoms[i] << "\n";
		}
	}
}

std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > construct_protein_graph(const std::vector<protein::Atom>& unrefined_protein_atoms)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	const Hierarchy hierarchy(unrefined_protein_atoms, 1.4*3, 50);

	const std::set<std::size_t> hidden_spheres=hierarchy.find_all_hidden_spheres();
	report_hidden_atoms(unrefined_protein_atoms, hidden_spheres);

	if(hidden_spheres.empty())
	{
		const std::vector< std::vector<std::size_t> > graph=Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(hierarchy), unrefined_protein_atoms.size());
		report_atoms_without_neighbours(unrefined_protein_atoms, graph);
		return std::make_pair(unrefined_protein_atoms, graph);
	}
	else
	{
		std::vector<protein::Atom> refined_protein_atoms;
		refined_protein_atoms.reserve(unrefined_protein_atoms.size()-hidden_spheres.size());
		for(std::size_t i=0;i<unrefined_protein_atoms.size();i++)
		{
			if(hidden_spheres.find(i)==hidden_spheres.end())
			{
				refined_protein_atoms.push_back(unrefined_protein_atoms[i]);
			}
		}

		const Hierarchy refined_hierarchy(refined_protein_atoms, 1.4*3, 50);
		const std::vector< std::vector<std::size_t> > graph=Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(refined_hierarchy), refined_protein_atoms.size());
		report_atoms_without_neighbours(refined_protein_atoms, graph);
		return std::make_pair(refined_protein_atoms, graph);
	}
}

