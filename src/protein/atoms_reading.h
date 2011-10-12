#ifndef ATOMS_H_
#define ATOMS_H_

#include <string>
#include <vector>
#include <iostream>

#include "atom.h"
#include "pdb_parsing.h"
#include "van_der_waals_radius_assigner.h"

namespace protein
{

class AtomsReading
{
public:
	static std::vector<Atom> read_atoms_from_PDB_file_stream(
			std::istream& pdb_file_stream,
			const VanDerWaalsRadiusAssigner& vdwr_assigner,
			const bool include_heteroatoms,
			const bool include_water)
	{
		return collect_atoms_from_PDB_atom_records(
				PDBParsing::read_PDB_atom_records_from_PDB_file_stream(pdb_file_stream),
				include_heteroatoms,
				include_water,
				vdwr_assigner);
	}

private:
	AtomsReading()
	{
	}

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

}

#endif /* ATOMS_H_ */
