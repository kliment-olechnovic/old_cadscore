#include <iostream>
#include <limits>

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
	output.precision(std::numeric_limits<double>::digits10);
	output << std::fixed << a.x << ", " << a.y << ", " << a.z;
	return output.str();
}

void print_tringle_fan(const std::vector<apollo::SimplePoint>& mesh_vertices, const apollo::SimplePoint& normal)
{
	if(!mesh_vertices.empty())
	{
		std::cout << "    BEGIN, TRIANGLE_FAN,\n";
		std::cout << "    COLOR, 1.0, 1.0, 0.0,\n";
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

	std::cout << "from pymol.cgo import *\nfrom pymol import cmd\nobj = [\n";
	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const protein::Atom& a=atoms[it->first.get(0)];
		const protein::Atom& b=atoms[it->first.get(1)];

		if(a.chain_id!=b.chain_id)
		{
			std::vector<const protein::Atom*> cs;
			cs.reserve(it->second.size());
			for(Apollo::PairsNeighboursMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
			{
				cs.push_back(&(atoms[*jt]));
			}

			const CellFace cell_face=CellFace::construct(a, b, cs, probe_radius, step_length, projections_count);
			const apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(a, b).unit();
			print_tringle_fan(cell_face.mesh_vertices(), normal);

		}
	}
	std::cout << "]\ncmd.load_cgo(obj, 'cgo01')\n";
}
