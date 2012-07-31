#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "protein/atoms_reading.h"
#include "protein/atoms_classification.h"
#include "protein/residue_ids_collection.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

#include "resources/vdwr.h"

protein::VanDerWaalsRadiusAssigner construct_radius_assigner(const std::string& radius_classes_file_name, const std::string& radius_members_file_name)
{
	std::istream* radius_classes_stream=0;
	if(radius_classes_file_name.empty())
	{
		radius_classes_stream=new std::istringstream(std::string(reinterpret_cast<const char*>(vdwr_classes), vdwr_classes_len));
	}
	else
	{
		radius_classes_stream=new std::ifstream(radius_classes_file_name.c_str());
	}

	std::istream* radius_members_stream=0;
	if(radius_members_file_name.empty())
	{
		radius_members_stream=new std::istringstream(std::string(reinterpret_cast<const char*>(vdwr_members), vdwr_members_len));
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
	clo.check_allowed_options("--radius-classes: --radius-members: --HETATM --HOH --no-classification --rename-chain:");

	std::string radius_classes_file_name="";
	std::string radius_members_file_name="";
	if(clo.isopt("--radius-classes") || clo.isopt("--radius-members"))
	{
		radius_classes_file_name=clo.arg<std::string>("--radius-classes");
		radius_members_file_name=clo.arg<std::string>("--radius-members");
	}

	const bool include_heteroatoms=clo.isopt("--HETATM");
	const bool include_water=clo.isopt("--HOH");
	const bool no_classification=clo.isopt("--no-classification");
	const std::string chain_renaming=clo.isopt("--rename-chain") ? clo.arg<std::string>("--rename-chain") : std::string("");

	const protein::VanDerWaalsRadiusAssigner radius_assigner=construct_radius_assigner(radius_classes_file_name, radius_members_file_name);

	std::vector<protein::Atom> atoms=protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, include_heteroatoms, include_water);

	if(!no_classification)
	{
		protein::AtomsClassification::classify_atoms(atoms);
	}

	if(!chain_renaming.empty())
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].chain_id=chain_renaming;
		}
	}

	if(!atoms.empty())
	{
		auxiliaries::print_file_header(std::cout, "atoms");
		auxiliaries::print_vector(std::cout, atoms);
	}
}

void collect_residue_ids(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids=protein::collect_residue_ids_from_atoms(atoms);

	if(!residue_ids.empty())
	{
		auxiliaries::print_file_header(std::cout, "residue_ids");
		auxiliaries::print_map(std::cout, residue_ids, false);
	}
}

void filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_of_model=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms_of_target=auxiliaries::read_vector<protein::Atom>(std::cin);
	const std::map<protein::ResidueID, protein::ResidueSummary> residue_ids_of_target=protein::collect_residue_ids_from_atoms(atoms_of_target);

	std::vector<protein::Atom> result;
	result.reserve(atoms_of_model.size());
	for(std::size_t i=0;i<atoms_of_model.size();i++)
	{
		const protein::Atom& atom=atoms_of_model[i];
		std::map<protein::ResidueID, protein::ResidueSummary>::const_iterator it=residue_ids_of_target.find(protein::ResidueID::from_atom(atom));
		if(it!=residue_ids_of_target.end())
		{
			if(atom.residue_name==it->second.name)
			{
				result.push_back(atom);
			}
			else
			{
				std::ostringstream output;
				output << "Model atom chain name and residue number matched the target, but model atom residue name did not: " << atom.string_for_human_reading();
				throw std::runtime_error(output.str());
			}
		}
	}

	if(!result.empty())
	{
		auxiliaries::print_file_header(std::cout, "atoms");
		auxiliaries::print_vector(std::cout, result);
	}
}
