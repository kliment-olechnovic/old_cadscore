#ifndef STANDARDTYPEPROXIES_H_
#define STANDARDTYPEPROXIES_H_

#include <set>
#include <map>
#include <tr1/unordered_set>
#include <tr1/unordered_map>

class StandardTypeProxies
{
public:
	template<typename T>
	class OrderedSetTypeProxy
	{
	public:
		typedef std::set<T> Type;
	};

	template<typename A, typename B>
	class OrderedMapTypeProxy
	{
	public:
		typedef std::map<A, B> Type;
	};

	template<typename T>
	class UnorderedSetTypeProxy
	{
	public:
		typedef std::tr1::unordered_set<T> Type;
	};

	template<typename A, typename B>
	class UnorderedMapTypeProxy
	{
	public:
		typedef std::tr1::unordered_map<A, B> Type;
	};
};

#endif /* STANDARDTYPEPROXIES_H_ */
