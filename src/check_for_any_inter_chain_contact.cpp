#include <iostream>
#include <fstream>
#include <tr1/memory>

#include "protein/atom.h"
#include "protein/atoms_reading.h"

#include "apollo/spheres_hierarchy.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

protein::VanDerWaalsRadiusAssigner construct_radius_assigner(const std::string& radius_classes_file_name, const std::string& radius_members_file_name);

void check_for_any_inter_chain_contact(const auxiliaries::CommandLineOptions& clo)
{
	typedef std::map< std::string, std::vector<apollo::SimpleSphere> > ChainsMap;
	typedef apollo::SpheresHierarchy<apollo::SimpleSphere> Hierarchy;
	typedef std::tr1::shared_ptr<Hierarchy> HierarchyPtr;
	typedef std::map< std::string, HierarchyPtr > HierarchiesMap;

	clo.check_allowed_options("--probe: --direct-reading --read-heteroatoms");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;

	ChainsMap chains;

	if(clo.isopt("--direct-reading"))
	{
		const bool read_heteroatoms=clo.isopt("--read-heteroatoms");
		const protein::VanDerWaalsRadiusAssigner radius_assigner=construct_radius_assigner("", "");
		while(std::cin.good())
		{
			std::string filename;
			std::cin >> filename;
			if(!filename.empty())
			{
				std::ifstream input(filename.c_str());
				if(input.good())
				{
					const std::vector<protein::Atom> atoms=protein::AtomsReading::read_atoms_from_PDB_file_stream(input, radius_assigner, read_heteroatoms, false);
					if(!atoms.empty())
					{
						std::vector<apollo::SimpleSphere>& chain_spheres=chains[filename];
						chain_spheres.clear();
						chain_spheres.reserve(atoms.size());
						for(std::size_t i=0;i<atoms.size();i++)
						{
							chain_spheres.push_back(apollo::custom_sphere_from_point<apollo::SimpleSphere>(atoms[i], atoms[i].r+probe_radius));
						}
					}
					else
					{
						std::cerr << "Could not find any atoms in file: " << filename << "\n";
					}
				}
				else
				{
					std::cerr << "Could not open input file: " << filename << "\n";
				}
			}
		}
	}
	else
	{
		std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", true);

		while(auxiliaries::check_file_header(std::cin, "atoms"))
		{
			std::vector<protein::Atom> more_atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "more atoms", "", true);
			atoms.insert(atoms.end(), more_atoms.begin(), more_atoms.end());
		}

		for(std::size_t i=0;i<atoms.size();i++)
		{
			chains[atoms[i].chain_id].push_back(apollo::custom_sphere_from_point<apollo::SimpleSphere>(atoms[i], atoms[i].r+probe_radius));
		}
	}

	std::vector< std::pair<std::string, std::string> > pairs_of_interacting_chains;

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
				bounding_sphere.r=std::max(bounding_sphere.r, apollo::maximal_distance_from_point_to_sphere(center, it->second[i]));
			}
			bounding_spheres[it->first]=bounding_sphere;
		}

		HierarchiesMap hierarchies;
		for(ChainsMap::const_iterator it=chains.begin();it!=chains.end();++it)
		{
			ChainsMap::const_iterator jt=it;
			++jt;
			for(;jt!=chains.end();++jt)
			{
				const std::pair<std::string, std::string> chains_names=((it->second.size())>=(jt->second.size()) ? std::make_pair(it->first, jt->first) : std::make_pair(jt->first, it->first));
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

					bool found_interaction=false;
					for(std::size_t i=0;i<checkable_spheres.size() && !found_interaction;i++)
					{
						const apollo::SimpleSphere& checkable_sphere=checkable_spheres[i];
						if(apollo::sphere_intersects_sphere(checkable_sphere, boinding_sphere_second) &&
								(!(hierarchy_ptr->find_any_collision(checkable_sphere).empty())))
						{
							pairs_of_interacting_chains.push_back(std::make_pair(it->first, jt->first));
							found_interaction=true;
						}
					}
				}
			}
		}
	}

	std::cout << "pairs_of_interacting_structures\n";
	std::cout << pairs_of_interacting_chains.size() << "\n";
	for(std::size_t i=0;i<pairs_of_interacting_chains.size();i++)
	{
		std::cout << pairs_of_interacting_chains[i].first << " " << pairs_of_interacting_chains[i].second << "\n";
	}
}
