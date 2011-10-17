#ifndef INTER_ATOM_CONTACT_H_
#define INTER_ATOM_CONTACT_H_

namespace contacto
{

struct InterAtomContact
{
	int a;
	int b;
	double area;

	InterAtomContact(const int a, const int b, const double area) : a(a), b(b), area(area)
	{
	}
};

}

#endif /* INTER_ATOM_CONTACT_H_ */
