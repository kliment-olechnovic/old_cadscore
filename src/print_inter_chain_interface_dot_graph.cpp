#include <iostream>

#include "protein/atom.h"
#include "protein/residue_id.h"

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

	clo.check_allowed_options("--probe: --grouped");
	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const bool grouped_vertices=clo.isopt("--grouped");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);

	std::vector< std::pair< std::pair<std::size_t, std::size_t>, std::pair<std::size_t, std::size_t> > > pairs_pairs;
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 1), std::make_pair(2, 3)));
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 2), std::make_pair(1, 3)));
	pairs_pairs.push_back(std::make_pair(std::make_pair(0, 3), std::make_pair(1, 2)));

	typedef std::pair<protein::ResidueID, protein::ResidueID> ResiduePair;
	typedef std::map< ResiduePair, std::set<ResiduePair> > ResiduePairsNeighboursMap;
	typedef std::tr1::unordered_set<apollo::Pair, apollo::Pair::HashFunctor> PairsSet;
	typedef std::map< ResiduePair, PairsSet > ResiduePairsContentsMap;

	ResiduePairsNeighboursMap rpn_map;
	ResiduePairsContentsMap rpc_map;

	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
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
			if(a.chain_id!=b.chain_id && c.chain_id!=d.chain_id &&
					a.chain_id!="?" && b.chain_id!="?" && c.chain_id!="?" && d.chain_id!="?" &&
					apollo::minimal_distance_from_sphere_to_sphere(a, b)<probe_radius*2 &&
					apollo::minimal_distance_from_sphere_to_sphere(c, d)<probe_radius*2)
			{
				ResiduePair residue_pair_ab=std::make_pair(protein::ResidueID::from_atom(a), protein::ResidueID::from_atom(b));
				if(residue_pair_ab.second<residue_pair_ab.first)
				{
					std::swap(residue_pair_ab.second, residue_pair_ab.first);
				}

				ResiduePair residue_pair_cd=std::make_pair(protein::ResidueID::from_atom(c), protein::ResidueID::from_atom(d));
				if(residue_pair_cd.second<residue_pair_cd.first)
				{
					std::swap(residue_pair_cd.second, residue_pair_cd.first);
				}

				if(residue_pair_ab!=residue_pair_cd)
				{
					rpn_map[residue_pair_ab].insert(residue_pair_cd);
					rpn_map[residue_pair_cd].insert(residue_pair_ab);
				}
			}
		}
	}

	if(grouped_vertices)
	{
		std::map< protein::ResidueID, std::set<protein::ResidueID> > residues_vertical_neighbours_map;
		std::map< protein::ResidueID, std::set<protein::ResidueID> > residues_horizontal_neighbours_map;

		for(ResiduePairsNeighboursMap::const_iterator it=rpn_map.begin();it!=rpn_map.end();++it)
		{
			residues_vertical_neighbours_map[it->first.first].insert(it->first.second);
			const std::set<ResiduePair>& neighbours=it->second;
			for(std::set<ResiduePair>::const_iterator jt=neighbours.begin();jt!=neighbours.end();++jt)
			{
				if(it->first.first<jt->first)
				{
					residues_horizontal_neighbours_map[it->first.first].insert(jt->first);
				}
			}
		}

		std::cout << "graph\n{\n";

		for(std::map< protein::ResidueID, std::set<protein::ResidueID> >::const_iterator it=residues_vertical_neighbours_map.begin();it!=residues_vertical_neighbours_map.end();++it)
		{
			const std::set<protein::ResidueID>& neighbours=it->second;
			std::cout << "node_" << it->first.chain_id << it->first.residue_number;
			std::cout << " [label=\"";
			std::cout << it->first.chain_id << it->first.residue_number;
			std::cout << " <-> ( ";
			for(std::set<protein::ResidueID>::const_iterator jt=neighbours.begin();jt!=neighbours.end();++jt)
			{
				std::cout << jt->chain_id << jt->residue_number << " ";
			}
			std::cout << ")";
			std::cout << "\"]\n";
		}

		for(std::map< protein::ResidueID, std::set<protein::ResidueID> >::const_iterator it=residues_horizontal_neighbours_map.begin();it!=residues_horizontal_neighbours_map.end();++it)
		{
			const std::set<protein::ResidueID>& neighbours=it->second;
			for(std::set<protein::ResidueID>::const_iterator jt=neighbours.begin();jt!=neighbours.end();++jt)
			{
				std::cout << "node_" << it->first.chain_id << it->first.residue_number;
				std::cout << " -- ";
				std::cout << "node_" << jt->chain_id << jt->residue_number;
				std::cout << "\n";
			}
		}

		std::cout << "}\n";
	}
	else
	{
		std::cout << "graph\n{\n";

		for(ResiduePairsNeighboursMap::const_iterator it=rpn_map.begin();it!=rpn_map.end();++it)
		{
			const ResiduePair& residue_pair_ab=it->first;
			std::cout << "node_" << residue_pair_ab.first.chain_id << residue_pair_ab.first.residue_number << "_" << residue_pair_ab.second.chain_id << residue_pair_ab.second.residue_number;
			std::cout << " [label=\"" << residue_pair_ab.first.chain_id << residue_pair_ab.first.residue_number << " with " << residue_pair_ab.second.chain_id << residue_pair_ab.second.residue_number << "\"]\n";
		}

		for(ResiduePairsNeighboursMap::const_iterator it=rpn_map.begin();it!=rpn_map.end();++it)
		{
			const ResiduePair& residue_pair_ab=it->first;
			const std::set<ResiduePair>& neighbours=it->second;
			for(std::set<ResiduePair>::const_iterator jt=neighbours.begin();jt!=neighbours.end();++jt)
			{
				const ResiduePair& residue_pair_cd=(*jt);
				if(residue_pair_ab<residue_pair_cd)
				{
					std::cout << "node_" << residue_pair_ab.first.chain_id << residue_pair_ab.first.residue_number << "_" << residue_pair_ab.second.chain_id << residue_pair_ab.second.residue_number;
					std::cout << " -- ";
					std::cout << "node_" << residue_pair_cd.first.chain_id << residue_pair_cd.first.residue_number << "_" << residue_pair_cd.second.chain_id << residue_pair_cd.second.residue_number;
					std::cout << "\n";
				}
			}
		}

		std::cout << "}\n";
	}
}
