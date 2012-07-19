#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "contacto/inter_atom_contact.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/set_io.h"

void calc_inter_atom_faces(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--mode: --probe: --step: --projections:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.7;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::set<contacto::InterAtomContact> inter_atom_contacts;

	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const std::size_t a_id=it->first.get(0);
		const std::size_t b_id=it->first.get(1);

		const protein::Atom& a=atoms[a_id];
		const protein::Atom& b=atoms[b_id];

		std::vector<const protein::Atom*> cs;
		cs.reserve(it->second.size());
		for(Apollo::PairsNeighboursMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
		{
			cs.push_back(&(atoms[*jt]));
		}

		const double face_area=CellFace::construct(a, b, cs, probe_radius, step_length, projections_count).area();
		if(apollo::greater(face_area, 0.0))
		{
			inter_atom_contacts.insert(contacto::InterAtomContact(a_id, b_id, face_area));
			inter_atom_contacts.insert(contacto::InterAtomContact(b_id, a_id, face_area));
		}
	}

	if(!atoms.empty() && !inter_atom_contacts.empty())
	{
		auxiliaries::print_file_header(std::cout, "atoms");
		auxiliaries::print_vector(std::cout, atoms);

		auxiliaries::print_file_header(std::cout, "contacts");
		auxiliaries::print_set(std::cout, inter_atom_contacts);
	}
}
