#ifndef EMMENTAL_H_
#define EMMENTAL_H_

#include <cmath>
#include <limits>

template<typename SphereType, typename FloatingPointType, template<typename, typename> class MapTypeProxyType, template<typename> class ListTypeProxyType>
class Emmental
{
public:
	typedef SphereType Sphere;
	typedef FloatingPointType FloatingPoint;
	typedef typename ListTypeProxyType<const Sphere*>::Type List;

	struct Statistics
	{
		std::size_t cells_count;
		std::size_t all_records_count;
		std::size_t min_records_count;
		std::size_t max_records_count;
		Statistics() : cells_count(0), all_records_count(0), min_records_count(std::numeric_limits<std::size_t>::max()), max_records_count(0) {}
	};

	Emmental(const FloatingPoint scale_factor) : scale_factor_(scale_factor) {}

	void add(const Sphere& s)
	{
		const Box box=bounding_box(s);
		for(int xi=box.x0;xi<=box.x1;xi++)
		{
			for(int yi=box.y0;yi<=box.y1;yi++)
			{
				for(int zi=box.z0;zi<=box.z1;zi++)
				{
					map_x_[xi][yi][zi].push_back(&s);
				}
			}
		}
	}

	List intersect(const Sphere& s) const
	{
		const Box box=bounding_box(s);
		List result;
		for(typename MapX::const_iterator iter_x=map_x_.lower_bound(box.x0); iter_x!=map_x_.end() && iter_x->first<=box.x1; ++iter_x)
		{
			const MapY& map_y=iter_x->second;
			for(typename MapY::const_iterator iter_y=map_y.lower_bound(box.y0); iter_y!=map_y.end() && iter_y->first<=box.y1; ++iter_y)
			{
				const MapZ& map_z=iter_y->second;
				for(typename MapZ::const_iterator iter_z=map_z.lower_bound(box.z0); iter_z!=map_z.end() && iter_z->first<=box.z1; ++iter_z)
				{
					const List& list=iter_z->second;
					result.insert(result.end(), list.begin(), list.end());
				}
			}
		}
		return result;
	}

	Statistics collect_statistics() const
	{
		Statistics stat;
		for(typename MapX::const_iterator iter_x=map_x_.begin(); iter_x!=map_x_.end(); ++iter_x)
		{
			const MapY& map_y=iter_x->second;
			for(typename MapY::const_iterator iter_y=map_y.begin(); iter_y!=map_y.end(); ++iter_y)
			{
				const MapZ& map_z=iter_y->second;
				stat.cells_count+=map_z.size();
				for(typename MapZ::const_iterator iter_z=map_z.begin(); iter_z!=map_z.end(); ++iter_z)
				{
					const List& list=iter_z->second;
					stat.all_records_count+=list.size();
					stat.min_records_count=std::min(stat.min_records_count, list.size());
					stat.max_records_count=std::max(stat.max_records_count, list.size());
				}
			}
		}
		return stat;
	}

private:
	typedef typename MapTypeProxyType< int, List >::Type MapZ;
	typedef typename MapTypeProxyType< int, MapZ >::Type MapY;
	typedef typename MapTypeProxyType< int, MapY >::Type MapX;

	struct Box
	{
		int x0;
		int x1;
		int y0;
		int y1;
		int z0;
		int z1;
	};

	int coordinate_int(const FloatingPoint v) const
	{
		return static_cast<int>(floor(v*scale_factor_));
	}

	Box bounding_box(const Sphere& s) const
	{
		const FloatingPoint x=s.x;
		const FloatingPoint y=s.y;
		const FloatingPoint z=s.z;
		const FloatingPoint r=s.r;
		Box box;
		box.x0=coordinate_int(x-r);
		box.x1=coordinate_int(x+r);
		box.y0=coordinate_int(y-r);
		box.y1=coordinate_int(y+r);
		box.z0=coordinate_int(z-r);
		box.z1=coordinate_int(z+r);
		return box;
	}

	FloatingPoint scale_factor_;
	MapX map_x_;
};

#endif /* EMMENTAL_H_ */
