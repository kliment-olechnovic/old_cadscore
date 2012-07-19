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
	output << a.x << " " << a.y << " " << a.z;
	return output.str();
}

template<typename SphereType>
void print_sphere(const SphereType& a)
{
	std::cout << "sphere center " << point_to_string(a) << " radius " << a.r << " quality 3\n";
}

template<typename SphereType>
void print_sphere_scale(const SphereType& a)
{
	std::cout << "scale " << point_to_string(a) << " " << (a.r*2) << "\n";
}

template<typename PointType>
void print_segment(const PointType& a, const PointType& b)
{
	std::cout << "line start " << point_to_string(a) << " end " << point_to_string(b) << "\n";
}

template<typename ListOfPointsType>
void print_contour(const ListOfPointsType& contour)
{
	if(!contour.empty())
	{
		for(typename ListOfPointsType::const_iterator it=contour.begin();it!=contour.end();++it)
		{
			typename ListOfPointsType::const_iterator jt=it;
			++jt;
			if(jt!=contour.end())
			{
				print_segment(*it, *jt);
			}
		}
		print_segment(contour.back(), contour.front());
	}
}

void print_mesh(const std::vector<apollo::SimplePoint>& mesh_vertices, const std::vector<apollo::Triple>& mesh_triples, const apollo::SimplePoint& normal)
{
	for(std::size_t i=0;i<mesh_triples.size();i++)
	{
		const apollo::Triple& t=mesh_triples[i];
		std::cout << "triangle a " << point_to_string(mesh_vertices[t.get(0)])
				<< " b " << point_to_string(mesh_vertices[t.get(1)])
				<< " c " << point_to_string(mesh_vertices[t.get(2)])
				<< " normal " << point_to_string(normal)
				<< "\n";
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

		std::cout << "ncolor 100\n";
		print_sphere(a);

		std::cout << "ncolor 200\n";
		print_contour(cf.contour_points());

		apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(cf.s2(), cf.s1()).unit();
		if(apollo::spheres_equal(cf.s2(), a))
		{
			normal=apollo::inverted_point<apollo::SimplePoint>(normal);
		}

		std::cout << "ncolor 300\n";
		print_mesh(cf.mesh_vertices(), cf.mesh_triples(), normal);
	}
	print_sphere_scale(a);
}

void calc_hyperbolic_cells_faces(const auxiliaries::CommandLineOptions& clo)
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

	for(std::size_t i=0;i<atoms_selection.size();i++)
	{
		std::size_t atom_number=atoms_selection[i];
		if(atom_number>=atoms.size())
		{
			std::cerr << "Invalid atom number: " << atom_number << "\n";
		}
		print_cell(atoms[atom_number], cells_faces, cells[atom_number]);
	}
}
