#include <iostream>
#include <map>

#include "protein/atom.h"
#include "protein/nucleotide_plane.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/opengl_printer.h"

void print_nucleotides_planes(const auxiliaries::CommandLineOptions& clo)
{
	typedef protein::NucleotidePlane<apollo::SimplePoint> Plane;
	clo.check_allowed_options("--graphics");

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map< protein::ResidueID, Plane > nucleotides_planes=Plane::calc_nucleotides_planes(atoms);

	if(nucleotides_planes.empty())
	{
		throw std::runtime_error("No nucleotides planes were defined for the provided atoms stream");
	}
	else
	{
		if(clo.isopt("--graphics"))
		{
			std::cout << "from pymol.cgo import *\n";
			const auxiliaries::OpenGLPrinter opengl_printer("obj_np", "cgo_np");
			for(std::map< protein::ResidueID, Plane >::const_iterator it=nucleotides_planes.begin();it!=nucleotides_planes.end();++it)
			{
				const Plane& plane=it->second;
				opengl_printer.print_cylinder(plane.point-plane.normal, plane.point+plane.normal, 3.0, auxiliaries::Color::from_code(0xFF0000), auxiliaries::Color::from_code(0x0000FF));
			}
		}
		else
		{
			auxiliaries::print_map(std::cout, "nucleotides_planes", nucleotides_planes, false);
		}
	}
}

void print_nucleotides_contacts_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;
	typedef protein::NucleotidePlane<apollo::SimplePoint> Plane;

	clo.check_allowed_options("--probe: --only-inter-side-chains");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=0.5;
	const int projections_count=5;
	const bool only_inter_side_chains=clo.isopt("--only-inter-side-chains");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::vector<CellFace> faces_vector;
	std::map< std::pair<std::size_t, std::size_t>, std::size_t > faces_vector_map;
	std::vector< std::pair<std::size_t, std::size_t> > interface_vector;

	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const std::pair<std::size_t, std::size_t> atoms_ids_pair=std::make_pair(it->first.get(0), it->first.get(1));

		const protein::Atom& a=atoms[atoms_ids_pair.first];
		const protein::Atom& b=atoms[atoms_ids_pair.second];

		if(protein::ResidueID::from_atom(a)!=protein::ResidueID::from_atom(b) && (!only_inter_side_chains || (a.location_class==protein::Atom::side_chain && b.location_class==protein::Atom::side_chain)))
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
				interface_vector.push_back(atoms_ids_pair);
				interface_vector.push_back(reversed_atoms_ids_pair);
			}
		}
	}


	const std::map< protein::ResidueID, Plane > nucleotides_planes=Plane::calc_nucleotides_planes(atoms);

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	const auxiliaries::OpenGLPrinter opengl_printer("obj_contacts", "cgo_contacts");
	for(std::size_t i=0;i<interface_vector.size();++i)
	{
		const std::pair<std::size_t, std::size_t> atoms_ids_pair=interface_vector[i];
		const protein::Atom& a=atoms[atoms_ids_pair.first];
		const protein::Atom& b=atoms[atoms_ids_pair.second];
		const CellFace& cell_face=faces_vector[faces_vector_map.find(atoms_ids_pair)->second];
		const apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(b, a).unit();

		auxiliaries::Color color=auxiliaries::Color::from_code(0xFFFF00);

		if(a.location_class==protein::Atom::side_chain && b.location_class==protein::Atom::side_chain)
		{
			const protein::ResidueID a_residue_id=protein::ResidueID::from_atom(a);
			if(nucleotides_planes.count(a_residue_id)>0)
			{
				const Plane& a_plane=nucleotides_planes.find(a_residue_id)->second;
				const int hs=apollo::halfspace_of_sphere(a_plane.point, a_plane.normal, b);
				if(hs==-1)
				{
					color=auxiliaries::Color::from_code(0xFF0000);
				}
				else if(hs==0)
				{
					color=auxiliaries::Color::from_code(0x00FF00);
				}
				else if(hs==1)
				{
					color=auxiliaries::Color::from_code(0x0000FF);
				}
			}
		}

		opengl_printer.print_tringle_fan(cell_face.mesh_vertices(), normal, color);
	}
}
