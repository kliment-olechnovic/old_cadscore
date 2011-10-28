#include <iostream>
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
		if(mode=="collect-atoms")
		{
			main_collect_atoms(clo);
		}
		else if(mode=="collect-residue-ids")
		{
			main_collect_residue_ids(clo);
		}
		else if(mode=="construct-inter-atom-contacts")
		{
			main_construct_inter_atom_contacts(clo);
		}
		else if(mode=="construct-inter-residue-contacts")
		{
			main_construct_inter_residue_contacts(clo);
		}
		else if(mode=="combine-inter-residue-contacts")
		{
			main_combine_inter_residue_contacts(clo);
		}
		else if(mode=="calculate-contact-area-difference-profile")
		{
			main_calculate_contact_area_difference_profile(clo);
		}
		else if(mode=="calculate-contact-area-difference-global-score")
		{
			main_calculate_contact_area_difference_global_score(clo);
		}
		else if(mode=="filter-atoms-by-target")
		{
			main_filter_atoms_by_target(clo);
		}
		else if(mode=="construct-apollonius-quadrupalization")
		{
			main_construct_apollonius_quadrupalization(clo);
		}
		else if(mode=="calculate-contact-area-difference-local-scores")
		{
			main_calculate_contact_area_difference_local_scores(clo);
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
