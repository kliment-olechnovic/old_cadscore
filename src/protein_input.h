#ifndef PROTEIN_INPUT_H_
#define PROTEIN_INPUT_H_

#include <vector>

#include "protein/atom.h"

std::vector<protein::Atom> read_protein_atoms(const std::string& radius_classes_file_name, const std::string& radius_members_file_name, const bool include_heteroatoms, const bool include_water);

#endif /* PROTEIN_INPUT_H_ */
