#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "protein/atoms_reading.h"
#include "protein/atoms_classification.h"
#include "protein/residue_ids_collection.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/set_io.h"

#include "resources/vdwr.h"

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

void collect_atoms(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode: --radius-classes: --radius-members: --HETATM --HOH --force-one-chain");

	std::string radius_classes_file_name="";
	std::string radius_members_file_name="";
	if(clo.isopt("--radius-classes") || clo.isopt("--radius-members"))
	{
		std::string radius_classes_file_name=clo.arg<std::string>("--radius-classes");
		std::string radius_members_file_name=clo.arg<std::string>("--radius-members");
	}

	const bool include_heteroatoms=clo.isopt("--HETATM");
	const bool include_water=clo.isopt("--HOH");
	const bool force_one_chain=clo.isopt("--force-one-chain");

	const protein::VanDerWaalsRadiusAssigner radius_assigner=construct_radius_assigner(radius_classes_file_name, radius_members_file_name);

	std::vector<protein::Atom> atoms=protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, include_heteroatoms, include_water);

	if(force_one_chain)
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

void collect_residue_ids(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode:");

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>();
	const std::set<protein::ResidueID> residue_ids=protein::collect_residue_ids_from_atoms(atoms);
	auxiliaries::print_file_header("residue_ids");
	auxiliaries::print_set(residue_ids);
}

void filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--mode:");

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_1=auxiliaries::read_vector<protein::Atom>();
	std::set<protein::ResidueID> residue_ids_1=protein::collect_residue_ids_from_atoms(atoms_1);

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> atoms_2=auxiliaries::read_vector<protein::Atom>();

	std::vector<protein::Atom> result;
	result.reserve(atoms_2.size());
	for(std::size_t i=0;i<atoms_2.size();i++)
	{
		const protein::Atom& atom=atoms_2[i];
		if(residue_ids_1.count(protein::ResidueID::from_atom(atom))>0)
		{
			result.push_back(atom);
		}
	}

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(result);
}
