#ifndef SPHERESMAP_H_
#define SPHERESMAP_H_

#include <map>
#include <tr1/unordered_set>
#include <cmath>

template<typename SphereType>
class SpheresMap
{
public:
	typedef SphereType Sphere;
	typedef typename Sphere::FloatType FloatType;
	typedef std::tr1::unordered_set<const Sphere*> Set;

	SpheresMap(
			const FloatType coordinate_scale_factor,
			const FloatType radius_scale_factor,
			const FloatType max_mappeable_radius) :
				coordinate_scale_factor_(coordinate_scale_factor),
				radius_scale_factor_(radius_scale_factor),
				max_mappeable_radius_(max_mappeable_radius)
	{}

	void add(const Sphere* s)
	{
		if(all_.find(s)==all_.end())
		{
			if(s->r()<=max_mappeable_radius_)
			{
				map_r_[radius_int(s->r())][coordinate_int(s->x())][coordinate_int(s->y())][coordinate_int(s->z())].insert(s);
			}
			else
			{
				unmapped_.insert(s);
			}
			all_.insert(s);
		}
	}

	void remove(const Sphere* s)
	{
		if(all_.find(s)!=all_.end())
		{
			if(s->r()<=max_mappeable_radius_)
			{
				typename MapR::iterator iter_r=map_r_.find(radius_int(s->r()));
				if(iter_r!=map_r_.end())
				{
					MapX& map_x=iter_r->second;
					typename MapX::iterator iter_x=map_x.find(coordinate_int(s->x()));
					if(iter_x!=map_x.end())
					{
						MapY& map_y=iter_x->second;
						typename MapY::iterator iter_y=map_y.find(coordinate_int(s->y()));
						if(iter_y!=map_y.end())
						{
							MapZ& map_z=iter_y->second;
							typename MapZ::iterator iter_z=map_z.find(coordinate_int(s->z()));
							if(iter_z!=map_z.end())
							{
								Set& set=iter_z->second;
								set.erase(s);
								if(set.empty()) { map_z.erase(iter_z); }
							}
							if(map_z.empty()) { map_y.erase(iter_y); }
						}
						if(map_y.empty()) { map_x.erase(iter_x); }
					}
					if(map_x.empty()) { map_r_.erase(iter_r); }
				}
			}
			else
			{
				unmapped_.erase(s);
			}
			all_.erase(s);
		}
	}

	Set find_neighbours(const Sphere& s) const
	{
		Set result;
		for(typename MapR::const_iterator iter_r=map_r_.begin();iter_r!=map_r_.end();iter_r++)
		{
			const FloatType offset=s.r()+(iter_r->first*radius_scale_factor_);

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
						const Set& set=iter_z->second;
						result.insert(set.begin(), set.end());
					}
				}
			}
		}
		result.insert(unmapped_.begin(), unmapped_.end());
		return result;
	}

private:
	typedef std::map< int, Set > MapZ;
	typedef std::map< int, MapZ > MapY;
	typedef std::map< int, MapY > MapX;
	typedef std::map< int, MapX > MapR;

	int coordinate_int(const FloatType v) const
	{
		return static_cast<int>(floor(v*coordinate_scale_factor_));
	}

	int radius_int(const FloatType v) const
	{
		return static_cast<int>(ceil(v*radius_scale_factor_));
	}

	FloatType coordinate_scale_factor_;
	FloatType radius_scale_factor_;
	FloatType max_mappeable_radius_;
	Set all_;
	MapR map_r_;
	Set unmapped_;
};

#endif /* SPHERESMAP_H_ */
