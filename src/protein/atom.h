#ifndef ATOM_H_
#define ATOM_H_

#include <string>

namespace protein
{

struct Atom
{
	std::string chain_id;
	int atom_number;
	int residue_number;
	std::string residue_name;
	std::string atom_name;
	double x;
	double y;
	double z;
	double r;

	Atom() : atom_number(0), residue_number(0), x(0), y(0), z(0), r(0) {}

	bool operator==(const Atom& a) const
	{
		return (this==&a ||
				(atom_number==a.atom_number &&
				residue_number==a.residue_number &&
				chain_id==a.chain_id &&
				residue_name==a.residue_name &&
				atom_name==a.atom_name &&
				x==a.x &&
				y==a.y &&
				z==a.z &&
				r==a.r));
	}
};

}

#endif /* ATOM_H_ */
