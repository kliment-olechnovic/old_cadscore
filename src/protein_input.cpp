#include "protein_input.h"

#include <iostream>
#include <fstream>

#include "protein/atoms_reading.h"

std::vector<protein::Atom> read_protein_atoms(const bool include_heteroatoms, const bool include_water)
{
	std::ifstream radius_classes_stream("resources/vdwr_classes.txt");
	std::ifstream radius_members_stream("resources/vdwr_members.txt");
	const protein::VanDerWaalsRadiusAssigner radius_assigner(radius_classes_stream, radius_members_stream);
	return protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, include_heteroatoms, include_water);
}
