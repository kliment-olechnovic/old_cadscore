#ifndef APOLLO2_TUPLES_H_
#define APOLLO2_TUPLES_H_

#include <vector>
#include <algorithm>

namespace apollo2
{

template<unsigned int N>
class Tuple
{
public:
	struct HashFunctor
	{
		std::size_t operator()(const Tuple& t) const
		{
			return t.hash_value();
		}
	};

	Tuple()
	{
		for(unsigned int i=0;i<N;i++)
		{
			unsafe_set(i, 0);
		}
	}

	Tuple(const std::vector<std::size_t>& values)
	{
		for(unsigned int i=0;i<N && i<values.size();i++)
		{
			unsafe_set(i, values[i]);
		}
		sort();
	}

	Tuple(const Tuple<N-1>& shorter, const std::size_t tail)
	{
		for(unsigned int i=0;i<(N-1);i++)
		{
			unsafe_set(i, shorter.get(i));
		}
		unsafe_set((N-1), tail);
		sort();
	}

	unsigned int size() const
	{
		return N;
	}

	std::size_t get(unsigned int i) const
	{
		return v_[i];
	}

	bool contains(std::size_t x) const
	{
		for(unsigned int i=0;i<N;i++)
		{
			if(get(i)==x)
			{
				return true;
			}
		}
		return false;
	}

	Tuple<N-1> exclude(unsigned int i) const
	{
		std::vector<std::size_t> values;
		values.reserve(N-1);
		for(unsigned int j=0;j<N;j++)
		{
			if(j!=i)
			{
				values.push_back(get(j));
			}
		}
		return Tuple<N-1>(values);
	}

	bool has_repetetions() const
	{
		for(unsigned int i=0;i+1<N;i++)
		{
			if(v_[i]==v_[i+1])
			{
				return true;
			}
		}
		return false;
	}

	std::size_t hash_value() const
	{
		std::size_t h=0;
		for(unsigned int i=0;i<N;i++)
		{
			h += get(i);
			h += (h << 10);
			h ^= (h >> 6);
		}
		h += (h << 3);
		h ^= (h >> 11);
		h += (h << 15);
		return h;
	}

	bool operator==(const Tuple& t) const
	{
		for(unsigned int i=0;i<N;i++)
		{
			if(get(i)!=t.get(i))
			{
				return false;
			}
		}
		return true;
	}

	bool operator<(const Tuple& t) const
	{
		for(unsigned int i=0;i<N;i++)
		{
			if(get(i)<t.get(i))
			{
				return true;
			}
			else if(get(i)>t.get(i))
			{
				return false;
			}
		}
		return false;
	}

private:
	void unsafe_set(unsigned int i, std::size_t x)
	{
		v_[i]=x;
	}

	void sort()
	{
		std::sort(v_, v_+N);
	}

	std::size_t v_[N];
};

typedef Tuple<2> Pair;

inline Pair make_pair(const std::size_t a, const std::size_t b)
{
	std::vector<std::size_t> v(3);
	v[0]=a;
	v[1]=b;
	return Pair(v);
}

typedef Tuple<3> Triple;

inline Triple make_triple(const std::size_t a, const std::size_t b, const std::size_t c)
{
	std::vector<std::size_t> v(3);
	v[0]=a;
	v[1]=b;
	v[2]=c;
	return Triple(v);
}

typedef Tuple<4> Quadruple;

inline Quadruple make_quadruple(const std::size_t a, const std::size_t b, const std::size_t c, const std::size_t d)
{
	std::vector<std::size_t> v(4);
	v[0]=a;
	v[1]=b;
	v[2]=c;
	v[3]=d;
	return Quadruple(v);
}

}

#endif /* APOLLO2_TUPLES_H_ */
