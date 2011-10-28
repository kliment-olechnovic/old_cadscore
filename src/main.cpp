#include <iostream>
#include <functional>
#include <exception>

#include "auxiliaries/command_line_options.h"

void main_collect_atoms(const auxiliaries::CommandLineOptions& clo);
void main_collect_residue_ids(const auxiliaries::CommandLineOptions& clo);
void main_construct_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo);
void main_construct_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void main_combine_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo);
void main_calculate_contact_area_difference_profile(const auxiliaries::CommandLineOptions& clo);
void main_calculate_contact_area_difference_global_score(const auxiliaries::CommandLineOptions& clo);
void main_filter_atoms_by_target(const auxiliaries::CommandLineOptions& clo);
void main_construct_apollonius_quadrupalization(const auxiliaries::CommandLineOptions& clo);
void main_calculate_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo);

int main(const int argc, const char** argv)
{
	std::cin.exceptions(std::istream::badbit);
	std::cout.exceptions(std::ostream::badbit);

	try
	{
		const auxiliaries::CommandLineOptions clo(argc, argv);
		const std::string mode=clo.arg<std::string>("--mode");

		typedef std::pointer_to_unary_function<const auxiliaries::CommandLineOptions&, void> ModeFunctionPointer;
		std::map< std::string, ModeFunctionPointer > modes_map;
		modes_map["collect-atoms"]=ModeFunctionPointer(main_collect_atoms);
		modes_map["collect-residue-ids"]=ModeFunctionPointer(main_collect_residue_ids);
		modes_map["construct-inter-atom-contacts"]=ModeFunctionPointer(main_construct_inter_atom_contacts);
		modes_map["construct-inter-residue-contacts"]=ModeFunctionPointer(main_construct_inter_residue_contacts);
		modes_map["combine-inter-residue-contacts"]=ModeFunctionPointer(main_combine_inter_residue_contacts);
		modes_map["calculate-contact-area-difference-profile"]=ModeFunctionPointer(main_calculate_contact_area_difference_profile);
		modes_map["calculate-contact-area-difference-global-score"]=ModeFunctionPointer(main_calculate_contact_area_difference_global_score);
		modes_map["filter-atoms-by-target"]=ModeFunctionPointer(main_filter_atoms_by_target);
		modes_map["construct-apollonius-quadrupalization"]=ModeFunctionPointer(main_construct_apollonius_quadrupalization);
		modes_map["calculate-contact-area-difference-local-scores"]=ModeFunctionPointer(main_calculate_contact_area_difference_local_scores);

		if(modes_map.count(mode)==1)
		{
			modes_map.find(mode)->second(clo);
		}
		else
		{
			std::cerr << "Unknown mode. Available modes are:" << std::endl;
			for(std::map< std::string, ModeFunctionPointer >::const_iterator it=modes_map.begin();it!=modes_map.end();++it)
			{
				std::cerr << it->first << std::endl;
			}
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
