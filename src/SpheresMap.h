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

	SpheresMap(const double scale_factor=1.0) : scale_factor_(scale_factor) {}

	void add(const Sphere* s)
	{
		map4D_[right_int(s->r())][left_int(s->x())][left_int(s->y())][left_int(s->z())].insert(s);
	}

	Set find_potential_neighbours(const Sphere& s) const
	{
		Set result;
		for(typename Map4D::const_iterator r_iter=map4D_.begin();r_iter!=map4D_.end();r_iter++)
		{
			const int max_r=r_iter->first;

			const int lx=left_int(s.x()-s.r()-max_r);
			const int rx=right_int(s.x()+s.r()+max_r);
			const Map3D& map3D=r_iter->second;
			typename Map3D::const_iterator x_iter=map3D.lower_bound(lx);
			if(x_iter->first<lx) { x_iter++; }
			typename Map3D::const_iterator x_end=map3D.upper_bound(rx);

			for(;x_iter!=x_end;x_iter++)
			{
				const int ly=left_int(s.y()-s.r()-max_r);
				const int ry=right_int(s.y()+s.r()+max_r);
				const Map2D& map2D=x_iter->second;
				typename Map2D::const_iterator y_iter=map2D.lower_bound(ly);
				if(y_iter->first<ly) { y_iter++; }
				typename Map2D::const_iterator y_end=map2D.upper_bound(ry);

				for(;y_iter!=y_end;y_iter++)
				{
					const int lz=left_int(s.z()-s.r()-max_r);
					const int rz=right_int(s.z()+s.r()+max_r);
					const Map1D& map1D=y_iter->second;
					typename Map1D::const_iterator z_iter=map1D.lower_bound(lz);
					if(z_iter->first<lz) { z_iter++; }
					typename Map1D::const_iterator z_end=map1D.upper_bound(rz);

					for(;z_iter!=z_end;z_iter++)
					{
						const Set& set=y_iter->second;
						result.insert(set.begin(), set.end());
					}
				}
			}
		}
		return result;
	}

private:
	typedef std::map< int, Set > Map1D;
	typedef std::map< int, Map1D > Map2D;
	typedef std::map< int, Map2D > Map3D;
	typedef std::map< int, Map3D > Map4D;

	int left_int(const double v) const
	{
		return static_cast<int>(floor(v*scale_factor_));
	}

	int right_int(const double v) const
	{
		return static_cast<int>(ceil(v*scale_factor_));
	}

	double scale_factor_;
	Map4D map4D_;
};

#endif /* SPHERESMAP_H_ */
