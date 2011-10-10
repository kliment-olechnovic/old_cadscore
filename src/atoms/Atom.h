/****************************************************************************
 **
 ** Copyright (C) Kliment Olechnovic, Laboratory of Bioinformatics, Institute of Biotechnology, Vilnius, Lithuania.
 ** All rights reserved.
 ** Contact: Kliment Olechnovic (kliment@ibt.lt)
 **
 ** This file is part of Contactus.

 ** Contactus is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.

 ** Contactus is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with Contactus.  If not, see <http://www.gnu.org/licenses/>.
 **
****************************************************************************/

#ifndef ATOM_H_
#define ATOM_H_

#include <string>

#include "SerializationFunctor.h"

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

template<>
struct SerializationFunctor<Atom>
{
	void operator()(const Atom& atom, std::ostream& output) const
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
	}
};

template<>
struct UnserializationFunctor<Atom>
{
	Atom operator()(std::istream& input) const
	{
		Atom atom;
		input >> atom.chain_id;
		input >> atom.atom_number;
		input >> atom.residue_number;
		input >> atom.residue_name;
		input >> atom.atom_name;
		input >> atom.x;
		input >> atom.y;
		input >> atom.z;
		input >> atom.r;
		return atom;
	}
};

#endif /* ATOM_H_ */
