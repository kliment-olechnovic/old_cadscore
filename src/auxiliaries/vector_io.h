#ifndef AUXILIARIES_VECTOR_IO_H_
#define AUXILIARIES_VECTOR_IO_H_

#include <vector>
#include <iostream>

namespace auxiliaries
{

template<typename T>
void print_vector(const std::vector<T>& v)
{
	std::cout << v.size() << "\n";
	for(std::size_t i=0;i<v.size();i++)
	{
		std::cout << v[i] << "\n";
	}
}

template<typename T>
std::vector<T> read_vector()
{
	std::size_t n=0;
	std::cin >> n;
	if(n>0)
	{
		std::vector<T> v(n);
		for(std::size_t i=0;i<n;i++)
		{
			std::cin >> v[i];
		}
		return v;
	}
	else
	{
		return std::vector<T>();
	}
}

}

#endif /* AUXILIARIES_VECTOR_IO_H_ */
