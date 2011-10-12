#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vector>
#include <algorithm>

namespace apollo
{

template<typename ListType, typename FunctorType>
std::vector<std::size_t> sort_objects_by_functor_result(const ListType& list, const FunctorType& functor)
{
	std::vector< std::pair<double, std::size_t> > distances;
	distances.reserve(list.size());
	for(std::size_t i=0;i<list.size();i++)
	{
		distances.push_back(std::make_pair(functor(list[i]), i));
	}
	std::vector<std::size_t> result;
	result.reserve(distances.size());
	for(std::size_t i=0;i<distances.size();i++)
	{
		result.push_back(distances[i].second);
	}
	return result;
}

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

#endif /* UTILITIES_H_ */
