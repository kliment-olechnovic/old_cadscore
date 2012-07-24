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
		std::map<std::string, apollo::SimpleSphere> bounding_spheres;
		for(ChainsMap::const_iterator it=chains.begin();it!=chains.end();++it)
		{
			apollo::SimplePoint center;
			for(std::size_t i=0;i<it->second.size();i++)
			{
				center=center+apollo::custom_point_from_object<apollo::SimplePoint>(it->second[i]);
			}
			center=center/it->second.size();
			apollo::SimpleSphere bounding_sphere=apollo::custom_sphere_from_point<apollo::SimpleSphere>(center, 0.0);
			for(std::size_t i=0;i<it->second.size();i++)
			{
				bounding_sphere.r=std::max(bounding_sphere.r, apollo::maximal_distance_from_point_to_sphere(center, it->second[i])+probe_radius);
			}
			bounding_spheres[it->first]=bounding_sphere;
		}

		HierarchiesMap hierarchies;
		for(ChainsMap::const_iterator it=chains.begin();it!=chains.end();++it)
		{
			ChainsMap::const_iterator jt=it;
			++jt;
			if(jt!=chains.end())
			{
				if(apollo::sphere_intersects_sphere(bounding_spheres[it->first], bounding_spheres[jt->first]))
				{
					HierarchyPtr& a_ptr=hierarchies[it->first];
					if(a_ptr.get()==0)
					{
						a_ptr.reset(new Hierarchy(it->second, 5.6, 1));
					}
					const Hierarchy* a=a_ptr.get();

					HierarchyPtr& b_ptr=hierarchies[jt->first];
					if(b_ptr.get()==0)
					{
						b_ptr.reset(new Hierarchy(jt->second, 5.6, 1));
					}
					const Hierarchy* b=b_ptr.get();

					if((b->spheres().size())<(a->spheres().size()))
					{
						std::swap(a, b);
					}

					for(std::size_t i=0;i<(a->spheres().size());i++)
					{
						if(!(b->find_any_collision(a->spheres()[i]).empty()))
						{
							std::cout << "yes\n";
							return;
						}
					}
				}
			}
		}
	}

	std::cout << "no\n";
}
