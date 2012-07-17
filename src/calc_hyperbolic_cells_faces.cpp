#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

template<typename PointType>
void print_point(const PointType& a)
{
	std::cout << a.x << " " << a.y << " " << a.z << " ";
}

template<typename PointType>
void print_segment(const PointType& a, const PointType& b)
{
	std::cout << "stick start ";
	print_point(a);
	std::cout << "end ";
	print_point(b);
	std::cout << "radius 0.1 quality 1\n";
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

template<typename SphereType>
void print_sphere(const SphereType& a)
{
	std::cout << "sphere center ";
	print_point(a);
	std::cout << "radius " << a.r << " ";
	std::cout << "quality 3\n";
}

template<typename SphereType>
void print_sphere_scale(const SphereType& a)
{
	std::cout << "scale ";
	print_point(a);
	std::cout << (a.r*2) << "\n";
}

void calc_hyperbolic_cells_faces(const auxiliaries::CommandLineOptions& clo)
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

	for(std::map<std::size_t, std::vector<std::size_t> >::const_iterator it=cells.begin();it!=cells.end();++it)
	{
		const protein::Atom& a=atoms[it->first];
		std::cout << "$" << it->first << "\n";
		print_sphere(a);
		const std::vector<std::size_t>& faces=it->second;
		for(std::size_t i=0;i<faces.size();i++)
		{
			print_contour(cells_faces[faces[i]].contour_points());
		}
	}
	print_sphere_scale(atoms[atoms.size()/2]);
}
