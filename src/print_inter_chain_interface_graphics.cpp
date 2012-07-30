#include <iostream>
#include <limits>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/color.h"

inline std::map<std::string, double> map_of_hydropathy_indices_of_residues()
{
	std::map<std::string, double> m;
	m["ASP"]=(-3.5);
	m["GLU"]=(-3.5);
	m["CYS"]=(2.5);
	m["MET"]=(1.9);
	m["LYS"]=(-3.9);
	m["ARG"]=(-4.5);
	m["SER"]=(-0.8);
	m["THR"]=(-0.7);
	m["PHE"]=(2.8);
	m["TYR"]=(-1.3);
	m["ASN"]=(-3.5);
	m["GLN"]=(-3.5);
	m["GLY"]=(-0.4);
	m["LEU"]=(3.8);
	m["VAL"]=(4.2);
	m["ILE"]=(4.5);
	m["ALA"]=(1.8);
	m["TRP"]=(-0.9);
	m["HIS"]=(-3.2);
	m["PRO"]=(-1.6);
	return m;
}

inline auxiliaries::Color residue_color_by_hydropathy_index(const std::string& residue_name)
{
	static const std::map<std::string, double> map_of_hidropathy_indices=map_of_hydropathy_indices_of_residues();
	std::map<std::string, double>::const_iterator it=map_of_hidropathy_indices.find(residue_name);
	const double hi=(it==map_of_hidropathy_indices.end() ? 0.0 : it->second);
	return auxiliaries::Color::from_temperature_to_blue_white_red((1+hi/4.5)/2);
}

inline void list_residue_colors_by_hydropathy_index()
{
	std::map<std::string, double> map_of_hidropathy_indices=map_of_hydropathy_indices_of_residues();
	map_of_hidropathy_indices["undefined"]=0.0;
	for(std::map<std::string, double>::const_iterator it=map_of_hidropathy_indices.begin();it!=map_of_hidropathy_indices.end();++it)
	{
		const auxiliaries::Color color=residue_color_by_hydropathy_index(it->first);
		std::cout << "cmd.do('set_color custom_color_" << static_cast<int>(color.r) << "_" << static_cast<int>(color.g) << "_" << static_cast<int>(color.b) << ", ";
		std::cout << "[ " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << " ]')\n";
	}
	std::cout << "\n";
}

template<typename PointType>
std::string point_to_string(const PointType& a)
{
	std::ostringstream output;
	output.precision(std::numeric_limits<double>::digits10);
	output << std::fixed << a.x << ", " << a.y << ", " << a.z;
	return output.str();
}

void print_tringle_fan(const std::vector<apollo::SimplePoint>& mesh_vertices, const apollo::SimplePoint& normal, const auxiliaries::Color& color)
{
	if(!mesh_vertices.empty())
	{
		const apollo::SimplePoint shift=normal*0.001;

		std::cout << "    BEGIN, TRIANGLE_FAN,\n";
		std::cout << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
		std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
		std::cout << "    VERTEX, " << point_to_string(mesh_vertices.back()+shift) << ",\n";
		for(std::size_t i=0;i+1<mesh_vertices.size();i++)
		{
			std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
			std::cout << "    VERTEX, " << point_to_string(mesh_vertices[i]+shift) << ",\n";
		}
		std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
		std::cout << "    VERTEX, " << point_to_string(mesh_vertices.front()+shift) << ",\n";
		std::cout << "    END,\n";

//		std::cout << "    BEGIN, LINE_LOOP,\n";
//		std::cout << "    COLOR, 1.0, 1.0, 0.0,\n";
//		std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
//		for(std::size_t i=0;i+1<mesh_vertices.size();i++)
//		{
//			std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
//			std::cout << "    VERTEX, " << point_to_string(mesh_vertices[i]+shift) << ",\n";
//		}
//		std::cout << "    END,\n";
	}
}

void print_inter_chain_interface_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--mode: --probe: --step: --projections:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.5;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;

	auxiliaries::assert_file_header(std::cin, "atoms");
	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);
	while(auxiliaries::check_file_header(std::cin, "atoms"))
	{
		std::vector<protein::Atom> more_atoms=auxiliaries::read_vector<protein::Atom>(std::cin);
		atoms.insert(atoms.end(), more_atoms.begin(), more_atoms.end());
	}

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

		if(a.chain_id!=b.chain_id)
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
		std::cerr << "No interfaces found.\n";
	}

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
	{
		const std::string obj_name=std::string("obj_")+it->first.first+"_"+it->first.second;
		const std::string cgo_name=std::string("iface_")+it->first.first+"_"+it->first.second;
		std::cout << obj_name << " = [\n";
		for(std::size_t i=0;i<it->second.size();++i)
		{
			const std::pair<std::size_t, std::size_t> atoms_ids_pair=it->second[i];
			const protein::Atom& a=atoms[atoms_ids_pair.first];
			const protein::Atom& b=atoms[atoms_ids_pair.second];
			const CellFace& cell_face=faces_vector[faces_vector_map.find(atoms_ids_pair)->second];
			const apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(b, a).unit();
			print_tringle_fan(cell_face.mesh_vertices(), normal, residue_color_by_hydropathy_index(a.residue_name));
		}
		std::cout << "]\ncmd.load_cgo(" << obj_name << ", '" << cgo_name << "')\n\n";
	}

	std::cout << "cmd.do('util.cbc')\n\n";
	std::cout << "cmd.do('hide nonbonded')\n\n";

	list_residue_colors_by_hydropathy_index();

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
			const auxiliaries::Color color=residue_color_by_hydropathy_index(jt->second);
			std::cout << "cmd.do('color custom_color_" << static_cast<int>(color.r) << "_" << static_cast<int>(color.g) << "_" << static_cast<int>(color.b) << ", resi " << jt->first << " and chain " << it->first.first << "')\n";
		}
		std::cout << "\n";

		std::cout << "cmd.do('show sticks, " << selection_name << "')\n\n";
	}

	std::cout << "cmd.do('deselect')\n\n";
	std::cout << "cmd.do('center')\n\n";
}
