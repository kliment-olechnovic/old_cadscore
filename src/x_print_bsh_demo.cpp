#include <cstdlib>

#include "protein/atom.h"

#include "apollo2/bounding_spheres_hierarchy.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/opengl_printer.h"

void x_print_bsh_demo(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--bsi-init-radius: --max-level:");

	const double bsi_init_radius=clo.isopt("--bsi-init-radius") ? clo.arg_with_min_value<double>("--bsi-radius", 1) : 3.5;
	const std::size_t max_level=clo.isopt("--max-level") ? clo.arg_with_min_value<std::size_t>("--max-level", 0) : 0;

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);
	const apollo2::BoundingSpheresHierarchy<protein::Atom> bsh(atoms, bsi_init_radius, 1);

	if(bsh.levels()>0)
	{
		std::cout << "from pymol.cgo import *\n";
		std::cout << "from pymol import cmd\n\n";

		for(std::size_t l=0;l<bsh.levels() && l<=max_level;l++)
		{
			const std::vector<apollo2::SimpleSphere> bs=bsh.collect_bounding_spheres(l);

			std::vector<unsigned int> bs_color_codes(bs.size(), 0xFFFFFF);
			std::vector<unsigned int> atoms_color_codes(atoms.size(), 0xFFFFFF);
			for(std::size_t i=0;i<bs.size();i++)
			{
				bs_color_codes[i]=rand()%(0xFFFFFF);
				for(std::size_t j=0;j<atoms.size();j++)
				{
					if(apollo2::sphere_contains_sphere(bs[i], atoms[j]))
					{
						atoms_color_codes[j]=bs_color_codes[i];
					}
				}
			}

			{
				std::ostringstream obj_name;
				obj_name << "obj_as" << l;
				std::ostringstream cgo_name;
				cgo_name << "cgo_as" << l;
				auxiliaries::OpenGLPrinter opengl_printer(obj_name.str(), cgo_name.str());
				for(std::size_t i=0;i<atoms.size();i++)
				{
					opengl_printer.print_sphere(atoms[i], auxiliaries::Color::from_code(atoms_color_codes[i]));
				}
			}

			{
				std::ostringstream obj_name;
				obj_name << "obj_bs" << l;
				std::ostringstream cgo_name;
				cgo_name << "cgo_bs" << l;
				auxiliaries::OpenGLPrinter opengl_printer(obj_name.str(), cgo_name.str());
				for(std::size_t i=0;i<bs.size();i++)
				{
					opengl_printer.print_sphere(bs[i], auxiliaries::Color::from_code(bs_color_codes[i]));
				}
			}
		}
	}
}

