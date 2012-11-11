#include "protein/atom.h"
#include "protein/nucleotide_plane.h"

#include "apollo/spheres_basic_operations.h"

#include "contacto/contact_id.h"
#include "contacto/inter_residue_contact_areas.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

void categorize_inter_nucleotide_side_chain_contacts(const auxiliaries::CommandLineOptions& clo)
{
	typedef protein::NucleotidePlane<apollo::SimplePoint> Plane;

	clo.check_allowed_options("");

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=
			auxiliaries::read_map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >(std::cin, "inter-residue contacts", "residue_contacts", false);

	const std::map< protein::ResidueID, Plane > nucleotides_planes=Plane::calc_nucleotides_planes(atoms);

	const std::map< protein::ResidueID, std::vector<std::size_t> > residue_ids_atoms=protein::group_atoms_indices_by_residue_ids(atoms);

	for(std::map< contacto::ContactID<protein::ResidueID>, contacto::InterResidueContactAreas >::iterator it=inter_residue_contacts.begin();it!=inter_residue_contacts.end();++it)
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
					if(one_halfspace)
					{
						if(first_halfspace<0)
						{
							it->second.areas["rna_stacking_down"]=area;
						}
						else
						{
							it->second.areas["rna_stacking_up"]=area;
						}
						it->second.areas["rna_stacking"]=area;
					}
					else
					{
						it->second.areas["rna_siding"]=area;
					}
				}
			}
		}
	}

	auxiliaries::print_map(std::cout, "residue_contacts", inter_residue_contacts, true);
}
