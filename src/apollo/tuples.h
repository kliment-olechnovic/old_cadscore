#ifndef TUPLES_H_
#define TUPLES_H_

#include <vector>
#include <algorithm>
#include <sstream>

template<int N>
class Tuple
{
public:
	Tuple() : v_(N, 0)
	{
	}

	Tuple(const std::vector<std::size_t>& values) : v_(values)
	{
		v_.resize(N, 0);
		sort();
	}

	Tuple(const Tuple<N-1>& shorter, const size_t tail) : v_(N)
	{
		for(int i=0;i<shorter.size();i++) { unsafe_set(i, shorter.get(i)); }
		unsafe_set(shorter.size(), tail);
		sort();
	}

	int size() const
	{
		return N;
	}

	size_t get(int i) const
	{
		return v_[i];
	}

	bool contains(std::size_t x) const
	{
		for(int i=0;i<N;i++) { if(get(i)==x) return true; }
		return false;
	}

	bool equals(const Tuple& t) const
	{
		for(int i=0;i<N;i++) { if(get(i)!=t.get(i)) return false; }
		return true;
	}

	size_t hash() const
	{
		size_t h=0;
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

	void set(int i, size_t x)
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

private:
	void unsafe_set(int i, size_t x)
	{
		v_[i]=x;
	}

	void sort()
	{
		std::sort(v_.begin(), v_.end());
	}

	std::vector<std::size_t> v_;
};

typedef Tuple<3> Triple;

inline Triple make_triple(const size_t a, const size_t b, const size_t c)
{
	std::vector<std::size_t> v(3);
	v[0]=a;
	v[1]=b;
	v[2]=c;
	return Triple(v);
}

typedef Tuple<4> Quadruple;

inline Quadruple make_quadruple(const size_t a, const size_t b, const size_t c, const size_t d)
{
	std::vector<std::size_t> v(4);
	v[0]=a;
	v[1]=b;
	v[2]=c;
	v[3]=d;
	return Quadruple(v);
}

#endif /* TUPLES_H_ */
