#ifndef PROTEIN_NUCLEOTIDE_PLANE_H_
#define PROTEIN_NUCLEOTIDE_PLANE_H_

#include "atom.h"
#include "residue_ids_collection.h"

namespace protein
{

template<typename PointType>
struct NucleotidePlane
{
	PointType p;
	PointType n;

	friend std::ostream& operator<<(std::ostream &output, const NucleotidePlane& plane)
	{
		output << plane.p.x << " " << plane.p.y << " " << plane.p.z << " ";
		output << plane.n.x << " " << plane.n.y << " " << plane.n.z;
		return output;
	}

	friend std::istream& operator>>(std::istream& input, NucleotidePlane& plane)
	{
		input >> plane.p.x >> plane.p.y >> plane.p.z;
		input >> plane.n.x >> plane.n.y >> plane.n.z;
		return input;
	}

	static std::map<ResidueID, NucleotidePlane> calc_nucleotides_planes(const std::vector<Atom>& atoms)
	{
		std::map<ResidueID, NucleotidePlane> planes;
		const std::map<ResidueID, std::vector<std::size_t> > residue_ids_atoms=group_atoms_indices_by_residue_ids(atoms);
		for(std::map<ResidueID, std::vector<std::size_t> >::const_iterator it=residue_ids_atoms.begin();it!=residue_ids_atoms.end();++it)
		{
			const ResidueID& residue_id=it->first;
			const std::vector<std::size_t> atoms_ids=it->second;
			if(!atoms_ids.empty())
			{
				if(atoms.at(atoms_ids.front()).molecule_class==Atom::nucleotide)
				{
					std::vector<std::size_t> abc;
					for(std::size_t i=0;i<atoms_ids.size() && abc.size()<3;i++)
					{
						const std::size_t a_id=atoms_ids[i];
						const Atom& a=atoms.at(a_id);
						if(a.atom_name=="C2" || a.atom_name=="C4" || a.atom_name=="C6")
						{
							abc.push_back(a_id);
						}
					}
					if(abc.size()==3)
					{
						const Atom& a=atoms[abc[0]];
						const Atom& b=atoms[abc[1]];
						const Atom& c=atoms[abc[2]];
						const PointType pa(a.x, a.y, a.z);
						const PointType pb(b.x, b.y, b.z);
						const PointType pc(c.x, c.y, c.z);
						NucleotidePlane& plane=planes[residue_id];
						plane.p=((pa+pb)+pc)/3.0;
						plane.n=((pb-pa)&(pc-pa)).unit();
					}
				}
			}
		}
		return planes;
	}
};

}

#endif /* PROTEIN_NUCLEOTIDE_PLANE_H_ */
