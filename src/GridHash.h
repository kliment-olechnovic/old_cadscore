#ifndef GRIDHASH_H_
#define GRIDHASH_H_

#include <tr1/unordered_map>
#include <cmath>

template<typename SphereType, typename FloatingPointType, template<typename> class ListTypeProxyType, template<typename> class SetTypeProxyType>
class GridHash
{
public:
	typedef SphereType Sphere;
	typedef FloatingPointType FloatingPoint;
	typedef typename SetTypeProxyType<const Sphere*>::Type Set;

	GridHash(const FloatingPoint scale_factor) : scale_factor_(scale_factor) {}

	void add(const Sphere* s)
	{
		if(all_.find(s)==all_.end())
		{
			const FloatingPoint x=s->x;
			const FloatingPoint y=s->y;
			const FloatingPoint z=s->z;
			const FloatingPoint r=s->r;
			const int x0=coordinate_int(x-r);
			const int x1=coordinate_int(x+r);
			const int y0=coordinate_int(y-r);
			const int y1=coordinate_int(y+r);
			const int z0=coordinate_int(z-r);
			const int z1=coordinate_int(z+r);

			for(int xi=x0;xi<=x1;xi++)
			{
				for(int yi=y0;yi<=y1;yi++)
				{
					for(int zi=z0;zi<=z1;zi++)
					{
						map_[CellID(xi,yi,zi)].push_back(s);
					}
				}
			}
			all_.insert(s);
		}
	}

	Set intersect(const Sphere& s) const
	{
		const FloatingPoint x=s.x;
		const FloatingPoint y=s.y;
		const FloatingPoint z=s.z;
		const FloatingPoint r=s.r;
		const int x0=coordinate_int(x-r);
		const int x1=coordinate_int(x+r);
		const int y0=coordinate_int(y-r);
		const int y1=coordinate_int(y+r);
		const int z0=coordinate_int(z-r);
		const int z1=coordinate_int(z+r);

		Set result;
		for(int xi=x0;xi<=x1;xi++)
		{
			for(int yi=y0;yi<=y1;yi++)
			{
				for(int zi=z0;zi<=z1;zi++)
				{
					typename Map::const_iterator it=map_.find(CellID(xi,yi,zi));
					if(it!=map_.end())
					{
						const List& list=it->second;
						result.insert(list.begin(), list.end());
					}
				}
			}
		}
		return result;
	}

	std::size_t count() const
	{
		return all_.size();
	}

private:
	struct CellID
	{
		int x;
		int y;
		int z;
		CellID() : x(0), y(0), z(0) {}
		CellID(int x, int y, int z) : x(x), y(y), z(z) {}
	};

	struct Hash
	{
		std::size_t operator()(const CellID& cid) const
		{
			return static_cast<std::size_t>((cid.x+500)*1000*1000+(cid.y+500)*1000+(cid.z+500));
		}
	};

	struct Pred
	{
		bool operator()(const CellID& a, const CellID& b) const
		{
			return (a.x==b.x && a.y==b.y && a.z==b.z);
		}
	};

	typedef typename ListTypeProxyType<const Sphere*>::Type List;
	typedef typename std::tr1::unordered_map< CellID, List, Hash, Pred > Map;

	int coordinate_int(const FloatingPoint v) const
	{
		return static_cast<int>(floor(v*scale_factor_));
	}

	FloatingPoint scale_factor_;
	Set all_;
	Map map_;
};

#endif /* GRIDHASH_H_ */
