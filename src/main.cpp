#include <iostream>
#include <functional>
#include <exception>
#include <limits>

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/stream_redirector.h"

#ifdef USE_VERSION_HEADER
#include "resources/version.h"
#endif

void calc_combined_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_global_scores(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo);
void calc_contact_area_difference_size_scores(const auxiliaries::CommandLineOptions& clo);
void calc_inter_atom_contact_area_difference_score(const auxiliaries::CommandLineOptions& clo);
void calc_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_inter_atom_faces(const auxiliaries::CommandLineOptions& clo);
void calc_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void calc_quadruples(const auxiliaries::CommandLineOptions& clo);
void categorize_inter_nucleotide_side_chain_contacts(const auxiliaries::CommandLineOptions& clo);
void check_for_any_inter_chain_contact(const auxiliaries::CommandLineOptions& clo);
void collect_atoms(const auxiliaries::CommandLineOptions& clo);
void collect_residue_ids(const auxiliaries::CommandLineOptions& clo);
void filter_atoms_by_name(const auxiliaries::CommandLineOptions& clo);
void filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo);
void merge_atoms(const auxiliaries::CommandLineOptions& clo);
void print_inter_chain_interface_dot_graph(const auxiliaries::CommandLineOptions& clo);
void print_inter_chain_interface_graphics(const auxiliaries::CommandLineOptions& clo);
void print_inter_residue_contacts_map_image(const auxiliaries::CommandLineOptions& clo);
void print_inter_residue_distance_map_image(const auxiliaries::CommandLineOptions& clo);
void print_nucleotides_contacts_graphics(const auxiliaries::CommandLineOptions& clo);
void print_nucleotides_planes(const auxiliaries::CommandLineOptions& clo);
void print_optimal_rotation(const auxiliaries::CommandLineOptions& clo);
void print_quadruples_image(const auxiliaries::CommandLineOptions& clo);
void print_sequence_alignment(const auxiliaries::CommandLineOptions& clo);
void print_stacking_nucleotides_contacts(const auxiliaries::CommandLineOptions& clo);
void print_topological_ordering_of_residues(const auxiliaries::CommandLineOptions& clo);
void summarize_dssp_file(const auxiliaries::CommandLineOptions& clo);
void summarize_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo);

int main(const int argc, const char** argv)
{
	std::cin.exceptions(std::istream::badbit);
	std::cout.exceptions(std::ostream::badbit);
	std::ios_base::sync_with_stdio(false);

	std::string mode;

	try
	{
		auxiliaries::CommandLineOptions clo(argc, argv);

		if(clo.isopt("--version"))
		{
#ifdef USE_VERSION_HEADER
			std::cout << resources::get_version_string() << "\n";
#else
			std::cout << "Version information was not included in this executable\n";
#endif
			return 0;
		}

		mode=clo.isarg("--mode") ? clo.arg<std::string>("--mode") : std::string("");
		clo.remove_option("--mode");

		const std::string clog_file=clo.isarg("--clog-file") ? clo.arg<std::string>("--clog-file") : std::string("");
		clo.remove_option("--clog-file");

		auxiliaries::StreamRedirector stream_redirector;
		stream_redirector.init(clog_file);

		typedef std::pointer_to_unary_function<const auxiliaries::CommandLineOptions&, void> ModeFunctionPointer;
		std::map< std::string, ModeFunctionPointer > modes_map;

		modes_map["calc-CAD-global-scores"]=ModeFunctionPointer(calc_contact_area_difference_global_scores);
		modes_map["calc-CAD-local-scores"]=ModeFunctionPointer(calc_contact_area_difference_local_scores);
		modes_map["calc-CAD-profile"]=ModeFunctionPointer(calc_contact_area_difference_profile);
		modes_map["calc-CAD-size-scores"]=ModeFunctionPointer(calc_contact_area_difference_size_scores);
		modes_map["calc-combined-inter-residue-contacts"]=ModeFunctionPointer(calc_combined_inter_residue_contacts);
		modes_map["calc-inter-atom-CAD-score"]=ModeFunctionPointer(calc_inter_atom_contact_area_difference_score);
		modes_map["calc-inter-atom-contacts"]=ModeFunctionPointer(calc_inter_atom_contacts);
		modes_map["calc-inter-atom-faces"]=ModeFunctionPointer(calc_inter_atom_faces);
		modes_map["calc-inter-residue-contacts"]=ModeFunctionPointer(calc_inter_residue_contacts);
		modes_map["calc-quadruples"]=ModeFunctionPointer(calc_quadruples);
		modes_map["categorize-inter-nucleotide-side-chain-contacts"]=ModeFunctionPointer(categorize_inter_nucleotide_side_chain_contacts);
		modes_map["check-for-any-inter-chain-contact"]=ModeFunctionPointer(check_for_any_inter_chain_contact);
		modes_map["collect-atoms"]=ModeFunctionPointer(collect_atoms);
		modes_map["collect-residue-ids"]=ModeFunctionPointer(collect_residue_ids);
		modes_map["filter-atoms-by-name"]=ModeFunctionPointer(filter_atoms_by_name);
		modes_map["filter-atoms-by-target"]=ModeFunctionPointer(filter_atoms_by_target);
		modes_map["merge-atoms"]=ModeFunctionPointer(merge_atoms);
		modes_map["print-inter-chain-interface-dot-graph"]=ModeFunctionPointer(print_inter_chain_interface_dot_graph);
		modes_map["print-inter-chain-interface-graphics"]=ModeFunctionPointer(print_inter_chain_interface_graphics);
		modes_map["print-inter-residue-contacts-map-image"]=ModeFunctionPointer(print_inter_residue_contacts_map_image);
		modes_map["print-inter-residue-distance-map-image"]=ModeFunctionPointer(print_inter_residue_distance_map_image);
		modes_map["print-nucleotides-contacts-graphics"]=ModeFunctionPointer(print_nucleotides_contacts_graphics);
		modes_map["print-nucleotides-planes"]=ModeFunctionPointer(print_nucleotides_planes);
		modes_map["print-optimal-rotation"]=ModeFunctionPointer(print_optimal_rotation);
		modes_map["print-quadruples-image"]=ModeFunctionPointer(print_quadruples_image);
		modes_map["print-sequence-alignment"]=ModeFunctionPointer(print_sequence_alignment);
		modes_map["print-stacking-nucleotides-contacts"]=ModeFunctionPointer(print_stacking_nucleotides_contacts);
		modes_map["print-topological-ordering-of-residues"]=ModeFunctionPointer(print_topological_ordering_of_residues);
		modes_map["summarize-dssp-file"]=ModeFunctionPointer(summarize_dssp_file);
		modes_map["summarize-inter-atom-contacts"]=ModeFunctionPointer(summarize_inter_atom_contacts);

		if(modes_map.count(mode)==1)
		{
			modes_map.find(mode)->second(clo);
		}
		else
		{
			std::cerr << "Unspecified running mode. Available modes are:" << std::endl;
			for(std::map< std::string, ModeFunctionPointer >::const_iterator it=modes_map.begin();it!=modes_map.end();++it)
			{
				std::cerr << "  --mode " << it->first << std::endl;
			}
			return 1;
		}
	}
	catch(const std::exception& e)
	{
		if(mode.empty())
		{
			std::cerr << "Exception was caught: ";
		}
		else
		{
			std::cerr << "Operation '" << mode << "' was not successful because exception was caught: ";
		}
		std::cerr << "[" << (e.what()) << "]" << std::endl;
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
