#include <iostream>
#include <map>

#include "protein/atom.h"
#include "protein/nucleotide_plane.h"

#include "apollo/points_basic_operations.h"

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
				opengl_printer.print_cylinder(plane.point, plane.point+plane.normal, 3.0, auxiliaries::Color::from_code(0xFF0000), auxiliaries::Color::from_code(0x0000FF));
			}
		}
		else
		{
			auxiliaries::print_map(std::cout, "nucleotides_planes", nucleotides_planes, false);
		}
	}
}
