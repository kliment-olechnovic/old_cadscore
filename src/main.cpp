#include <iostream>
#include <cstdlib>

#include "atoms/atoms.h"

#include "../../contactus/src/baltymus/utils/BlockTimer.h"

int main()
{
	typedef Atom Sphere;

	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
	std::cout << spheres.size() << " spheres read\n";

	return 0;
}
