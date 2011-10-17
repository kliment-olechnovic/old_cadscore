#ifndef INTER_ATOM_CONTACT_H_
#define INTER_ATOM_CONTACT_H_

#include <iostream>

namespace contacto
{

struct InterAtomContact
{
	int a;
	int b;
	double area;

	InterAtomContact() : a(0), b(0), area(0)
	{
	}

	InterAtomContact(const int a, const int b, const double area) : a(a), b(b), area(area)
	{
	}

	friend std::ostream& operator<<(std::ostream &output, const InterAtomContact &contact)
	{
		output << contact.a << " ";
		output << contact.b << " ";
		output << contact.area;
		return output;
	}

	friend std::istream& operator>>(std::istream &input, InterAtomContact &contact)
	{
		input >> contact.a;
		input >> contact.b;
		input >> contact.area;
		return input;
	}
};

}

#endif /* INTER_ATOM_CONTACT_H_ */
