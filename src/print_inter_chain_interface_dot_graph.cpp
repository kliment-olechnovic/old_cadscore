#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void print_inter_chain_interface_dot_graph(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

//	clo.check_allowed_options("--probe: --step: --projections:");
//	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
//	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.5;
//	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);

	std::vector< std::pair< std::pair<std::size_t, std::size_t>, std::pair<std::size_t, std::size_t> > > pairs_pairs;
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 1), std::make_pair(2, 3)));
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 2), std::make_pair(1, 3)));
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 3), std::make_pair(1, 2)));

	typedef std::tr1::unordered_set<apollo::Pair, apollo::Pair::HashFunctor> PairsSet;
	typedef std::tr1::unordered_map<apollo::Pair, PairsSet, apollo::Pair::HashFunctor> PairsNeighbouringPairsMap;
	PairsSet pairs_set;
	PairsNeighbouringPairsMap pnp_map;
	for(typename Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		const apollo::Quadruple& quadruple=it->first;
		for(std::size_t i=0;i<pairs_pairs.size();i++)
		{
			const apollo::Pair pair_ab=apollo::make_pair(quadruple.get(pairs_pairs[i].first.first), quadruple.get(pairs_pairs[i].first.second));
			const apollo::Pair pair_cd=apollo::make_pair(quadruple.get(pairs_pairs[i].second.first), quadruple.get(pairs_pairs[i].second.second));
			const protein::Atom& a=atoms[pair_ab.get(0)];
			const protein::Atom& b=atoms[pair_ab.get(1)];
			const protein::Atom& c=atoms[pair_cd.get(0)];
			const protein::Atom& d=atoms[pair_cd.get(1)];
			if(a.chain_id!=b.chain_id && c.chain_id!=d.chain_id && a.chain_id!="?" && b.chain_id!="?" && c.chain_id!="?" && d.chain_id!="?")
			{
				if(pair_ab<pair_cd)
				{
					pnp_map[pair_ab].insert(pair_cd);
				}
				else
				{
					pnp_map[pair_cd].insert(pair_ab);
				}
				pairs_set.insert(pair_ab);
				pairs_set.insert(pair_cd);
			}
		}
	}

	std::cout << "graph\n{\n";
	for(PairsSet::const_iterator it=pairs_set.begin();it!=pairs_set.end();++it)
	{
		const apollo::Pair& pair_ab=(*it);
		std::cout << "node_" << pair_ab.get(0) << "_" << pair_ab.get(1) << " ";
		const protein::Atom* a=&atoms[pair_ab.get(0)];
		const protein::Atom* b=&atoms[pair_ab.get(1)];
		if(std::make_pair(b->chain_id, b->residue_number)<std::make_pair(a->chain_id, a->residue_number))
		{
			std::swap(a, b);
		}
		std::cout << "[label=\"" << a->chain_id << "" << a->residue_number << "," << a->residue_name << "," << a->atom_name << " with " << b->chain_id << "" << b->residue_number << "," << b->residue_name << "," << b->atom_name << "\"]\n";
	}

	for(PairsNeighbouringPairsMap::const_iterator it=pnp_map.begin();it!=pnp_map.end();++it)
	{
		const apollo::Pair& pair_ab=it->first;
		const PairsSet& neighbours=it->second;
		for(PairsSet::const_iterator jt=neighbours.begin();jt!=neighbours.end();++jt)
		{
			const apollo::Pair& pair_cd=(*jt);
			std::cout << "node_" << pair_ab.get(0) << "_" << pair_ab.get(1) << " -- " << "node_" << pair_cd.get(0) << "_" << pair_cd.get(1) << "\n";
		}
	}

	std::cout << "}\n";
}
