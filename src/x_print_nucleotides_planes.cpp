#include <iostream>
#include <map>

#include "protein/atom.h"
#include "protein/nucleotide_plane.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "contacto/contact_id.h"
#include "contacto/inter_residue_contact_areas.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/opengl_printer.h"

void x_print_nucleotides_planes(const auxiliaries::CommandLineOptions& clo)
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
			auxiliaries::OpenGLPrinter opengl_printer("obj_np", "cgo_np");
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

void x_print_nucleotides_contacts_graphics(const auxiliaries::CommandLineOptions& clo)
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

	auxiliaries::OpenGLPrinter opengl_printer("obj_contacts", "cgo_contacts");
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

void x_print_stacking_nucleotides_contacts(const auxiliaries::CommandLineOptions& clo)
{
	typedef protein::NucleotidePlane<apollo::SimplePoint> Plane;

	clo.check_allowed_options("--prefix:");

	const std::string prefix=clo.arg<std::string>("--prefix");

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	const std::map< protein::ResidueID, Plane > nucleotides_planes=Plane::calc_nucleotides_planes(atoms);

	const std::map< protein::ResidueID, std::vector<std::size_t> > residue_ids_atoms=protein::group_atoms_indices_by_residue_ids(atoms);

	std::map< contacto::ContactID<protein::ResidueID>, std::pair<bool, double> > statuses;

	for(std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >::const_iterator it=inter_residue_contacts.begin();it!=inter_residue_contacts.end();++it)
	{
		const double area=it->second.area("SS");
		if(area>0.0)
		{
			const protein::ResidueID& rid_a=it->first.a;
			const protein::ResidueID& rid_b=it->first.b;
			if(nucleotides_planes.count(rid_a)==1 && residue_ids_atoms.count(rid_b)==1)
			{
				const Plane& plane_a=nucleotides_planes.find(rid_a)->second;
				const std::vector<std::size_t> atoms_ids_b=residue_ids_atoms.find(rid_b)->second;
				std::vector<std::size_t> sc_atoms_ids_b;
				for(std::size_t i=0;i<atoms_ids_b.size();i++)
				{
					std::size_t atom_id=atoms_ids_b[i];
					if(atoms[atom_id].molecule_class==protein::Atom::nucleotide && atoms[atom_id].location_class==protein::Atom::side_chain)
					{
						sc_atoms_ids_b.push_back(atom_id);
					}
				}
				if(!sc_atoms_ids_b.empty())
				{
					const int first_halfspace=apollo::halfspace_of_sphere(plane_a.point, plane_a.normal, atoms[sc_atoms_ids_b[0]]);
					bool one_halfspace=(first_halfspace!=0);
					for(std::size_t i=1;i<sc_atoms_ids_b.size() && one_halfspace;i++)
					{
						const int atom_halfspace=apollo::halfspace_of_sphere(plane_a.point, plane_a.normal, atoms[sc_atoms_ids_b[i]]);
						if(atom_halfspace!=first_halfspace)
						{
							one_halfspace=false;
						}
					}
					statuses[contacto::ContactID<protein::ResidueID>(rid_a, rid_b)]=std::make_pair(one_halfspace, area);
				}
			}
		}
	}

	for(std::map< contacto::ContactID<protein::ResidueID>, std::pair<bool, double> >::const_iterator it=statuses.begin();it!=statuses.end();++it)
	{
		const protein::ResidueID& rid_a=it->first.a;
		const protein::ResidueID& rid_b=it->first.b;
		if(rid_a<rid_b)
		{
			const std::pair<bool, double>& status_ab=it->second;
			std::map< contacto::ContactID<protein::ResidueID>, std::pair<bool, double> >::const_iterator jt=statuses.find(contacto::ContactID<protein::ResidueID>(rid_b, rid_a));
			if(jt!=statuses.end())
			{
				const std::pair<bool, double>& status_ba=jt->second;
				std::cout << prefix << " " << ((status_ab.first && status_ba.first) ? "stack" : "side" ) << " " << rid_a.chain_id << " " << rid_a.residue_number << " " << rid_b.chain_id << " " << rid_b.residue_number << " " << status_ab.second << "\n";
			}
		}
	}
}
