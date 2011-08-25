#ifndef STANDART_CONTAINERS_PROXIES_H_
#define STANDART_CONTAINERS_PROXIES_H_

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <tr1/unordered_set>
#include <tr1/unordered_map>

template<typename T>
class VectorTypeProxy
{
public:
	typedef std::vector<T> Type;
};

template<typename T>
class DequeTypeProxy
{
public:
	typedef std::deque<T> Type;
};

template<typename T>
class ListTypeProxy
{
public:
	typedef std::list<T> Type;
};

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

#endif /* STANDART_CONTAINERS_PROXIES_H_ */
