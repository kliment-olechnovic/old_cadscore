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
				const std::pair<std::string, std::string> chains_names=((it->second.size())<=(jt->second.size()) ? std::make_pair(it->first, jt->first) : std::make_pair(jt->first, it->first));
				const apollo::SimpleSphere& boinding_sphere_first=bounding_spheres[chains_names.first];
				const apollo::SimpleSphere& boinding_sphere_second=bounding_spheres[chains_names.second];
				if(apollo::sphere_intersects_sphere(boinding_sphere_first, boinding_sphere_second))
				{
					const std::vector<apollo::SimpleSphere>& checkable_spheres=chains[chains_names.first];
					const std::vector<apollo::SimpleSphere>& reference_spheres=chains[chains_names.second];

					HierarchyPtr& hierarchy_ptr_handle=hierarchies[chains_names.second];
					if(hierarchy_ptr_handle.get()==0)
					{
						hierarchy_ptr_handle.reset(new Hierarchy(reference_spheres, 4.2+probe_radius, 1));
					}
					const Hierarchy* hierarchy_ptr=hierarchy_ptr_handle.get();

					for(std::size_t i=0;i<checkable_spheres.size();i++)
					{
						const apollo::SimpleSphere& checkable_sphere=checkable_spheres[i];
						if(apollo::sphere_intersects_sphere(checkable_sphere, boinding_sphere_second) && (!(hierarchy_ptr->find_any_collision(checkable_sphere).empty())))
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
