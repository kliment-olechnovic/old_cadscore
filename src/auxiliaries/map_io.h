#ifndef AUXILIARIES_MAP_IO_H_
#define AUXILIARIES_MAP_IO_H_

#include <map>
#include <iostream>

namespace auxiliaries
{

template<typename A, typename B>
void print_map(std::ostream& out, const std::map<A, B>& map, const bool separate_with_new_line=true)
{
	out << map.size() << "\n";
	for(typename std::map<A, B>::const_iterator it=map.begin();it!=map.end();++it)
	{
		out << it->first << (separate_with_new_line ? "\n" : " ") << it->second << "\n";
	}
}

template<typename A, typename B>
std::map<A, B> read_map(std::istream& in)
{
	std::size_t n=0;
	in >> n;
	std::map<A, B> map;
	if(n>0)
	{
		typename std::map<A, B>::iterator prev=map.begin();
		for(std::size_t i=0;i<n;i++)
		{
			A key;
			in >> key;
			B value;
			in >> value;
			prev=map.insert(prev, std::make_pair(key, value));
		}
	}
	return map;
}

}

#endif /* AUXILIARIES_MAP_IO_H_ */
