#ifndef PROTEIN_ATOM_H_
#define PROTEIN_ATOM_H_

#include <string>
#include <iostream>
#include <sstream>

namespace protein
{

struct Atom
{
	enum MoleculeClass {unidentified_molecule_class, amino_acid, nucleotide};
	enum LocationClass {unidentified_location_class, main_chain, side_chain};

	std::string chain_id;
	int atom_number;
	int residue_number;
	std::string residue_name;
	std::string atom_name;
	double x;
	double y;
	double z;
	double r;
	int molecule_class;
	int location_class;

	Atom() :
		chain_id("?"),
		atom_number(0),
		residue_number(0),
		residue_name("?"),
		atom_name("?"),
		x(0),
		y(0),
		z(0),
		r(0),
		molecule_class(unidentified_molecule_class),
		location_class(unidentified_location_class) {}

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
				r==a.r &&
				molecule_class==a.molecule_class &&
				location_class==a.location_class));
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
		output << atom.r << " ";
		output << atom.molecule_class << " ";
		output << atom.location_class;
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
		input >> atom.molecule_class;
		input >> atom.location_class;
		return input;
	}

	std::string string_for_human_reading() const
	{
		std::ostringstream output;
		output << "atom number = " << atom_number << ", ";
		output << "chain = " << chain_id << ", ";
		output << "residue number = " << residue_number << ", ";
		output << "residue name = " << residue_name << ", ";
		output << "atom name = " << atom_name << ", ";
		output << "x = " << x << ", ";
		output << "y = " << y << ", ";
		output << "z = " << z << ", ";
		output << "radius = " << r;
		return output.str();
	}
};

}

#endif /* PROTEIN_ATOM_H_ */
