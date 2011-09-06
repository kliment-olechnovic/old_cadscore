#ifndef TUPLES_H_
#define TUPLES_H_

#include <vector>
#include <algorithm>
#include <sstream>

namespace apollo
{

template<int N>
class Tuple
{
public:
	Tuple()
	{
		for(int i=0;i<N;i++) { unsafe_set(i, 0); }
	}

	Tuple(const std::vector<std::size_t>& values)
	{
		for(int i=0;i<N;i++) { unsafe_set(i, (i<static_cast<int>(values.size()) ? values[i] : 0)); }
		sort();
	}

	Tuple(const Tuple<N-1>& shorter, const std::size_t tail)
	{
		for(int i=0;i<shorter.size();i++) { unsafe_set(i, shorter.get(i)); }
		unsafe_set(shorter.size(), tail);
		sort();
	}

	int size() const
	{
		return N;
	}

	std::size_t get(int i) const
	{
		return v_[i];
	}

	bool contains(std::size_t x) const
	{
		for(int i=0;i<N;i++) { if(get(i)==x) return true; }
		return false;
	}

	bool operator==(const Tuple& t) const
	{
		for(int i=0;i<N;i++) { if(get(i)!=t.get(i)) return false; }
		return true;
	}

	bool operator<(const Tuple& t) const
	{
		for(int i=0;i<N;i++)
		{
			if(get(i)<t.get(i)) return true;
			else if(get(i)>t.get(i)) return false;
		}
		return false;
	}

	std::size_t hash_value() const
	{
		std::size_t h=0;
		for(int i=0;i<N;i++)
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

	void set(int i, std::size_t x)
	{
		unsafe_set(i, x);
		sort();
	}

	Tuple<N-1> exclude(int i) const
	{
		std::vector<std::size_t> values;
		values.reserve(N-1);
		for(int j=0;j<N;j++)
		{
			if(j!=i)
			{
				values.push_back(get(j));
			}
		}
		return Tuple<N-1>(values);
	}

	std::string str() const
	{
		std::ostringstream output;
		for(int i=0;i<N;i++)
		{
			output << get(i) << (i+1<N ? " " : "");
		}
		return output.str();
	}

	template<typename T>
	bool intersects(const T& t) const
	{
		if(t.size()<size()) { return t.intersects(*this); }
		else
		{
			for(int i=0;i<t.size();i++)
			{
				if(!contains(t.get(i)))
				{
					return false;
				}
			}
		}
		return true;
	}

private:
	void unsafe_set(int i, std::size_t x)
	{
		if(i<N) { v_[i]=x; }
	}

	void sort()
	{
		std::sort(v_, v_+N);
	}

	std::size_t v_[N];
};

template<typename T>
struct TupleHashOperator
{
	std::size_t operator()(const T& t) const
	{
		return t.hash_value();
	}
};

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

#endif /* TUPLES_H_ */
