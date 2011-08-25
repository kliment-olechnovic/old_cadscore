#ifndef EMMENTAL_H_
#define EMMENTAL_H_

#include <cmath>

template<typename SphereType, typename FloatingPointType, template<typename, typename> class MapTypeProxyType, template<typename> class ListTypeProxyType, template<typename> class SetTypeProxyType>
class Emmental
{
public:
	typedef SphereType Sphere;
	typedef FloatingPointType FloatingPoint;
	typedef typename SetTypeProxyType<const Sphere*>::Type Set;

	Emmental(const FloatingPoint scale_factor) : scale_factor_(scale_factor) {}

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
						map_x_[xi][yi][zi].push_back(s);
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
			typename MapX::const_iterator iter_x=map_x_.find(xi);
			if(iter_x!=map_x_.end())
			{
				const MapY& map_y=iter_x->second;
				for(int yi=y0;yi<=y1;yi++)
				{
					typename MapY::const_iterator iter_y=map_y.find(yi);
					if(iter_y!=map_y.end())
					{
						const MapZ& map_z=iter_y->second;
						for(int zi=z0;zi<=z1;zi++)
						{
							typename MapZ::const_iterator iter_z=map_z.find(zi);
							if(iter_z!=map_z.end())
							{
								const List& list=iter_z->second;
								result.insert(list.begin(), list.end());
							}
						}
					}
				}
			}
		}
		return result;

//		Using ordered map:
//		Set result;
//		typename MapX::const_iterator iter_x=map_x_.lower_bound(x0);
//		const typename MapX::const_iterator end_x=map_x_.upper_bound(x1);
//		for(;iter_x!=end_x;++iter_x)
//		{
//			const MapY& map_y=iter_x->second;
//			typename MapY::const_iterator iter_y=map_y.lower_bound(y0);
//			const typename MapY::const_iterator end_y=map_y.upper_bound(y1);
//			for(;iter_y!=end_y;++iter_y)
//			{
//				const MapZ& map_z=iter_y->second;
//				typename MapZ::const_iterator iter_z=map_z.lower_bound(z0);
//				const typename MapZ::const_iterator end_z=map_z.upper_bound(z1);
//				for(;iter_z!=end_z;++iter_z)
//				{
//					const List& list=iter_z->second;
//					result.insert(list.begin(), list.end());
//				}
//			}
//		}
//		return result;
	}

	std::size_t count() const
	{
		return all_.size();
	}

private:
	typedef typename ListTypeProxyType<const Sphere*>::Type List;
	typedef typename MapTypeProxyType< int, List >::Type MapZ;
	typedef typename MapTypeProxyType< int, MapZ >::Type MapY;
	typedef typename MapTypeProxyType< int, MapY >::Type MapX;

	int coordinate_int(const FloatingPoint v) const
	{
		return static_cast<int>(floor(v*scale_factor_));
	}

	FloatingPoint scale_factor_;
	Set all_;
	MapX map_x_;
};

#endif /* EMMENTAL_H_ */
