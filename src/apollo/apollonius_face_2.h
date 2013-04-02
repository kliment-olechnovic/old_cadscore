#ifndef APOLLO_APOLLONIUS_FACE_2_H_
#define APOLLO_APOLLONIUS_FACE_2_H_

#include <vector>

#include "tuples.h"
#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusFace2
{
public:
	typedef SphereType Sphere;
	static const std::size_t npos=static_cast<std::size_t>(-1);

	ApolloniusFace2(const std::vector<Sphere>& spheres, const Triple& abc_ids) :
		spheres_(&spheres),
		abc_ids_(abc_ids),
		tangent_planes_(construct_spheres_tangent_planes(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)))),
		can_have_d_(tangent_planes_.size()==2),
		can_have_e_(!equal(spheres_->at(abc_ids_.get(0)).r, 0) && !equal(spheres_->at(abc_ids_.get(1)).r, 0) && !equal(spheres_->at(abc_ids_.get(2)).r, 0)),
		expansion_for_d_tangent_spheres_(std::max(0.0, std::max(spheres_->at(abc_ids_.get(0)).r, std::max(spheres_->at(abc_ids_.get(1)).r, spheres_->at(abc_ids_.get(2)).r))))
	{
		if(can_have_d_)
		{
			d_ids_and_tangent_spheres_.resize(2, std::pair<std::size_t, SimpleSphere>(npos, SimpleSphere()));
		}
		else
		{
			tangent_planes_.clear();
		}
	}

	const Triple& abc_ids() const
	{
		return abc_ids_;
	}

	bool can_have_d() const
	{
		return can_have_d_;
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_d(const InputSphereType& input_sphere, const std::size_t d_number) const
	{
		return (can_have_d_
				&& d_number<2
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, input_sphere)>-1)
				&& !sphere_is_contained_in_recorded_tangent_sphere(d_ids_and_tangent_spheres_, input_sphere, d_number)
				&& !sphere_is_contained_in_recorded_tangent_sphere(e_ids_and_tangent_spheres_, input_sphere, npos));
	}

	std::vector<SimpleSphere> check_candidate_for_d(const std::size_t d_id, const std::size_t d_number) const
	{
		if(can_have_d_
				&& d_id!=npos
				&& d_id!=d_ids_and_tangent_spheres_[d_number].first
				&& d_number<2
				&& !abc_ids_.contains(d_id)
				&& !id_equals_recorded_id(e_ids_and_tangent_spheres_, d_id, npos))
		{
			const Sphere& d_sphere=spheres_->at(d_id);
			if((halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, input_sphere)>-1)
					&& !sphere_intersects_recorded_tangent_sphere(d_ids_and_tangent_spheres_, d_sphere, d_number)
					&& !sphere_intersects_recorded_tangent_sphere(e_ids_and_tangent_spheres_, d_sphere, npos))
			{
				const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), d_sphere);
				for(std::size_t i=0;i<tangent_spheres.size();i++)
				{
					const SimpleSphere& tangent_sphere=tangent_spheres[i];
					if(!sphere_intersects_recorded_sphere(d_ids_and_tangent_spheres_, tangent_sphere, d_number)
							&& !sphere_intersects_recorded_sphere(e_ids_and_tangent_spheres_, tangent_sphere, npos))
					{
						if(tangent_spheres.size()==1
								|| (halfspace_of_point(tangent_planes_[d_number].first, tangent_planes_[d_number].second, SimplePoint(d_sphere)+((SimplePoint(tangent_sphere)-SimplePoint(d_sphere)).unit()*d_sphere.r))==1))
						{
							return std::vector<SimpleSphere>(1, tangent_sphere);
						}
					}
				}
			}
		}
		return std::vector<SimpleSphere>();
	}

	bool can_have_e() const
	{
		return can_have_e_;
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_e(const InputSphereType& input_sphere) const
	{
		return (can_have_e_
				&& (!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, e_sphere)!=1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, e_sphere)!=1))
				&& !sphere_is_contained_in_recorded_tangent_sphere(d_ids_and_tangent_spheres_, input_sphere, npos)
				&& !sphere_is_contained_in_recorded_tangent_sphere(e_ids_and_tangent_spheres_, input_sphere, npos)
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[0].first==npos || sphere_intersects_sphere(input_sphere, SimpleSphere(d_ids_and_tangent_spheres_[0].second, d_ids_and_tangent_spheres_[0].second.r+expansion_for_d_tangent_spheres_)))
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[1].first==npos || sphere_intersects_sphere(input_sphere, SimpleSphere(d_ids_and_tangent_spheres_[1].second, d_ids_and_tangent_spheres_[1].second.r+expansion_for_d_tangent_spheres_))));
	}

	std::vector<SimpleSphere> check_candidate_for_e(const std::size_t e_id) const
	{
		if(can_have_e_
				&& e_id!=npos
				&& !abc_ids_.contains(e_id)
				&& !id_equals_recorded_id(d_ids_and_tangent_spheres_, e_id, npos)
				&& !id_equals_recorded_id(e_ids_and_tangent_spheres_, e_id, npos))
		{
			const Sphere& e_sphere=spheres_->at(e_id);
			if((!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, e_sphere)==-1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, e_sphere)==-1))
					&& sphere_may_contain_candidate_for_e(e_sphere)
					&& !sphere_intersects_recorded_tangent_sphere(d_ids_and_tangent_spheres_, e_sphere, npos)
					&& !sphere_intersects_recorded_tangent_sphere(e_ids_and_tangent_spheres_, e_sphere, npos))
			{
				const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), e_sphere);
				std::vector<SimpleSphere> valid_tangent_spheres;
				for(std::size_t i=0;i<tangent_spheres.size();i++)
				{
					const SimpleSphere& tangent_sphere=tangent_spheres[i];
					if(!sphere_intersects_recorded_sphere(d_ids_and_tangent_spheres_, tangent_sphere, npos)
							&& !sphere_intersects_recorded_sphere(e_ids_and_tangent_spheres_, tangent_sphere, npos))
					{
						valid_tangent_spheres.push_back(tangent_sphere);
					}
				}
				return valid_tangent_spheres;
			}
		}
		return std::vector<SimpleSphere>();
	}

private:
	static bool id_equals_recorded_id(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const std::size_t id, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && id==recorded_ids_and_tangent_spheres[i].first)
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_intersects_recorded_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_intersects_sphere(input_sphere, spheres_->at(recorded_ids_and_tangent_spheres[i].first)))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_intersects_recorded_tangent_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_intersects_sphere(input_sphere, recorded_ids_and_tangent_spheres[i].second))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_is_contained_in_recorded_tangent_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_contains_sphere(recorded_ids_and_tangent_spheres[i].second, input_sphere))
			{
				return true;
			}
		}
		return false;
	}

	const std::vector<Sphere>* spheres_;
	Triple abc_ids_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::vector< std::pair<std::size_t, SimpleSphere> > d_ids_and_tangent_spheres_;
	std::vector< std::pair<std::size_t, SimpleSphere> > e_ids_and_tangent_spheres_;
	bool can_have_d_;
	bool can_have_e_;
	double expansion_for_d_tangent_spheres_;
};

}

#endif /* APOLLO_APOLLONIUS_FACE_2_H_ */
