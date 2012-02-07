#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../protein/atoms_reading.h"
#include "../protein/atoms_classification.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"

#include "../resources/vdwr.h"

protein::VanDerWaalsRadiusAssigner construct_radius_assigner(const std::string& radius_classes_file_name, const std::string& radius_members_file_name)
{
	std::istream* radius_classes_stream=0;
	if(radius_classes_file_name.empty())
	{
		radius_classes_stream=new std::istringstream(std::string(reinterpret_cast<const char*>(vdwr_classes_txt), vdwr_classes_txt_len));
	}
	else
	{
		radius_classes_stream=new std::ifstream(radius_classes_file_name.c_str());
	}

	std::istream* radius_members_stream=0;
	if(radius_members_file_name.empty())
	{
		radius_members_stream=new std::istringstream(std::string(reinterpret_cast<const char*>(vdwr_members_txt), vdwr_members_txt_len));
	}
	else
	{
		radius_members_stream=new std::ifstream(radius_members_file_name.c_str());
	}

	const protein::VanDerWaalsRadiusAssigner radius_assigner(*radius_classes_stream, *radius_members_stream);
	delete radius_classes_stream;
	delete radius_members_stream;
	return radius_assigner;
}

void main_collect_atoms(const auxiliaries::CommandLineOptions& clo)
{
	const std::string radius_classes_file_name=clo.arg_or_default_value<std::string>("--radius-classes", "");
	const std::string radius_members_file_name=clo.arg_or_default_value<std::string>("--radius-members", "");
	const int include_heteroatoms=clo.arg<int>("--include-heteroatoms", 0, 1);
	const int include_water=clo.arg<int>("--include-water", 0, 1);
	const int force_one_chain=clo.arg_or_default_value<int>("--force-one-chain", 0);

	const protein::VanDerWaalsRadiusAssigner radius_assigner=construct_radius_assigner(radius_classes_file_name, radius_members_file_name);

	std::vector<protein::Atom> atoms=protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, (include_heteroatoms==1), (include_water==1));

	if(force_one_chain>0)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].chain_id="X";
		}
	}

	protein::AtomsClassification::classify_atoms(atoms);

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(atoms);
}
