#ifndef SPHERESMAP_H_
#define SPHERESMAP_H_

#include <map>
#include <tr1/unordered_set>
#include <cmath>

template<typename Sphere>
class SpheresMap
{
public:
	typedef std::tr1::unordered_set<const Sphere*> Set;

	SpheresMap(const double coordinate_scale_factor, const double radius_scale_factor) :
		coordinate_scale_factor_(coordinate_scale_factor),
		radius_scale_factor_(radius_scale_factor)
	{}

	void add(const Sphere* s)
	{
		map_r_[radius_int(s->r())][coordinate_int(s->x())][coordinate_int(s->y())][coordinate_int(s->z())].insert(s);
	}

	Set find_potential_neighbours(const Sphere& s) const
	{
		Set result;
		for(typename MapR::const_iterator iter_r=map_r_.begin();iter_r!=map_r_.end();iter_r++)
		{
			const double offset=s.r()+(iter_r->first*radius_scale_factor_);

			const MapX& map_x=iter_r->second;
			typename MapX::const_iterator iter_x=map_x.lower_bound(coordinate_int(s.x()-offset));
			const typename MapX::const_iterator end_x=map_x.upper_bound(coordinate_int(s.x()+offset));

			for(;iter_x!=end_x;iter_x++)
			{
				const MapY& map_y=iter_x->second;
				typename MapY::const_iterator iter_y=map_y.lower_bound(coordinate_int(s.y()-offset));
				const typename MapY::const_iterator end_y=map_y.upper_bound(coordinate_int(s.y()+offset));

				for(;iter_y!=end_y;iter_y++)
				{
					const MapZ& map_z=iter_y->second;
					typename MapZ::const_iterator iter_z=map_z.lower_bound(coordinate_int(s.z()-offset));
					const typename MapZ::const_iterator end_z=map_z.upper_bound(coordinate_int(s.z()+offset));

					for(;iter_z!=end_z;iter_z++)
					{
						const Set& set=iter_y->second;
						result.insert(set.begin(), set.end());
					}
				}
			}
		}
		return result;
	}

private:
	typedef std::map< int, Set > MapZ;
	typedef std::map< int, MapZ > MapY;
	typedef std::map< int, MapY > MapX;
	typedef std::map< int, MapX > MapR;

	int coordinate_int(const double v) const
	{
		return static_cast<int>(floor(v*coordinate_scale_factor_));
	}

	int radius_int(const double v) const
	{
		return static_cast<int>(ceil(v*radius_scale_factor_));
	}

	double coordinate_scale_factor_;
	double radius_scale_factor_;
	MapR map_r_;
};

#endif /* SPHERESMAP_H_ */
