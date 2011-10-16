#ifndef PROTEIN_INPUT_H_
#define PROTEIN_INPUT_H_

#include <vector>

#include "protein/atom.h"

std::vector<protein::Atom> read_protein_atoms(const bool include_heteroatoms, const bool include_water);

#endif /* PROTEIN_INPUT_H_ */
