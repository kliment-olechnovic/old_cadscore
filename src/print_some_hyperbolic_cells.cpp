#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

template<typename PointType>
std::string point_to_string(const PointType& a)
{
	std::ostringstream output;
	output << a.x << ", " << a.y << ", " << a.z;
	return output.str();
}

void print_mesh(const std::vector<apollo::SimplePoint>& mesh_vertices, const apollo::SimplePoint& basic_normal)
{
	if(!mesh_vertices.empty())
	{
		for(int j=0;j<2;j++)
		{
			const apollo::SimplePoint normal=(j==0 ? basic_normal : apollo::inverted_point<apollo::SimplePoint>(basic_normal));
			const apollo::SimplePoint shift=(normal*0.01);
			std::cout << "    BEGIN, TRIANGLE_FAN,\n";
			std::cout << "    COLOR, 1.0, 1.0, 0.0,\n";
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
		}
	}
}

template<typename SphereType>
void print_cell(
		const SphereType& a,
		const std::vector<apollo::HyperbolicCellFace>& cells_faces,
		const std::vector<std::size_t>& faces)
{
	for(std::size_t i=0;i<faces.size();i++)
	{
		const apollo::HyperbolicCellFace& cf=cells_faces[faces[i]];

		apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(cf.s2(), cf.s1()).unit();
		if(apollo::spheres_equal(cf.s2(), a))
		{
			normal=apollo::inverted_point<apollo::SimplePoint>(normal);
		}

		print_mesh(cf.mesh_vertices(), normal);
	}
}

void print_some_hyperbolic_cells(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--mode: --probe: --step: --projections: --atoms-selection:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.5;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;
	const std::vector<std::size_t> atoms_selection=clo.arg_vector<std::size_t>("--atoms-selection");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::vector<CellFace> cells_faces;
	std::map<std::size_t, std::vector<std::size_t> > cells;
	cells_faces.reserve(pairs_neighbours_map.size());
	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const protein::Atom& a=atoms[it->first.get(0)];
		const protein::Atom& b=atoms[it->first.get(1)];

		std::vector<const protein::Atom*> cs;
		cs.reserve(it->second.size());
		for(Apollo::PairsNeighboursMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
		{
			cs.push_back(&(atoms[*jt]));
		}

		cells_faces.push_back(CellFace::construct(a, b, cs, probe_radius, step_length, projections_count));

		cells[it->first.get(0)].push_back(cells_faces.size()-1);
		cells[it->first.get(1)].push_back(cells_faces.size()-1);
	}

	std::cout << "from pymol.cgo import *\nfrom pymol import cmd\nobj = [\n";
	for(std::size_t i=0;i<atoms_selection.size();i++)
	{
		std::size_t atom_number=atoms_selection[i];
		if(atom_number>=atoms.size())
		{
			std::cerr << "Invalid atom number: " << atom_number << "\n";
		}
		print_cell(atoms[atom_number], cells_faces, cells[atom_number]);
	}
	std::cout << "]\n";
	std::cout << "cmd.load_cgo(obj, 'cgo01')\n";
}
