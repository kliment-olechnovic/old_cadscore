#include <iostream>
#include <cstdlib>

#include "protein/atom.h"
#include "protein/residue_id.h"

#include "contacto/inter_atom_contact.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

void summarize_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	auxiliaries::assert_file_header(std::cin, "contacts");
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=auxiliaries::read_vector<contacto::InterAtomContact>(std::cin);

	std::map<std::string, double> values;
	for(std::size_t i=0;i<inter_atom_contacts.size();i++)
	{
		const contacto::InterAtomContact& contact=inter_atom_contacts[i];
		const protein::Atom& a=atoms.at(contact.a);
		const protein::Atom& b=atoms.at(contact.b);

		if(protein::ResidueID::from_atom(a)!=protein::ResidueID::from_atom(b))
		{
			values["all"]+=contact.area;

			if(a.chain_id!=b.chain_id)
			{
				values[std::string("inter_chain_")+a.chain_id+"_"+b.chain_id]+=contact.area;
			}
		}
		else if(a==b)
		{
			values["sas"]+=contact.area;
		}
	}

	auxiliaries::print_file_header(std::cout, "summary_values");
	auxiliaries::print_map(std::cout, values, false);
}
