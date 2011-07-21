#ifndef GRIDHASH_H_
#define GRIDHASH_H_

#include <tr1/unordered_map>
#include <tr1/unordered_set>

template<typename Object>
class GridHash
{
public:
	struct Coordinate
	{
		int x;
		int y;
		int z;

		Coordinate() : x(0), y(0), z(0) {}
		Coordinate(int x, int y, int z) : x(x), y(y), z(z) {}
	};

	struct CoordinateHash
	{
		static const int max_width=1000;
		static const int shift=max_width/2;
		static const int x_width=max_width*max_width;
		static const int y_width=max_width;

		std::size_t operator()(const Coordinate& c) const
		{
			return static_cast<std::size_t>((c.x+shift)*x_width+(c.y+shift)*y_width+(c.z+shift));
		}
	};

	struct CoordinateEquality
	{
		bool operator()(const Coordinate& a, const Coordinate& b) const
		{
			return (a.x==b.x && a.y==b.y && a.z==b.z);
		}
	};

	typedef std::tr1::unordered_set<Object*> Set;

	typedef std::tr1::unordered_map<Coordinate, Set, CoordinateHash, CoordinateEquality> Map;

	void add_object(const Object* object)
	{
	}

	void remove_object(const Object* object)
	{
	}
private:
};

#endif /* GRIDHASH_H_ */
