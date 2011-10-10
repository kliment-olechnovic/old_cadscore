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

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "atoms.h"

#include "PDBParsing.h"
#include "VanDerWaalsRadiusAssigner.h"

struct Implementation
{
	static Atom atom_from_PDB_atom_record(const PDBParsing::AtomRecord& record, const VanDerWaalsRadiusAssigner& vdwr_assigner)
	{
		Atom atom;
		atom.chain_id=record.chain_name.empty() ? std::string("X") : record.chain_name;
		atom.atom_number=record.atom_serial_number;
		atom.residue_number=record.residue_sequence_number;
		atom.residue_name=record.residue_name;
		atom.atom_name=record.name;
		atom.x=record.x;
		atom.y=record.y;
		atom.z=record.z;
		atom.r=vdwr_assigner.radius(atom.residue_name, atom.atom_name);
		return atom;
	}

	static std::vector<Atom> collect_atoms_from_PDB_atom_records(
			const std::vector<PDBParsing::AtomRecord>& PDB_atom_records,
			const bool include_heteroatoms,
			const bool include_water,
			const VanDerWaalsRadiusAssigner& vdwr_assigner)
	{
		std::vector<Atom> atoms;
		for(std::size_t i=0;i<PDB_atom_records.size();i++)
		{
			const PDBParsing::AtomRecord& record=PDB_atom_records[i];
			if(record.alternate_location_indicator.empty() || record.alternate_location_indicator=="A")
			{
				if(record.label=="ATOM" || (include_heteroatoms && record.label=="HETATM"))
				{
					if(include_water || record.residue_name!="HOH")
					{
						atoms.push_back(atom_from_PDB_atom_record(record, vdwr_assigner));
					}
				}
			}
		}
		return atoms;
	}
};

std::vector<Atom> read_atoms_from_PDB_file_stream(std::istream& pdb_file_stream, const bool include_heteroatoms, const bool include_water)
{
	std::ifstream vdwr_classes_stream("resources/vdwr_classes.txt", std::ios::in);
	std::ifstream vdwr_members_stream("resources/vdwr_members.txt", std::ios::in);
	return Implementation::collect_atoms_from_PDB_atom_records(
			PDBParsing::read_PDB_atom_records_from_PDB_file_stream(pdb_file_stream),
			include_heteroatoms,
			include_water,
			VanDerWaalsRadiusAssigner(vdwr_classes_stream, vdwr_members_stream));
}
