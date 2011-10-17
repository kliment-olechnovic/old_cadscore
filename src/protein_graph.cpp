#include "protein_graph.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"

std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > construct_protein_graph(const std::vector<protein::Atom>& unrefined_protein_atoms)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	const Hierarchy hierarchy(unrefined_protein_atoms, 1.4*3, 50);

	const std::set<std::size_t> hidden_spheres=hierarchy.find_all_hidden_spheres();

	if(hidden_spheres.empty())
	{
		return std::make_pair(
				unrefined_protein_atoms,
				Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(hierarchy), unrefined_protein_atoms.size())
		);
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

		return std::make_pair(
				refined_protein_atoms,
				Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(refined_hierarchy), refined_protein_atoms.size())
		);
	}
}

