#ifndef APOLLONIUS_FACE_H_
#define APOLLONIUS_FACE_H_

#include <vector>
#include <stdexcept>

#include "tuples.h"
#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"

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
				d2_id_(npos),
				d2_(NULL),
				d2_tangent_sphere_(SimpleSphere(0,0,0,0)),
				tangent_planes_(construct_spheres_tangent_planes(a_, b_, c_)),
				tangent_stick_(tangent_planes_.empty() ? select_tangent_stick(a_, b_, c_) : std::make_pair(NULL, NULL))
	{
		if(!(sphere_touches_sphere(d1_tangent_sphere_, a_) &&
				sphere_touches_sphere(d1_tangent_sphere_, b_) &&
				sphere_touches_sphere(d1_tangent_sphere_, c_) &&
				sphere_touches_sphere(d1_tangent_sphere_, d1_)))
		{
			throw std::logic_error("Invalid d1 tangent sphere");
		}

		if(!((tangent_planes_.size()==2) ||
				(tangent_planes_.empty() && tangent_stick_.first!=NULL && tangent_stick_.second!=NULL)))
		{
			throw std::logic_error("Invalid tangency information");
		}
	}

private:
	const std::vector<Sphere>& spheres_;
	const Triple abc_ids_;
	const Sphere& a_;
	const Sphere& b_;
	const Sphere& c_;
	const std::size_t d1_id_;
	const Sphere& d1_;
	const SimpleSphere d1_tangent_sphere_;
	std::size_t d2_id_;
	const Sphere* d2_;
	SimpleSphere d2_tangent_sphere_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::pair<const Sphere*, const Sphere*> tangent_stick_;
};

}

#endif /* APOLLONIUS_FACE_H_ */
