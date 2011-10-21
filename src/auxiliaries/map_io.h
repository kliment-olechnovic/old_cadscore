#ifndef MAP_IO_H_
#define MAP_IO_H_

#include <map>
#include <iostream>

namespace auxiliaries
{

template<typename A, typename B>
void print_map(const std::map<A, B>& map)
{
	std::cout << map.size() << "\n";
	for(typename std::map<A, B>::const_iterator it=map.begin();it!=map.end();++it)
	{
		std::cout << it->first << "\n" << it->second << "\n";
	}
}

template<typename A, typename B>
std::map<A, B> read_map()
{
	std::size_t n=0;
	std::cin >> n;
	std::map<A, B> map;
	if(n>0)
	{
		typename std::map<A, B>::iterator prev=map.begin();
		for(std::size_t i=0;i<n;i++)
		{
			A key;
			std::cin >> key;
			B value;
			std::cin >> value;
			prev=map.insert(prev, std::make_pair(key, value));
		}
	}
	return map;
}

}

#endif /* MAP_IO_H_ */
