#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>

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

	utils::BlockTimer bt("Time");

	std::tr1::unordered_map< Triple, int, TupleHashOperator<Triple> > map;
//	std::tr1::unordered_map< std::string, int > map;
	for(int i=0;i<1099900;i++)
	{
		Triple t=make_triple(rand()%1000,rand()%1000,rand()%1000);
		map[t]=i;
//		map[t.str()]=i;
	}

	return 0;
}
