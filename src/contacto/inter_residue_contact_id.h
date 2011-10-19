#ifndef INTER_RESIDUE_CONTACT_ID_H_
#define INTER_RESIDUE_CONTACT_ID_H_

namespace contacto
{

template<typename ResidueIDType>
struct InterResidueContactID
{
	typedef ResidueIDType ResidueID;

	ResidueID a;
	ResidueID b;

	InterResidueContactID()
	{
	}

	InterResidueContactID(const ResidueID& a, const ResidueID& b) : a(a), b(b)
	{
	}

	bool operator<(const InterResidueContactID& irc) const
	{
		return (a<irc.a || (a==irc.a && b<irc.b));
	}

	friend std::ostream& operator<<(std::ostream& output, const InterResidueContactID& contact_id)
	{
		output << contact_id.a << " " << contact_id.b;
		return output;
	}

	friend std::istream& operator>>(std::istream& input, InterResidueContactID& contact_id)
	{
		input >> contact_id.a;
		input >> contact_id.b;
		return input;
	}
};

}

#endif /* INTER_RESIDUE_CONTACT_ID_H_ */
