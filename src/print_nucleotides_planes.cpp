#include <iostream>
#include <map>

#include "protein/atom.h"
#include "protein/residue_ids_collection.h"

#include "apollo/points_basic_operations.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/opengl_printer.h"

struct Plane
{
	apollo::SimplePoint p;
	apollo::SimplePoint n;

	friend std::ostream& operator<<(std::ostream &output, const Plane& plane)
	{
		output << plane.p.x << " " << plane.p.y << " " << plane.p.z << " ";
		output << plane.n.x << " " << plane.n.y << " " << plane.n.z;
		return output;
	}

	friend std::istream& operator>>(std::istream &input, Plane &plane)
	{
		input >> plane.p.x >> plane.p.y >> plane.p.z;
		input >> plane.n.x >> plane.n.y >> plane.n.z;
		return input;
	}
};

std::map<protein::ResidueID, Plane> calc_nucleotides_planes(const std::vector<protein::Atom>& atoms)
{
	std::map<protein::ResidueID, Plane> planes;
	const std::map<protein::ResidueID, std::vector<std::size_t> > residue_ids_atoms=group_atoms_indices_by_residue_ids(atoms);
	for(std::map<protein::ResidueID, std::vector<std::size_t> >::const_iterator it=residue_ids_atoms.begin();it!=residue_ids_atoms.end();++it)
	{
		const protein::ResidueID& residue_id=it->first;
		const std::vector<std::size_t> atoms_ids=it->second;
		if(!atoms_ids.empty())
		{
			if(atoms.at(atoms_ids.front()).molecule_class==protein::Atom::nucleotide)
			{
				std::vector<std::size_t> abc;
				for(std::size_t i=0;i<atoms_ids.size() && abc.size()<3;i++)
				{
					const std::size_t a_id=atoms_ids[i];
					const protein::Atom& a=atoms.at(a_id);
					if(a.atom_name=="C2" || a.atom_name=="C4" || a.atom_name=="C6")
					{
						abc.push_back(a_id);
					}
				}
				if(abc.size()==3)
				{
					apollo::SimplePoint pp[3]={
							apollo::custom_point_from_object<apollo::SimplePoint>(atoms[abc[0]]),
							apollo::custom_point_from_object<apollo::SimplePoint>(atoms[abc[1]]),
							apollo::custom_point_from_object<apollo::SimplePoint>(atoms[abc[2]])
					};
					Plane& plane=planes[residue_id];
					plane.p=((pp[0]+pp[1])+pp[2])/3.0;
					plane.n=((pp[1]-pp[0])&(pp[2]-pp[0])).unit();
				}
			}
		}
	}
	return planes;
}

void print_nucleotides_planes(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--graphics");

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const std::map<protein::ResidueID, Plane> nucleotides_planes=calc_nucleotides_planes(atoms);

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
			for(std::map<protein::ResidueID, Plane>::const_iterator it=nucleotides_planes.begin();it!=nucleotides_planes.end();++it)
			{
				const Plane& plane=it->second;
				opengl_printer.print_cylinder(plane.p, plane.p+plane.n, 3.0, auxiliaries::Color::from_code(0xFF0000), auxiliaries::Color::from_code(0x0000FF));
			}
		}
		else
		{
			auxiliaries::print_map(std::cout, "nucleotides_planes", nucleotides_planes, false);
		}
	}
}
