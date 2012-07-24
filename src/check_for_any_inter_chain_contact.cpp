#include <iostream>
#include <tr1/memory>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void check_for_any_inter_chain_contact(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map< std::string, std::vector<apollo::SimpleSphere> > ChainsMap;
	typedef apollo::SpheresHierarchy<apollo::SimpleSphere> Hierarchy;
	typedef std::tr1::shared_ptr<Hierarchy> HierarchyPtr;
	typedef std::map< std::string, HierarchyPtr > HierarchiesMap;

	clo.check_allowed_options("--mode: --probe:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;

	auxiliaries::assert_file_header(std::cin, "atoms");
	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);
	while(auxiliaries::check_file_header(std::cin, "atoms"))
	{
		std::vector<protein::Atom> more_atoms=auxiliaries::read_vector<protein::Atom>(std::cin);
		atoms.insert(atoms.end(), more_atoms.begin(), more_atoms.end());
	}

	ChainsMap chains;
	for(std::size_t i=0;i<atoms.size();i++)
	{
		chains[atoms[i].chain_id].push_back(apollo::custom_sphere_from_point<apollo::SimpleSphere>(atoms[i], atoms[i].r+probe_radius));
	}

	if(chains.size()>1)
	{
		HierarchiesMap hierarchies;
		for(ChainsMap::const_iterator it=chains.begin();it!=chains.end();++it)
		{
			hierarchies[it->first].reset(new Hierarchy(it->second, 5.6, 1));
		}
		for(HierarchiesMap::const_iterator it=hierarchies.begin();it!=hierarchies.end();++it)
		{
			HierarchiesMap::const_iterator jt=it;
			++jt;
			if(jt!=hierarchies.end())
			{
				const Hierarchy* a=it->second.get();
				const Hierarchy* b=jt->second.get();
				if((b->spheres().size())<(a->spheres().size()))
				{
					std::swap(a, b);
				}
				for(std::size_t i=0;i<(a->spheres().size());i++)
				{
					if(!(b->find_any_collision(a->spheres()[i]).empty()));
					{
						std::cout << "yes\n";
						return;
					}
				}
			}
		}
	}

	std::cout << "no\n";
}
