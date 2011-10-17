#ifndef ATOM_H_
#define ATOM_H_

#include <string>
#include <iostream>

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

	friend std::ostream& operator<<(std::ostream &output, const Atom &atom)
	{
		output << atom.chain_id << " ";
		output << atom.atom_number << " ";
		output << atom.residue_number << " ";
		output << atom.residue_name << " ";
		output << atom.atom_name << " ";
		output << atom.x << " ";
		output << atom.y << " ";
		output << atom.z << " ";
		output << atom.r;
		return output;
	}

	friend std::istream& operator>>(std::istream &input, Atom &atom)
	{
		input >> atom.chain_id;
		input >> atom.atom_number;
		input >> atom.residue_number;
		input >> atom.residue_name;
		input >> atom.atom_name;
		input >> atom.x;
		input >> atom.y;
		input >> atom.z;
		input >> atom.r;
		return input;
	}
};

}

#endif /* ATOM_H_ */
