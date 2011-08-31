/****************************************************************************
 **
 ** Copyright (C) Kliment Olechnovic, Laboratory of Bioinformatics, Institute of Biotechnology, Vilnius, Lithuania.
 ** All rights reserved.
 ** Contact: Kliment Olechnovic (kliment@ibt.lt)
 **
 ** This file is part of Contactus.

 ** Contactus is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.

 ** Contactus is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with Contactus.  If not, see <http://www.gnu.org/licenses/>.
 **
****************************************************************************/

#include "atoms.h"

#include "utilities.h"
#include "../../../contactus/src/baltymus/prot/AtomDescription.h"
#include "../../../contactus/src/baltymus/prot/pdb_parsing.h"
#include "../../../contactus/src/baltymus/geometry/spheres_containment.h"

struct Implementation
{
	static std::vector<Atom> gather_atoms(std::istream& pdb_file)
	{
		const prot::VanDerWaalsRadiusAssigner vdwr_assigner("resources/vdwr_classes.txt", "resources/vdwr_members.txt");
		return gather_atoms(gather_atoms_descriptions(pdb_file, vdwr_assigner));
	}

	static std::vector<Atom> gather_atoms(const std::vector<prot::AtomDescription>& atoms_descriptions)
	{
		std::vector<Atom> atoms;
		atoms.reserve(atoms_descriptions.size());
		for(std::size_t i=0;i<atoms_descriptions.size();i++)
		{
			const prot::AtomDescription& ad=atoms_descriptions[i];
			Atom a;
			a.chain_id=ad.chain_name;
			a.residue_number=convert_string<int>(ad.residue_serial);
			a.residue_name=ad.residue_name;
			a.atom_name=ad.atom_name;
			a.x=ad.coordinates.x;
			a.y=ad.coordinates.y;
			a.z=ad.coordinates.z;
			a.r=ad.radius_VDW;
			atoms.push_back(a);
		}
		return atoms;
	}

	static std::vector<prot::AtomDescription> gather_atoms_descriptions(std::istream& pdb_file, const prot::VanDerWaalsRadiusAssigner& vdwr_assigner)
	{
		const std::vector<prot::AtomDescription> original_atoms=prot::AtomDescription::gather_descriptions(prot::pdb_parsing::chains_from_PDB_file_stream(pdb_file, false, false, NULL), vdwr_assigner, false, false, NULL);
		return select_free_protein_atoms_descriptions(original_atoms, geometry::set_of_excludable_ids_from_containment(geometry::spheres_containment(prot::AtomDescription::get_spheres_VDW(original_atoms))));
	}

	static std::vector<prot::AtomDescription> select_free_protein_atoms_descriptions(const std::vector<prot::AtomDescription>& protein_atoms_descriptions, const std::set<std::size_t>& set_to_exclude)
	{
		std::vector<prot::AtomDescription> cleaned;
		for(std::size_t i=0;i<protein_atoms_descriptions.size();i++)
		{
			if(set_to_exclude.find(i)==set_to_exclude.end())
			{
				cleaned.push_back(protein_atoms_descriptions[i]);
			}
		}
		return cleaned;
	}
};

std::vector<Atom> read_atoms_from_PDB_file_stream(std::istream& pdb_file)
{
	return Implementation::gather_atoms(pdb_file);
}
