#include <iostream>
#include <cstdlib>
#include <tr1/unordered_map>
#include <map>

//#include "atoms/atoms.h"

#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/tuples.h"

int main()
{
//	typedef Atom Sphere;
//	std::vector<Sphere> spheres=read_atoms_from_PDB_file_stream(std::cin);
//	std::cout << spheres.size() << " spheres read\n";

	utils::BlockTimer bt("Time");

	typedef Quadruple Element;

	typedef std::tr1::unordered_map< Element, int, TupleHashOperator<Element> >Map;
//	typedef std::map< Element, int > Map;
	Map map;
	for(int i=0;i<1099900;i++)
	{
		Element t=make_quadruple(rand()%1000,rand()%1000,rand()%1000,rand()%1000);
		map[t]=i;
	}

//	for(Map::const_iterator it=map.begin();it!=map.end();++it)
//	{
//		std::cout << it->first.str() << "\n";
//	}

	Element a=make_quadruple(10,20,2,1);
	Element b=a;
	a.set(0, 5);
	std::cout << a.str() << "\n";
	std::cout << b.str() << "\n";

	return 0;
}
