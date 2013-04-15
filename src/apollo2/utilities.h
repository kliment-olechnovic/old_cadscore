#ifndef APOLLO2_UTILITIES_H_
#define APOLLO2_UTILITIES_H_

#include <vector>
#include <algorithm>

namespace apollo2
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
	std::sort(distances.begin(), distances.end());
	std::vector<std::size_t> result;
	result.reserve(distances.size());
	for(std::size_t i=0;i<distances.size();i++)
	{
		result.push_back(distances[i].second);
	}
	return result;
}

}

#endif /* APOLLO2_UTILITIES_H_ */
