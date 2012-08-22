#include <iostream>
#include <limits>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/name_colorizing.h"
#include "auxiliaries/opengl_printer.h"

class ResidueNameColorizer : public auxiliaries::NameColorizerForPymol<std::string>
{
public:
	ResidueNameColorizer(const std::string& mode)
	{
		if(mode=="residue_hydrophobicity")
		{
			set_map_of_colors(create_map_of_residue_colors_by_hydropathy_indices());
		}
		else if(mode=="residue_type")
		{
			set_map_of_colors(create_map_of_residue_colors_by_type());
		}
	}

private:
	static auxiliaries::Color color_from_hydropathy_index(const double hi)
	{
		return auxiliaries::Color::from_temperature_to_blue_white_red((1+hi/4.5)/2);
	}

	static std::map<std::string, auxiliaries::Color> create_map_of_residue_colors_by_hydropathy_indices()
	{
		std::map<std::string, auxiliaries::Color> m;
		m["ASP"]=color_from_hydropathy_index(-3.5);
		m["GLU"]=color_from_hydropathy_index(-3.5);
		m["CYS"]=color_from_hydropathy_index(2.5);
		m["MET"]=color_from_hydropathy_index(1.9);
		m["LYS"]=color_from_hydropathy_index(-3.9);
		m["ARG"]=color_from_hydropathy_index(-4.5);
		m["SER"]=color_from_hydropathy_index(-0.8);
		m["THR"]=color_from_hydropathy_index(-0.7);
		m["PHE"]=color_from_hydropathy_index(2.8);
		m["TYR"]=color_from_hydropathy_index(-1.3);
		m["ASN"]=color_from_hydropathy_index(-3.5);
		m["GLN"]=color_from_hydropathy_index(-3.5);
		m["GLY"]=color_from_hydropathy_index(-0.4);
		m["LEU"]=color_from_hydropathy_index(3.8);
		m["VAL"]=color_from_hydropathy_index(4.2);
		m["ILE"]=color_from_hydropathy_index(4.5);
		m["ALA"]=color_from_hydropathy_index(1.8);
		m["TRP"]=color_from_hydropathy_index(-0.9);
		m["HIS"]=color_from_hydropathy_index(-3.2);
		m["PRO"]=color_from_hydropathy_index(-1.6);
		return m;
	}

	static std::map<std::string, auxiliaries::Color> create_map_of_residue_colors_by_type()
	{
		const auxiliaries::Color nonpolar(255, 255, 0);
		const auxiliaries::Color acidic(255, 0, 0);
		const auxiliaries::Color basic(0, 0, 255);
		const auxiliaries::Color uncharged(0, 255, 0);

		std::map<std::string, auxiliaries::Color> m;

		m["LEU"]=nonpolar;
		m["VAL"]=nonpolar;
		m["ILE"]=nonpolar;
		m["ALA"]=nonpolar;
		m["PHE"]=nonpolar;
		m["TRP"]=nonpolar;
		m["MET"]=nonpolar;
		m["PRO"]=nonpolar;

		m["ASP"]=acidic;
		m["GLU"]=acidic;

		m["LYS"]=basic;
		m["ARG"]=basic;
		m["HIS"]=basic;

		m["CYS"]=uncharged;
		m["SER"]=uncharged;
		m["THR"]=uncharged;
		m["TYR"]=uncharged;
		m["ASN"]=uncharged;
		m["GLN"]=uncharged;
		m["GLY"]=uncharged;

		return m;
	}
};

void print_inter_chain_interface_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--probe: --step: --projections: --coloring:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.5;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;
	const ResidueNameColorizer colorizer(clo.isopt("--coloring") ? clo.arg<std::string>("--coloring") : std::string(""));

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::vector<CellFace> faces_vector;
	std::map< std::pair<std::size_t, std::size_t>, std::size_t > faces_vector_map;
	typedef std::map< std::pair<std::string, std::string>, std::vector< std::pair<std::size_t, std::size_t> > > InterfacesMap;
	InterfacesMap inter_chain_interfaces;
	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const std::pair<std::size_t, std::size_t> atoms_ids_pair=std::make_pair(it->first.get(0), it->first.get(1));

		const protein::Atom& a=atoms[atoms_ids_pair.first];
		const protein::Atom& b=atoms[atoms_ids_pair.second];

		if(a.chain_id!=b.chain_id && a.chain_id!="?" && b.chain_id!="?")
		{
			std::vector<const protein::Atom*> cs;
			cs.reserve(it->second.size());
			for(Apollo::PairsNeighboursMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
			{
				cs.push_back(&(atoms[*jt]));
			}

			const CellFace cell_face=CellFace::construct(a, b, cs, probe_radius, step_length, projections_count);
			if(!cell_face.mesh_vertices().empty())
			{
				const std::pair<std::size_t, std::size_t> reversed_atoms_ids_pair=std::make_pair(atoms_ids_pair.second, atoms_ids_pair.first);
				faces_vector.push_back(cell_face);
				faces_vector_map[atoms_ids_pair]=faces_vector.size()-1;
				faces_vector_map[reversed_atoms_ids_pair]=faces_vector.size()-1;
				inter_chain_interfaces[std::make_pair(a.chain_id, b.chain_id)].push_back(atoms_ids_pair);
				inter_chain_interfaces[std::make_pair(b.chain_id, a.chain_id)].push_back(reversed_atoms_ids_pair);
			}
		}
	}

	if(inter_chain_interfaces.empty())
	{
		throw std::runtime_error("No interfaces found");
	}

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
	{
		const std::string obj_name=std::string("obj_")+it->first.first+"_"+it->first.second;
		const std::string cgo_name=std::string("iface_")+it->first.first+"_"+it->first.second;
		const auxiliaries::OpenGLPrinter opengl_printer(obj_name, cgo_name);
		for(std::size_t i=0;i<it->second.size();++i)
		{
			const std::pair<std::size_t, std::size_t> atoms_ids_pair=it->second[i];
			const protein::Atom& a=atoms[atoms_ids_pair.first];
			const protein::Atom& b=atoms[atoms_ids_pair.second];
			const CellFace& cell_face=faces_vector[faces_vector_map.find(atoms_ids_pair)->second];
			const apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(b, a).unit();
			opengl_printer.print_tringle_fan(cell_face.mesh_vertices(), normal, colorizer.color(a.residue_name));
		}
	}

	std::cout << "cmd.do('util.cbc')\n\n";
	std::cout << "cmd.do('hide nonbonded')\n\n";

	colorizer.list_colors();

	for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
	{
		const std::string selection_name=std::string("iface_sel_")+it->first.first+"_"+it->first.second;
		std::map<int, std::string> sequence_numbers;
		for(std::size_t i=0;i<it->second.size();++i)
		{
			const protein::Atom& a=atoms[it->second[i].first];
			sequence_numbers[a.residue_number]=a.residue_name;
		}

		std::cout << "cmd.do('select " << selection_name << ", resi ";
		for(std::map<int, std::string>::const_iterator jt=sequence_numbers.begin();jt!=sequence_numbers.end();++jt)
		{
			if(jt!=sequence_numbers.begin())
			{
				std::cout << "+";
			}
			std::cout << jt->first;
		}
		std::cout << " and chain " << it->first.first << "')\n\n";

		for(std::map<int, std::string>::const_iterator jt=sequence_numbers.begin();jt!=sequence_numbers.end();++jt)
		{
			std::cout << "cmd.do('color " << colorizer.color_string(jt->second) << ", resi " << jt->first << " and chain " << it->first.first << "')\n";
		}
		std::cout << "\n";

		std::cout << "cmd.do('show sticks, " << selection_name << "')\n\n";
	}

	std::cout << "cmd.do('deselect')\n\n";
	std::cout << "cmd.do('center')\n\n";
	std::cout << "cmd.do('zoom')\n\n";
}
