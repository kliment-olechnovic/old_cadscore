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

inline double hydropathy_index_from_residue_name(const std::string& residue_name)
{
	if(residue_name=="ASP") return (-3.5);
	if(residue_name=="GLU") return (-3.5);
	if(residue_name=="CYS") return (2.5);
	if(residue_name=="MET") return (1.9);
	if(residue_name=="LYS") return (-3.9);
	if(residue_name=="ARG") return (-4.5);
	if(residue_name=="SER") return (-0.8);
	if(residue_name=="THR") return (-0.7);
	if(residue_name=="PHE") return (2.8);
	if(residue_name=="TYR") return (-1.3);
	if(residue_name=="ASN") return (-3.5);
	if(residue_name=="GLN") return (-3.5);
	if(residue_name=="GLY") return (-0.4);
	if(residue_name=="LEU") return (3.8);
	if(residue_name=="VAL") return (4.2);
	if(residue_name=="ILE") return (4.5);
	if(residue_name=="ALA") return (1.8);
	if(residue_name=="TRP") return (-0.9);
	if(residue_name=="HIS") return (-3.2);
	if(residue_name=="PRO") return (-1.6);
	return 0.0;
}

inline auxiliaries::Color color_from_hydropathy_index(const double hi)
{
	const double max_hi=4.5;
	return auxiliaries::Color::from_temperature_to_blue_white_red((1+hi/max_hi)/2);
}

inline auxiliaries::Color color_from_hydropathy_index(const std::string& residue_name)
{
	return color_from_hydropathy_index(hydropathy_index_from_residue_name(residue_name));
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
		std::cout << "    BEGIN, TRIANGLE_FAN,\n";
		std::cout << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
		std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
		std::cout << "    VERTEX, " << point_to_string(mesh_vertices.back()) << ",\n";
		for(std::size_t i=0;i+1<mesh_vertices.size();i++)
		{
			std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
			std::cout << "    VERTEX, " << point_to_string(mesh_vertices[i]) << ",\n";
		}
		std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
		std::cout << "    VERTEX, " << point_to_string(mesh_vertices.front()) << ",\n";
		std::cout << "    END,\n";
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
			print_tringle_fan(cell_face.mesh_vertices(), normal, color_from_hydropathy_index(a.residue_name));
		}
		std::cout << "]\ncmd.load_cgo(" << obj_name << ", '" << cgo_name << "')\n\n";
	}

	for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
	{
		const std::string selection_name=std::string("iface_sel_")+it->first.first+"_"+it->first.second;
		std::set<int> sequence_numbers;
		for(std::size_t i=0;i<it->second.size();++i)
		{
			sequence_numbers.insert(atoms[it->second[i].first].residue_number);
		}
		std::cout << "cmd.do('select " << selection_name << ", resi ";
		for(std::set<int>::const_iterator jt=sequence_numbers.begin();jt!=sequence_numbers.end();++jt)
		{
			if(jt!=sequence_numbers.begin())
			{
				std::cout << "+";
			}
			std::cout << (*jt);
		}
		std::cout << " and chain " << it->first.first << "')\n\n";
	}
}
