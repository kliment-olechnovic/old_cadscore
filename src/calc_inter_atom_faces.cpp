#include <iostream>

#include "protein/atom.h"

#include "apollo2/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "contacto/inter_atom_contact.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/set_io.h"

void calc_inter_atom_faces(const auxiliaries::CommandLineOptions& clo)
{

	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--probe: --step: --projections:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.7;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	if(atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	const apollo2::ApolloniusTriangulation::PairsNeighborsMap pairs_neighbours_map=apollo2::ApolloniusTriangulation::collect_pairs_neighbors_map_from_quadruples_map(apollo2::ApolloniusTriangulation::construct(atoms, 3.5, false).quadruples_map);

	std::set<contacto::InterAtomContact> inter_atom_contacts;

	for(apollo2::ApolloniusTriangulation::PairsNeighborsMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const std::size_t a_id=it->first.get(0);
		const std::size_t b_id=it->first.get(1);

		const protein::Atom& a=atoms[a_id];
		const protein::Atom& b=atoms[b_id];

		std::vector<const protein::Atom*> cs;
		cs.reserve(it->second.size());
		for(apollo2::ApolloniusTriangulation::PairsNeighborsMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
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

	if(atoms.empty() || inter_atom_contacts.empty())
	{
		throw std::runtime_error("No inter-atom faces constructed");
	}
	else
	{
		auxiliaries::print_vector(std::cout, "atoms", atoms);
		auxiliaries::print_set(std::cout, "contacts", inter_atom_contacts);
	}
}
