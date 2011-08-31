#include <iostream>
#include <cstdlib>

//#include "atoms/atoms.h"

#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/tuples.h"

int main()
{
//	typedef Atom Sphere;
//	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
//	std::cout << spheres.size() << " spheres read\n";

	Quadruple q(make_triple(3,1,2),0);
	std::cout << q.exclude(0).str() << "\n";

	return 0;
}
