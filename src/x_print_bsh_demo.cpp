#include "protein/atom.h"

#include "apollo2/bounding_spheres_hierarchy.h"
#include "apollo2/opengl_printer.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/std_containers_io.h"
#include "auxiliaries/color.h"

void x_print_bsh_demo(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--bsi-init-radius: --max-level:");

	const double bsi_init_radius=clo.isopt("--bsi-init-radius") ? clo.arg_with_min_value<double>("--bsi-radius", 1) : 3.5;
	const std::size_t max_level=clo.isopt("--max-level") ? clo.arg_with_min_value<std::size_t>("--max-level", 0) : 0;

	const std::vector<protein::Atom> atoms=auxiliaries::STDContainersIO::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);
	const apollo2::BoundingSpheresHierarchy<protein::Atom> bsh(atoms, bsi_init_radius, 1);

	if(bsh.levels()>0)
	{
		apollo2::OpenGLPrinter::print_setup(std::cout);

		{
			std::ostringstream obj_name;
			obj_name << "obj_as";
			std::ostringstream cgo_name;
			cgo_name << "cgo_as";
			apollo2::OpenGLPrinter opengl_printer(std::cout, obj_name.str(), cgo_name.str());
			for(std::size_t i=0;i<atoms.size();i++)
			{
				opengl_printer.print_color(auxiliaries::Color::from_code(0x36BBCE));
				opengl_printer.print_sphere(atoms[i]);
			}
		}

		for(std::size_t l=0;l<bsh.levels() && l<=max_level;l++)
		{
			const std::vector<apollo2::SimpleSphere> bs=bsh.collect_bounding_spheres(l);
			std::ostringstream obj_name;
			obj_name << "obj_bs" << l;
			std::ostringstream cgo_name;
			cgo_name << "cgo_bs" << l;
			apollo2::OpenGLPrinter opengl_printer(std::cout, obj_name.str(), cgo_name.str());
			for(std::size_t i=0;i<bs.size();i++)
			{
				opengl_printer.print_color(auxiliaries::Color::from_code(0x37DE6A));
				opengl_printer.print_sphere(bs[i]);
			}
		}
	}
}

