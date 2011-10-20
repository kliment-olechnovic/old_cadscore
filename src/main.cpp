#include <iostream>
#include <exception>

#include "auxiliaries/command_line_options.h"

void main_construct_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo);
void main_construct_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void main_combine_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);

int main(const int argc, const char** argv)
{
	std::cin.exceptions(std::istream::badbit);
	std::cout.exceptions(std::ostream::badbit);

	try
	{
		const auxiliaries::CommandLineOptions clo(argc, argv);
		if(clo.arg<std::string>("--mode")=="ciac")
		{
			main_construct_inter_atom_contacts(clo);
		}
		else if(clo.arg<std::string>("--mode")=="circ")
		{
			main_construct_inter_residue_contacts(clo);
		}
		else if(clo.arg<std::string>("--mode")=="cmirc")
		{
			main_combine_inter_residue_contacts(clo);
		}
		else
		{
			std::cerr << "Unknown mode" << std::endl;
			return -1;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "STD exception caught: " << (e.what()) << std::endl;
		return -1;
	}
	catch(...)
	{
		std::cerr << "Unknown exception caught" << std::endl;
		return -1;
	}

	return 0;
}
