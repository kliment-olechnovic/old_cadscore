#ifndef AUXILIARIES_SET_IO_H_
#define AUXILIARIES_SET_IO_H_

#include <set>
#include <iostream>

namespace auxiliaries
{

template<typename T>
void print_set(const std::set<T>& set)
{
	std::cout << set.size() << "\n";
	for(typename std::set<T>::const_iterator it=set.begin();it!=set.end();++it)
	{
		std::cout << (*it) << "\n";
	}
}

template<typename T>
std::set<T> read_set()
{
	std::size_t n=0;
	std::cin >> n;
	std::set<T> set;
	if(n>0)
	{
		typename std::set<T>::iterator prev=set.begin();
		for(std::size_t i=0;i<n;i++)
		{
			T value;
			std::cin >> value;
			prev=set.insert(prev, value);
		}
	}
	return set;
}

}

#endif /* AUXILIARIES_SET_IO_H_ */
