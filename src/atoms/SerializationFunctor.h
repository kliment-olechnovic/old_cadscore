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

#ifndef SERIALIZATIONFUNCTOR_H_
#define SERIALIZATIONFUNCTOR_H_

#include <iostream>

template<typename T>
struct SerializationFunctor
{
	void operator()(const T& v, std::ostream& output) const
	{
		output << v;
	}
};

template<typename T>
struct UnserializationFunctor
{
	T operator()(std::istream& input) const
	{
		T v;
		input >> v;
		return v;
	}
};

#endif /* SERIALIZATIONFUNCTOR_H_ */
