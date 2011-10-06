#ifndef APOLLONIUS_FACE_H_
#define APOLLONIUS_FACE_H_

#include <vector>
#include <stdexcept>

#include "tuples.h"
#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"
#include "spheres_tangent_stick.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusFace
{
public:
	typedef SphereType Sphere;
	static const std::size_t npos=-1;

	ApolloniusFace(
			const std::vector<Sphere>& spheres,
			const Triple& abc_ids,
			const std::size_t d1_id,
			const SimpleSphere& d1_tangent_sphere) :
				spheres_(spheres),
				abc_ids_(abc_ids),
				a_(spheres_[abc_ids_.get(0)]),
				b_(spheres_[abc_ids_.get(1)]),
				c_(spheres_[abc_ids_.get(2)]),
				d1_id_(d1_id),
				d1_(spheres_[d1_id_]),
				d1_tangent_sphere_(d1_tangent_sphere),
				tangent_planes_(construct_spheres_tangent_planes(a_, b_, c_)),
				tangent_stick_(tangent_planes_.empty() ? select_tangent_stick(a_, b_, c_) : std::pair<const Sphere*, const Sphere*>(NULL, NULL)),
				free_tangent_plane_id_(select_free_tangent_plane_id(a_, b_, c_, tangent_planes_, d1_, d1_tangent_sphere_)),
				d2_id_(npos),
				d2_tangent_sphere_(SimpleSphere())
	{
		die_if_invalid();
	}

	Triple abc_ids() const
	{
		return abc_ids_;
	}

	std::size_t d1_id() const
	{
		return d1_id_;
	}

	SimpleSphere d1_tangent_sphere() const
	{
		return d1_tangent_sphere_;
	}

	std::size_t d2_id() const
	{
		return d2_id_;
	}

	SimpleSphere d2_tangent_sphere() const
	{
		return d2_tangent_sphere_;
	}

	void die_if_invalid() const
	{
		if(abc_ids_.contains(d1_id_))
		{
			throw std::logic_error("Invalid d1 id");
		}

		if(!check_spheres_tangent(a_, b_, c_, d1_, d1_tangent_sphere_))
		{
			throw std::logic_error("Invalid d1 tangent sphere");
		}

		if(!((tangent_planes_.size()==2) ||
				(tangent_planes_.empty() && tangent_stick_.first!=NULL && tangent_stick_.second!=NULL)))
		{
//			std::cout << "LIST\n";
//			std::cout << "SPHERE " << a_.r << " " << a_.x << " " << a_.y << " " << a_.z << "\n";
//			std::cout << "SPHERE " << b_.r << " " << b_.x << " " << b_.y << " " << b_.z << "\n";
//			std::cout << "SPHERE " << c_.r << " " << c_.x << " " << c_.y << " " << c_.z << "\n";
			throw std::logic_error("Invalid tangency information");
		}

		if(d2_id_!=npos)
		{
			if(abc_ids_.contains(d2_id_))
			{
				throw std::logic_error("Invalid d2 id");
			}

			const Sphere& d2=spheres_[d2_id_];

			if(!check_spheres_tangent(a_, b_, c_, d2, d2_tangent_sphere_) || sphere_intersects_sphere(d2_tangent_sphere_, d1_))
			{
				throw std::logic_error("Invalid d2 tangent sphere");
			}
		}
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_d2(const InputSphereType& x) const
	{
		if(free_tangent_plane_id_!=npos)
		{
			if(!sphere_is_on_free_plane(x))
			{
				return false;
			}
		}
		else
		{
			if(!sphere_is_inner(x))
			{
				return false;
			}
		}
		return true;
	}

	std::pair<bool, SimpleSphere> check_candidate_for_d2(const std::size_t d2_id) const
	{
		if(!abc_ids_.contains(d2_id))
		{
			const Sphere& d2=spheres_[d2_id];
			if(sphere_may_contain_candidate_for_d2(d2))
			{
				const std::vector<SimpleSphere> tangents=construct_spheres_tangent<SimpleSphere>(a_, b_, c_, d2);
				for(std::size_t i=0;i<tangents.size();i++)
				{
					if(!sphere_intersects_sphere(tangents[i], d1_))
					{
						if(d2_id==d1_id_)
						{
							if(!spheres_equal(tangents[i], d1_tangent_sphere_))
							{
								return std::make_pair(true, tangents[i]);
							}
						}
						else
						{
							return std::make_pair(true, tangents[i]);
						}
					}
				}
			}
		}
		return std::make_pair(false, SimpleSphere());
	}

	void set_d2(const std::size_t d2_id, const SimpleSphere& d2_tangent_sphere)
	{
		d2_id_=d2_id;
		d2_tangent_sphere_=d2_tangent_sphere;
		die_if_invalid();
	}

	void unset_d2()
	{
		d2_id_=npos;
		d2_tangent_sphere_=SimpleSphere();
	}

	Triple get_abc_ids_for_d2(const std::size_t num) const
	{
		if(d2_id_==npos || num>2)
		{
			throw std::logic_error("Invalid face triple request");
		}

		return Triple(abc_ids_.exclude(num), d2_id_);
	}

	ApolloniusFace get_face_for_d2(const std::size_t num) const
	{
		return ApolloniusFace(spheres_,
				get_abc_ids_for_d2(num),
				abc_ids_.get(num),
				d2_tangent_sphere_);
	}

private:
	static std::size_t select_free_tangent_plane_id(
			const Sphere& a,
			const Sphere& b,
			const Sphere& c,
			const std::vector< std::pair<SimplePoint, SimplePoint> >& tangent_planes,
			const Sphere& d1,
			const SimpleSphere& d1_tangent_sphere)
	{
		if(tangent_planes.empty())
		{
			return npos;
		}
		else
		{
			const int h0=halfspace_of_sphere(tangent_planes[0].first, tangent_planes[0].second, d1);
			const int h1=halfspace_of_sphere(tangent_planes[1].first, tangent_planes[1].second, d1);
			if(h0>-1 && h1==-1)
			{
				return 1;
			}
			else if(h1>-1 && h0==-1)
			{
				return 0;
			}
			else if(h0==-1 && h1==-1)
			{
				return npos;
			}
			else
			{
				const SimplePoint normal=plane_normal_from_three_points<SimplePoint>(a, b, c);
				const int th=halfspace_of_point(a, normal, d1_tangent_sphere);
				const int th0=halfspace_of_point(a, normal, tangent_planes[0].first+tangent_planes[0].second);
				const int th1=halfspace_of_point(a, normal, tangent_planes[1].first+tangent_planes[1].second);
				if(th0==th && th1!=th)
				{
					return 1;
				}
				else if(th1==th && th0!=th)
				{
					return 0;
				}
				else
				{
					return npos;
				}
			}
		}
	}

	template<typename InputSphereType>
	bool sphere_is_on_free_plane(const InputSphereType& x) const
	{
		return (free_tangent_plane_id_==npos || halfspace_of_sphere(tangent_planes_[free_tangent_plane_id_].first, tangent_planes_[free_tangent_plane_id_].second, x)>-1);
	}

	template<typename InputSphereType>
	bool sphere_is_inner(const InputSphereType& x) const
	{
		if(!tangent_planes_.empty())
		{
			for(std::size_t i=0;i<tangent_planes_.size();i++)
			{
				if(halfspace_of_sphere(tangent_planes_[i].first, tangent_planes_[i].second, x)!=-1)
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			return stick_contains_sphere(*tangent_stick_.first, *tangent_stick_.second, x);
		}
	}

	const std::vector<Sphere>& spheres_;
	Triple abc_ids_;
	const Sphere& a_;
	const Sphere& b_;
	const Sphere& c_;
	std::size_t d1_id_;
	const Sphere& d1_;
	SimpleSphere d1_tangent_sphere_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::pair<const Sphere*, const Sphere*> tangent_stick_;
	std::size_t free_tangent_plane_id_;

	std::size_t d2_id_;
	SimpleSphere d2_tangent_sphere_;
};

}

#endif /* APOLLONIUS_FACE_H_ */
