#include <iostream>
#include <functional>
#include <exception>
#include <limits>

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/stream_redirector.h"

void collect_atoms(const auxiliaries::CommandLineOptions& clo);
void collect_residue_ids(const auxiliaries::CommandLineOptions& clo);
void filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo);
void calc_quadruples(const auxiliaries::CommandLineOptions& clo);
void calc_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_global_scores(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_size_scores(const auxiliaries::CommandLineOptions& clo);
void calc_inter_atom_faces(const auxiliaries::CommandLineOptions& clo);
void summarize_dssp_file(const auxiliaries::CommandLineOptions& clo);
void check_for_any_inter_chain_contact(const auxiliaries::CommandLineOptions& clo);
void print_inter_chain_interface_graphics(const auxiliaries::CommandLineOptions& clo);

int main(const int argc, const char** argv)
{
	std::cin.exceptions(std::istream::badbit);
	std::cout.exceptions(std::ostream::badbit);
	std::ios_base::sync_with_stdio(false);

	try
	{
		auxiliaries::CommandLineOptions clo(argc, argv);

		const std::string mode=clo.arg<std::string>("--mode");
		clo.remove_option("--mode");

		const std::string clog_file=clo.isarg("--clog-file") ? clo.arg<std::string>("--clog-file") : std::string("");
		clo.remove_option("--clog-file");

		auxiliaries::StreamRedirector stream_redirector;
		stream_redirector.init(clog_file);

		typedef std::pointer_to_unary_function<const auxiliaries::CommandLineOptions&, void> ModeFunctionPointer;
		std::map< std::string, ModeFunctionPointer > modes_map;

		modes_map["collect-atoms"]=ModeFunctionPointer(collect_atoms);
		modes_map["collect-residue-ids"]=ModeFunctionPointer(collect_residue_ids);
		modes_map["filter-atoms-by-target"]=ModeFunctionPointer(filter_atoms_by_target);
		modes_map["calc-quadruples"]=ModeFunctionPointer(calc_quadruples);
		modes_map["calc-inter-atom-contacts"]=ModeFunctionPointer(calc_inter_atom_contacts);
		modes_map["calc-inter-residue-contacts"]=ModeFunctionPointer(calc_inter_residue_contacts);
		modes_map["calc-combined-inter-residue-contacts"]=ModeFunctionPointer(calc_combined_inter_residue_contacts);
		modes_map["calc-CAD-profile"]=ModeFunctionPointer(calc_contact_area_difference_profile);
		modes_map["calc-CAD-local-scores"]=ModeFunctionPointer(calc_contact_area_difference_local_scores);
		modes_map["calc-CAD-global-scores"]=ModeFunctionPointer(calc_contact_area_difference_global_scores);
		modes_map["calc-CAD-size-scores"]=ModeFunctionPointer(calc_contact_area_difference_size_scores);
		modes_map["calc-inter-atom-faces"]=ModeFunctionPointer(calc_inter_atom_faces);
		modes_map["summarize-dssp-file"]=ModeFunctionPointer(summarize_dssp_file);
		modes_map["check-for-any-inter-chain-contact"]=ModeFunctionPointer(check_for_any_inter_chain_contact);
		modes_map["print-inter-chain-interface-graphics"]=ModeFunctionPointer(print_inter_chain_interface_graphics);

		if(modes_map.count(mode)==1)
		{
			modes_map.find(mode)->second(clo);
		}
		else
		{
			std::cerr << "Unknown mode. Available modes are:" << std::endl;
			for(std::map< std::string, ModeFunctionPointer >::const_iterator it=modes_map.begin();it!=modes_map.end();++it)
			{
				std::cerr << "--mode " << it->first << std::endl;
			}
			return 1;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "STD exception caught: " << (e.what()) << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "Unknown exception caught" << std::endl;
		return 1;
	}

	std::cin.ignore(std::numeric_limits<std::streamsize>::max());

	return 0;
}
