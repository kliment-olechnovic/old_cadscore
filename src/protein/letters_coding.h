#ifndef PROTEIN_LETTERS_CODING_H_
#define PROTEIN_LETTERS_CODING_H_

#include <map>
#include <string>

namespace protein
{

class LettersCoding
{
public:
	inline static std::string convert_residue_codes_big_to_small(const std::string& aaa)
	{
		static const std::map<std::string, std::string> m=create_map_of_residue_codes_big_to_small();
		const std::map<std::string, std::string>::const_iterator it=m.find(aaa);
		return (it==m.end() ? std::string("*") : it->second);
	}

	inline static std::string convert_residue_codes_small_to_big(const std::string& a)
	{
		static const std::map<std::string, std::string> m=create_map_of_residue_codes_small_to_big();
		const std::map<std::string, std::string>::const_iterator it=m.find(a);
		return (it==m.end() ? std::string("XXX") : it->second);
	}
private:
	static std::map<std::string, std::string> create_map_of_residue_codes_big_to_small()
	{
		std::map<std::string, std::string> m;
		m["LEU"]="L";
		m["VAL"]="V";
		m["ILE"]="I";
		m["ALA"]="A";
		m["PHE"]="F";
		m["TRP"]="W";
		m["MET"]="M";
		m["PRO"]="P";
		m["ASP"]="D";
		m["GLU"]="E";
		m["LYS"]="K";
		m["ARG"]="R";
		m["HIS"]="H";
		m["CYS"]="C";
		m["SER"]="S";
		m["THR"]="T";
		m["TYR"]="Y";
		m["ASN"]="N";
		m["GLN"]="Q";
		m["GLY"]="G";
		m["ASX"]="B";
		m["GLX"]="Z";
		m["A"]="a";
		m["C"]="c";
		m["G"]="g";
		m["T"]="t";
		m["U"]="u";
		return m;
	}

	static std::map<std::string, std::string> create_map_of_residue_codes_small_to_big()
	{
		const std::map<std::string, std::string> m_big_to_small;
		std::map<std::string, std::string> m;
		for(std::map<std::string, std::string>::const_iterator it=m_big_to_small.begin();it!=m_big_to_small.end();++it)
		{
			m[it->second]=it->first;
		}
		return m;
	}
};

}

#endif /* PROTEIN_LETTERS_CODING_H_ */
