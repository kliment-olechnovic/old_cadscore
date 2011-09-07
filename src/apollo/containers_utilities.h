#ifndef CONTAINERS_UTILITIES_H_
#define CONTAINERS_UTILITIES_H_

#include <vector>

namespace apollo
{

template<typename A, typename B>
std::vector< std::pair<typename A::value_type, typename B::value_type> > make_pairs(const A& a, const B& b)
{
	std::vector< std::pair<typename A::value_type, typename B::value_type> > pairs;
	pairs.reserve(std::min(a.size(), b.size()));
	typename A::const_iterator ait=a.begin();
	typename B::const_iterator bit=b.begin();
	while(ait!=a.end() && bit!=b.end())
	{
		pairs.push_back(std::make_pair(*ait, *bit));
		++ait;
		++bit;
	}
	return pairs;
}

template<typename T>
std::pair< std::vector<typename T::value_type::first_type>, std::vector<typename T::value_type::second_type> > split_pairs(const T& pairs)
{
	std::vector<typename T::value_type::first_type> a;
	a.reserve(pairs.size());
	std::vector<typename T::value_type::second_type> b;
	b.reserve(pairs.size());
	for(typename T::const_iterator it=pairs.begin();it!=pairs.end();++it)
	{
		a.push_back(it->first);
		b.push_back(it->second);
	}
	return std::make_pair(a, b);
}

}

#endif /* CONTAINERS_UTILITIES_H_ */
