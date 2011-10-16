#ifndef PROTEIN_GRAPH_H_
#define PROTEIN_GRAPH_H_

#include <vector>

#include "protein/atom.h"

std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > construct_protein_graph(const std::vector<protein::Atom>& unrefined_protein_atoms);

#endif /* PROTEIN_GRAPH_H_ */
